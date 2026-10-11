# SPDX-License-Identifier: BSD-3-Clause
"""Host research checks only; every terminal is an exclusively owned PTY."""
import fcntl
import os
from pathlib import Path
import select
import signal
import struct
import subprocess
import tempfile
import termios
import time
import unittest

ROOT = Path(__file__).resolve().parents[2]


class PlainUITests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.fixture = tempfile.TemporaryDirectory(prefix="plain-ui-", dir=ROOT / ".tools")
        cls.binary = Path(cls.fixture.name) / "plain-ui-host"
        subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-pedantic",
                        str(ROOT / "research/ui/plain-ui.c"), "-o", str(cls.binary)], check=True)

    @classmethod
    def tearDownClass(cls):
        cls.fixture.cleanup()

    def run_input(self, data):
        p = subprocess.run([str(self.binary)], input=data, capture_output=True, timeout=15)
        self.assertLess(len(p.stdout), 65537)
        self.assertNotIn(b"\x1b", p.stdout)
        return p

    def test_valid_help_back_and_delimiters(self):
        for delimiter in (b"\n", b"\r", b"\r\n"):
            p = self.run_input(delimiter.join([b"Sample", b"h", b"alpha", b"b", b"beta", b"complete", b""]))
            self.assertEqual(p.returncode, 0)
            self.assertIn(b"Synthetic choices only", p.stdout)
            self.assertIn(b"Review label:\nSample\nReview choice:\nbeta", p.stdout)
            self.assertEqual(p.stdout.count(b"SIMULATION COMPLETE"), 1)

    def test_back_preserves_validated_values(self):
        p = self.run_input(b"Saved\nalpha\nb\nb\nh\nkeep\nkeep\ncomplete\n")
        self.assertEqual(p.returncode, 0)
        self.assertIn(b"Saved label:\nSaved", p.stdout)
        self.assertIn(b"Saved choice:\nalpha", p.stdout)
        self.assertIn(b"Review label:\nSaved\nReview choice:\nalpha", p.stdout)

    def test_validation_and_label_boundaries(self):
        for invalid in (b"", b"!", b"A" * 65):
            p = self.run_input(invalid + b"\n" + b"B" * 64 + b"\nwrong\nbeta\ncomplete\n")
            self.assertEqual(p.returncode, 0)
            self.assertGreaterEqual(p.stdout.count(b"Input rejected"), 2)
            self.assertIn(b"B" * 40 + b"\n" + b"B" * 24, p.stdout)

    def test_control_and_overflow_lines_rejected_whole(self):
        for bad in (b"\x1b]0;spoof", b"abc\0", b"\x01", b"\x80", b"\xff", b"A" * 1024, b"A" * 1025):
            p = self.run_input(bad + b"\nValid\nalpha\ncomplete\n")
            self.assertEqual(p.returncode, 0)
            self.assertIn(b"Input rejected", p.stdout)
            self.assertNotIn(b"spoof", p.stdout)
            self.assertIn(b"Review label:\nValid", p.stdout)

    def test_eof_cancel_and_no_enter_default(self):
        for data in (b"", b"Valid", b"q\n", b"Valid\nalpha\n\n", b"A" * 70000):
            p = self.run_input(data)
            self.assertNotEqual(p.returncode, 0)
            self.assertNotIn(b"SIMULATION COMPLETE", p.stdout)

    def test_idle_timeout(self):
        p = subprocess.Popen([str(self.binary)], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        try:
            started = time.monotonic()
            p.wait(timeout=12)
            self.assertGreater(time.monotonic() - started, 9)
            self.assertNotEqual(p.returncode, 0)
        finally:
            if p.poll() is None:
                p.kill()
                p.wait()
            p.stdin.close()
            p.stdout.close()

    def read_until(self, p, marker):
        data = b""
        deadline = time.monotonic() + 3
        while marker not in data and time.monotonic() < deadline:
            if select.select([p.stdout], [], [], .1)[0]:
                chunk = os.read(p.stdout.fileno(), 4096)
                if not chunk:
                    break
                data += chunk
        self.assertIn(marker, data)
        return data

    def tty_case(self, rows, cols, action):
        master, slave = os.openpty()
        original = termios.tcgetattr(slave)
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", rows, cols, 0, 0))
        p = subprocess.Popen([str(self.binary)], stdin=slave, stdout=subprocess.PIPE, bufsize=0)
        try:
            output = self.read_until(p, b"Label (ASCII")
            self.assertFalse(termios.tcgetattr(slave)[3] & termios.ECHO)
            output += action(p, master, slave)
            p.wait(timeout=3)
            output += p.stdout.read()
            if p.returncode == -signal.SIGKILL:
                self.assertNotEqual(termios.tcgetattr(slave), original)
                # Forced death cannot restore: owned supervisor does so explicitly.
                termios.tcsetattr(slave, termios.TCSANOW, original)
                termios.tcflush(slave, termios.TCIFLUSH)
            self.assertEqual(termios.tcgetattr(slave), original)
            self.assertNotIn(b"\x1b", output)
            return p.returncode, output
        finally:
            if p.poll() is None:
                p.kill()
                p.wait()
            termios.tcsetattr(slave, termios.TCSANOW, original)
            termios.tcflush(slave, termios.TCIFLUSH)
            p.stdout.close()
            os.close(master)
            os.close(slave)

    def test_tty_sizes_and_normal_restoration(self):
        def finish(p, master, slave):
            os.write(master, b"A" * 64 + b"\nalpha\ncomplete\n")
            return b""
        for rows, cols in ((24, 80), (12, 40), (0, 0)):
            code, output = self.tty_case(rows, cols, finish)
            self.assertEqual(code, 0)
            if cols:
                self.assertTrue(all(len(line) <= cols for line in output.splitlines()))
            else:
                self.assertIn(b"Dimensions unknown", output)

    def test_signals_resize_and_forced_cleanup(self):
        for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGHUP, signal.SIGKILL):
            def stop(p, master, slave, sig=sig):
                p.send_signal(sig)
                return b""
            code, output = self.tty_case(24, 80, stop)
            self.assertNotEqual(code, 0)
            self.assertNotIn(b"SIMULATION COMPLETE", output)
        def resize(p, master, slave):
            os.write(master, b"Valid\nalpha\n")
            output = self.read_until(p, b"Type complete")
            fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 12, 40, 0, 0))
            p.send_signal(signal.SIGWINCH)
            return output
        code, output = self.tty_case(24, 80, resize)
        self.assertNotEqual(code, 0)
        self.assertNotIn(b"SIMULATION COMPLETE", output)

    def test_broken_output_and_output_budget(self):
        read_fd, write_fd = os.pipe()
        os.close(read_fd)
        p = subprocess.Popen([str(self.binary)], stdin=subprocess.PIPE, stdout=write_fd)
        os.close(write_fd)
        try:
            p.communicate(b"Valid\nalpha\ncomplete\n", timeout=3)
            self.assertNotEqual(p.returncode, 0)
        finally:
            if p.poll() is None:
                p.kill()
                p.wait()
        p = self.run_input(b"h\n" * 2000)
        self.assertNotEqual(p.returncode, 0)
        self.assertNotIn(b"SIMULATION COMPLETE", p.stdout)

    def test_tty_control_echo_and_timeout_restoration(self):
        def reject(p, master, slave):
            os.write(master, b"\x1b]0;spoof\n")
            output = self.read_until(p, b"Input rejected.")
            self.assertFalse(select.select([master], [], [], .1)[0])
            os.write(master, b"q\n")
            return output
        code, output = self.tty_case(24, 80, reject)
        self.assertNotEqual(code, 0)
        self.assertNotIn(b"spoof", output)
        def idle(p, master, slave):
            p.wait(timeout=12)
            return b""
        code, _ = self.tty_case(24, 80, idle)
        self.assertNotEqual(code, 0)

    def test_lost_owned_console(self):
        master, slave = os.openpty()
        original = termios.tcgetattr(slave)
        p = subprocess.Popen([str(self.binary)], stdin=slave, stdout=subprocess.PIPE, bufsize=0)
        try:
            self.read_until(p, b"Label (ASCII")
            os.close(master)
            master = -1
            p.wait(timeout=3)
            self.assertNotEqual(p.returncode, 0)
            self.assertNotIn(b"SIMULATION COMPLETE", p.stdout.read())
            # A hung-up PTY may no longer accept restoration. Always discard it.
            try:
                termios.tcsetattr(slave, termios.TCSANOW, original)
                termios.tcflush(slave, termios.TCIFLUSH)
            except termios.error:
                self.assertEqual(p.returncode, 3)
        finally:
            if p.poll() is None:
                p.kill()
                p.wait()
            p.stdout.close()
            if master >= 0:
                os.close(master)
            os.close(slave)

    def test_blocked_output_deadline(self):
        p = subprocess.Popen([str(self.binary)], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        try:
            p.stdin.write(b"h\n" * 1000)
            p.stdin.close()
            p.wait(timeout=7)
            self.assertNotEqual(p.returncode, 0)
        finally:
            if p.poll() is None:
                p.kill()
                p.wait()
            p.stdout.close()

    def test_source_and_resource_ceiling(self):
        self.assertLess((ROOT / "research/ui/plain-ui.c").stat().st_size, 32769)
        maximum = 0
        for data in (b"", b"Valid\nalpha\ncomplete\n", b"A" * 1025 + b"\nq\n", b"h\n" * 1000):
            read_fd, write_fd = os.pipe()
            try:
                os.write(write_fd, data)
                os.close(write_fd)
                write_fd = -1
                pid = os.posix_spawn(str(self.binary), [str(self.binary)], {}, file_actions=[
                    (os.POSIX_SPAWN_DUP2, read_fd, 0),
                    (os.POSIX_SPAWN_CLOSE, read_fd),
                    (os.POSIX_SPAWN_OPEN, 1, os.devnull, os.O_WRONLY, 0)])
                _, _, usage = os.wait4(pid, 0)
                rss = usage.ru_maxrss if os.uname().sysname == "Darwin" else usage.ru_maxrss * 1024
                self.assertLess(rss, 32 * 1024 * 1024)
                maximum = max(maximum, rss)
            finally:
                os.close(read_fd)
                if write_fd >= 0:
                    os.close(write_fd)
        print("Measured simulation peak RSS bytes:", maximum)


if __name__ == "__main__":
    unittest.main()
