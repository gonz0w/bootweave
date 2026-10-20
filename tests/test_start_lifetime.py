# SPDX-License-Identifier: BSD-3-Clause
"""Finite closed-start/real-material/synthetic-SV lifecycle checks.

Guard process mutations are modeled after SDK declarations; no host settings or
signals change. Actual owned private files/pipes exercise material/capture/FD
cleanup. The synthetic PID never denotes a born process. Policy's existing held
launcher retains every direct child's streams and consuming wait, even on error.
"""
import base64
from collections import Counter
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest

import start_lifetime_vectors as vectors
import test_policy

ROOT = Path(__file__).resolve().parents[1]
IDENTITY = {"supported", "getuid", "geteuid", "getgid", "getegid"}
SIGNALS = vectors.CANCEL_SIGNALS + vectors.CLASSIC_SIGNALS
SV_CAUSES = {name: i for i, name in enumerate(("NONE", "UNSUPPORTED", "IDENTITY", "SIGNALS",
    "DESCRIPTOR", "CANCEL", "CLOCK", "DEADLINE", "IO", "FORK", "GROUP", "BOUNDARY",
    "GATE", "CHILD", "OUTPUT", "WAIT", "STOP", "CLOSE"))}
INPUTS = {"definition": b"exact\0definition\n", "payload": b"retained\0payload\n",
          "key": b"untrusted comment: inert lifetime key\n" + base64.b64encode(b"Ed" + bytes(40)) + b"\n",
          "signature": b"untrusted comment: inert lifetime signature\n" + base64.b64encode(b"Ed" + bytes(72)) + b"\n",
          "canary": b"canary\n"}
FIELDS = set(("first first_unchanged first_events first_dirs retry legacy_first repeat_clean repeat_at origin_count "
    "origin_ok flag_ok flag_value callbacks callbacks_clean mkdirs members member_writes capture capture_exact "
    "capture_reads capture_closes prepare signature sv_cause sv_budget sv_launch sv_consumed sv_lost sv_bytes "
    "sv_final_failed sv_material_unknown forks stops consumes synthetic_pid bad_order bad_close sv_reuse_clean final_calls "
    "material_state material_cause material_uncertain released final_owned mutations blocked unblocked "
    "host_unchanged readonly_checks samples events").split())


def identity(path):
    info = path.stat()
    return {"device": info.st_dev, "inode": info.st_ino, "mode": info.st_mode,
            "uid": info.st_uid, "gid": info.st_gid, "links": info.st_nlink, "size": info.st_size,
            "mtime_ns": info.st_mtime_ns, "ctime_ns": info.st_ctime_ns,
            "sha256": hashlib.sha256(path.read_bytes()).hexdigest()}


