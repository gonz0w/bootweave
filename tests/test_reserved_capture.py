# SPDX-License-Identifier: BSD-3-Clause
"""Finite retained reservation/capture/material tests; no verifier or native entry."""
import base64
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest

import reserved_capture_vectors as vectors
import test_policy

ROOT = Path(__file__).resolve().parents[1]
CAUSES = {name: i for i, name in enumerate(("NONE", "ARGUMENT", "RECORD", "CANCEL",
                                           "DEADLINE", "IO", "OWNERSHIP", "CLOSE"))}
KEY = b"untrusted comment: inert reserved-capture key\n" + base64.b64encode(b"Ed" + bytes(40)) + b"\n"
SIGNATURE = b"untrusted comment: inert reserved-capture signature\n" + base64.b64encode(b"Ed" + bytes(72)) + b"\n"
INPUTS = {"definition": b"exact\0definition\n", "payload": b"retained\0payload\n", "signature": SIGNATURE,
          "key": KEY, "canary": b"fresh rebound input canary\n"}
FIELDS = set(("return state cause taken uncertain capture_close_errors capture_unknown_closes reads queries ticks "
    "reservation_stats closes no_write zero repeat_ok origin_flag prepare member_opens member_writes "
    "material_closes release_calls released terminal final_state final_cause final_uncertain ready_bytes "
    "capture_protocol final_origin_flag lengths digests").split())


def identity(path):
    info = path.stat()
    return {"device": info.st_dev, "inode": info.st_ino, "mode": info.st_mode,
            "uid": info.st_uid, "gid": info.st_gid, "links": info.st_nlink,
            "size": info.st_size, "mtime_ns": info.st_mtime_ns, "ctime_ns": info.st_ctime_ns,
            "sha256": hashlib.sha256(path.read_bytes()).hexdigest()}


