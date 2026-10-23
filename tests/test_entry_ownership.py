# SPDX-License-Identifier: BSD-3-Clause
"""Finite coordinator ownership integration; no host guard/security/child effects.

Independent literal EO cases are frozen externally. Real private material,
capture, parser, hash, inspector and fixed-output pipeline remain linked. The
existing Policy held-child launcher retains all actual direct-process evidence.
"""
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest

import authenticated_vectors
import content_vectors
import entry_ownership_vectors as vectors
import test_policy

ROOT = Path(__file__).resolve().parents[1]
FIELDS = set(("return state result cause capture input_close_errors input_unknown_closes final_boundary_failed "
    "guard_calls origin_count arms closefrom reserve boundary capture_calls auth_calls entry_release entry_final total_final "
    "material_consumptions output_attempts material_state material_cause material_close_errors material_unknown_closes "
    "capture_close_errors capture_unknown_closes capture_reads snapshot_zero repeat_clean shared_clean canaries flags_unchanged "
    "origin_identity flag_identity span_identity callback_purity wire_protocol after_final bad_order bad_close members writes "
    "prepare signature content hashes inspect forks stops consumes high_closed captured_exact first_no_write "
    "auth_state auth_cause sv_cause sv_final_failed sv_unknown sv_lost sv_consumed wire_hex entry_closes capture_closes presence events").split())
MATERIAL_STATES = {name: i for i, name in enumerate(("EMPTY", "RESERVED", "WORKING", "READY", "CLOSED", "REJECTED", "UNCERTAIN", "VERIFYING"))}


def inventory(directory):
    rows = []
    for path in sorted(directory.rglob("*")):
        if len(rows) >= 16:
            raise AssertionError("retained private inventory cap")
        info = path.lstat()
        row = {"path": str(path.relative_to(directory)), "mode": info.st_mode, "size": info.st_size,
               "device": info.st_dev, "inode": info.st_ino, "links": info.st_nlink}
        if path.is_file():
            if info.st_size > 262144:
                raise AssertionError("retained private file cap")
            row["sha256"] = hashlib.sha256(path.read_bytes()).hexdigest()
        rows.append(row)
    return rows