class StartLifetimeTests(unittest.TestCase):
    execute = classmethod(test_policy.PolicyTests.execute.__func__)

    @classmethod
    def setUpClass(cls):
        if sys.platform != "darwin":
            raise RuntimeError("Mac portable modeled lifetime fixture only; no native dispatch")
        cls.directory = Path(tempfile.mkdtemp(prefix="start-lifetime-build-", dir=ROOT / ".tools"))
        cls.ledger = cls.directory / "observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1",
                   "ASAN_OPTIONS": "detect_leaks=0:abort_on_error=1"}
        cls.calls = cls.builds = cls.invocations = 0
        cls.cache, cls.quarantine = {}, []
        cls.flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
                     "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g", "-I", str(ROOT / "src")]
        sources = sorted([*ROOT.joinpath("src").glob("*.[ch]"),
            *(ROOT / "tests" / n for n in ("start_lifetime_driver.c", "start_lifetime_vectors.py",
                "test_start_lifetime.py", "signature_driver.c", "test_policy.py", "entry_ownership_driver.c"))])
        (cls.directory / "source-manifest.json").write_text(json.dumps({str(p.relative_to(ROOT)):
            {"bytes": p.stat().st_size, "sha256": hashlib.sha256(p.read_bytes()).hexdigest()}
            for p in sources}, sort_keys=True) + "\n")
        cls.plain, guard, signature, material, capture, platform = (
            cls.directory / n for n in ("guard-production.o", "guard.o", "signature.o", "material.o", "capture.o", "platform.o"))
        driver = str(ROOT / "tests/start_lifetime_driver.c")
        jobs = ((cls.plain, "trusted_start.c", []),
            (guard, "trusted_start.c", ["-DLS_START_PREAMBLE", "-include", driver]),
            (signature, "signature_verify.c", ["-DLS_SIGNATURE_PREAMBLE", "-include", driver]),
            (material, "auth_material.c", ["-Dclock_gettime=lt_material_clock", "-Dmkdtemp=lt_mkdtemp",
                "-Dopenat=lt_material_openat", "-Dwrite=lt_material_write", "-Dclose=lt_material_close"]),
            (capture, "auth_input.c", ["-Dclock_gettime=lt_material_clock", "-Dread=lt_capture_read", "-Dclose=lt_capture_close"]),
            (platform, "signature_platform.c", []))
        for target, source, extra in jobs:
            cls.execute(cls.flags + extra + ["-c", str(ROOT / "src" / source), "-o", str(target)], compile_job=True)
        dependencies = []
        for source in ("authenticated_review", "content_review", "policy_binding", "plan", "payload_review", "entry_platform"):
            target = cls.directory / (source + ".o")
            cls.execute(cls.flags + ["-c", str(ROOT / "src" / (source + ".c")), "-o", str(target)], compile_job=True)
            dependencies.append(target)
        support = cls.directory / "unused-entry-policy.o"
        cls.execute(cls.flags + ["-DEO_UNUSED_DEPENDENCIES", "-c", str(ROOT / "tests/entry_ownership_driver.c"),
                                "-o", str(support)], compile_job=True)
        dependencies.append(support)
        cls.material = material
        cls.modeled, cls.normal = cls.directory / "modeled-lifetime", cls.directory / "normal-unsupported"
        for target, objects, extra in ((cls.modeled, [guard, signature, material, capture], []),
                (cls.normal, [cls.plain, platform, signature, material, capture], ["-DLS_NORMAL", "-DTEST_REAL_PLATFORM"])):
            cls.execute(cls.flags + extra + list(map(str, [*objects, *dependencies])) + [driver, "-o", str(target)], compile_job=True)

    def run_case(self, case):
        type(self).invocations += 1
        directory = self.directory / f"material-case-{self.invocations:03d}"
        directory.mkdir(mode=0o700)
        for name, data in INPUTS.items():
            (directory / name).write_bytes(data)
        before = {name: identity(directory / name) for name in INPUTS}
        # command[1] is the private retained case root, for outer receipt bijection.
        command = [str(self.normal if case["kind"] == "normal-host" else self.modeled), str(directory),
            case["kind"], case.get("mode", case["name"]), case.get("signal", "INT"), case.get("phase", "none"),
            str(case.get("start", 1000000000)), str(case.get("work", 1000000000)), str(case.get("cleanup", 1000000000))]
        (directory / "case.json").write_text(json.dumps(case, sort_keys=True) + "\n")
        data = self.execute(command)
        self.assertTrue(data.startswith(b"LS_RESULT "), case["name"])
        self.assertEqual(data.count(b"\n"), 1, case["name"])
        actual = json.loads(data[len(b"LS_RESULT "):])
        (directory / "result.json").write_text(json.dumps(actual, sort_keys=True) + "\n")
        self.assertEqual(set(actual), FIELDS)
        self.assertTrue(all(type(v) is int for k, v in actual.items() if k not in ("events", "samples", "capture_closes")))
        self.assertEqual(len(actual["capture_closes"]), 3)
        self.assertTrue(all(type(n) is int for n in actual["capture_closes"]))
        self.assertLessEqual(len(actual["samples"]), 128)
        self.assertTrue(all(type(n) is int for n in actual["samples"]))
        self.assertLessEqual(len(actual["events"]), 100)
        for event in actual["events"]:
            self.assertEqual(set(event), {"call", "outcome", "values"})
            self.assertIs(type(event["call"]), str)
            self.assertLess(len(event["call"]), 56)
            self.assertIs(type(event["outcome"]), int)
            self.assertEqual(len(event["values"]), 4)
            self.assertTrue(all(type(n) is int for n in event["values"]))
        self.assertEqual(before, {name: identity(directory / name) for name in INPUTS}, case["name"])
        self.assertEqual(sorted(p.name for p in directory.iterdir()), sorted((*INPUTS, "case.json", "result.json")), case["name"])
        return actual

    def setup(self, case, actual):
        # Repeat probes must never renew the origin, mutate settings, reserve a
        # second directory, restore settings, or execute a second setup stage.
        self.assertEqual(actual["repeat_clean"], 1)
        self.assertTrue(all(e["call"] in IDENTITY for e in actual["events"][actual["repeat_at"]:]))
        events = actual["events"][:actual["repeat_at"]]
        calls = [e["call"] for e in events]
        if case["kind"] in ("null", "nonempty"):
            self.assertEqual(actual["first_events"], 0)
            self.assertEqual(actual["first_dirs"], 0)
            self.assertEqual(actual["retry"], 1)
        else:
            self.assertEqual(actual["retry"], -9)
        if case["kind"] == "normal-host":
            self.assertFalse(events)
            self.assertEqual(actual["readonly_checks"], 36)
            self.assertEqual(actual["host_unchanged"], 1)
            self.assertEqual(actual["mutations"], 0)
            return
        self.assertEqual(actual["readonly_checks"], 0)
        self.assertEqual(actual["host_unchanged"], -1)
        counts = Counter(calls)
        self.assertTrue(all(n == 1 for call, n in counts.items() if call not in IDENTITY and not call.startswith("limit-get:")))
        for resource in vectors.RESOURCE_ORDER:
            self.assertLessEqual(counts["limit-get:" + resource], 2)
        # This finite prefix follows the adopted setup contract, independently
        # of the implementation trace. Identity reads may repeat/interleave
        # before any mutation; setup operations may neither retry nor restore.
        ordered = ["timer-get:REAL:initial", "timer-get:PROF:initial", "empty:0", "empty:1"]
        if case["kind"] != "legacy-first":
            ordered += ["empty:2"]
            for signal in vectors.CANCEL_SIGNALS:
                ordered += ["add:" + signal + ":cancel", "add:" + signal + ":unblock"]
            ordered += ["block", *("action:" + signal for signal in vectors.CANCEL_SIGNALS), "empty:3"]
        for signal in vectors.CLASSIC_SIGNALS:
            ordered += ["action:" + signal, "add:" + signal + ":unblock"]
        for resource in vectors.RESOURCE_ORDER:
            ordered += ["limit-get:" + resource, "limit-set:" + resource, "limit-get:" + resource]
        ordered += ["unblock"]
        if case["kind"] != "legacy-first":
            ordered += ["origin"]
        ordered += ["timer-set:REAL", "timer-get:REAL:final", "timer-set:PROF", "timer-get:PROF:final"]
        operations = [call for call in calls if call not in IDENTITY]
        self.assertEqual(operations, ordered[:len(operations)], case["name"])
        if operations and case["kind"] != "legacy-first":
            first_operation = next(i for i, c in enumerate(calls) if c not in IDENTITY)
            self.assertTrue(all(c not in IDENTITY for c in calls[first_operation:]))
        self.assertEqual(actual["mutations"], sum(e["call"].startswith(("action:", "limit-set:", "timer-set:")) or
                                                  e["call"] in ("block", "unblock") for e in events))
        for event in events:
            call, values = event["call"], event["values"]
            if call.startswith("action:"):
                i = SIGNALS.index(call.split(":")[1])
                self.assertEqual(values, [1 if i < 3 else 2 if i < 7 else 3 if i == 7 else 4, 0, 7 if i < 3 else 0, 0])
            elif call == "block":
                self.assertEqual(values, [7, 0, 0, 0])
            elif call == "unblock":
                self.assertEqual(values, [504 if case["kind"] == "legacy-first" else 511, 0, 0, 0])
            elif call.startswith("limit-set:"):
                cap = vectors.RESOURCE_CAPS[vectors.RESOURCE_ORDER.index(call.split(":")[1])]
                self.assertEqual(values, [cap, cap, 0, 0])
            elif call.startswith("timer-get:") and call.endswith(":initial"):
                self.assertEqual(values, [0, 0, 0, 0])
            elif call.startswith("timer-set:") or (call.startswith("timer-get:") and call.endswith(":final")):
                self.assertEqual(values, [15 if ":REAL" in call else 4, 0, 0, 0])
        if case["kind"] == "setup":
            self.assertEqual(calls[-1], case["terminal"])
            self.assertEqual(actual["first_events"], len(events))
            if case["mode"] == "unsupported":
                self.assertEqual(events[-1]["outcome"], 0)
            elif case["mode"] == "root":
                self.assertEqual(events[-1]["values"][0], 0)
            elif case["mode"] not in ("origin-negative", "origin-invalid", "origin-negative-nsec"):
                self.assertEqual(events[-1]["outcome"], -1)
            return
        if case["kind"] == "cancel-setup":
            mode = case["mode"]
            self.assertEqual(actual["origin_count"], 0 if mode == "pending-unblock" else 1)
            self.assertEqual(tuple(c for c in calls if c.startswith("timer-set:")),
                () if mode in ("pending-unblock", "cancel-at-origin") else
                ("timer-set:REAL",) if mode == "cancel-after-real" else ("timer-set:REAL", "timer-set:PROF"))
            self.assertEqual(calls[-1], {"pending-unblock": "unblock", "cancel-at-origin": "origin",
                "cancel-after-real": "timer-get:REAL:final", "cancel-after-prof": "timer-get:PROF:final"}[mode])
        else:
            self.assertEqual(actual["origin_count"], 0 if case["kind"] == "legacy-first" else 1)
            self.assertEqual(tuple(c for c in calls if c.startswith("timer-set:")), ("timer-set:REAL", "timer-set:PROF"))
        if "origin" in calls and case["kind"] != "setup":
            event = events[calls.index("origin")]
            self.assertEqual(event["values"], [*vectors.SEED, 0, 0])
            if case.get("mode") != "cancel-at-origin":
                self.assertEqual(calls[calls.index("origin") + 1], "timer-set:REAL")
        complete = case["kind"] not in ("setup",) and case.get("mode") not in ("pending-unblock", "cancel-at-origin")
        if complete:
            if case["kind"] != "cancel-setup":
                self.assertEqual(operations, ordered)
            self.assertEqual(tuple(c.split(":")[1] for c in calls if c.startswith("action:")),
                             vectors.CLASSIC_SIGNALS if case["kind"] == "legacy-first" else SIGNALS)
            self.assertEqual(tuple(c.split(":")[1] for c in calls if c.startswith("limit-set:")), vectors.RESOURCE_ORDER)
            self.assertEqual(actual["blocked"], 0 if case["kind"] == "legacy-first" else 7)
            self.assertEqual(actual["unblocked"], 504 if case["kind"] == "legacy-first" else 511)

    def check(self, case):
        actual, kind = self.run_case(case), case["kind"]
        self.setup(case, actual)
        for field in ("repeat_clean", "origin_ok", "flag_ok", "callbacks_clean", "synthetic_pid", "final_owned", "sv_reuse_clean"):
            self.assertEqual(actual[field], 1, (case["name"], field))
        for field in ("bad_order", "bad_close", "sv_bytes", "sv_final_failed"):
            self.assertEqual(actual[field], 0, (case["name"], field))
        self.assertEqual(actual["callbacks"], 1 if kind in ("cancel-setup", "flag") else 0)
        self.assertEqual(actual["legacy_first"], 1 if kind == "legacy-first" else -9)
        no_material = kind in ("setup", "cancel-setup", "normal-host", "legacy-first")
        invalid = kind in ("null", "nonempty")
        reserve_spent = kind == "spent" and case["phase"] == "reserve"
        self.assertEqual(actual["first"], 0 if no_material or invalid or reserve_spent else 1)
        self.assertEqual(actual["first_unchanged"], 1 if no_material or invalid else 0)
        self.assertEqual(actual["first_dirs"], 0 if no_material or invalid or reserve_spent else 1)
        if no_material or invalid or reserve_spent:
            for field in ("capture", "prepare", "signature"):
                self.assertEqual(actual[field], -9)
            for field in ("members", "member_writes", "capture_reads", "forks", "stops", "consumes", "final_calls"):
                self.assertEqual(actual[field], 0)
            self.assertEqual(actual["capture_closes"], [0, 0, 0])
            self.assertFalse(actual["samples"])
            self.assertEqual(actual["capture_exact"], 0)
            self.assertEqual(actual["mkdirs"], 1 if invalid else 0)
            self.assertEqual(actual["material_state"], 4 if invalid else 5 if reserve_spent else 0)
            self.assertEqual(actual["material_cause"], 4 if reserve_spent else 0)
            self.assertEqual(actual["material_uncertain"], 0)
            self.assertEqual(actual["released"], 1 if invalid else -9)
            self.assertEqual(actual["flag_value"], 0 if invalid or reserve_spent else -1)
            return
        self.assertEqual(actual["mkdirs"], 1)
        self.assertEqual(actual["capture_closes"], [1, 1, 1])
        capture_failure = kind == "spent" and case["phase"] == "capture" or kind == "flag" and case["phase"] == "after-reserve"
        prepare_failure = kind == "spent" or kind == "flag" and case["phase"] in ("after-reserve", "after-capture")
        self.assertEqual(actual["capture"], 0 if capture_failure else 1)
        self.assertEqual(actual["capture_exact"], 0 if capture_failure else 1)
        if capture_failure:
            self.assertEqual(actual["capture_reads"], 0)
        else:
            self.assertGreater(actual["capture_reads"], 0)
            self.assertLessEqual(actual["capture_reads"], 16)
        self.assertEqual(actual["prepare"], 0 if prepare_failure else 1)
        self.assertEqual(actual["members"], 0 if prepare_failure else 6)
        self.assertEqual(actual["member_writes"], 0 if prepare_failure else 3)
        self.assertEqual(actual["released"], -9)
        self.assertEqual(actual["flag_value"], 1 if kind == "flag" else 0)
        if prepare_failure:
            self.assertEqual(actual["material_state"], 5)
            self.assertEqual(actual["material_cause"], 3 if kind == "flag" else 4)
            self.assertEqual(actual["material_uncertain"], 0)
            self.assertEqual(actual["signature"], -9)
            self.assertEqual(actual["final_calls"], 0)
            self.assertFalse(actual["samples"])
            for field in ("forks", "stops", "consumes", "sv_material_unknown"):
                self.assertEqual(actual[field], 0)
            return
        expected = case if kind == "signature" else vectors.SIGNATURE_CASES[0]
        if kind == "flag" and case["phase"] in ("after-prepare", "signature-gate"):
            expected = {"return": 0, "cause": "CANCEL", "budget": 0,
                "forks": int(case["phase"] == "signature-gate"), "launch": int(case["phase"] == "signature-gate"),
                "stop": int(case["phase"] == "signature-gate"), "consumed": int(case["phase"] == "signature-gate"), "lost": 0}
        mapping = {"signature": "return", "sv_cause": "cause", "sv_budget": "budget", "forks": "forks",
                   "sv_launch": "launch", "stops": "stop", "sv_consumed": "consumed", "consumes": "consumed", "sv_lost": "lost"}
        for field, key in mapping.items():
            self.assertEqual(actual[field], SV_CAUSES[expected[key]] if key == "cause" else expected[key], (case["name"], field))
        unknown = case["name"] == "signature-unknown-close-late"
        self.assertEqual(actual["sv_material_unknown"], 8 if unknown else 0)
        self.assertEqual(actual["material_state"], 6 if unknown else 4)
        self.assertEqual(actual["material_cause"], 7 if unknown else 0)
        self.assertEqual(actual["material_uncertain"], 0)
        self.assertEqual(actual["final_calls"], 1)
        self.assertTrue(actual["samples"])
        self.assertEqual(actual["samples"][0], case.get("start", 1000000000))
        # Callback purity, same flag/origin, first cause, finalizers and the
        # original/phase bounds are asserted above without a renewed origin.

    def test_finite_closed_start_and_signature_lifetimes(self):
        cases = vectors.cases() + vectors.SIGNATURE_POSITIVE_CONTROLS + vectors.SIGNATURE_COMPLETED_WORK_BOUNDARIES
        self.assertEqual(len(cases), vectors.CASE_COUNT)
        self.assertEqual(len({case["name"] for case in cases}), 80)
        for case in cases:
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_closed_symbols_private_bridge_and_source_accounting(self):
        undefined = {row.split()[-1] for row in self.execute(["nm", "-u", str(self.plain)]).decode().splitlines()}
        self.assertLessEqual({n for n in undefined if "asan" not in n and "ubsan" not in n}, {
            "_bw_sv_supported", "_bw_material_reserve_started", "_clock_gettime", "_getuid", "_geteuid", "_getgid", "_getegid",
            "_bw_entry_boundary", "_bw_auth_capture_reserved", "_bw_authenticated_output", "_bw_verifier_release",
            "_bw_sv_closefrom", "_bw_sv_parent_boundary", "_fcntl", "_close", "___error",
            "_getitimer", "_setitimer", "_getrlimit", "_setrlimit", "_sigemptyset", "_sigaddset", "_sigaction", "_sigprocmask",
            "__exit", "_memset", "_bzero", "_memcpy", "___stack_chk_fail", "___stack_chk_guard"})
        exports = lambda obj: {row.split()[-1] for row in self.execute(["nm", "-gU", str(obj)]).decode().splitlines()
                               if "asan" not in row and "ubsan" not in row}
        self.assertEqual(exports(self.plain), {"_bw_process_guard", "_bw_start_reserve", "_bw_entry_output", "_bw_entry_observe", "_bw_entry_live"})
        self.assertEqual(exports(self.material), {"_bw_signify_public_key", "_bw_signify_records", "_bw_verifier_reserve",
            "_bw_material_reserve_started", "_bw_verifier_prepare", "_bw_verifier_release"})
        self.assertNotIn("bw_material_reserve_started", (ROOT / "src/auth_material.h").read_text())
        self.assertIn("bw_material_reserve_started", (ROOT / "src/private_start_material.h").read_text())
        self.assertEqual(next(row for row in (ROOT / "Makefile").read_text().splitlines() if row.startswith("PLAN_SOURCES =")),
                         "PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c")
        self.assertLessEqual(sum(p.stat().st_size for p in (ROOT / "src").rglob("*") if p.suffix in (".c", ".h")), 100 * 1024)
        budgets = ((16, ("trusted_start.c", "trusted_start.h", "entry_platform.c", "entry_native.c", "entry_openbsd.c", "entry_cli.c", "private_entry.h", "private_cli.h", "main.c", "private_limits.h", "private_start_material.h")),
            (18, ("auth_material.c", "auth_material.h", "private_start_material.h")),
            (18, ("signature_verify.c", "signature_verify.h", "signature_platform.c", "private_limits.h")),
            (8, ("auth_input.c", "auth_input.h")),
            (16, ("authenticated_review.c", "authenticated_review.h", "private_span.h", "private_content_format.h")))
        for cap, names in budgets:
            self.assertLessEqual(sum((ROOT / "src" / n).stat().st_size for n in names), cap * 1024, names)
