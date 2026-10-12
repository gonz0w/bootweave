# SPDX-License-Identifier: BSD-3-Clause
"""Synthetic host admission checks; no extraction or native qualification."""
import fcntl
import io
import os
from pathlib import Path
import signal
import subprocess
import sys
import tarfile
import tempfile
import time
import unittest

ROOT = Path(__file__).resolve().parents[2]
ZERO = bytes(512)


def header(name='viewer/readme.txt', size=0, directory=False):
    item = tarfile.TarInfo(name)
    item.mode = 0o755 if directory else 0o644
    item.type = tarfile.DIRTYPE if directory else tarfile.REGTYPE
    item.size = size
    return item.tobuf(format=tarfile.USTAR_FORMAT)


def checksum(block):
    result = bytearray(block)
    result[148:156] = b' ' * 8
    result[148:156] = f'{sum(result):06o}'.encode() + b'\0 '
    return bytes(result)


def change(block, offset, data):
    result = bytearray(block)
    result[offset:offset + len(data)] = data
    return checksum(result)


def member(name='viewer/readme.txt', data=b'', directory=False):
    return header(name, len(data), directory) + data + bytes((-len(data)) % 512)


def archive(*members):
    return b''.join(members) + ZERO * 2


def wire_directory(name, prefix=b''):
    """Construct exact directory fields without tarfile's slash normalization."""
    if not 0 < len(name) <= 100 or len(prefix) > 155:
        raise ValueError('Fixture name/prefix exceeds its header field')
    raw = bytearray(512)
    raw[:len(name)] = name
    raw[100:124] = b'0000755\0' + b'0000000\0' * 2
    raw[124:148] = b'00000000000\0' * 2
    raw[148:156] = b' ' * 8
    raw[156] = ord('5')
    raw[257:265] = b'ustar\x0000'
    raw[345:345 + len(prefix)] = prefix
    raw[148:156] = f'{sum(raw):06o}'.encode() + b'\0 '
    return bytes(raw)


class PayloadTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix='payload-test-', dir=ROOT / '.tools')
        cls.directory = Path(cls.temp.name)
        cls.binary = cls.directory / 'inspect'
        cls.peak_rss_bytes = 0
        cls.peak_allocated_bytes = 0
        subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-pedantic',
                        str(ROOT / 'research/payload/inspect.c'), '-o', str(cls.binary)],
                       check=True, capture_output=True)
        if (ROOT / 'research/payload/inspect.c').stat().st_size > 32768:
            raise RuntimeError('Payload research source exceeds accepted size ceiling')

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()
        print(f'Payload host admission-case peak RSS: {cls.peak_rss_bytes} bytes')
        print(f'Payload host sampled fixture allocation: {cls.peak_allocated_bytes} bytes')

    def run_inspector(self, data, args=(), expected=True):
        before = sorted(path.name for path in self.directory.iterdir())
        # Owned files avoid producer/consumer deadlock and permit an independent
        # wait4 RSS observation for each supplied-buffer admission/rejection case.
        with tempfile.TemporaryFile(dir=self.directory) as source, \
                tempfile.TemporaryFile(dir=self.directory) as output, \
                tempfile.TemporaryFile(dir=self.directory) as errors:
            source.write(data); source.seek(0)
            process = subprocess.Popen([str(self.binary), *args], stdin=source,
                                       stdout=output, stderr=errors, cwd=self.directory)
            deadline = time.monotonic() + 15
            try:
                while True:
                    pid, status, usage = os.wait4(process.pid, os.WNOHANG)
                    if pid:
                        process.returncode = os.waitstatus_to_exitcode(status)
                        break
                    if time.monotonic() >= deadline:
                        process.kill(); process.wait()
                        self.fail('Owned inspector exceeded supervisor deadline')
                    time.sleep(.005)
                rss = usage.ru_maxrss * (1 if sys.platform == 'darwin' else 1024)
                type(self).peak_rss_bytes = max(self.peak_rss_bytes, rss)
                self.assertLessEqual(rss, 32 * 1024 * 1024)
                self.assertLessEqual(os.fstat(output.fileno()).st_size +
                                     os.fstat(errors.fileno()).st_size, 1048576)
                allocation = sum(os.fstat(stream.fileno()).st_blocks * 512
                                 for stream in (source, output, errors))
                allocation += sum(path.stat().st_blocks * 512 for path in self.directory.iterdir())
                type(self).peak_allocated_bytes = max(self.peak_allocated_bytes, allocation)
                self.assertLessEqual(allocation, 20 * 1024 * 1024)
                output.seek(0); errors.seek(0)
                result = subprocess.CompletedProcess(process.args, process.returncode,
                                                     output.read(4097), errors.read(4097))
                source.seek(0)
                self.assertEqual(source.read(len(data) + 1), data)
            finally:
                if process.returncode is None: process.kill(); process.wait()
        self.assertEqual(sorted(path.name for path in self.directory.iterdir()), before)
        self.assertLess(len(result.stdout) + len(result.stderr), 4096)
        self.assertNotIn(b'\x1b', result.stdout + result.stderr)
        if expected:
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stderr, b'')
            self.assertTrue(result.stdout.startswith(b'Inspection passed: '))
        else:
            self.assertNotEqual(result.returncode, 0, result.stdout)
            self.assertEqual(result.stdout, b'')
            self.assertTrue(result.stderr.startswith(b'Inspection rejected: '), result.stderr)
        return result

    def test_PAY01_viewer_counts_order_and_directories(self):
        readme = member(data=b'Offline example documentation.\n')
        for entries in ((member('viewer/', directory=True), readme),
                        (readme, member('viewer', directory=True)), (readme,)):
            result = self.run_inspector(archive(*entries))
            self.assertEqual(result.stdout,
                             f'Inspection passed: {len(entries)} members, 1 files, 31 content bytes.\n'.encode())
            self.assertNotIn(b'viewer', result.stdout)
        self.run_inspector(archive(change(header(), 156, b'\0')))
        self.run_inspector(archive(change(header(), 329, b'0000000\0' * 2)))

    def test_PAY02_header_numeric_and_truncation(self):
        good = header(size=513)
        for offset, data in ((257, b'xxxxx\0'), (262, b' '), (263, b'01'),
                             (500, b'X'), (100, b'0000644X')):
            self.run_inspector(archive(change(good, offset, data)), expected=False)
        for offset, width in ((100, 8), (108, 8), (116, 8), (124, 12), (136, 12), (148, 8)):
            for data in (b'8' + b'0' * (width - 2) + b'\0',
                         b'-' + b'0' * (width - 2) + b'\0',
                         b'\x80' + bytes(width - 1), b'0' * width,
                         b' ' + b'0' * (width - 2) + b'\0',
                         b'0\0X' + bytes(width - 3), bytes(width)):
                # Checksum mutations stay uncorrected to exercise its parser.
                bad = bytearray(header())
                bad[offset:offset + width] = data
                self.run_inspector(archive(bytes(bad) if offset == 148 else checksum(bad)), expected=False)
        bad = bytearray(member(data=b'abc'))
        bad[0] ^= 1
        self.run_inspector(archive(bytes(bad)), expected=False)
        complete = archive(member(data=b'x' * 513))
        for cut in (0, 1, 511, 512, 513, 1023, 1024, 1535, 1536, len(complete) - 1):
            self.run_inspector(complete[:cut], expected=False)
        # The last content block's unused bytes must be zero.
        bad = bytearray(archive(member(data=b'x')))
        bad[513] = 1
        self.run_inspector(bad, expected=False)
        self.run_inspector(archive(change(header(), 136, b'77777777777\0')))
        self.run_inspector(archive(change(header(), 108, b'0000000 ')))
        self.run_inspector(archive(change(header(), 124, b'00000000000 ')))

    def test_PAY03_path_rejection_and_private_output(self):
        for name in ('/absolute', '../escape', 'a/../escape', './file', 'a/./file',
                     'a//file', 'a\\file', 'file/', '.', '..', 'a\x1b[2J', 'a\x7f',
                     'a\nspoof', 'a\xff', '', 'private-$(touch canary)', '`touch canary`'):
            raw = header()
            data = name.encode('latin1')
            raw = change(raw, 0, data + bytes(100 - len(data)))
            result = self.run_inspector(archive(raw), expected=False)
            if len(data) > 2:
                self.assertNotIn(data, result.stdout + result.stderr)
        for prefix in (b'../escape', b'/absolute', b'a/', b'a//b', b'a\x00hidden'):
            self.run_inspector(archive(change(header(), 345, prefix + bytes(155 - len(prefix)))), expected=False)
        self.run_inspector(archive(change(header(), 0, b'a\0hidden')), expected=False)

    def test_PAY04_duplicates_aliases_and_ancestor_conflicts(self):
        pairs = ((member('a'), member('a')), (wire_directory(b'a'), wire_directory(b'a/')),
                 (member('a'), member('a', directory=True)), (member('a'), member('a/b')),
                 (member('a'), member('a/b/', directory=True)))
        for first, second in pairs:
            self.run_inspector(archive(first, second), expected=False)
            self.run_inspector(archive(second, first), expected=False)
        self.run_inspector(archive(member('a'), member('ab/b')))
        self.run_inspector(archive(member('a/b'), member('a/', directory=True)))

    def test_PAY05_excluded_types_ownership_mode_and_metadata(self):
        for kind in range(256):
            if kind in (0, ord('0'), ord('5')):
                continue
            self.run_inspector(archive(change(header(), 156, bytes([kind]))), expected=False)
        for offset, value in ((108, b'0000001\0'), (116, b'0000001\0'),
                              (100, b'0004755\0'), (100, b'0000755\0'),
                              (157, b'link'), (265, b'root'), (297, b'wheel'),
                              (329, b'0000001\0'), (337, b'0000001\0'),
                              (329, b'\0hidden')):
            self.run_inspector(archive(change(header(), offset, value)), expected=False)
        self.run_inspector(archive(header(directory=True, size=1)), expected=False)
        self.run_inspector(archive(change(header(directory=True), 100, b'0000644\0')), expected=False)

    def test_PAY06_limits_and_extremes(self):
        for length in (254, 255, 256):
            prefix = b'p' * (length - 101)
            raw = change(header(), 0, b'n' * 100)
            raw = change(raw, 345, prefix + bytes(155 - len(prefix)))
            self.run_inspector(archive(raw), expected=length <= 255)
        raw = change(header(), 0, b'n' * 99 + b'\0')
        self.run_inspector(archive(change(raw, 345, b'p' * 155)))
        for count in (127, 128, 129):
            self.run_inspector(archive(*(member(f'f{i}') for i in range(count))), expected=count <= 128)
        for size in (262143, 262144, 262145):
            self.run_inspector(archive(member(data=b'x' * size)), expected=size <= 262144)
        for size in (524287, 524288, 524289):
            self.run_inspector(archive(member('a', b'x' * 262144), member('b', b'x' * 262143),
                                       member('c', b'x' * (size - 524287))), expected=size <= 524288)
        raw = archive(member())
        for size in (1048064, 1048576, 1049088, 1048577):
            self.run_inspector(raw + bytes(size - len(raw)), expected=size <= 1048576)
        self.run_inspector(archive(change(header(), 124, b'77777777777\0')), expected=False)

    def test_PAY07_trailer_padding_and_concatenation(self):
        raw = archive(member())
        self.run_inspector(raw + ZERO * 4)
        for bad in (member() + ZERO, raw + b'\0', raw + ZERO[:-1], raw + b'garbage',
                    raw + raw, member() + ZERO + member() + ZERO * 2):
            self.run_inspector(bad, expected=False)

    def test_PAY08_deadlines_signals_partial_writes_and_output_failure(self):
        self.run_inspector(b'', expected=False)
        self.run_inspector(ZERO * 2, expected=False)
        self.run_inspector(archive(member('only-dir/', directory=True)), expected=False)
        for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGHUP):
            process = subprocess.Popen([str(self.binary)], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE, cwd=self.directory)
            try:
                process.stdin.write(header(size=10)); process.stdin.flush()
                time.sleep(.05)
                process.send_signal(sig)
                stdout, _ = process.communicate(timeout=3)
                self.assertNotEqual(process.returncode, 0)
                self.assertEqual(stdout, b'')
            finally:
                if process.poll() is None: process.kill(); process.communicate()
        process = subprocess.Popen([str(self.binary)], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE, cwd=self.directory)
        started = time.monotonic()
        try:
            process.stdin.write(header(size=10)); process.stdin.flush()
            process.wait(timeout=12)
            self.assertGreater(time.monotonic() - started, 9)
            self.assertLess(time.monotonic() - started, 12)
            stdout, _ = process.communicate()
            self.assertNotEqual(process.returncode, 0); self.assertEqual(stdout, b'')
        finally:
            if process.poll() is None: process.kill(); process.communicate()
        # Header declares an excessive body while the input writer stays open.
        process = subprocess.Popen([str(self.binary)], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE, cwd=self.directory)
        try:
            process.stdin.write(header(size=262145)); process.stdin.flush()
            process.wait(timeout=3)
            stdout, stderr = process.communicate()
            self.assertNotEqual(process.returncode, 0); self.assertEqual(stdout, b'')
            self.assertIn(b'content limit', stderr)
        finally:
            if process.poll() is None: process.kill(); process.communicate()
        # A valid archive arrives in small pieces rather than whole headers.
        process = subprocess.Popen([str(self.binary)], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE, cwd=self.directory)
        try:
            raw = archive(member(data=b'hello'))
            for at in range(0, len(raw), 17):
                process.stdin.write(raw[at:at + 17]); process.stdin.flush()
            stdout, stderr = process.communicate(timeout=3)
            self.assertEqual(process.returncode, 0, stderr); self.assertIn(b'5 content bytes', stdout)
        finally:
            if process.poll() is None: process.kill(); process.communicate()
        for blocked in (False, True):
            read_end, write_end = os.pipe()
            original = fcntl.fcntl(write_end, fcntl.F_GETFL)
            if blocked:
                fcntl.fcntl(write_end, fcntl.F_SETFL, original | os.O_NONBLOCK)
                filled = 0
                while True:
                    try: filled += os.write(write_end, b'X' * 4096)
                    except BlockingIOError: break
                    self.assertLess(filled, 1048576)
                while True:
                    try: filled += os.write(write_end, b'X')
                    except BlockingIOError: break
                    self.assertLess(filled, 1048576)
                fcntl.fcntl(write_end, fcntl.F_SETFL, original)
            else:
                os.close(read_end); read_end = None
            started = time.monotonic()
            try:
                process = subprocess.Popen([str(self.binary)], stdin=subprocess.PIPE, stdout=write_end,
                                           stderr=subprocess.PIPE, cwd=self.directory)
                _, stderr = process.communicate(archive(member()), timeout=7)
                self.assertNotEqual(process.returncode, 0)
                self.assertEqual(stderr, b'')
                # Host checks only the bit changed by the reader. Like the
                # existing console suite, full native flags remain unqualified.
                self.assertEqual(fcntl.fcntl(write_end, fcntl.F_GETFL) & os.O_NONBLOCK,
                                 original & os.O_NONBLOCK)
                if blocked:
                    self.assertGreater(time.monotonic() - started, 4)
                    self.assertLess(time.monotonic() - started, 7)
            finally:
                if process.poll() is None: process.kill(); process.communicate()
                os.close(write_end)
                if read_end is not None: os.close(read_end)

    @unittest.skipIf(sys.platform.startswith('openbsd'), 'host-refusal check')
    def test_PAY10_native_tools_refuse_host_effects(self):
        binary = self.directory / 'native-check-host'
        before = sorted(path.name for path in self.directory.iterdir())
        try:
            subprocess.run(['cc', '-std=c99', '-Os', '-Wall', '-Wextra', '-Werror', '-pedantic',
                            str(ROOT / 'research/payload/native_check.c'), '-o', str(binary)],
                           check=True, capture_output=True)
            result = subprocess.run([str(binary)], capture_output=True, timeout=3)
            self.assertEqual(result.returncode, 77)
            self.assertEqual(result.stdout, b'')
            self.assertEqual(result.stderr, b'Native payload checks require OpenBSD.\n')
            for name in ('native_build.sh', 'native_run.sh'):
                result = subprocess.run(['sh', str(ROOT / 'research/payload' / name)],
                                        capture_output=True, timeout=3)
                self.assertEqual(result.returncode, 77)
                self.assertEqual(result.stdout, b'')
            self.assertLess((ROOT / 'research/payload/native_check.c').stat().st_size, 32768)
        finally:
            binary.unlink(missing_ok=True)
        self.assertEqual(sorted(path.name for path in self.directory.iterdir()), before)

    def test_PAY11_native_fixture_recipe_matches_observed_bytes(self):
        import hashlib
        import native_fixtures
        cases = native_fixtures.make_cases()
        self.assertEqual(len(cases), 35)
        self.assertEqual(sum(map(len, cases)), 2806787)
        self.assertEqual(tuple(hashlib.sha256(data).hexdigest() for data in cases),
                         native_fixtures.DIGESTS)
        self.assertEqual(hashlib.sha256(native_fixtures.case_manifest(cases)).hexdigest(),
                         '052f68eeb8c267e6efe4c44ba6c233efc94b9574fed97a6dcaf1b5985b7f2b23')

    def test_PAY12_distinct_wire_directory_aliases(self):
        # The historical native fixture stays frozen. Confirm why its two
        # nominal spellings cannot establish distinct on-wire coverage.
        self.assertEqual(header('a', directory=True), header('a/', directory=True))
        groups = (((b'a', b''), (b'a/', b'')),
                  ((b'viewer/docs', b''), (b'viewer/docs/', b''),
                   (b'docs', b'viewer'), (b'docs/', b'viewer')))
        content = member('readme.txt', b'hello')
        for fields in groups:
            headers = [wire_directory(name, prefix) for name, prefix in fields]
            self.assertEqual(len(set(headers)), len(fields))
            for raw, (name, prefix) in zip(headers, fields):
                with self.subTest(name=name, prefix=prefix):
                    self.assertEqual(len(raw), 512)
                    self.assertEqual(raw[:100], name + bytes(100 - len(name)))
                    self.assertEqual(raw[345:500], prefix + bytes(155 - len(prefix)))
                    self.assertEqual(int(raw[148:156].strip(b'\0 '), 8),
                                     sum(raw[:148]) + 8 * 32 + sum(raw[156:]))
                    result = self.run_inspector(archive(content, raw))
                    self.assertEqual(result.stdout,
                                     b'Inspection passed: 2 members, 1 files, 5 content bytes.\n')
            for first in range(len(headers)):
                for second in range(len(headers)):
                    with self.subTest(first=fields[first], second=fields[second]):
                        # Each header admits on its own, and an unrelated file
                        # prevents directory-only rejection from masking a bug.
                        result = self.run_inspector(archive(content, headers[first], headers[second]),
                                                    expected=False)
                        self.assertEqual(result.returncode, 2)
                        self.assertEqual(result.stderr, b'Inspection rejected: tree conflict.\n')

    def test_PAY09_independent_header_no_effects_and_argument_boundary(self):
        # Independent fixed header, not tarfile or the mutation helper.
        raw = bytearray(512)
        raw[:10] = b'readme.txt'
        raw[100:124] = b'0000644\0' + b'0000000\0' * 2
        raw[124:148] = b'00000000003\0' + b'00000000000\0'
        raw[156] = ord('0'); raw[257:265] = b'ustar\x0000'
        raw[148:156] = b'007734\0 '
        result = self.run_inspector(bytes(raw) + b'abc' + bytes(509) + ZERO * 2)
        self.assertIn(b'1 members, 1 files, 3 content bytes', result.stdout)
        # Separate host encoder readback verifies content, without extracting.
        good = archive(member('canary', b'operator-private\x1b[2J'))
        with tarfile.open(fileobj=io.BytesIO(good)) as supplied:
            self.assertEqual(supplied.extractfile('canary').read(), b'operator-private\x1b[2J')
        canary = self.directory / 'canary'
        canary.write_bytes(b'untouched owned sentinel')
        try:
            result = self.run_inspector(good)
            self.assertNotIn(b'operator-private', result.stdout + result.stderr)
            self.assertEqual(canary.read_bytes(), b'untouched owned sentinel')
            for args in (('--debug',), ('$(touch unexpected-effect)',), ('/bin/sh',)):
                self.run_inspector(good, args=args, expected=False)
            self.assertEqual(canary.read_bytes(), b'untouched owned sentinel')
            self.assertFalse((self.directory / 'unexpected-effect').exists())
        finally:
            canary.unlink()


if __name__ == '__main__':
    unittest.main()