class EntryOwnershipTests(unittest.TestCase):
    execute = classmethod(test_policy.PolicyTests.execute.__func__)

    @classmethod
    def setUpClass(cls):
        if sys.platform != "darwin":
            raise RuntimeError("Mac portable coordinator fixture only; no native dispatch")
        cls.directory = Path(tempfile.mkdtemp(prefix="entry-ownership-build-", dir=ROOT / ".tools"))
        cls.ledger = cls.directory / "observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1",
                   "ASAN_OPTIONS": "detect_leaks=0:abort_on_error=1"}
        cls.calls = cls.builds = cls.invocations = 0
        cls.cache, cls.quarantine, cls.objects = {}, [], []
        cls.flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
                     "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g", "-I", str(ROOT / "src")]
        sources = sorted([*ROOT.joinpath("src").glob("*.[ch]"),
            *(ROOT / "tests" / n for n in ("entry_ownership_driver.c", "entry_ownership_vectors.py",
                "test_entry_ownership.py", "signature_driver.c", "authenticated_vectors.py", "content_vectors.py", "test_policy.py"))])
        (cls.directory / "source-manifest.json").write_text(json.dumps({str(p.relative_to(ROOT)):
            {"bytes": p.stat().st_size, "sha256": hashlib.sha256(p.read_bytes()).hexdigest()}
            for p in sources}, sort_keys=True) + "\n")
        driver = str(ROOT / "tests/entry_ownership_driver.c")
        jobs = [("guard-production", "trusted_start.c", []),
            ("guard", "trusted_start.c", ["-DEO_ENTRY_PREAMBLE", "-include", driver]),
            ("signature", "signature_verify.c", ["-DEO_SIGNATURE_PREAMBLE", "-include", driver,
                "-Dbw_verifier_release=eo_signature_release", "-Dbw_sv_parent_boundary=eo_signature_boundary"]),
            ("material", "auth_material.c", ["-Dclock_gettime=eo_material_clock", "-Dmkdtemp=eo_mkdtemp",
                "-Dfstat=eo_material_fstat", "-Dopenat=eo_material_openat", "-Dwrite=eo_material_write", "-Dclose=eo_material_close"]),
            ("capture", "auth_input.c", ["-Dclock_gettime=eo_capture_clock", "-Dfstat=eo_capture_fstat",
                "-Dread=eo_capture_read", "-Dclose=eo_capture_close"]),
            ("authenticated", "authenticated_review.c", ["-Dclock_gettime=eo_authenticated_clock",
                "-Dbw_verifier_prepare=eo_prepare", "-Dbw_verifier_release=eo_fused_release",
                "-Dbw_signature_verify=eo_signature_verify", "-Dbw_content_review=eo_content",
                "-Dbw_sv_parent_boundary=eo_fused_final", "-Dfcntl=eo_output_fcntl", "-Dfstat=eo_output_fstat",
                "-Disatty=eo_output_isatty", "-Dwrite=eo_output_write"]),
            ("content", "content_review.c", ["-Dbw_payload_structure_review=eo_structure"]),
            *( (name, name + ".c", []) for name in ("policy_binding", "plan", "payload_review", "entry_platform"))]
        cls.plain = cls.directory / "guard-production.o"
        cls.platform = cls.directory / "entry_platform.o"
        for name, source, defines in jobs:
            target = cls.directory / (name + ".o")
            cls.execute(cls.flags + defines + ["-c", str(ROOT / "src" / source), "-o", str(target)], compile_job=True)
            if name != "guard-production":
                cls.objects.append(str(target))
        cls.valid = authenticated_vectors.valid_cases()[0]
        cls.content_refusal = dict(content_vectors.structure_cases()[0], signature=authenticated_vectors.SIGNATURE)
        # Exactly two compiled immutable policies: ordinary viewer and a pinned
        # invalid-archive stimulus which passes real hashes before inspector refusal.
        for label, case in (("valid", cls.valid), ("content", cls.content_refusal)):
            policy = case["policy"]
            source = '#include "policy_binding.h"\nconst struct bw_policy_definition bw_compiled_policy = {\n' + ',\n'.join(
                [str(policy["profile"]), str(policy["key_length"])] +
                [test_policy.array(policy[n]) for n in ("key", "digest", "id", "release", "arch")]) + '\n};\n'
            path = cls.directory / ("policy-" + label + ".c")
            path.write_text(source)
            binary = path.with_suffix("")
            cls.execute(cls.flags + cls.objects + [driver, str(path), "-o", str(binary)], compile_job=True)
            cls.cache[label] = binary

    def run_case(self, case):
        type(self).invocations += 1
        directory = self.directory / f"material-case-{self.invocations:03d}"
        directory.mkdir(mode=0o700)
        selected = self.content_refusal if case["stimulus"] == "content-rejected" else self.valid
        parts = {n: selected[n] for n in ("definition", "payload", "signature")}
        if case["stimulus"] == "prepare-record":
            parts["signature"] = b"malformed inert signature record\n"
        if case["stimulus"] == "policy-rejected":
            parts["definition"] = parts["definition"].replace(b"Offline Viewer", b"Changed Viewer", 1)
        parts.update(canary=b"canary\n", unrelated=b"inert unrelated input\n")
        for name, data in parts.items():
            (directory / name).write_bytes(data)
        before = {n: hashlib.sha256((directory / n).read_bytes()).hexdigest() for n in parts}
        (directory / "case.json").write_text(json.dumps(case, sort_keys=True) + "\n")
        binary = self.cache["content" if case["stimulus"] == "content-rejected" else "valid"]
        data = self.execute([str(binary), str(directory), case["stimulus"]])
        self.assertTrue(data.startswith(b"EO_RESULT "), case["name"])
        self.assertEqual(data.count(b"\n"), 1, case["name"])
        actual = json.loads(data[len(b"EO_RESULT "):])
        (directory / "result.json").write_text(json.dumps(actual, sort_keys=True) + "\n")
        (directory / "inventory.json").write_text(json.dumps(inventory(directory), sort_keys=True) + "\n")
        self.assertEqual(set(actual), FIELDS, case["name"])
        self.assertTrue(all(type(v) is int for n, v in actual.items() if n not in
            ("wire_hex", "entry_closes", "capture_closes", "presence", "events")))
        for name in ("entry_closes", "capture_closes"):
            self.assertEqual(len(actual[name]), 3)
            self.assertTrue(all(type(n) is int for n in actual[name]))
        self.assertLessEqual(len(actual["presence"]), 5)
        self.assertLessEqual(len(actual["events"]), 128)
        for row in actual["events"]:
            self.assertEqual(len(row), 2)
            self.assertIs(type(row[0]), str)
            self.assertIs(type(row[1]), int)
        bytes.fromhex(actual["wire_hex"])
        self.assertEqual(before, {n: hashlib.sha256((directory / n).read_bytes()).hexdigest() for n in parts})
        # Evidence is retained, including the ownership-refused scratch directory.
        extra = [p for p in directory.iterdir() if p.is_dir()]
        self.assertEqual(len(extra), int(case["stimulus"] == "reserve:ownership"))
        return actual

    def check(self, case):
        actual, expected = self.run_case(case), case["expected"]
        for name in ("input_close_errors", "input_unknown_closes", "guard_calls", "closefrom", "reserve", "boundary",
                     "capture_calls", "auth_calls", "entry_release", "entry_final", "total_final", "material_consumptions", "output_attempts",
                     "capture_close_errors", "capture_unknown_closes", "material_unknown_closes", "final_boundary_failed"):
            self.assertEqual(actual[name], expected[name], (case["name"], name))
        self.assertEqual(actual["return"], expected["result"])
        self.assertEqual(actual["result"], 0 if expected["result"] == -2 else expected["result"])
        self.assertEqual(actual["state"], 0 if expected["result"] == -2 else 2)
        self.assertEqual(actual["cause"], vectors.ENTRY_CAUSES.index(expected["cause"]))
        self.assertEqual(actual["material_state"], MATERIAL_STATES[expected["material_terminal"]])
        for name in ("presence", "entry_closes", "capture_closes"):
            self.assertEqual(actual[name], list(expected[name]), (case["name"], name))
        # Selected closer is proven by the independent close journal and rebound
        # canaries. It is not inferred from a removed public observation summary.
        if expected["input_closer"] == "CAPTURE":
            self.assertEqual(actual["entry_closes"], [0, 0, 0])
            self.assertEqual(actual["capture_closes"], [1, 1, 1])
        elif expected["input_closer"] == "ENTRY":
            self.assertEqual(actual["entry_closes"], [1, 1, 1])
            self.assertEqual(actual["capture_closes"], [0, 0, 0])
        else:
            self.assertEqual(actual["entry_closes"], [0, 0, 0])
            self.assertEqual(actual["capture_closes"], [0, 0, 0])
        for name in ("repeat_clean", "shared_clean", "canaries", "flags_unchanged", "origin_identity", "flag_identity",
                     "span_identity", "callback_purity", "wire_protocol", "captured_exact", "first_no_write"):
            self.assertEqual(actual[name], 1, (case["name"], name))
        for name in ("after_final", "bad_order", "bad_close"):
            self.assertEqual(actual[name], 0, (case["name"], name))
        if expected["original_origin"]:
            self.assertEqual(actual["origin_count"], 1)
            self.assertEqual(actual["arms"], 2)
        if not expected["capture_calls"]:
            self.assertEqual(actual["capture"], -2 if expected["result"] != -2 else 0)
            self.assertEqual(actual["capture_reads"], 0)
            self.assertEqual(actual["snapshot_zero"], -1)
        else:
            self.assertEqual(actual["snapshot_zero"], 1)
            self.assertEqual(actual["capture"], -2 if expected["input_closer"] == "ENTRY" else
                -1 if expected["capture_unknown_closes"] else 0 if expected["cause"] == "CAPTURE" else 1)
        if not expected["auth_calls"]:
            self.assertEqual(actual["prepare"], 0)
            self.assertEqual(actual["signature"], 0)
        not_taken = case["stimulus"] in ("auth:not-taken", "combined:auth-not-taken-material-final")
        self.assertEqual(actual["auth_state"], 2 if expected["auth_calls"] and not not_taken else 0)
        skip_signature = not expected["auth_calls"] or not_taken or case["stimulus"] in ("prepare-record", "output-preflight")
        self.assertEqual(actual["signature"], 0 if skip_signature else 1)
        sv_causes = {"signature-rejected": 13, "signature-stop-unknown": 16, "signature-wait-lost": 15,
                     "signature-material-close-unknown": 17, "auth-final-boundary-error": 11, "cancel:signature-gate": 5}
        self.assertEqual(actual["sv_cause"], sv_causes.get(case["stimulus"], 0))
        self.assertEqual(actual["sv_final_failed"], int(case["stimulus"] == "auth-final-boundary-error"))
        self.assertEqual(actual["sv_unknown"], 8 if case["stimulus"] == "signature-material-close-unknown" else 0)
        self.assertEqual(actual["sv_lost"], int(case["stimulus"] == "signature-wait-lost"))
        self.assertEqual(actual["sv_consumed"], 0 if skip_signature or case["stimulus"] == "signature-wait-lost" else 1)
        if expected["unrelated_uncertain"]:
            self.assertEqual(actual["result"], -1)
            self.assertEqual(actual["cause"], vectors.ENTRY_CAUSES.index("UNRELATED"))
        if case["stimulus"] == "closefrom-high":
            self.assertEqual(actual["high_closed"], 1)
            self.assertIn(["synthetic-high-close", 70], actual["events"])
        self.check_order(case, actual)
        report = bytes.fromhex(actual["wire_hex"])
        golden = self.valid["expected"]["report"]
        if expected["result"] == 1:
            self.assertEqual(report, golden)
            self.assertEqual(len(report.splitlines()), 18)
            self.assertEqual(actual["prepare"], 1)
            self.assertEqual(actual["signature"], 1)
            self.assertEqual(actual["content"], 1)
            self.assertEqual(actual["hashes"], 2)
            self.assertEqual(actual["inspect"], 1)
            self.assertEqual((actual["forks"], actual["stops"], actual["consumes"]), (1, 1, 1))
        else:
            self.assertTrue(golden.startswith(report))
            if not expected["output_attempts"]:
                self.assertFalse(report)

    def check_order(self, case, actual):
        events = [row[0] for row in actual["events"]]
        for earlier, later in (("guard", "presence"), ("presence", "closefrom"), ("closefrom", "reserve"),
                               ("reserve", "entry-boundary"), ("entry-boundary", "capture"), ("capture", "authenticated"),
                               ("authenticated", "prepare"), ("prepare", "signature")):
            if earlier in events and later in events:
                self.assertLess(max(i for i, n in enumerate(events) if n == earlier), events.index(later), case["name"])
        for name in ("entry-final", "fused-final", "signature-final"):
            self.assertLessEqual(events.count(name), 1)
        if "entry-final" in events:
            self.assertEqual(events[-1], "entry-final")
        if "entry-release" in events and "entry-final" in events:
            self.assertLess(events.index("entry-release"), events.index("entry-final"))
        # Prefix clock witnesses surround every fixed query and precede every
        # later dependent handoff. Cancellation can refuse before sampling.
        for i, name in enumerate(events):
            if name in ("presence", "closefrom", "entry-boundary", "authenticated"):
                self.assertEqual(events[i - 1], "prefix-clock", (case["name"], name))

    def test_independent_finite_ownership_corpus(self):
        self.assertEqual(len(vectors.CASES), vectors.CASE_COUNT)
        self.assertEqual(len({c["name"] for c in vectors.CASES}), 139)
        for case in vectors.CASES:
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_symbols_and_full_component_accounting(self):
        undefined = {row.split()[-1] for row in self.execute(["nm", "-u", str(self.plain)]).decode().splitlines()}
        self.assertLessEqual({n for n in undefined if "asan" not in n and "ubsan" not in n}, {
            "_bw_sv_supported", "_bw_material_reserve_started", "_bw_entry_boundary", "_bw_auth_capture_reserved",
            "_bw_authenticated_output", "_bw_verifier_release", "_bw_sv_closefrom", "_bw_sv_parent_boundary",
            "_clock_gettime", "_getuid", "_geteuid", "_getgid", "_getegid", "_getitimer", "_setitimer", "_getrlimit", "_setrlimit",
            "_sigemptyset", "_sigaddset", "_sigaction", "_sigprocmask", "_fcntl", "_close", "___error", "__exit",
            "_memset", "_bzero", "_memcpy", "___stack_chk_fail", "___stack_chk_guard"})
        exports = {row.split()[-1] for row in self.execute(["nm", "-gU", str(self.plain)]).decode().splitlines()
                   if "asan" not in row and "ubsan" not in row}
        self.assertEqual(exports, {"_bw_process_guard", "_bw_start_reserve", "_bw_entry_output", "_bw_entry_observe", "_bw_entry_live"})
        platform_names = {row.split()[-1] for row in self.execute(["nm", "-u", str(self.platform)]).decode().splitlines()}
        self.assertFalse({n for n in platform_names if "asan" not in n and "ubsan" not in n})
        self.assertEqual(next(row for row in (ROOT / "Makefile").read_text().splitlines() if row.startswith("PLAN_SOURCES =")),
                         "PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c")
        self.assertLessEqual(sum(p.stat().st_size for p in (ROOT / "src").rglob("*") if p.suffix in (".c", ".h")), 256 * 1024)
        for cap, names in ((24, ("trusted_start.c", "trusted_start.h", "entry_platform.c", "entry_native.c", "entry_openbsd.c", "entry_cli.c", "private_entry.h", "private_cli.h", "main.c", "private_limits.h", "private_start_material.h",
            "install_entry.c", "install_platform.c", "private_install.h", "install_profile.h")),
            (18, ("auth_material.c", "auth_material.h", "private_start_material.h")),
            (18, ("signature_verify.c", "signature_verify.h", "signature_platform.c", "private_limits.h")),
            (8, ("auth_input.c", "auth_input.h")),
            (16, ("authenticated_review.c", "authenticated_review.h", "private_span.h", "private_content_format.h"))):
            self.assertLessEqual(sum((ROOT / "src" / n).stat().st_size for n in names), cap * 1024, names)