class ReservedCaptureTests(unittest.TestCase):
    # Existing owned launcher retains every stream, wait and publication result.
    execute = classmethod(test_policy.PolicyTests.execute.__func__)

    @classmethod
    def setUpClass(cls):
        if sys.platform != "darwin":
            raise RuntimeError("Mac portable reserved capture fixture only; no native dispatch")
        cls.directory = Path(tempfile.mkdtemp(prefix="reserved-capture-build-", dir=ROOT / ".tools"))
        cls.ledger = cls.directory / "observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1",
                   "ASAN_OPTIONS": "detect_leaks=0:abort_on_error=1"}
        cls.calls = cls.builds = cls.invocations = 0
        cls.cache, cls.quarantine = {}, []
        cls.flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
                     "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g", "-I", str(ROOT / "src")]
        sources = sorted([*ROOT.joinpath("src").glob("*.[ch]"),
            *(ROOT / "tests" / n for n in ("reserved_capture_driver.c", "reserved_capture_vectors.py",
                                            "test_reserved_capture.py", "test_policy.py"))])
        (cls.directory / "source-manifest.json").write_text(json.dumps({str(p.relative_to(ROOT)):
            {"bytes": p.stat().st_size, "sha256": hashlib.sha256(p.read_bytes()).hexdigest()}
            for p in sources}, sort_keys=True) + "\n")
        cls.plain, injected, material = (cls.directory / name for name in ("capture-production.o", "capture.o", "material.o"))
        capture_defines = ["-Dclock_gettime=rc_capture_clock", "-Dfstat=rc_capture_fstat",
            "-Dfcntl=rc_capture_fcntl", "-Dlseek=rc_capture_lseek", "-Dread=rc_capture_read", "-Dclose=rc_capture_close"]
        material_defines = ["-Dclock_gettime=rc_material_clock", "-Dmkdtemp=rc_mkdtemp",
            "-Dopenat=rc_material_openat", "-Dwrite=rc_material_write", "-Dclose=rc_material_close"]
        for target, source, defines in ((cls.plain, "auth_input.c", []),
                (injected, "auth_input.c", capture_defines), (material, "auth_material.c", material_defines)):
            cls.execute(cls.flags + defines + ["-c", str(ROOT / "src" / source), "-o", str(target)], compile_job=True)
        cls.binary = cls.directory / "reserved-capture"
        cls.execute(cls.flags + [str(injected), str(material), str(ROOT / "tests/reserved_capture_driver.c"),
                                "-o", str(cls.binary)], compile_job=True)

    def run_case(self, case):
        type(self).invocations += 1
        directory = self.directory / f"material-case-{self.invocations:03d}"
        directory.mkdir(mode=0o700)
        for name, data in INPUTS.items():
            (directory / name).write_bytes(data)
        before = {name: identity(directory / name) for name in INPUTS}
        command = [str(self.binary), str(directory), case["mode"], str(case["index"]), case["phase"], case["action"]]
        data = self.execute(command)
        self.assertTrue(data.startswith(b"RC_RESULT "), case["name"])
        self.assertEqual(data.count(b"\n"), 1, case["name"])
        actual = json.loads(data[len(b"RC_RESULT "):])
        self.assertEqual(set(actual), FIELDS)
        self.assertTrue(all(type(value) is int for key, value in actual.items()
                            if key not in ("closes", "lengths", "digests")))
        self.assertEqual(len(actual["closes"]), 3)
        self.assertEqual(len(actual["lengths"]), 3)
        self.assertEqual(len(actual["digests"]), 3)
        self.assertEqual(before, {name: identity(directory / name) for name in INPUTS}, case["name"])
        self.assertEqual(sorted(p.name for p in directory.iterdir()), sorted(INPUTS), case["name"])
        return actual

    def check(self, case):
        actual, expected = self.run_case(case), case["expected"]
        self.assertEqual(actual["return"], expected["return"], case["name"])
        self.assertEqual(actual["closes"], list(expected["input_closes"]), case["name"])
        self.assertEqual(actual["repeat_ok"], 1, case["name"])
        self.assertEqual(actual["origin_flag"], 1, case["name"])
        self.assertEqual(actual["final_origin_flag"], 1, case["name"])
        self.assertEqual(actual["capture_protocol"], 1, case["name"])
        self.assertEqual(actual["terminal"], 1, case["name"])
        if expected["return"] == -2:
            self.assertEqual(actual["no_write"], 1, case["name"])
            for key in ("reads", "queries", "ticks", "reservation_stats", "member_opens", "member_writes"):
                self.assertEqual(actual[key], 0, (case["name"], key))
            self.assertEqual(actual["prepare"], -9)
            self.assertEqual(actual["release_calls"], 1)
            self.assertEqual(actual["released"], 1)
            self.assertEqual(actual["final_state"], 4)
            self.assertEqual(actual["final_cause"], 0)
            self.assertEqual(actual["final_uncertain"], 0)
            self.assertEqual(actual["material_closes"], 1)
            return
        self.assertEqual(actual["state"], 1, case["name"])
        self.assertEqual(actual["taken"], 1, case["name"])
        self.assertEqual(actual["cause"], CAUSES[expected["cause"]], case["name"])
        self.assertEqual(actual["final_cause"], CAUSES[expected["cause"]], case["name"])
        self.assertEqual(actual["uncertain"], expected["uncertain"])
        self.assertEqual(actual["final_uncertain"], expected["uncertain"])
        self.assertEqual(actual["capture_close_errors"], expected["capture_close_errors"])
        self.assertEqual(actual["capture_unknown_closes"], expected["capture_unknown_closes"])
        if expected["reads"] == "none":
            self.assertEqual(actual["reads"], 0, case["name"])
        else:
            self.assertGreater(actual["reads"], 0, case["name"])
        self.assertLessEqual(actual["reads"], 64)
        self.assertLessEqual(actual["ticks"], 256)
        self.assertLessEqual(actual["queries"], 32)
        if case["phase"] == "preflight":
            self.assertIn(actual["reservation_stats"], (0, 1))
            self.assertEqual(actual["queries"], 0)
        else:
            self.assertEqual(actual["reservation_stats"], 1)
        if expected["snapshot"] == "exact":
            self.assertEqual(actual["lengths"], [len(INPUTS[name]) for name in vectors.ROLE_NAMES])
            self.assertEqual(actual["digests"], [hashlib.sha256(INPUTS[name]).hexdigest() for name in vectors.ROLE_NAMES])
            self.assertEqual(actual["zero"], 0)
        else:
            self.assertEqual(actual["zero"], 1)
            self.assertEqual(actual["lengths"], [0, 0, 0])
        if case["action"] == "release":
            self.assertEqual(actual["prepare"], -9)
            self.assertEqual(actual["release_calls"], 1)
            self.assertEqual(actual["released"], 1 if expected["return"] == 1 else -1 if expected["uncertain"] else 0)
            self.assertEqual(actual["member_opens"], 0)
            self.assertEqual(actual["member_writes"], 0)
            self.assertEqual(actual["material_closes"], 1)
        else:
            self.assertEqual(actual["prepare"], expected["prepare_result"], case["name"])
            if expected["prepare_members"]:
                self.assertEqual(actual["member_opens"], 6)
                self.assertEqual(actual["member_writes"], 3)
                self.assertEqual(actual["ready_bytes"], 1)
                self.assertEqual(actual["release_calls"], 1)
                self.assertEqual(actual["released"], 1)
                self.assertEqual(actual["material_closes"], 7)
            else:
                self.assertEqual(actual["member_opens"], 0)
                self.assertEqual(actual["member_writes"], 0)
                self.assertEqual(actual["ready_bytes"], 0)
                self.assertEqual(actual["release_calls"], 0)
                self.assertEqual(actual["material_closes"], 1)
        self.assertEqual(actual["final_state"], 4 if expected["return"] == 1 else 6 if expected["uncertain"] else 5)

    def test_independent_reserved_capture_matrix(self):
        requests = vectors.cases()
        self.assertEqual(len(requests), vectors.CASE_COUNT)
        self.assertLessEqual(len(requests), 160)
        for case in requests:
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_closed_symbols_source_accounting_and_unchanged_planner_link(self):
        undefined = self.execute(["nm", "-u", str(self.plain)]).decode()
        names = {line.split()[-1] for line in undefined.splitlines()}
        self.assertLessEqual({name for name in names if "asan" not in name and "ubsan" not in name}, {
            "_clock_gettime", "_fstat", "_fcntl", "_lseek", "_read", "_close", "_memset", "_bzero",
            "_memcpy", "___error", "___stack_chk_fail", "___stack_chk_guard"})
        public = self.execute(["nm", "-gU", str(self.plain)]).decode()
        self.assertEqual({line.split()[-1] for line in public.splitlines() if "asan" not in line and "ubsan" not in line},
                         {"_bw_auth_capture", "_bw_auth_capture_reserved"})
        makefile = (ROOT / "Makefile").read_text()
        self.assertEqual(next(line for line in makefile.splitlines() if line.startswith("PLAN_SOURCES =")),
                         "PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c")
        self.assertNotIn("auth_input", makefile)
        self.assertLessEqual(sum((ROOT / "src" / name).stat().st_size for name in
            ("auth_input.c", "auth_input.h")), 8 * 1024)
        self.assertLessEqual(sum(path.stat().st_size for path in (ROOT / "src").rglob("*")
                                 if path.suffix in (".c", ".h")), 96 * 1024)
