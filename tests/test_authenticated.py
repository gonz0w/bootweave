# SPDX-License-Identifier: BSD-3-Clause
"""Finite inert composition checks against separately frozen semantic oracles.

The reviewed Policy launcher owns direct children and retained streams.  The
outer suite owns the final terminal process check; this ledger never discards
birth witnesses because a result could not be decoded.  No native dispatch.
"""
import hashlib
import errno
import json
import os
from pathlib import Path
import stat
import struct
import sys
import tempfile
import unittest

import authenticated_vectors as vectors
import test_policy
import test_signature

ROOT = Path(__file__).resolve().parents[1]
CAUSES = {s: n for n, s in enumerate(("NONE", "ARGUMENT", "POLICY", "MATERIAL",
    "SIGNATURE", "CONTENT", "FORMAT", "CANCEL", "CLOCK", "DEADLINE", "OUTPUT"))}
MATERIAL_CAUSES = {s: n for n, s in enumerate(("NONE", "ARGUMENT", "RECORD", "CANCEL",
    "DEADLINE", "IO", "OWNERSHIP", "CLOSE"))}
SIGNATURE_CAUSES = {s: n for n, s in enumerate(("NONE", "UNSUPPORTED", "IDENTITY", "SIGNALS",
    "DESCRIPTOR", "CANCEL", "CLOCK", "DEADLINE", "IO", "FORK", "GROUP", "BOUNDARY",
    "GATE", "CHILD", "OUTPUT", "WAIT", "STOP", "CLOSE"))}
CONTENT_CAUSES = {s: n for n, s in enumerate(("NONE", "ARGUMENT", "POLICY", "STRUCTURE", "FORMAT"))}
POLICY_CAUSES = {s: n for n, s in enumerate(("NONE", "ARGUMENT", "POLICY", "KEY",
    "HASH_UNAVAILABLE", "DEFINITION", "SYNTAX", "TUPLE", "PAYLOAD"))}
RESULT_FIELDS = set(("return state cause material_cause signature_cause content_cause policy_cause length "
    "prepare signature content policy hash inspect content_format authenticated_format fused_release_calls "
    "signature_release_calls material_terminal_consumptions final_boundary_calls boundary_owner "
    "final_boundary_failed signature_final_boundary_failed path_or_process_calls_after_final_boundary "
    "close_retries bad_order prepare_identity span_identity descriptor_witness cleared output_zero outside "
    "no_write immutable reuse_ok material_state clock_calls wait_consumed wait_lost fake members files "
    "content_bytes output_attempted output_bytes sink_fcntl sink_fstat sink_isatty write_calls "
    "output_protocol write_order write_identity wire_hex write_trace").split())
WITNESS_PREFIXES = (b"SV_BORN ", b"SV_GROUP ", b"SV_SETGROUP ", b"SV_SETGROUP_FAILED ")


def parse(data):
    """Decode one private composed result; witness recovery is a separate step."""
    before, marker, rest = data.partition(b"AR_RESULT ")
    if not marker or any(not row.startswith(WITNESS_PREFIXES) for row in before.splitlines()):
        raise AssertionError("missing composition result or unexpected pre-result output")
    header, separator, remaining = rest.partition(b"\n")
    if not separator:
        raise AssertionError("truncated composition result")
    result = json.loads(header.decode("ascii"))
    if set(result) != RESULT_FIELDS or any(type(v) is not int for k, v in result.items()
                                         if k not in ("boundary_owner", "wire_hex", "write_trace")):
        raise AssertionError("unexpected composition result schema")
    if result["boundary_owner"] not in ("caller", "none", "fused", "signature"):
        raise AssertionError("invalid boundary owner")
    bytes.fromhex(result["wire_hex"])
    if len(result["write_trace"]) > 4 or any(len(row) != 4 or any(type(n) is not int for n in row)
                                            for row in result["write_trace"]):
        raise AssertionError("invalid modeled write trace")
    model = None
    if result["return"] == vectors.RETURNS["COMPLETE"]:
        model_line, separator, remaining = remaining.partition(b"\n")
        if not separator or not model_line.startswith(b"AR_MODEL "):
            raise AssertionError("missing complete model")
        fields = model_line.split()[1:]
        if len(fields) != 6:
            raise AssertionError("invalid complete model width")
        model = dict(zip(("id", "name", "release", "architecture", "file", "sha256"),
                         (bytes.fromhex(x.decode("ascii")).decode("ascii") for x in fields)))
    elif remaining:
        raise AssertionError("refusal published model, diagnostics or report")
    return result, model, remaining


