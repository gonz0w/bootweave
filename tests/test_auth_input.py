# SPDX-License-Identifier: BSD-3-Clause
"""Independent byte/descriptor oracles; no cryptography, target or native entry."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class AuthInputTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory(prefix="auth-input-", dir=ROOT / ".tools")
        cls.addClassCleanup(cls.tmp.cleanup)
        cls.directory = Path(cls.tmp.name)
        cls.binary = cls.directory / "capture"
        cls.object = cls.directory / "capture.o"
        flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic", "-I", str(ROOT / "src")]
        subprocess.run(flags + ["-c", str(ROOT / "src/auth_input.c"), "-o", str(cls.object)],
                       check=True, capture_output=True, timeout=15)
        injected = cls.directory / "injected.o"
        replacements = ["read", "close", "fstat", "fcntl", "lseek", "clock_gettime"]
        subprocess.run(flags + [f"-D{x}=bw_io_{x}" for x in replacements] +
                       ["-c", str(ROOT / "src/auth_input.c"), "-o", str(injected)],
                       check=True, capture_output=True, timeout=15)
        subprocess.run(flags + [str(injected), str(ROOT / "tests/auth_input_driver.c"), "-o", str(cls.binary)],
                       check=True, capture_output=True, timeout=15)

    def capture(self, data=(b"definition", b"payload\0bytes", b"signature"), behavior="ok", fault="none", paths=None):
        if paths is None:
            paths = [self.directory / str(i) for i in range(3)]
            for p, content in zip(paths, data):
                p.unlink(missing_ok=True)
                p.write_bytes(content)
        return subprocess.run([self.binary, *paths, behavior, fault], capture_output=True, timeout=3)

    def assert_captured(self, data, **kwargs):
        result = self.capture(data, **kwargs)
        expected = ("captured-only %d %d %d\n" % tuple(map(len, data))).encode() + b"".join(data)
        self.assertEqual((result.returncode, result.stdout, result.stderr), (0, expected, b""))

    def test_binary_exact_caps_short_reads_and_repeatability(self):
        for data in [(b"x", b"y", b"z"), (bytes(range(256))*64, b"p"*1048576, b"s"*16384)]:
            self.assert_captured(data)
        data = (b"a"*50, b"b\0c"*50, b"d"*50)
        self.assert_captured(data, fault="short")
        self.assert_captured(data, behavior="repeat")
        self.assert_captured(data, fault="changed-atime")

    def test_empty_and_one_over_each_cap_reject_before_reads(self):
        for i, cap in enumerate([16384, 1048576, 16384]):
            for content in [b"", b"x"*(cap+1)]:
                data = [b"a", b"b", b"c"]; data[i] = content
                result = self.capture(data, behavior="no-read")
                self.assertEqual((result.returncode, result.stdout, result.stderr), (2, b"", b""))

    def test_descriptor_roles_access_offset_and_missing(self):
        for behavior in ["rw", "offset", "missing", "no-output", "pipe", "socket", "null"]:
            result = self.capture(behavior=behavior)
            self.assertEqual((result.returncode, result.stdout, result.stderr), (2, b"", b""), behavior)

    def test_nonregular_inputs_reject_before_reads(self):
        paths = [self.directory / str(i) for i in range(3)]
        for i in range(3):
            for kind in ["device", "directory", "fifo"]:
                self.capture()
                special = self.directory / "special"
                if special.is_dir(): special.rmdir()
                else: special.unlink(missing_ok=True)
                if kind == "device": replacement = Path("/dev/null")
                elif kind == "directory": special.mkdir(); replacement = special
                else: os.mkfifo(special); replacement = special
                candidate = list(paths); candidate[i] = replacement
                result = self.capture(paths=candidate, behavior="no-read")
                self.assertEqual((result.returncode, result.stdout, result.stderr), (2, b"", b""))

    def test_input_and_output_aliases_preserve_bytes(self):
        for behavior in ["input-alias", "stdout-alias", "stderr-alias"]:
            result = self.capture(behavior=behavior)
            self.assertEqual((result.returncode, result.stdout, result.stderr), (2, b"", b""))
            self.assertEqual([p.read_bytes() for p in [self.directory/str(i) for i in range(3)]],
                             [b"definition", b"payload\0bytes", b"signature"])
        for a, b in [(0, 1), (0, 2), (1, 2)]:
            self.capture()
            paths = [self.directory/str(i) for i in range(3)]
            paths[b].unlink(); os.link(paths[a], paths[b])
            before = [p.read_bytes() for p in paths]
            result = self.capture(paths=paths, behavior="no-read")
            self.assertEqual((result.returncode, result.stdout, result.stderr), (2, b"", b""))
            self.assertEqual([p.read_bytes() for p in paths], before)

    def test_errors_growth_shrink_and_metadata_changes_clear_snapshots(self):
        for fault in ["read-error", "stat-error", "flags-error", "grow", "shrink", "changed-offset"] + \
                     ["changed-"+x for x in ["device", "inode", "mode", "uid", "gid", "link", "size", "mtime", "ctime"]]:
            result = self.capture((b"a"*4097, b"b", b"c"), fault=fault)
            self.assertEqual((result.returncode, result.stdout, result.stderr), (2, b"", b""), fault)

    def test_clock_failures_and_deadlines_close_every_input(self):
        for fault in ["clock-error", "clock-invalid", "clock-backwards", "deadline", "eintr-forever"]:
            result = self.capture(fault=fault)
            self.assertEqual((result.returncode, result.stdout, result.stderr), (2, b"", b""), fault)

    def test_close_uncertainty_and_unexpected_absence_never_succeed(self):
        for suffix in ["", "-3", "-4"]:
            for fault, code in [("close-eintr"+suffix, 3), ("close-ebadf"+suffix, 2)]:
                result = self.capture(fault=fault)
                self.assertEqual((result.returncode, result.stdout, result.stderr), (code, b"", b""))
        for fault, code in [("close-deadline", 2), ("close-clock-error", 2), ("close-eintr-deadline", 3)]:
            result = self.capture(fault=fault)
            self.assertEqual((result.returncode, result.stdout, result.stderr), (code, b"", b""))

    def test_component_budget_symbols_and_unchanged_cli_link(self):
        self.assertLessEqual(sum((ROOT / "src" / p).stat().st_size for p in ["auth_input.c", "auth_input.h"]), 8*1024)
        symbols = subprocess.run(["nm", "-u", self.object], check=True, capture_output=True, timeout=3).stdout
        for forbidden in [b"open", b"write", b"exec", b"fork", b"socket", b"malloc"]:
            self.assertNotIn(forbidden, symbols)
        makefile = (ROOT / "Makefile").read_text()
        self.assertIn("PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c", makefile)
        self.assertNotIn("auth_input", makefile)
