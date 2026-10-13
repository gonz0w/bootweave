# SPDX-License-Identifier: BSD-3-Clause
"""Owned host binding/PTY observations; no native or write qualification."""
import ctypes
import errno
import fcntl
import os
from pathlib import Path
import pty
import select
import signal
import struct
import subprocess
import tempfile
import termios
import time
import unittest
from test_placement import Plan

ROOT = Path(__file__).resolve().parents[2]


class Binding(ctypes.Structure):
    _fields_ = [('plan', Plan), *[(x, ctypes.c_uint64) for x in
                 ('length', 'device', 'inode', 'owner', 'group', 'mode')],
                *[(x, ctypes.c_ubyte * 32) for x in ('payload', 'key', 'verifier', 'inspector')]]


class ReviewTooling(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory(prefix='bootweave-review-host-')
        cls.directory = Path(cls.tmp.name)
        cls.driver = cls.directory / 'driver'
        flags = ['cc', '-std=c99', '-Os', '-Wall', '-Wextra', '-Werror', '-pedantic']
        subprocess.run([*flags, str(ROOT / 'research/payload/review.c'),
                        str(ROOT / 'research/payload/review_host.c'), '-o', str(cls.driver)],
                       check=True, capture_output=True)
        lib = cls.directory / 'binding.dylib'
        subprocess.run([*flags, '-shared', '-fPIC', str(ROOT / 'research/payload/review.c'),
                        str(ROOT / 'research/payload/placement_plan.c'), '-o', str(lib)],
                       check=True, capture_output=True)
        cls.lib = ctypes.CDLL(str(lib))
        cls.lib.rv_valid.argtypes = [ctypes.POINTER(Binding)]
        cls.lib.rv_same.argtypes = [ctypes.POINTER(Binding), ctypes.POINTER(Binding)]

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def binding(self):
        b = Binding()
        b.length, b.device, b.inode, b.owner, b.group, b.mode = 3072, 1, 2, 32767, 32767, 0o700
        b.plan.count, b.plan.members, b.plan.files, b.plan.bytes = 2, 2, 1, 5
        a, c = b.plan.nodes[:2]
        a.path, a.directory, a.explicit_member, a.parent = b'viewer', 1, 1, -1
        c.path, c.directory, c.explicit_member, c.parent, c.offset, c.size = b'viewer/readme.txt', 0, 1, 0, 1024, 5
        return b

    def test_PAFH01_bindings_reject_each_changed_authority_field(self):
        b = self.binding()
        self.assertEqual(self.lib.rv_valid(ctypes.byref(b)), 1)
        for field in ('length', 'device', 'inode', 'owner', 'group', 'mode'):
            changed = Binding.from_buffer_copy(b)
            setattr(changed, field, getattr(changed, field) + 1)
            self.assertEqual(self.lib.rv_same(ctypes.byref(b), ctypes.byref(changed)), 0, field)
        for field in ('payload', 'key', 'verifier', 'inspector'):
            changed = Binding.from_buffer_copy(b)
            getattr(changed, field)[17] = 1
            self.assertEqual(self.lib.rv_same(ctypes.byref(b), ctypes.byref(changed)), 0, field)
        for field, value in [('path', b'viewer/different.txt'), ('size', 4), ('offset', 1023),
                             ('parent', -1), ('explicit_member', 0), ('directory', 1)]:
            changed = Binding.from_buffer_copy(b)
            setattr(changed.plan.nodes[1], field, value)
            self.assertEqual(self.lib.rv_same(ctypes.byref(b), ctypes.byref(changed)), 0, field)
        for path in (b'../escape', b'/absolute', b'a//b', b'a/./b', b'a/../b', b'a\x1b[0mb', b'a;command'):
            changed = Binding.from_buffer_copy(b)
            changed.plan.nodes[1].path = path
            self.assertEqual(self.lib.rv_valid(ctypes.byref(changed)), 0)
        for count in (0, 17, 128):
            changed = Binding.from_buffer_copy(b)
            changed.plan.count = count
            self.assertEqual(self.lib.rv_valid(ctypes.byref(changed)), 0)

    def run_console(self, actions, size=(24, 80), prequeue=b'', sig=None, kill=False,
                    lose=False, resize=False, timeout=15, expected=2):
        master, slave = pty.openpty()
        proc = None
        captured = bytearray()
        try:
            fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack('HHHH', *size, 0, 0))
            # Establish an active owned terminal before saving exact flags. Darwin
            # adds a sticky first-write flag; a virgin PTY cannot restore that bit.
            os.write(slave, b'Owned host terminal fixture\n')
            os.read(master, 128)
            initial = termios.tcgetattr(slave)
            if prequeue:
                initial[3] &= ~termios.ECHO
                termios.tcsetattr(slave, termios.TCSANOW, initial)
                os.write(master, prequeue)
            before_flags = fcntl.fcntl(slave, fcntl.F_GETFL)
            proc = subprocess.Popen([str(self.driver)], stdin=slave, stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE, start_new_session=True)
            limit = time.monotonic() + timeout
            cursor = 0

            def until(needle):
                nonlocal cursor
                while needle not in captured[cursor:]:
                    self.assertLess(time.monotonic(), limit, bytes(captured[-500:]))
                    ready, _, _ = select.select([master], [], [], min(.05, max(0, limit-time.monotonic())))
                    if ready:
                        try:
                            chunk = os.read(master, 4096)
                        except OSError as exc:
                            if exc.errno != errno.EIO:
                                raise
                            chunk = b''
                        captured.extend(chunk.replace(b'\r\n', b'\n'))
                        self.assertLessEqual(len(captured), 32768)
                    self.assertIsNone(proc.poll(), bytes(captured[-500:]))
                cursor = len(captured)

            for prompt, data in actions:
                until(prompt)
                if data is not None:
                    os.write(master, data)
            if sig:
                os.kill(proc.pid, sig)
            if resize:
                fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack('HHHH', 12, 40, 0, 0))
                os.kill(proc.pid, signal.SIGWINCH)
            if lose:
                os.close(master)
                master = -1
            if kill:
                os.kill(proc.pid, signal.SIGKILL)
            out, err = proc.communicate(timeout=max(.1, limit-time.monotonic()))
            if master >= 0:
                while select.select([master], [], [], 0)[0]:
                    captured.extend(os.read(master, 4096).replace(b'\r\n', b'\n'))
            if isinstance(expected, tuple):
                self.assertIn(proc.returncode, expected, (out, err, captured[-500:]))
            else:
                self.assertEqual(proc.returncode, -signal.SIGKILL if kill else expected, (out, err, captured[-500:]))
            if not kill:
                self.assertEqual(err, b'')
                self.assertIn(b'Host synthetic dispatch=' + (b'1' if expected == 0 else b'0'), out)
            if kill:
                self.assertNotEqual(termios.tcgetattr(slave), initial)
                termios.tcsetattr(slave, termios.TCSANOW, initial)
                fcntl.ioctl(slave, termios.FIONREAD, struct.pack('i', 0))
                termios.tcsetattr(slave, termios.TCSANOW, initial)
                fcntl.fcntl(slave, fcntl.F_SETFL, before_flags)
            if not lose:
                self.assertEqual(termios.tcgetattr(slave), initial)
                self.assertEqual(fcntl.fcntl(slave, fcntl.F_GETFL), before_flags)
            self.assertNotIn(b'\x1b', captured)
            return bytes(captured), out
        finally:
            if proc and proc.poll() is None:
                proc.kill()
                proc.wait()
            if master >= 0:
                os.close(master)
            os.close(slave)

    def positive(self, delimiter=b'\n', size=(24, 80)):
        return self.run_console([(b'Type review to confirm these effects.', b'review'+delimiter),
                                 (b'place viewer-case 1', b'place viewer-case 1'+delimiter)], size=size, expected=0)

    def test_PAFH02_exact_fresh_confirmation_and_full_review(self):
        for delimiter in (b'\n', b'\r', b'\r\n'):
            text, _ = self.positive(delimiter)
            for value in (b'Root dev=1 inode=2', b'32767/32767 mode 0700', b'32767/32767 mode 0600',
                          b'File viewer/readme.txt bytes=5', b'Directory viewer bytes=0',
                          b'Planned=2 files=1 bytes=5 input=3072'):
                self.assertIn(value, text)

    def test_PAFH03_help_back_invalidate_and_stale_phrase_rejects(self):
        for command in (b'h\n', b'b\n'):
            text, _ = self.run_console([(b'Type review to confirm these effects.', b'review\n'),
                                       (b'place viewer-case 1', command),
                                       (b'Type review to confirm these effects.', b'review\n'),
                                       (b'place viewer-case 2', b'place viewer-case 1\n')])
            self.assertIn(b'Review invalidated; no file effects.', text)

    def test_PAFH04_controls_overflow_defaults_and_queued_input(self):
        prompt = b'Type review to confirm these effects.'
        for data in (b'\n', b'place viewer-case 1\n', b'review\x00\n', b'review\x1b[31m\n',
                     b'review\x80\n', b'x'*1025+b'\n', b'review\nplace viewer-case 1\n', b'q\n'):
            self.run_console([(prompt, data)])
        self.run_console([], prequeue=b'review\nplace viewer-case 1\n')
        self.run_console([(prompt, b'review\n'), (b'place viewer-case 1', b'place viewer-case 1\nq\n')])

    def test_PAFH05_dimensions_and_resize(self):
        self.positive(size=(12, 40))
        text, _ = self.positive(size=(0, 0))
        self.assertIn(b'Dimensions unknown; fit unqualified.', text)
        self.run_console([], size=(11, 39))
        self.run_console([(b'Type review to confirm these effects.', None)], resize=True)

    def test_PAFH06_signals_timeout_death_and_lost_console(self):
        for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGHUP):
            self.run_console([(b'Type review to confirm these effects.', None)], sig=sig)
        self.run_console([(b'Type review to confirm these effects.', None)])
        self.run_console([(b'Type review to confirm these effects.', None)], kill=True)
        self.run_console([(b'Type review to confirm these effects.', None)], lose=True, expected=(2, 3))

    def test_PAFH07_no_context_argument_backend_and_source_caps(self):
        r = subprocess.run([str(self.driver), '$(touch forbidden)'], capture_output=True)
        self.assertEqual((r.returncode, r.stdout, r.stderr), (2, b'', b''))
        binary = self.directory/'refusal'
        subprocess.run(['cc', '-std=c99', '-Os', '-Wall', '-Wextra', '-Werror', '-pedantic',
                        '-DAUTH_TEST', '-DAUTH_PLACEMENT', '-DAUTH_REVIEW',
                        *[str(ROOT/'research/payload'/p) for p in ('auth_native.c', 'placement_plan.c',
                           'placement_native.c', 'placement_check.c', 'review.c', 'review_native.c')],
                        '-o', str(binary)], check=True, capture_output=True)
        r = subprocess.run([str(binary)], capture_output=True)
        self.assertEqual(r.returncode, 77)
        self.assertEqual(r.stdout, b'')
        self.assertFalse((self.directory/'forbidden').exists())
        self.assertLessEqual(sum(p.stat().st_size for p in (ROOT/'research/payload').glob('review*')
                                 if p.suffix in ('.c', '.h')), 32768)
