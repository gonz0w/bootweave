# SPDX-License-Identifier: BSD-3-Clause
"""Host checks of trace-only resource substitution and exact source sealing."""

import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

import native_trace_prepare

ROOT = Path(__file__).resolve().parents[1]


class TracePreparationTests(unittest.TestCase):
    def test_real_limits_and_rejected_substitutions(self):
        with tempfile.TemporaryDirectory(prefix="planner-trace-limits-", dir=ROOT / ".tools") as owned:
            binary = Path(owned) / "trace-limits"
            subprocess.run(["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
                            "-DBW_TRACE_POLICY", str(ROOT / "tests/native_trace.c"),
                            str(ROOT / "tests/trace_limits.c"), "-o", str(binary)],
                           check=True, capture_output=True)
            result = subprocess.run([binary], capture_output=True, timeout=3)
            self.assertEqual(result.returncode, 0, result.stderr.decode())
            self.assertEqual(result.stdout,
                             b"Trace-only limits checked; production FSIZE remains separate.\n")

    def test_sealed_trace_sources_keep_production_unchanged(self):
        directory = native_trace_prepare.prepare()
        try:
            self.assertEqual(len(list(directory.iterdir())), 12)
            self.assertEqual((directory / "SHA256").read_bytes(),
                             (ROOT / "docs/testing/project-plan-trace-bindings.txt").read_bytes())
            for source in ["main.c", "plan.c", "plan.h", "plan_io.c", "native.c"]:
                self.assertEqual((directory / source).read_bytes(), (ROOT / "src" / source).read_bytes())
            for entry in (directory / "SHA256").read_text().splitlines():
                name, digest = entry.removeprefix("SHA256 (").split(") = ")
                self.assertEqual(hashlib.sha256((directory / name).read_bytes()).hexdigest(), digest)
                self.assertEqual((directory / name).stat().st_mode & 0o777, 0o600)
            if os.uname().sysname != "OpenBSD":
                result = subprocess.run(["sh", directory / "native_trace_build.sh"], capture_output=True)
                self.assertEqual(result.returncode, 77)
            build = (ROOT / "Makefile").read_text().split("PLAN_SOURCES = ", 1)[1].splitlines()[0]
            self.assertNotIn("trace", build)
        finally:
            shutil.rmtree(directory)


if __name__ == "__main__":
    unittest.main()
