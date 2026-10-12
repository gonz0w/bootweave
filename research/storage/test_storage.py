# SPDX-License-Identifier: BSD-3-Clause
"""Host checks of the pure validation model; no native device emulation."""
from pathlib import Path
import platform
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "research/storage"


class StorageTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.fixture = tempfile.TemporaryDirectory(prefix="storage-host-", dir=ROOT / ".tools")
        cls.validator = Path(cls.fixture.name) / "validator-test"
        cls.probe = Path(cls.fixture.name) / "probe"
        for main, output in (("validator-test.c", cls.validator), ("probe.c", cls.probe)):
            subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-pedantic",
                            str(SOURCE / main), str(SOURCE / "validate.c"), "-o", str(output)], check=True)

    @classmethod
    def tearDownClass(cls):
        cls.fixture.cleanup()

    def test_validation_boundaries(self):
        result = subprocess.run([str(self.validator)], capture_output=True, timeout=5)
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn(b"failures: 0", result.stdout)

    @unittest.skipIf(platform.system() == "OpenBSD", "Host stub only; native device tests need a confirmed fixture")
    def test_host_is_not_native_evidence(self):
        result = subprocess.run([str(self.probe)], capture_output=True, timeout=5)
        self.assertEqual(result.returncode, 77)
        self.assertIn(b"Requires OpenBSD 7.9 amd64.", result.stdout)
        self.assertLess(len(result.stdout), 1024)
        self.assertTrue(all(len(line) <= 40 for line in result.stdout.splitlines()))
        self.assertNotIn(b"\x1b", result.stdout)

    def test_arguments_do_not_select_devices(self):
        for argument in ("/dev/rsd0c", "--write", "\x1b[2J", "$(id)"):
            result = subprocess.run([str(self.probe), argument], capture_output=True, timeout=5)
            self.assertEqual(result.returncode, 2)
            self.assertEqual(result.stdout, b"")


if __name__ == "__main__":
    unittest.main()
