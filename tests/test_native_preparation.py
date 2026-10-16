# SPDX-License-Identifier: BSD-3-Clause
"""Exercise native qualification tooling on host-owned temporary fixtures only."""

import hashlib
import json
import os
import pty
import select
from pathlib import Path
import shutil
import subprocess
import tempfile
import termios
import unittest

import native_cases
import native_plan
from plan_cases import BASE, encode, expected

ROOT = Path(__file__).resolve().parents[1]


class NativePreparationTests(unittest.TestCase):
    def test_argument_preview_cannot_change_inventory(self):
        for phase in native_plan.PHASES:
            arguments = native_plan.arguments(phase)
            self.assertIn("-S", arguments)
            self.assertIn("tcg,thread=single", arguments)
            self.assertEqual(arguments[arguments.index("-nic") + 1], "none")
            self.assertIn(f"file={native_plan.TARGET},if=none,id=target,format=raw,readonly=off", arguments)
            disks = [arguments[i + 1] for i, value in enumerate(arguments) if value == "-drive"]
            self.assertEqual(sum("readonly=off" in disk for disk in disks), 1)
            self.assertEqual(len(disks), 3 if phase == "provision" else 2)
            self.assertFalse(any("tcp:" in value or "hostfwd" in value or "snapshot" in value
                                 or "backing" in value or "vfio" in value for value in arguments))
        with self.assertRaises(ValueError):
            native_plan.arguments("unknown")

    def test_frozen_cases_and_portable_supervisor(self):
        first = native_cases.artifacts()
        self.assertEqual(first, native_cases.artifacts())
        manifest = json.loads(first["cases.json"])
        self.assertEqual([item["index"] for item in manifest], list(range(len(manifest))))
        cases = native_cases.corpus()
        self.assertEqual(len(cases), 1401)
        self.assertEqual(sum(case[2] == 0 for case in cases), 688)
        self.assertEqual(hashlib.sha256(first["cases.json"]).hexdigest(),
                         "4bdfa1b1e132da11027749d078b0e424b630d6bd9cb3f1e46eec1ec0804721ae")
        self.assertEqual(hashlib.sha256(first["native_cases.h"]).hexdigest(),
                         "e4c6f6d660ce87249c65fadbbb9218d15ea71c018c15e971d29b859e85d8c581")
        for item, (_, data, status, output) in zip(manifest, cases, strict=True):
            self.assertEqual(item["input_sha256"], hashlib.sha256(data).hexdigest())
            self.assertEqual(item["output_sha256"], hashlib.sha256(output).hexdigest())
            self.assertEqual(item["exit"], status)
        with tempfile.TemporaryDirectory(prefix="planner-native-host-", dir=ROOT / ".tools") as owned:
            directory = Path(owned)
            for name, data in first.items():
                (directory / name).write_bytes(data)
            flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic"]
            common = ["-I", str(ROOT / "src"), str(ROOT / "src/plan.c"),
                      str(ROOT / "src/plan_io.c"), str(ROOT / "src/native.c")]
            subprocess.run(flags + common + [str(ROOT / "tests/plan_driver.c"), "-o",
                                            str(directory / "bootweave")],
                           check=True, capture_output=True)
            shutil.copyfile(directory / "bootweave", directory / "bootweave-static")
            (directory / "bootweave-static").chmod(0o700)
            subprocess.run(flags + ["-I", str(directory), str(ROOT / "tests/native_check.c"),
                                    "-lutil", "-o", str(directory / "native-check")],
                           check=True, capture_output=True)
            (directory / "work").mkdir()
            (directory / "work/input").write_bytes(encode(BASE))
            (directory / "work/canary").write_bytes(b"planner outside canary\n")
            (directory / "outside-canary").write_bytes(b"planner outside canary\n")
            for mode, expected_count in [("--corpus", len(cases) * 2), ("--tracecase", 2),
                                         ("--edges", 30)]:
                result = subprocess.run([directory / "native-check", mode], cwd=directory,
                                        capture_output=True, timeout=120)
                self.assertEqual(result.returncode, 0, result.stderr.decode())
                self.assertIn(f"{expected_count} checks;".encode(), result.stdout)
            for width in [80, 40]:
                for variant in ["dynamic", "static"]:
                    master, slave = pty.openpty()
                    try:
                        attributes = termios.tcgetattr(slave)
                        window = termios.tcgetwinsize(slave)
                        process = subprocess.Popen(
                            [directory / "native-check", f"--console{width}-{variant}"],
                            cwd=directory, stdin=subprocess.DEVNULL, stdout=slave,
                            stderr=subprocess.PIPE)
                        _, errors = process.communicate(timeout=3)
                        self.assertEqual(process.returncode, 0, errors.decode())
                        self.assertEqual(termios.tcgetattr(slave), attributes)
                        self.assertEqual(termios.tcgetwinsize(slave), window)
                        captured = bytearray()
                        while select.select([master], [], [], 0.1)[0]:
                            captured.extend(os.read(master, 4096))
                        self.assertIn(expected(BASE).replace(b"\n", b"\r\n"), captured)
                        self.assertIn(b"TTY restored.", captured)
                    finally:
                        os.close(master)
                        os.close(slave)
            subprocess.run(flags + common + [str(ROOT / "tests/native_probe.c"),
                                            "-o", str(directory / "probe")],
                           check=True, capture_output=True)
            if os.uname().sysname != "OpenBSD":
                result = subprocess.run([directory / "probe", "open"], capture_output=True)
                self.assertEqual(result.returncode, 77)
                subprocess.run(flags + [str(ROOT / "tests/native_compile.c"), "-o", str(directory / "compile")],
                               check=True, capture_output=True)
                result = subprocess.run([directory / "compile"], capture_output=True)
                self.assertEqual(result.returncode, 77)
            # A deliberately changed oracle must fail, rather than self-certifying.
            (directory / "work/canary").write_bytes(b"changed")
            result = subprocess.run([directory / "native-check", "--tracecase"], cwd=directory,
                                    capture_output=True, timeout=2)
            self.assertEqual(result.returncode, 1)
            self.assertIn(b"fixture content or named identity changed", result.stderr)
            (directory / "work/canary").write_bytes(b"planner outside canary\n")
            # The supervisor must reject a successful process with the wrong text.
            fake = directory / "fake.c"
            fake.write_text("int main(void) { return 0; }\n")
            subprocess.run(flags + [str(fake), "-o", str(directory / "bootweave")],
                           check=True, capture_output=True)
            result = subprocess.run([directory / "native-check", "--tracecase"], cwd=directory,
                                    capture_output=True, timeout=2)
            self.assertEqual(result.returncode, 1)
            self.assertIn(b"complete output oracle", result.stderr)
            # A stalled executable must be killed and reaped by the independent guard.
            fake.write_text("#include <unistd.h>\nint main(void) { for (;;) pause(); }\n")
            subprocess.run(flags + [str(fake), "-o", str(directory / "bootweave")],
                           check=True, capture_output=True)
            result = subprocess.run([directory / "native-check", "--tracecase"], cwd=directory,
                                    capture_output=True, timeout=10)
            self.assertEqual(result.returncode, 1)
            self.assertIn(b"child deadline", result.stderr)

    def test_sealed_source_preparation(self):
        directory = native_cases.prepare()
        try:
            self.assertEqual(directory.stat().st_mode & 0o777, 0o700)
            self.assertEqual((directory / "SHA256").read_bytes(),
                             (ROOT / "docs/testing/project-plan-native-bindings.txt").read_bytes())
            entries = (directory / "SHA256").read_text().splitlines()
            for entry in entries:
                name, digest = entry.removeprefix("SHA256 (").split(") = ")
                self.assertEqual(hashlib.sha256((directory / name).read_bytes()).hexdigest(), digest)
                self.assertEqual((directory / name).stat().st_mode & 0o777, 0o600)
            self.assertFalse(any(p.suffix in {".iso", ".raw", ".qcow2"} for p in directory.iterdir()))
            if os.uname().sysname != "OpenBSD":
                for script in ["native_build.sh", "native_run.sh"]:
                    result = subprocess.run(["sh", directory / script], capture_output=True)
                    self.assertEqual(result.returncode, 77)
        finally:
            shutil.rmtree(directory)


if __name__ == "__main__":
    unittest.main()
