# SPDX-License-Identifier: BSD-3-Clause
"""Host model and owned PTY checks; not native installer qualification."""
import fcntl
import os
from pathlib import Path
import platform
import select
import signal
import struct
import subprocess
import tempfile
import termios
import time
import unittest

ROOT = Path(__file__).resolve().parents[2]


class HandoffTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix='handoff-test-', dir=ROOT / '.tools')
        cls.driver = Path(cls.temp.name) / 'driver'
        cls.production = Path(cls.temp.name) / 'production'
        cls.model = Path(cls.temp.name) / 'model'
        cls.native_pty = Path(cls.temp.name) / 'native-pty'
        common = ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-pedantic',
                  '-I' + str(ROOT / 'research/storage')]
        shared = [str(ROOT / p) for p in ('research/handoff/snapshot.c', 'research/storage/validate.c')]
        for binary, main, console in ((cls.driver, 'host-driver.c', True),
                                      (cls.production, 'handoff.c', True),
                                      (cls.model, 'snapshot-test.c', False)):
            sources = [str(ROOT / 'research/handoff' / main), *shared]
            if console:
                sources.append(str(ROOT / 'research/handoff/console.c'))
            subprocess.run([*common, *sources, '-o', str(binary)], check=True)
        if platform.system() != 'OpenBSD':
            subprocess.run([*common, str(ROOT / 'research/handoff/native-pty.c'),
                            '-o', str(cls.native_pty)], check=True)

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def test_model_and_argument_boundary(self):
        subprocess.run([str(self.model)], check=True, capture_output=True)
        for args, code in ((['--debug'], 2), (['/bin/sh'], 2), (['$(id)'], 2)):
            p = subprocess.run([str(self.production), *args], capture_output=True, timeout=3)
            self.assertEqual(p.returncode, code)
        p = subprocess.run([str(self.driver)], input=b'next\nnext\nerase 0123456789abcdef\n',
                           capture_output=True, timeout=3)
        self.assertNotEqual(p.returncode, 0)

    @unittest.skipIf(platform.system() == 'OpenBSD',
                     'Native observation needs a separately approved target')
    def test_host_production_stub(self):
        p = subprocess.run([str(self.production)], capture_output=True, timeout=3)
        self.assertEqual(p.returncode, 77)
        p = subprocess.run([str(self.native_pty)], capture_output=True, timeout=3)
        self.assertEqual(p.returncode, 77)

    def run_tty(self, action, size=(24, 80), mode=None):
        master, slave = os.openpty()
        original = termios.tcgetattr(slave)
        # Host checks the O_NONBLOCK bit this console changes; native checks
        # exact F_GETFL restoration as well as all terminal attributes.
        original_flags = fcntl.fcntl(slave, fcntl.F_GETFL) & os.O_NONBLOCK
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack('HHHH', *size, 0, 0))
        p = subprocess.Popen([str(self.driver), *([mode] if mode else [])],
                             stdin=slave, stdout=slave, stderr=subprocess.PIPE)
        output = bytearray()

        def until(marker, timeout=3):
            deadline = time.monotonic() + timeout
            while marker not in output and time.monotonic() < deadline:
                if select.select([master], [], [], .05)[0]:
                    output.extend(os.read(master, 4096))
                if p.poll() is not None:
                    break
            self.assertIn(marker, output)
            return bytes(output)

        try:
            if (size[1] and size[1] < 40) or (size[0] and size[0] < 12):
                p.wait(timeout=3)
            else:
                until(b'Type next to review effects.')
                self.assertFalse(termios.tcgetattr(slave)[3] & termios.ECHO)
                action(p, master, until)
                p.wait(timeout=12)
            while select.select([master], [], [], .05)[0]:
                output.extend(os.read(master, 4096))
            self.assertEqual(termios.tcgetattr(slave), original)
            self.assertEqual(fcntl.fcntl(slave, fcntl.F_GETFL) & os.O_NONBLOCK, original_flags)
            self.assertNotIn(b'\x1b', output)
            self.assertLessEqual(len(output), 65536)
            return p.returncode, bytes(output)
        finally:
            if p.poll() is None:
                p.kill()
                p.wait()
            termios.tcsetattr(slave, termios.TCSANOW, original)
            p.stderr.close()
            os.close(master)
            os.close(slave)

    @staticmethod
    def confirm(p, master, until, delimiter=b'\n'):
        os.write(master, b'next' + delimiter)
        until(b'Type next to confirm.')
        os.write(master, b'next' + delimiter)
        until(b'erase 0123456789abcdef')
        os.write(master, b'erase 0123456789abcdef' + delimiter)

    def test_fresh_confirmation_sizes_and_delimiters(self):
        for size in ((24, 80), (12, 40), (0, 0)):
            for delimiter in (b'\n', b'\r', b'\r\n'):
                code, output = self.run_tty(lambda p, m, u: self.confirm(p, m, u, delimiter), size)
                self.assertEqual(code, 0)
                self.assertIn(b'FIXTURE CONFIRMED', output)
                self.assertTrue(all(len(line) <= 40 for line in output.splitlines()))
                if not size[1]:
                    self.assertIn(b'Dimensions unknown', output)
        for size in ((12, 39), (0, 39), (1, 80)):
            code, _ = self.run_tty(None, size)
            self.assertNotEqual(code, 0)

    def test_invalid_controls_overflow_queue_and_cancel(self):
        for data in (b'\n', b'q\n', b'erase 0123456789abcdef\n', b'\x1b]0;spoof\n',
                     b'next\x00\n', b'\xff\n', b'a' * 1025 + b'\n',
                     b'next\nnext\nerase 0123456789abcdef\n'):
            code, output = self.run_tty(lambda p, m, u: os.write(m, data))
            self.assertNotEqual(code, 0)
            self.assertNotIn(b'FIXTURE CONFIRMED', output)
            self.assertNotIn(b'spoof', output)

    def test_changed_observation(self):
        for mode in ('changed', 'unavailable'):
            code, output = self.run_tty(self.confirm, mode=mode)
            self.assertNotEqual(code, 0)
            self.assertIn(b'Observation changed', output)

    def test_input_arriving_during_reobservation_rejects(self):
        def contaminate(p, master, until):
            self.confirm(p, master, until)
            until(b'FIXTURE OBSERVING')
            os.write(master, b'next\nnext\n')
        code, output = self.run_tty(contaminate, mode='late-input')
        self.assertNotEqual(code, 0)
        self.assertNotIn(b'FIXTURE CONFIRMED', output)

    def test_lost_owned_console(self):
        master, slave = os.openpty()
        p = subprocess.Popen([str(self.driver)], stdin=slave, stdout=slave,
                             stderr=subprocess.PIPE)
        try:
            deadline = time.monotonic() + 3
            data = b''
            while b'Type next to review effects.' not in data and time.monotonic() < deadline:
                if select.select([master], [], [], .05)[0]:
                    data += os.read(master, 4096)
            self.assertIn(b'Type next to review effects.', data)
            os.close(master)
            master = None
            p.wait(timeout=3)
            self.assertNotEqual(p.returncode, 0)
            # The vanished terminal cannot be restored or qualify handoff.
        finally:
            if p.poll() is None:
                p.kill()
                p.wait()
            p.stderr.close()
            if master is not None:
                os.close(master)
            os.close(slave)

    def test_blocked_output_deadline_restores_input_terminal(self):
        master, slave = os.openpty()
        output_master, output_slave = os.openpty()
        before = termios.tcgetattr(slave)
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack('HHHH', 24, 80, 0, 0))
        flags = fcntl.fcntl(output_slave, fcntl.F_GETFL)
        fcntl.fcntl(output_slave, fcntl.F_SETFL, flags | os.O_NONBLOCK)
        filled = 0
        while True:
            try:
                filled += os.write(output_slave, b'X' * 4096)
            except BlockingIOError:
                break
            self.assertLess(filled, 1048576)
        started = time.monotonic()
        p = subprocess.Popen([str(self.driver)], stdin=slave, stdout=output_slave,
                             stderr=subprocess.PIPE)
        try:
            p.wait(timeout=7)
            self.assertNotEqual(p.returncode, 0)
            self.assertGreater(time.monotonic() - started, 4)
            self.assertLess(time.monotonic() - started, 7)
            self.assertEqual(termios.tcgetattr(slave), before)
            self.assertTrue(fcntl.fcntl(output_slave, fcntl.F_GETFL) & os.O_NONBLOCK)
        finally:
            if p.poll() is None:
                p.kill()
                p.wait()
            termios.tcsetattr(slave, termios.TCSANOW, before)
            p.stderr.close()
            for fd in (master, slave, output_master, output_slave):
                os.close(fd)

    def test_signals_resize_and_timeout(self):
        for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGHUP, signal.SIGWINCH):
            code, output = self.run_tty(lambda p, m, u: p.send_signal(sig))
            self.assertNotEqual(code, 0)
            self.assertNotIn(b'FIXTURE CONFIRMED', output)
        started = time.monotonic()
        code, output = self.run_tty(lambda p, m, u: None)
        self.assertNotEqual(code, 0)
        self.assertGreater(time.monotonic() - started, 9)

    def test_help_back_do_not_authorize(self):
        def navigate(p, m, until):
            os.write(m, b'h\n')
            until(b'Research only; stock prompts follow.')
            # Wait for the new page, not its earlier occurrence.
            time.sleep(.2)
            os.write(m, b'next\n')
            until(b'Type next to confirm.')
            os.write(m, b'b\n')
            time.sleep(.2)
            os.write(m, b'q\n')
        code, output = self.run_tty(navigate)
        self.assertNotEqual(code, 0)
        self.assertNotIn(b'FIXTURE CONFIRMED', output)
