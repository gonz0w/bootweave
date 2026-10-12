# SPDX-License-Identifier: BSD-3-Clause
"""Accepted host-only authentication checks; no native or extraction evidence."""
from dataclasses import replace
import fcntl
import hashlib
import os
from pathlib import Path
import resource
import signal
import stat
import subprocess
import sys
import tempfile
import time
import unittest
from unittest.mock import patch

import authenticate as gate
from test_payload import ZERO, archive, member

ROOT = gate.ROOT
SOURCE_SHA = '96006fa573cf1cdeae4b96682ced2ade7750830984346d009bd88e07e942d264'


@unittest.skipUnless(sys.platform == 'darwin', 'reviewed macOS-only host experiment')
class AuthenticationTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if not gate.VERIFIER.exists():
            raise unittest.SkipTest('Reviewed local verifier unavailable; authentication not run')
        gate._pin(gate.VERIFIER, gate.VERIFIER_SHA256)
        source = gate.VERIFIER.parent / 'src/usr.bin/signify/signify.c'
        if hashlib.sha256(source.read_bytes()).hexdigest() != SOURCE_SHA:
            raise RuntimeError('Reviewed verifier source identity mismatch')
        inspector_source = ROOT / 'research/payload/inspect.c'
        if hashlib.sha256(inspector_source.read_bytes()).hexdigest() != '8009aed82f67ae60c70538579fa493995a87fa6ff1ea324dc23ee79af0847698':
            raise RuntimeError('Inspector differs from accepted unchanged source')
        cls.child_pids = []
        cls.temp = tempfile.TemporaryDirectory(prefix='payload-auth-tests-', dir=ROOT / '.tools')
        cls.addClassCleanup(cls.dispose)
        cls.directory = Path(cls.temp.name)
        cls.binary = cls.directory / 'inspect'
        subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-pedantic',
                        str(ROOT / 'research/payload/inspect.c'), '-o', str(cls.binary)],
                       check=True, capture_output=True, timeout=15)
        cls.binary.chmod(0o500)
        cls.inspector = gate.Inspector(cls.binary, hashlib.sha256(cls.binary.read_bytes()).hexdigest())
        for name in ('selected', 'other'):
            result = cls.child((str(gate.VERIFIER), '-G', '-n', '-c', 'disposable research',
                                '-p', name + '.pub', '-s', name + '.sec'))
            if result.status != 0 or result.stdout or result.stderr:
                raise RuntimeError('Disposable test-key generation failed')
        cls.public = (cls.directory / 'selected.pub').read_bytes()
        cls.payload = archive(member('viewer/readme.txt', b'hello'))
        cls.signature = cls.sign(cls.payload)
        cls.other_signature = cls.sign(cls.payload, 'other')
        (cls.directory / 'input.sig').write_bytes(cls.signature)
        cls.context = gate.Context(len(cls.payload), hashlib.sha256(cls.payload).hexdigest(), cls.public)
        # Real transport prerequisites. No controller/double supplies these
        # cryptographic positive/negative outcomes.
        good = cls.child((str(gate.VERIFIER), '-V', '-q', '-p', 'selected.pub',
                          '-x', 'input.sig', '-m', '-'), cls.payload)
        bad = cls.child((str(gate.VERIFIER), '-V', '-q', '-p', 'selected.pub',
                         '-x', 'input.sig', '-m', '-'), cls.payload + b'x')
        if good.status != 0 or good.stdout or good.stderr or bad.status == 0:
            raise RuntimeError('Detached stdin positive/negative prerequisite failed')

    @classmethod
    def dispose(cls):
        directory = Path(cls.temp.name)
        cls.temp.cleanup()
        if directory.exists():
            raise RuntimeError('Owned signing fixtures were not disposed')

    @classmethod
    def child(cls, argv, data=b'', seconds=15):
        original = gate.subprocess.Popen
        def observe(*args, **kwargs):
            process = original(*args, **kwargs)
            cls.child_pids.append(process.pid)
            return process
        with patch.object(gate.subprocess, 'Popen', observe):
            return gate._run(argv, data, cls.directory, time.monotonic() + seconds, [False])

    @classmethod
    def sign(cls, data, name='selected'):
        path = cls.directory / 'input.sig'
        path.unlink(missing_ok=True)
        result = cls.child((str(gate.VERIFIER), '-S', '-s', name + '.sec',
                            '-m', '-', '-x', 'input.sig'), data)
        if result.status != 0 or result.stdout or result.stderr:
            raise RuntimeError('Synthetic test signature creation failed')
        return path.read_bytes()

    @classmethod
    def tearDownClass(cls):
        rss = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
        rss *= 1 if sys.platform == 'darwin' else 1024
        if rss > 64 * 1024 * 1024:
            raise RuntimeError('Host controller/test-process RSS exceeded ceiling')
        allocated = sum(p.stat().st_blocks * 512 for p in cls.directory.iterdir())
        if allocated > 20 * 1024 * 1024:
            raise RuntimeError('Owned fixture allocation exceeded ceiling')
        for pid in cls.child_pids:
            try:
                os.kill(pid, 0)
            except ProcessLookupError:
                continue
            raise RuntimeError('An owned controller test process remains')
        print(f'Authentication host test-process lifetime peak RSS: {rss} bytes')
        print(f'Authentication child-lifetime peak RSS: {gate.METRICS["child_peak_rss_bytes"]} bytes')
        print(f'Authentication sampled owned allocation: {allocated} bytes')

    def execute(self, payload=None, signature=None, context=None, inspector=None):
        payload = self.payload if payload is None else payload
        signature = self.signature if signature is None else signature
        context = self.context if context is None else context
        inspector = self.inspector if inspector is None else inspector
        before = set((ROOT / '.tools').glob('payload-auth-*'))
        with tempfile.TemporaryFile(dir=self.directory) as source:
            source.write(payload); source.seek(0)
            original = fcntl.fcntl(source, fcntl.F_GETFL)
            original_popen = gate.subprocess.Popen
            def observe(*args, **kwargs):
                process = original_popen(*args, **kwargs)
                self.child_pids.append(process.pid)
                return process
            with patch.object(gate.subprocess, 'Popen', observe):
                result = gate.inspect_authenticated(source.fileno(), signature, context, inspector)
            self.assertEqual(fcntl.fcntl(source, fcntl.F_GETFL) & os.O_NONBLOCK,
                             original & os.O_NONBLOCK)
            source.seek(0)
            self.assertTrue(source.read() == payload)
        self.assertEqual(set((ROOT / '.tools').glob('payload-auth-*')), before)
        self.assertLessEqual(len(result.summary), gate.OUTPUT_CAP)
        self.assertNotIn(b'\x1b', result.summary)
        return result

    def test_PAV01_real_signature_then_inspection(self):
        calls = []
        original = gate._run
        def observe(argv, data, *args):
            calls.append((argv, hashlib.sha256(data).hexdigest()))
            return original(argv, data, *args)
        with patch.object(gate, '_run', observe):
            result = self.execute()
        self.assertTrue(result.completed and result.verified)
        self.assertEqual(result.summary, b'Inspection passed: 1 members, 1 files, 5 content bytes.\n')
        self.assertEqual(len(calls), 2)
        self.assertEqual(calls[0][0][0], str(gate.VERIFIER))
        self.assertEqual(calls[1][0], (str(self.binary),))
        self.assertTrue(all(digest == self.context.expected_sha256 for _, digest in calls))

    def test_PAV02_tampering_identity_and_wrong_signer(self):
        damaged = bytearray(self.signature)
        at = damaged.index(b'\n') + 1 + 16
        damaged[at] = ord('A') if damaged[at] != ord('A') else ord('B')
        changed = archive(member('viewer/readme.txt', b'world'))
        changed_context = replace(self.context, expected_sha256=hashlib.sha256(changed).hexdigest())
        cases = ((self.payload + b'x', self.signature, self.context),
                 (changed, self.signature, changed_context),
                 (self.payload, self.signature, replace(self.context, expected_length=len(self.payload) - 1)),
                 (self.payload, self.signature, replace(self.context, expected_sha256='0' * 64)),
                 (self.payload, b'', self.context), (self.payload, self.signature[:-2], self.context),
                 (self.payload, bytes(damaged), self.context),
                 (self.payload, self.other_signature, self.context))
        original = gate._run
        for index, (data, signature, context) in enumerate(cases):
            with self.subTest(case=index):
                calls = []
                def observe(argv, *args):
                    calls.append(argv[0])
                    return original(argv, *args)
                with patch.object(gate, '_run', observe):
                    result = self.execute(data, signature, context)
                self.assertFalse(result.completed or result.verified)
                self.assertEqual(result.summary, b'')
                self.assertNotIn(str(self.binary), calls)

    def test_PAV03_comments_cannot_select_key_or_emit_controls(self):
        encoded = self.signature.split(b'\n')[1]
        for comment in (b'verify with other.pub', b'\x1b[2J\x1b]0;spoof\x07',
                        b'$(touch unexpected-effect)', b'`touch unexpected-effect`'):
            result = self.execute(signature=b'untrusted comment: ' + comment + b'\n' + encoded + b'\n')
            self.assertTrue(result.completed)
            self.assertFalse((self.directory / 'unexpected-effect').exists())
        for malformed in (b'bad\n' + encoded + b'\n', self.signature + b'extra\n',
                          b'untrusted comment: null\0hidden\n' + encoded + b'\n'):
            with patch.object(gate, '_run') as child:
                self.assertFalse(self.execute(signature=malformed).completed)
                child.assert_not_called()

    def test_PAV04_signed_malformed_archive_is_not_completion(self):
        malformed = archive(member('../escape'))
        context = replace(self.context, expected_length=len(malformed),
                          expected_sha256=hashlib.sha256(malformed).hexdigest())
        result = self.execute(malformed, self.sign(malformed), context)
        self.assertTrue(result.verified)
        self.assertFalse(result.completed)
        self.assertEqual(result.category, 'inspection')
        self.assertEqual(result.summary, b'')

    def test_PAV05_original_source_changed_after_capture(self):
        source_path = self.directory / 'source'
        original_run = gate._run
        for replacement in (False, True):
            source_path.write_bytes(self.payload)
            with source_path.open('rb') as source:
                calls = []
                def change_original(argv, data, *args):
                    calls.append(hashlib.sha256(data).hexdigest())
                    result = original_run(argv, data, *args)
                    if len(calls) == 1:
                        if replacement:
                            source_path.unlink()
                        source_path.write_bytes(b'changed original')
                    return result
                with patch.object(gate, '_run', change_original):
                    result = gate.inspect_authenticated(source.fileno(), self.signature,
                                                       self.context, self.inspector)
                self.assertTrue(result.completed)
                self.assertEqual(calls, [self.context.expected_sha256] * 2)
                self.assertTrue(source_path.read_bytes() == b'changed original')
        source_path.unlink()
        self.assertFalse(self.execute(b'pre-capture change').completed)
        # Independent consumer hashes the bytes actually received through its
        # pipe. Only this trusted test substitutes a reviewed observer program.
        source_path.write_bytes(self.payload)
        observed = []
        observer = ('import hashlib,os,sys; data=sys.stdin.buffer.read(1048577); '
                    'open("received.digest","xb").write(hashlib.sha256(data).hexdigest().encode()); '
                    'os.write(1,b"Inspection passed: 1 members, 1 files, 5 content bytes.\\n")')
        def observe_received(argv, data, owned, deadline, cancelled):
            if argv[0] == str(gate.VERIFIER):
                result = original_run(argv, data, owned, deadline, cancelled)
                source_path.write_bytes(archive(member('viewer/readme.txt', b'world')))
                return result
            result = original_run((sys.executable, '-I', '-c', observer), data, owned, deadline, cancelled)
            captured = owned / 'received.digest'
            observed.append(captured.read_bytes())
            captured.unlink()
            return result
        try:
            with source_path.open('rb') as source, patch.object(gate, '_run', observe_received):
                result = gate.inspect_authenticated(source.fileno(), self.signature, self.context, self.inspector)
            self.assertTrue(result.completed)
            self.assertTrue(observed == [self.context.expected_sha256.encode()])
            self.assertNotEqual(hashlib.sha256(source_path.read_bytes()).hexdigest(), self.context.expected_sha256)
        finally:
            source_path.unlink()

    def test_PAV06_bounds_and_injected_environment_or_arguments(self):
        exact = self.payload + bytes(gate.INPUT_CAP - len(self.payload))
        context = replace(self.context, expected_length=len(exact), expected_sha256=hashlib.sha256(exact).hexdigest())
        self.assertTrue(self.execute(exact, self.sign(exact), context).completed)
        for payload in (b'', exact + b'x'):
            with patch.object(gate, '_run') as child:
                self.assertFalse(self.execute(payload).completed)
                child.assert_not_called()
        for context in (replace(self.context, expected_length=True),
                        replace(self.context, expected_length=gate.INPUT_CAP + 1),
                        replace(self.context, expected_sha256='$(touch unexpected-effect)'),
                        replace(self.context, public_key=self.public + bytes(gate.RECORD_CAP))):
            with patch.object(gate, '_run') as child:
                self.assertFalse(self.execute(context=context).completed)
                child.assert_not_called()
        with patch.object(gate, '_run') as child:
            self.assertFalse(self.execute(signature=self.signature + bytes(gate.RECORD_CAP)).completed)
            child.assert_not_called()
        with patch.object(gate, '_run') as child:
            result = self.execute(inspector=replace(self.inspector, sha256='0' * 64))
            self.assertEqual(result.category, 'tools')
            self.assertFalse(result.completed or result.verified)
            child.assert_not_called()
        with patch.dict(os.environ, {'PATH': str(self.directory), 'SIGNIFY_OPTIONS': '-e',
                                      'PYTHONPATH': str(self.directory), 'LC_ALL': 'invalid'}):
            self.assertTrue(self.execute().completed)
        for args, expected in (([], 77), (['--debug'], 2), (['$(touch unexpected-effect)'], 2)):
            result = subprocess.run([sys.executable, str(ROOT / 'research/payload/authenticate.py'), *args],
                                    capture_output=True, timeout=3)
            self.assertEqual(result.returncode, expected)
            self.assertEqual(result.stdout, b'')
            self.assertEqual(result.stderr, b'Host authentication research requires trusted test context.\n')

    def test_PAV07_child_failures_and_bounded_supervision(self):
        original = gate._run
        for stage in (0, 1):
            for status, out, errors in ((1, b'', b'private diagnostic\x1b[2J'),
                                        (0, b'fake success', b''),
                                        (0, b'Inspection passed: 999 members, 1 files, 0 content bytes.\n', b'')):
                calls = []
                def controlled(argv, *args):
                    calls.append(argv[0])
                    if len(calls) - 1 == stage:
                        return gate.ChildResult(status, out, errors, 0)
                    return original(argv, *args)
                with patch.object(gate, '_run', controlled):
                    result = self.execute()
                self.assertFalse(result.completed)
                self.assertEqual(result.summary, b'')
                if stage == 0:
                    self.assertEqual(len(calls), 1)
        # Fixed reviewed test programs exercise the actual supervisor, with no
        # runtime flag or input-selected executable in the controller.
        programs = (
            ('import os; os.write(1,b"X"*4097)', b'', 'output'),
            ('import os; os.close(0)', b'X' * gate.INPUT_CAP, 'io'),
            ('import time; time.sleep(2)', b'', 'deadline'),
            ('import time; time.sleep(2)', b'X' * gate.INPUT_CAP, 'deadline'),
        )
        for program, data, category in programs:
            with self.subTest(category=category):
                started = time.monotonic()
                with self.assertRaises(gate.Failure) as failure:
                    self.child((sys.executable, '-I', '-c', program), data, seconds=.4)
                self.assertEqual(failure.exception.category, category)
                self.assertLess(time.monotonic() - started, 2)
        limits = self.child((sys.executable, '-I', '-c',
                            'import resource; print(resource.getrlimit(resource.RLIMIT_CORE)[0],'
                            'resource.getrlimit(resource.RLIMIT_CPU)[0],'
                            'resource.getrlimit(resource.RLIMIT_FSIZE)[0])'))
        self.assertEqual(limits.stdout, b'0 15 1048576\n')
        self.assertEqual(limits.status, 0)
        with patch.object(gate, '_run', side_effect=gate.Failure('cleanup')):
            result = self.execute()
            self.assertFalse(result.completed)
            self.assertEqual(result.category, 'cleanup')
        def changed_inventory(argv, data, owned, *args):
            result = original(argv, data, owned, *args)
            (owned / 'unexpected').write_bytes(b'trusted test effect')
            return result
        with patch.object(gate, '_run', changed_inventory):
            result = self.execute()
            self.assertFalse(result.completed or result.verified)
            self.assertEqual(result.category, 'cleanup')
        def changed_mode(argv, data, owned, *args):
            result = original(argv, data, owned, *args)
            (owned / 'selected.pub').chmod(0o644)
            return result
        with patch.object(gate, '_run', changed_mode):
            result = self.execute()
            self.assertFalse(result.completed or result.verified)
            self.assertEqual(result.category, 'cleanup')
        secret = (self.directory / 'selected.sec').read_bytes()
        with patch.object(gate, '_run', return_value=gate.ChildResult(1, b'', secret, 0)):
            result = self.execute()
            self.assertFalse(result.completed or result.verified)
            self.assertFalse(secret in result.summary)

    def test_PAV08_permissions_inventory_signals_and_cleanup(self):
        canary = self.directory / 'canary'
        canary.write_bytes(b'untouched owned sentinel')
        before = sorted(p.name for p in self.directory.iterdir())
        original = gate._run
        def permissions(argv, data, owned, *args):
            self.assertEqual(stat.S_IMODE(owned.stat().st_mode), 0o700)
            self.assertEqual(sorted(p.name for p in owned.iterdir()), ['selected.pub', 'supplied.sig'])
            for path in owned.iterdir():
                self.assertEqual(stat.S_IMODE(path.stat().st_mode), 0o600)
            return original(argv, data, owned, *args)
        with patch.object(gate, '_run', permissions):
            self.assertTrue(self.execute().completed)
        self.assertTrue(canary.read_bytes() == b'untouched owned sentinel')
        self.assertEqual(sorted(p.name for p in self.directory.iterdir()), before)
        for name in ('selected.sec', 'other.sec', 'selected.pub', 'other.pub', 'input.sig'):
            self.assertEqual(stat.S_IMODE((self.directory / name).stat().st_mode), 0o600)
        for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGHUP):
            read_end, write_end = os.pipe()
            report_read, report_write = os.pipe()
            pid = os.fork()
            if pid == 0:
                os.close(write_end); os.close(report_read)
                try:
                    # Signal at the trusted launch barrier after a real child
                    # exists; the supervisor must terminate and reap it.
                    original_popen = gate.subprocess.Popen
                    launched = []
                    def stop_during_child(*args, **kwargs):
                        process = original_popen(*args, **kwargs)
                        launched.append(process.pid)
                        os.kill(os.getpid(), sig)
                        return process
                    prior = gate.METRICS['reaped_children']
                    with patch.object(gate.subprocess, 'Popen', stop_during_child):
                        result = gate.inspect_authenticated(read_end, self.signature, self.context, self.inspector)
                    passed = (not result.completed and result.category == 'interrupted'
                              and gate.METRICS['reaped_children'] == prior + 1)
                    for child_pid in launched:
                        try:
                            os.kill(child_pid, 0)
                            passed = False
                        except ProcessLookupError:
                            pass
                    os.write(report_write, b'pass' if passed else b'fail')
                finally:
                    os._exit(0)
            self.child_pids.append(pid)
            os.close(read_end); os.close(report_write)
            os.write(write_end, self.payload); os.close(write_end)
            try:
                _, status, _ = os.wait4(pid, 0)
                self.assertEqual(os.waitstatus_to_exitcode(status), 0)
                self.assertEqual(os.read(report_read, 5), b'pass')
            finally:
                os.close(report_read)
        # An open unfinished input is bounded by the real ten-second capture
        # deadline; parent checks source flags and never writes archive members.
        read_end, write_end = os.pipe()
        started = time.monotonic()
        try:
            os.write(write_end, self.payload[:512])
            result = gate.inspect_authenticated(read_end, self.signature, self.context, self.inspector)
            self.assertFalse(result.completed or result.verified)
            self.assertEqual(result.category, 'deadline')
            self.assertGreater(time.monotonic() - started, 9)
            self.assertLess(time.monotonic() - started, 12)
        finally:
            os.close(read_end); os.close(write_end); canary.unlink()


if __name__ == '__main__':
    unittest.main()
