# SPDX-License-Identifier: BSD-3-Clause
"""Finite source-only boundary/CLI models; no native/security activation.

The literal oracle precedes candidate inspection. Policy's unchanged owned-child
launcher retains every actual PID, stream and consuming wait. Boundary storage,
identity/stats/security and the private original-clock bridge are pure models.
CLI uses only newly owned pipe canaries and read-only inherited-FD observations.
"""
import errno
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest

import native_entry_vectors as vectors
import test_policy

ROOT = Path(__file__).resolve().parents[1]
BOUNDARY_FIELDS = {"family", "return", "material_unchanged", "owner_unchanged", "normal_unveil", "normal_pledge",
                   "lives", "work", "security", "trace", "outcomes"}
CLI_FIELDS = {"family", "status", "legacy_calls", "start_calls", "entry_calls", "availability_calls", "argv_identity",
              "canaries_unchanged", "writes", "stdout_hex", "stderr_hex", "fixture_pipe_bytes", "fixture_private_fds",
              "inherited_setup_mismatch_mask", "inherited_call_mismatch_mask", "private_setup_mismatch_mask",
              "private_call_mismatch_mask", "fd_before", "fd_ready", "fd_after", "canary_poll", "canary_read",
              "fixture_close_dispositions"}
CLI_STRUCTURED_FIELDS = {"fd_before", "fd_ready", "fd_after", "canary_poll", "canary_read", "fixture_close_dispositions"}
FD_FIELDS = {"fd", "fd_flags", "fd_errno", "status_flags", "status_errno", "stat_ok", "stat_result", "stat_errno", "device", "inode", "mode"}
FD_COMPARE_FIELDS = ("fd_flags", "status_flags", "stat_ok", "device", "inode", "mode")


def clean_symbols(data):
    return {row.split()[-1] for row in data.decode().splitlines() if "asan" not in row and "ubsan" not in row}