def inventory(directory):
    rows = []
    for path in sorted(directory.rglob("*")):
        if len(rows) >= 16:
            raise AssertionError("finite retained material inventory cap")
        info = path.lstat()
        row = {"path": str(path.relative_to(directory)), "device": info.st_dev,
               "inode": info.st_ino, "mode": info.st_mode, "size": info.st_size,
               "links": info.st_nlink}
        if stat.S_ISREG(info.st_mode):
            if info.st_size > 65536:
                raise AssertionError("retained inert material size cap")
            row["sha256"] = hashlib.sha256(path.read_bytes()).hexdigest()
        if stat.S_ISLNK(info.st_mode):
            row["link"] = os.readlink(path)
        rows.append(row)
    return rows


class AuthenticatedTests(unittest.TestCase):
    # Reuse the existing bounded, held-wait launcher without inheriting its tests.
    execute = classmethod(test_policy.PolicyTests.execute.__func__)

    @classmethod
    def setUpClass(cls):
        if sys.platform != "darwin":
            raise RuntimeError("Mac portable composition fixture only; no native dispatch")
        cls.directory = Path(tempfile.mkdtemp(prefix="authenticated-build-", dir=ROOT / ".tools"))
        cls.ledger = cls.directory / "observations.jsonl"
        cls.composition_ledger = cls.directory / "composition-observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1",
                   "ASAN_OPTIONS": "detect_leaks=0:abort_on_error=1"}
        cls.flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
                     "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g", "-I", str(ROOT / "src")]
        cls.calls = cls.builds = cls.invocations = 0
        cls.cache, cls.quarantine, cls.objects = {}, [], []
        manifest = {str(p.relative_to(ROOT)): {"bytes": p.stat().st_size,
                    "sha256": hashlib.sha256(p.read_bytes()).hexdigest()}
                    for p in sorted([*ROOT.joinpath("src").glob("*.[ch]"),
                        *(ROOT / "tests" / n for n in ("authenticated_driver.c", "test_authenticated.py",
                                                       "authenticated_vectors.py", "signature_driver.c",
                                                       "test_policy.py", "test_signature.py"))])}
        (cls.directory / "source-manifest.json").write_text(json.dumps(manifest, sort_keys=True) + "\n")
        replacements = ["clock_gettime", "open", "pipe", "fcntl", "lseek", "fstat", "close",
                        "fork", "setpgid", "dup2", "write", "read", "poll", "waitid", "wait4",
                        "kill", "getuid", "execve"]
        sources = [(n, "src/" + n + ".c", []) for n in
                   ("policy_binding", "plan", "payload_review", "auth_input")]
        sources += [
            ("material", "src/auth_material.c", ["-Dbw_verifier_release=bw_ar_real_release",
                "-Dmkdtemp=bw_sv_test_mkdtemp", "-Dclose=bw_ar_material_close", "-Dunlinkat=bw_ar_unlinkat"]),
            ("signature", "src/signature_verify.c", [f"-D{x}=bw_v_{x}" for x in replacements]),
            ("content", "src/content_review.c", ["-Dbw_policy_bind=bw_ar_policy",
                "-Dbw_payload_structure_review=bw_ar_structure", "-Dsnprintf=bw_ar_content_snprintf"]),
            ("authenticated", "src/authenticated_review.c", ["-Dclock_gettime=bw_ar_clock_gettime",
                "-Dbw_verifier_prepare=bw_ar_prepare", "-Dbw_signature_verify=bw_ar_signature",
                "-Dbw_content_review=bw_ar_content", "-Dbw_sv_parent_boundary=bw_ar_early_boundary",
                "-Dsnprintf=bw_ar_snprintf", "-Dmemcpy=bw_ar_memcpy",
                "-Dfcntl=bw_ar_output_fcntl", "-Dfstat=bw_ar_output_fstat",
                "-Disatty=bw_ar_output_isatty", "-Dwrite=bw_ar_output_write"]),
        ]
        for name, source, defines in sources:
            target = cls.directory / (name + ".o")
            cls.execute(cls.flags + defines + ["-c", str(ROOT / source), "-o", str(target)], compile_job=True)
            cls.objects.append(str(target))
        cls.plain, cls.platform = cls.directory / "authenticated-production.o", cls.directory / "platform.o"
        cls.driver, cls.unsupported_driver = cls.directory / "driver.o", cls.directory / "unsupported-driver.o"
        for source, target, defines in (("src/authenticated_review.c", cls.plain, []),
                ("src/signature_platform.c", cls.platform, []),
                ("tests/authenticated_driver.c", cls.driver, []),
                ("tests/authenticated_driver.c", cls.unsupported_driver, ["-DTEST_REAL_PLATFORM"])):
            cls.execute(cls.flags + defines + ["-c", str(ROOT / source), "-o", str(target)], compile_job=True)

    @classmethod
    def linked(cls, policy, adapter="modeled"):
        if adapter not in ("modeled", "normal-unsupported"):
            raise AssertionError("unreviewed composition adapter")
        source = '#include "policy_binding.h"\nconst struct bw_policy_definition bw_compiled_policy = {\n' + ",\n".join(
            [str(policy["profile"]), str(policy["key_length"])] +
            [test_policy.array(policy[x]) for x in ("key", "digest", "id", "release", "arch")]) + "\n};\n"
        identity = hashlib.sha256((adapter + source).encode()).hexdigest()
        if identity not in cls.cache:
            cls.builds += 1
            if cls.builds > 64:
                raise AssertionError("finite composition policy cap")
            path = cls.directory / f"policy-{cls.builds:02d}.c"
            path.write_text(source)
            binary = path.with_suffix("")
            extra = [str(cls.driver)] if adapter == "modeled" else [str(cls.unsupported_driver), str(cls.platform)]
            cls.execute(cls.flags + cls.objects + extra + [str(path), "-o", str(binary)], compile_job=True)
            cls.cache[identity] = binary
        return cls.cache[identity]

    def run_case(self, request):
        case = dict(vectors.valid_cases()[0], **request)
        policy = dict(case["policy"], **case.get("policy_change", {}))
        binary = self.linked(policy, case.get("adapter", "modeled"))
        type(self).invocations += 1
        directory = self.directory / f"material-case-{self.invocations:03d}"
        directory.mkdir(mode=0o700)
        fault = "hashfail" + str(case["hash_fail_at"]) if case.get("hash_fail_at") else case.get("fault", "normal")
        capacity = case.get("capacity", 2048)
        command = [str(binary), str(directory), fault, str(capacity), case.get("phase", "none"),
                   case.get("origin", "reservation"), str(case.get("offset_nanoseconds", 0)),
                   str(case.get("pure_origin_offset_nanoseconds") if
                       case.get("pure_origin_offset_nanoseconds") is not None else -1),
                   str(case.get("length", -1))]
        if case.get("destination") == "stdout":
            command.append(case["sink"])
        parts = [case[x] for x in ("key", "signature", "definition", "payload")]
        frame = struct.pack(">IIII", *(len(x) for x in parts)) + b"".join(parts)
        stdout = b""
        error = parse_error = inventory_error = stream_recovery_error = parsed = model = None
        report = b""
        rows = []
        stream_directory = None
        try:
            stream_directory = self.directory / f"case-{type(self).calls + 1:03d}"
            stdout = self.execute(command, frame)
        except BaseException as caught:
            error = caught
        finally:
            # execute retains streams before raising; recover witnesses from that
            # exact stream even if its custody/exit/publication check failed.
            try:
                if stream_directory is not None and (stream_directory / "stdout").is_file():
                    stdout = (stream_directory / "stdout").read_bytes()
            except BaseException as caught:
                stream_recovery_error = type(caught).__name__ + ": " + str(caught)[:1024]
            born, groups, setgroups, failedgroups, witness_errors = test_signature.witnesses(stdout)
            try:
                parsed, model, report = parse(stdout)
            except BaseException as caught:
                parse_error = type(caught).__name__ + ": " + str(caught)[:1024]
            try:
                rows = inventory(directory)
            except BaseException as caught:
                inventory_error = type(caught).__name__ + ": " + str(caught)[:1024]
            row = {"case": str(directory), "name": case["name"], "fault": fault,
                   "adapter": case.get("adapter", "modeled"), "phase": case.get("phase"),
                   "stream_case": str(stream_directory), "result": parsed,
                   "born": born, "groups": groups, "setgroups": setgroups, "failedgroups": failedgroups,
                   "witness_errors": witness_errors, "parse_error": parse_error,
                   "error_type": type(error).__name__ if error else None,
                   "stream_recovery_error": stream_recovery_error,
                   "error": str(error)[:2048] if error else None,
                   "stdout_bytes": len(stdout), "stdout_sha256": hashlib.sha256(stdout).hexdigest(),
                   "report_bytes": len(report), "report_sha256": hashlib.sha256(report).hexdigest(),
                   "inventory": rows, "inventory_error": inventory_error}
            with self.composition_ledger.open("a") as ledger:
                encoded = json.dumps(row, sort_keys=True) + "\n"
                if ledger.write(encoded) != len(encoded):
                    raise AssertionError("composition ledger publication incomplete")
                ledger.flush()
        if error:
            raise error
        self.assertIsNone(stream_recovery_error, row)
        self.assertIsNone(parse_error, row)
        self.assertIsNone(inventory_error, row)
        self.assertFalse(witness_errors, row)
        return case, parsed, model, report, row

    def check(self, request):
        case, actual, model, report, row = self.run_case(request)
        expected = case["expected"]
        self.assertEqual(actual["return"], expected["return"], case["name"])
        self.assertEqual(report, expected["report"], case["name"])
        self.check_fixed_output(case, actual)
        for name, count in expected["calls"].items():
            self.assertEqual(actual[name], count, (case["name"], name))
        for name in ("fused_release_calls", "signature_release_calls", "material_terminal_consumptions",
                     "final_boundary_calls", "boundary_owner"):
            self.assertEqual(actual[name], expected[name], (case["name"], name))
        for name in ("path_or_process_calls_after_final_boundary", "close_retries", "bad_order"):
            self.assertEqual(actual[name], 0, (case["name"], name))
        for name in ("prepare_identity", "span_identity", "outside", "reuse_ok"):
            self.assertEqual(actual[name], 1, (case["name"], name))
        # No operation implementation is linked; the production symbol allowlist
        # below closes that boundary and complete reports independently state it.
        self.assertEqual(expected["effects"], vectors.ZERO_EFFECTS)
        if expected["return"] == vectors.RETURNS["NOT_TAKEN"]:
            self.assertEqual(actual["no_write"], 1, case["name"])
            self.assertEqual(actual["clock_calls"], 0, case["name"])
            self.assertFalse(row["born"], case["name"])
            self.assertEqual(actual["immutable"], 1, case["name"])
            return
        self.assertEqual(actual["state"], 2, case["name"])
        for name, causes in (("cause", CAUSES), ("material_cause", MATERIAL_CAUSES),
                ("signature_cause", SIGNATURE_CAUSES), ("content_cause", CONTENT_CAUSES),
                ("policy_cause", POLICY_CAUSES)):
            self.assertEqual(actual[name], causes[expected[name]], (case["name"], name))
        self.assertEqual(actual["length"], expected["length"], case["name"])
        self.assertEqual(actual["final_boundary_failed"], expected["final_boundary_failed"])
        self.assertEqual(actual["signature_final_boundary_failed"], int(case.get("fault") == "final-boundary"))
        self.assertEqual(bool(row["born"]), expected["live_child"], case["name"])
        if case.get("nonspawning"):
            self.assertFalse(row["born"])
            self.assertEqual(actual["fake"], 1)
        else:
            self.assertEqual(actual["fake"], 0)
        if row["born"]:
            self.assertEqual(len(row["born"]), 1)
            self.assertTrue(row["groups"])
            self.assertTrue(all(group[0] == row["born"][0][0] for group in row["groups"]))
            self.assertEqual(len(row["setgroups"]), 1)
            self.assertEqual(row["setgroups"][0], [row["born"][0][0]] * 2)
            self.assertFalse(row["failedgroups"])
            self.assertEqual(actual["wait_consumed"], 1)
            self.assertEqual(actual["wait_lost"], 0)
        if expected["live_child"] and case.get("fault") != "material-close-0":
            self.assertEqual(actual["descriptor_witness"], 1, case["name"])
        mutation = case.get("fault", "").startswith("mutate-")
        if not mutation:
            self.assertEqual(actual["immutable"], 1, case["name"])
        if expected["output_disposition"] in ("zeroed", "unchanged"):
            self.assertEqual(actual["output_zero"], 1, case["name"])
            self.assertEqual(actual["cleared"], 1, case["name"])
            self.assertIsNone(model)
            self.assertEqual([actual[n] for n in ("members", "files", "content_bytes")], [0, 0, 0])
        else:
            self.assertEqual(model, expected["model"], case["name"])
            self.assertEqual(report.count(b"\n"), 18)
            self.assertNotIn(b"\n\n", report)
            self.assertEqual([actual[n] for n in ("members", "files", "content_bytes")],
                             [expected["summary"][n] for n in ("members", "files", "content_bytes")])
        if case.get("fault") == "prepare-unlink-error":
            self.assertTrue(row["inventory"], case["name"])
        elif not expected["quarantine_required"]:
            self.assertFalse(row["inventory"], case["name"])

    def check_fixed_output(self, case, actual):
        expected = case["expected"]
        for key in ("output_protocol", "write_order", "write_identity"):
            self.assertEqual(actual[key], 1, (case["name"], key))
        if case.get("destination") != "stdout":
            for key in ("output_attempted", "output_bytes", "sink_fcntl", "sink_fstat", "sink_isatty", "write_calls"):
                self.assertEqual(actual[key], 0, (case["name"], key))
            self.assertEqual(actual["wire_hex"], "")
            self.assertEqual(actual["write_trace"], [])
            return
        golden = case["expected"]["report"] if expected["return"] == 1 else vectors.valid_cases()[0]["expected"]["report"]
        trace, offset = [], 0
        for outcome in expected["write_outcomes"]:
            offered = len(golden) - offset
            error = 0
            if outcome == "all":
                returned = offered
            elif outcome == "oversized":
                returned = offered + 1
            elif outcome in ("EINTR", "EAGAIN", "EIO"):
                returned, error = -1, getattr(errno, outcome)
            else:
                returned = outcome
            trace.append([offered, returned, error, offset])
            if 0 < returned <= offered:
                offset += returned
        self.assertEqual(actual["write_trace"], trace, case["name"])
        self.assertEqual(actual["write_calls"], len(trace), case["name"])
        self.assertEqual(actual["output_attempted"], expected["output_attempted"], case["name"])
        self.assertEqual(actual["output_bytes"], offset, case["name"])
        self.assertEqual(bytes.fromhex(actual["wire_hex"]), golden[:offset], case["name"])
        self.assertEqual(tuple(actual[k] for k in ("sink_fcntl", "sink_fstat", "sink_isatty")), expected["sink_queries"])
        # The external sibling never touches the memory destination/canaries.
        self.assertEqual(actual["output_zero"], 1, case["name"])

    def test_independent_complete_reports_and_models(self):
        for case in vectors.valid_cases():
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_all_15_aliases_invalid_ranges_and_states_never_write(self):
        self.assertEqual(len(vectors.ALIAS_PAIRS), 15)
        for case in vectors.no_write_cases():
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_argument_and_compiled_policy_refusals_consume_once(self):
        for case in (*vectors.argument_cases(), *vectors.policy_cases()):
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_signature_acceptance_cannot_bypass_real_content(self):
        for case in vectors.content_cases():
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_later_consumers_recompute_exact_original_spans(self):
        for case in vectors.mutation_cases():
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_phase_cancellation_clock_and_original_budgets(self):
        for case in vectors.phase_cases():
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_single_owner_refusal_uncertainty_and_boundary(self):
        for case in vectors.ownership_cases():
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_capacity_formatting_and_maximum_publication_span(self):
        for case in vectors.formatting_cases():
            with self.subTest(case=case["name"]):
                self.check(case)
        self.check(dict(vectors.valid_cases()[0], name="maximum-output-span", capacity=65536))

    def test_fixed_stdout_independent_publication_and_refusal_cases(self):
        for case in vectors.output_cases():
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_production_symbols_source_budgets_and_cli_separation(self):
        symbols = self.execute(["nm", "-u", str(self.plain)]).decode()
        names = {line.split()[-1] for line in symbols.splitlines()}
        self.assertLessEqual({n for n in names if "asan" not in n and "ubsan" not in n}, {
            "_bw_compiled_policy", "_bw_verifier_prepare", "_bw_verifier_release", "_bw_signature_verify",
            "_bw_content_review", "_bw_sv_parent_boundary", "_clock_gettime", "_snprintf", "_memcpy",
            "_memset", "_bzero", "_fcntl", "_fstat", "_isatty", "_write", "___error",
            "___stack_chk_fail", "___stack_chk_guard"})
        exported = self.execute(["nm", "-gU", str(self.plain)]).decode()
        public = {row.split()[-1] for row in exported.splitlines()}
        self.assertEqual({n for n in public if "asan" not in n and "ubsan" not in n},
                         {"_bw_authenticated_review", "_bw_authenticated_output"})
        makefile = (ROOT / "Makefile").read_text()
        self.assertEqual(next(row for row in makefile.splitlines() if row.startswith("PLAN_SOURCES =")),
                         "PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c")
        self.assertLessEqual(sum((ROOT / "src" / n).stat().st_size for n in
            ("authenticated_review.c", "authenticated_review.h", "private_span.h", "private_content_format.h")), 16 * 1024)
        self.assertLessEqual(sum(p.stat().st_size for p in (ROOT / "src").rglob("*")
                                 if p.suffix in (".c", ".h")), 100 * 1024)