class NativeEntryTests(unittest.TestCase):
    execute = classmethod(test_policy.PolicyTests.execute.__func__)

    @classmethod
    def setUpClass(cls):
        if sys.platform != "darwin":
            raise RuntimeError("Mac source-only boundary/CLI fixture; no native dispatch")
        cls.directory = Path(tempfile.mkdtemp(prefix="native-entry-build-", dir=ROOT / ".tools"))
        cls.ledger = cls.directory / "observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1",
                   "ASAN_OPTIONS": "detect_leaks=0:abort_on_error=1"}
        cls.calls = cls.builds = cls.invocations = 0
        cls.cache, cls.quarantine = {}, []
        cls.flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
                     "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g", "-I", str(ROOT / "src")]
        sources = sorted([*ROOT.joinpath("src").rglob("*.[ch]"),
            *(ROOT / "tests" / n for n in ("native_entry_driver.c", "native_entry_vectors.py", "test_native_entry.py", "test_policy.py"))])
        (cls.directory / "source-manifest.json").write_text(json.dumps({str(p.relative_to(ROOT)):
            {"bytes": p.stat().st_size, "sha256": hashlib.sha256(p.read_bytes()).hexdigest()}
            for p in sources}, sort_keys=True) + "\n")
        driver = str(ROOT / "tests/native_entry_driver.c")
        jobs = (("boundary-production", "src/entry_native.c", []),
            ("boundary", "src/entry_native.c", ["-DNE_BOUNDARY_PREAMBLE", "-include", driver]),
            ("normal-primitives", "src/entry_openbsd.c", ["-Dbw_en_identity=ne_normal_identity",
                "-Dbw_en_unveil=ne_normal_unveil", "-Dbw_en_pledge=ne_normal_pledge"]),
            ("normal-entry", "src/entry_platform.c", []),
            ("cli-production", "src/entry_cli.c", []),
            ("cli", "src/entry_cli.c", ["-Dbw_entry_available=ne_available", "-Dbw_entry_output=ne_closed_entry",
                "-Dbw_legacy_cli=ne_legacy_call"]),
            ("legacy-main", "src/main.c", ["-Dmain=ne_actual_main", "-Dbw_start=ne_start", "-Dbw_write=ne_write",
                "-Dbw_confine=ne_confine", "-Dbw_run=ne_run"]),
            ("driver", "tests/native_entry_driver.c", []))
        cls.objects = {}
        for name, source, defines in jobs:
            target = cls.directory / (name + ".o")
            cls.execute(cls.flags + defines + ["-c", str(ROOT / source), "-o", str(target)], compile_job=True)
            cls.objects[name] = target
        cls.binary = cls.directory / "native-entry-model"
        linked = [cls.objects[n] for n in ("boundary", "normal-primitives", "normal-entry", "cli", "legacy-main", "driver")]
        cls.execute(cls.flags + list(map(str, linked)) + ["-o", str(cls.binary)], compile_job=True)

    def run_case(self, family, case):
        type(self).invocations += 1
        # These are retained model/argument receipts, never actual material roots.
        directory = self.directory / f"model-case-{self.invocations:03d}"
        directory.mkdir(mode=0o700)
        stimulus = case["stimulus"]
        (directory / "case.json").write_text(json.dumps(case, sort_keys=True) + "\n")
        if family == "boundary":
            args = [stimulus["mode"], str(stimulus["index"]), stimulus["field"], str(stimulus["value"]), "finite"]
        else:
            args = [stimulus["mode"], str(stimulus["availability"]), str(stimulus["entry_result"]),
                    str(stimulus["start_ok"]), str(stimulus["write_ok"]), *stimulus["argv"]]
        data = self.execute([str(self.binary), str(directory), family, *args])
        self.assertTrue(data.startswith(b"NE_RESULT "), case["name"])
        self.assertEqual(data.count(b"\n"), 1, case["name"])
        actual = json.loads(data[len(b"NE_RESULT "):])
        (directory / "result.json").write_text(json.dumps(actual, sort_keys=True) + "\n")
        self.assertEqual(actual["family"], family)
        self.assertEqual(set(actual), BOUNDARY_FIELDS if family == "boundary" else CLI_FIELDS)
        self.assertTrue(all(type(v) is int for n, v in actual.items() if n not in
            {"family", "trace", "outcomes", "stdout_hex", "stderr_hex"} | CLI_STRUCTURED_FIELDS))
        self.assertEqual(sorted(p.name for p in directory.iterdir()), ["case.json", "result.json"])
        return actual

    def check_boundary(self, case):
        actual, expected, stimulus = self.run_case("boundary", case), case["expected"], case["stimulus"]
        self.assertEqual(actual["return"], expected["return"], case["name"])
        self.assertEqual(actual["material_unchanged"], 1)
        self.assertEqual(actual["trace"], [list(row) for row in expected["trace"]], case["name"])
        self.assertLessEqual(len(actual["trace"]), 32)
        self.assertEqual(len(actual["outcomes"]), len(actual["trace"]))
        self.assertTrue(all(type(n) is int for n in actual["outcomes"]))
        self.assertEqual(actual["lives"], sum(row[0] == "live" for row in expected["trace"]))
        self.assertEqual(actual["work"], sum(row[0] != "live" for row in expected["trace"]))
        self.assertEqual(actual["security"], sum(row[0] in ("unveil", "pledge") for row in expected["trace"]))
        work_index = 0
        for row, outcome in zip(expected["trace"], actual["outcomes"]):
            call = row[0]
            if call == "live":
                self.assertEqual(outcome, row[1])
                continue
            wanted = 1 if call == "identity" else 0
            if call == "identity" and stimulus["mode"] == "identity":
                wanted = 0
            if call == "identity" and stimulus["mode"] == "identity-result":
                wanted = stimulus["value"]
            if stimulus["mode"] in ("stat-result", "security-result") and work_index == stimulus["index"]:
                wanted = stimulus["value"]
            self.assertEqual(outcome, wanted, (case["name"], row))
            work_index += 1
        if stimulus["mode"] == "normal-identity":
            self.assertEqual(actual["owner_unchanged"], 1)
            self.assertEqual(actual["normal_unveil"], -1)
            self.assertEqual(actual["normal_pledge"], -1)
        else:
            self.assertEqual((actual["normal_unveil"], actual["normal_pledge"]), (-9, -9))
        # The no-close/write/child/output/new-clock obligations are established
        # by the closed production object symbol allowlist below plus this exact
        # fixed call trace. No hardcoded runtime zero-effects field is claimed.
        self.assertTrue(all(expected[n] for n in ("material_unchanged", "no_close", "no_write", "no_child", "no_output")))
        self.assertEqual(expected["origin_refreshes"], 0)

    def check_cli_fd_witnesses(self, actual):
        for name in ("fd_before", "fd_ready", "fd_after"):
            probes = actual[name]
            self.assertIs(type(probes), list)
            self.assertEqual(len(probes), 7)
            for i, probe in enumerate(probes):
                self.assertEqual(set(probe), FD_FIELDS)
                self.assertTrue(all(type(value) is int for value in probe.values()))
                self.assertEqual(probe["fd"], i if i < 5 else actual["fd_ready"][i]["fd"])
                for result, error in (("fd_flags", "fd_errno"), ("status_flags", "status_errno")):
                    self.assertGreaterEqual(probe[result], -1)
                    self.assertGreaterEqual(probe[error], 0)
                    self.assertEqual(probe[error] == 0, probe[result] >= 0)
                self.assertIn(probe["stat_ok"], (0, 1))
                self.assertGreaterEqual(probe["stat_errno"], 0)
                self.assertEqual(probe["stat_errno"] == 0, probe["stat_ok"] == 1)
                valid = (probe["fd_flags"] >= 0 and probe["status_flags"] >= 0 and probe["stat_result"] == 0 and
                         probe["stat_ok"] == 1 and (probe["fd_errno"], probe["status_errno"], probe["stat_errno"]) == (0, 0, 0))
                absent = ((probe["fd_flags"], probe["status_flags"], probe["stat_result"], probe["stat_ok"]) == (-1, -1, -1, 0) and
                          (probe["fd_errno"], probe["status_errno"], probe["stat_errno"]) == (errno.EBADF, errno.EBADF, errno.EBADF))
                self.assertTrue(valid or (i < 5 and absent), (name, i, probe))
                if not probe["stat_ok"]:
                    self.assertEqual((probe["device"], probe["inode"], probe["mode"]), (0, 0, 0))
        private_fds = [row["fd"] for row in actual["fd_ready"][5:]]
        self.assertTrue(all(fd >= 5 for fd in private_fds))
        self.assertEqual(len(set(private_fds)), 2)
        for phase in ("fd_before", "fd_ready", "fd_after"):
            for probe in actual[phase][5:]:
                self.assertEqual(probe["stat_ok"], 1)
                self.assertGreaterEqual(probe["fd_flags"], 0)
                self.assertGreaterEqual(probe["status_flags"], 0)
        for name, before, after, first, count in (
            ("inherited_setup_mismatch_mask", "fd_before", "fd_ready", 0, 5),
            ("inherited_call_mismatch_mask", "fd_ready", "fd_after", 0, 5),
            ("private_setup_mismatch_mask", "fd_before", "fd_ready", 5, 2),
            ("private_call_mismatch_mask", "fd_ready", "fd_after", 5, 2)):
            observed = sum(1 << i for i in range(count) if any(
                actual[before][first + i][field] != actual[after][first + i][field] for field in FD_COMPARE_FIELDS))
            self.assertEqual(actual[name], observed, name)
            # Private before->ready includes our own promotions and first write.
            # All inherited setup and all dispatch preservation remain mandatory.
            if name != "private_setup_mismatch_mask":
                self.assertEqual(actual[name], 0, name)
        poll = actual["canary_poll"]
        self.assertEqual(set(poll), {"calls", "result", "errno", "requested", "revents"})
        self.assertTrue(all(type(value) is int for value in poll.values()))
        self.assertEqual((poll["calls"], poll["result"], poll["errno"]), (1, 1, 0))
        self.assertGreater(poll["requested"], 0)
        self.assertTrue(poll["revents"] & poll["requested"])
        read = actual["canary_read"]
        self.assertEqual(set(read), {"calls", "result", "errno", "hex", "exact_bytes"})
        self.assertTrue(all(type(value) is int for name, value in read.items() if name != "hex"))
        self.assertEqual((read["calls"], read["result"], read["errno"], read["exact_bytes"]), (1, 7, 0, 1))
        self.assertEqual(read["hex"], "63616e6172790a")
        closes = actual["fixture_close_dispositions"]
        self.assertIs(type(closes), list)
        self.assertEqual(len(closes), 2)
        for fd, close in zip(private_fds, closes):
            self.assertEqual(set(close), {"fd", "calls", "result", "errno"})
            self.assertTrue(all(type(value) is int for value in close.values()))
            self.assertEqual((close["fd"], close["calls"], close["result"], close["errno"]), (fd, 1, 0, 0))

    def check_cli(self, case):
        actual, expected = self.run_case("cli", case), case["expected"]
        for name in ("status", "legacy_calls", "start_calls", "entry_calls", "availability_calls"):
            self.assertEqual(actual[name], expected[name], (case["name"], name))
        self.assertEqual(actual["argv_identity"], 1)
        self.assertEqual(actual["canaries_unchanged"], 1)
        self.check_cli_fd_witnesses(actual)
        self.assertEqual(bytes.fromhex(actual["stdout_hex"]), expected["stdout"].encode("ascii"))
        self.assertEqual(bytes.fromhex(actual["stderr_hex"]), expected["stderr"].encode("ascii"))
        self.assertEqual(actual["writes"], int(bool(expected["stdout"] or expected["stderr"])))
        self.assertEqual((actual["fixture_pipe_bytes"], actual["fixture_private_fds"]), (7, 2))
        # The only real pipe write/read is the private canary; the modeled legacy
        # writer captures attempted bytes without writing inherited stdout/stderr.
        self.assertEqual((expected["guard_calls"], expected["security_calls"], expected["native_calls"]), (0, 0, 0))

    def test_boundary_literal_corpus(self):
        self.assertEqual(len(vectors.BOUNDARY_CASES), vectors.BOUNDARY_COUNT)
        self.assertEqual(vectors.BOUNDARY_COUNT, 148)
        for case in vectors.BOUNDARY_CASES:
            with self.subTest(case=case["name"]):
                self.check_boundary(case)

    def test_cli_literal_corpus(self):
        self.assertEqual(len(vectors.CLI_CASES), vectors.CLI_COUNT)
        self.assertEqual(vectors.CLI_COUNT, 25)
        self.assertEqual(vectors.CASE_COUNT, 173)
        self.assertEqual(len({case["name"] for case in vectors.BOUNDARY_CASES + vectors.CLI_CASES}), 173)
        for case in vectors.CLI_CASES:
            with self.subTest(case=case["name"]):
                self.check_cli(case)

    def test_source_closed_symbols_and_accounting(self):
        memory = {"_memcmp", "_memset", "_memcpy", "_bzero", "___stack_chk_fail", "___stack_chk_guard"}
        core = self.objects["boundary-production"]
        self.assertLessEqual(clean_symbols(self.execute(["nm", "-u", str(core)])), memory | {
            "_bw_entry_live", "_bw_en_identity", "_bw_en_unveil", "_bw_en_pledge", "_fstat", "_lstat", "_strspn"})
        self.assertEqual(clean_symbols(self.execute(["nm", "-gU", str(core)])), {"_bw_entry_native"})
        self.assertFalse(clean_symbols(self.execute(["nm", "-u", str(self.objects["normal-primitives"])])))
        self.assertFalse(clean_symbols(self.execute(["nm", "-u", str(self.objects["normal-entry"])])))
        cli = self.objects["cli-production"]
        self.assertLessEqual(clean_symbols(self.execute(["nm", "-u", str(cli)])), memory | {
            "_strcmp", "_bw_entry_available", "_bw_entry_output", "_bw_legacy_cli"})
        self.assertEqual(clean_symbols(self.execute(["nm", "-gU", str(cli)])), {"_bw_entry_cli"})
        legacy = self.objects["legacy-main"]
        self.assertLessEqual(clean_symbols(self.execute(["nm", "-u", str(legacy)])), memory | {"_ne_start", "_ne_write", "_strcmp", "_strlen"})
        self.assertEqual(clean_symbols(self.execute(["nm", "-gU", str(legacy)])), {"_bw_legacy_cli", "_ne_actual_main"})
        source = (ROOT / "src/main.c").read_bytes()
        begin = source.index(b"{", source.index(b"bw_legacy_cli(int argc"))
        end = source.index(b"\n}\n", begin) + 3
        self.assertEqual(hashlib.sha256(source[begin:end]).hexdigest(), vectors.LEGACY_BODY_SHA256)
        self.assertEqual(source[source.index(b"{", source.index(b"main(int argc")):].strip(),
                         b"{\n    return bw_legacy_cli(argc, argv);\n}")
        self.assertEqual(next(row for row in (ROOT / "Makefile").read_text().splitlines() if row.startswith("PLAN_SOURCES =")),
                         "PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c")
        self.assertLessEqual(sum(p.stat().st_size for p in (ROOT / "src").rglob("*") if p.suffix in (".c", ".h")), 256 * 1024)
        budgets = ((24, ("trusted_start.c", "trusted_start.h", "entry_platform.c", "entry_native.c", "entry_openbsd.c",
                         "entry_cli.c", "private_entry.h", "private_cli.h", "private_limits.h", "private_start_material.h", "main.c",
                         "install_entry.c", "install_platform.c", "private_install.h", "install_profile.h")),
            (24, ("main.c", "plan.c", "plan.h", "plan_io.c", "native.c", "private_cli.h")),
            (18, ("auth_material.c", "auth_material.h", "private_start_material.h")),
            (18, ("signature_verify.c", "signature_verify.h", "signature_platform.c", "private_limits.h")),
            (8, ("auth_input.c", "auth_input.h")),
            (16, ("authenticated_review.c", "authenticated_review.h", "private_span.h", "private_content_format.h")))
        for cap, names in budgets:
            self.assertLessEqual(sum((ROOT / "src" / n).stat().st_size for n in names), cap * 1024, names)
