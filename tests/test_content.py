# SPDX-License-Identifier: BSD-3-Clause
"""Finite pure composition checks against separately authored data oracles."""
import hashlib
import json
from pathlib import Path
import struct
import sys
import tempfile
import unittest

import content_vectors as vectors
import test_policy

ROOT = Path(__file__).resolve().parents[1]
CAUSES = {name: n for n, name in enumerate(("NONE", "ARGUMENT", "POLICY", "STRUCTURE", "FORMAT"))}
POLICY_CAUSES = {name: n for n, name in enumerate(("NONE", "ARGUMENT", "POLICY", "KEY",
    "HASH_UNAVAILABLE", "DEFINITION", "SYNTAX", "TUPLE", "PAYLOAD"))}


class ContentTests(unittest.TestCase):
    # The unchanged reviewed launcher supplies live stream caps, child custody,
    # finite compiler/driver limits and retained publication dispositions.
    execute = classmethod(test_policy.PolicyTests.execute.__func__)

    @classmethod
    def setUpClass(cls):
        if sys.platform != "darwin":
            raise RuntimeError("Mac portable content fixture only; no native dispatch")
        cls.directory = Path(tempfile.mkdtemp(prefix="content-build-", dir=ROOT / ".tools"))
        cls.ledger = cls.directory / "observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1",
                   "ASAN_OPTIONS": "detect_leaks=0:abort_on_error=1"}
        cls.flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
                     "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g", "-I", str(ROOT / "src")]
        cls.calls = cls.builds = 0
        cls.cache, cls.quarantine, cls.objects = {}, [], []
        for name in ("policy_binding", "plan", "auth_material", "payload_review", "policy_data", "policy_platform"):
            target = cls.directory / (name + ".o")
            cls.execute(cls.flags + ["-c", str(ROOT / "src" / (name + ".c")), "-o", str(target)], compile_job=True)
            if name not in ("policy_data", "policy_platform"):
                cls.objects.append(str(target))
        cls.plain = cls.directory / "content_review.o"
        cls.instrumented = cls.directory / "content_instrumented.o"
        cls.execute(cls.flags + ["-c", str(ROOT / "src/content_review.c"), "-o", str(cls.plain)], compile_job=True)
        cls.execute(cls.flags + ["-Dbw_policy_bind=bw_c_policy", "-Dbw_payload_structure_review=bw_c_structure",
                    "-Dsnprintf=bw_c_snprintf", "-c", str(ROOT / "src/content_review.c"),
                    "-o", str(cls.instrumented)], compile_job=True)
        cls.driver, cls.native_driver = cls.directory / "driver.o", cls.directory / "native-driver.o"
        cls.execute(cls.flags + ["-c", str(ROOT / "tests/content_driver.c"), "-o", str(cls.driver)], compile_job=True)
        cls.execute(cls.flags + ["-DCR_NATIVE", "-c", str(ROOT / "tests/content_driver.c"),
                    "-o", str(cls.native_driver)], compile_job=True)
        cls.default = cls.directory / "inert-default"
        cls.execute(cls.flags + cls.objects + [str(cls.instrumented), str(cls.driver),
                    str(cls.directory / "policy_data.o"), "-o", str(cls.default)], compile_job=True)

    @classmethod
    def linked(cls, policy, adapter="instrumented"):
        source = '#include "policy_binding.h"\nconst struct bw_policy_definition bw_compiled_policy = {\n' + ",\n".join(
            [str(policy["profile"]), str(policy["key_length"])] +
            [test_policy.array(policy[x]) for x in ("key", "digest", "id", "release", "arch")]) + "\n};\n"
        identity = hashlib.sha256((adapter + source).encode()).hexdigest()
        if identity in cls.cache:
            return cls.cache[identity]
        cls.builds += 1
        if cls.builds > 64:
            raise AssertionError("finite link-time content policy cap")
        path = cls.directory / f"policy-{cls.builds:02d}.c"
        path.write_text(source)
        binary = path.with_suffix("")
        extra = [str(cls.plain if adapter == "plain" else cls.instrumented),
                 str(cls.native_driver if adapter == "native" else cls.driver)]
        if adapter == "native":
            extra.append(str(cls.directory / "policy_platform.o"))
        cls.execute(cls.flags + cls.objects + extra + [str(path), "-o", str(binary)], compile_job=True)
        cls.cache[identity] = binary
        return binary

    def run_case(self, case, mode="normal", capacity=2048, binary=None):
        parts = [case[x] for x in ("key", "definition", "payload")]
        frame = struct.pack(">III", *(len(x) for x in parts)) + b"".join(parts)
        return self.execute([str(binary or self.linked(case["policy"])), mode, str(capacity)], frame)

    def check(self, case, expected=None, **kwargs):
        expected = expected or case["expected"]
        data = self.run_case(case, **kwargs)
        header, rest = data.split(b"\n", 1)
        self.assertTrue(header.startswith(b"CR_RESULT "), case["name"])
        values = list(map(int, header.split()[1:]))
        e, calls = expected, expected["calls"]
        self.assertEqual(values[:8], [e["return"], CAUSES[e["cause"]], POLICY_CAUSES[e["policy_cause"]],
            e["length"], calls["policy"], calls["hash"], calls["inspect"], calls["format"]], case["name"])
        self.assertEqual(values[8:12], [int(e["cleared"]), 1, 1, 1], case["name"])
        capacity = kwargs.get("capacity", 2048)
        if e["output_disposition"] == "zeroed":
            self.assertEqual(values[12], 1)
        if e["return"]:
            model_line, report = rest.split(b"\n", 1)
            model = e["model"]
            self.assertEqual(model_line, b"CR_MODEL " + b" ".join(model[x].encode().hex().encode()
                for x in ("id", "name", "release", "architecture", "file", "sha256")))
            self.assertEqual(report, e["report"])
            self.assertEqual(report.count(b"\n"), 18)
            self.assertNotIn(b"\n\n", report)
            self.assertEqual(values[13:], [e["summary"][x] for x in ("members", "files", "content_bytes")])
        else:
            self.assertEqual(rest, b"")
            self.assertEqual(values[13:], [0, 0, 0])
            if not capacity or capacity > 65536:
                # outside-span witness covers all bytes for invalid capacities.
                self.assertEqual(values[11], 1)
        return data

    def test_independent_complete_reports_and_models(self):
        for case in vectors.valid_cases():
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_exact_pinned_structure_failures_publish_nothing(self):
        for case in vectors.structure_cases():
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_policy_and_preflight_failures_never_inspect(self):
        for case in vectors.policy_cases():
            with self.subTest(case=case["name"]):
                mode = "hashfail" + str(case["hash_fail_at"]) if case.get("hash_fail_at") else "normal"
                self.check(case, mode=mode)

    def test_capacity_and_format_failures_clear_complete_model(self):
        base = vectors.valid_cases()[0]
        for request in vectors.formatting_cases():
            with self.subTest(case=request["name"]):
                self.check(base, request["expected"], capacity=request["capacity"], mode=request["fault"])
        self.check(base, capacity=65536)
        self.check(base, vectors.formatting_cases()[2]["expected"], capacity=0, mode="null-zero")

    def test_all_21_aliases_and_invalid_numeric_ranges_never_write(self):
        base = vectors.valid_cases()[0]
        modes = ["-".join(pair) for pair in vectors.ALIAS_PAIRS]
        self.assertEqual(len(modes), 21)
        modes += [f"{kind}-{name}" for kind in ("null", "overflow")
                  for name in ("request", "output", "result", "key", "definition", "payload")]
        for mode in modes:
            self.assertEqual(self.run_case(base, mode=mode), b"CR_NOWRITE 0 0 0 0 0 1 1 1\n", mode)
        # Invalid capacity cannot weaken the earlier no-write range guard.
        for mode in ("request-result", "overflow-payload", "output-policy"):
            self.assertEqual(self.run_case(base, mode=mode, capacity=65537), b"CR_NOWRITE 0 0 0 0 0 1 1 1\n")

    def test_repeat_success_failure_success_and_late_format_failure(self):
        base = vectors.valid_cases()[0]
        for mode, middle in (("repeat", b"CR_RESULT 0 2 8 0 1 2 0 0 1 1 1 1 1 0 0 0"),
                             ("repeat-format", b"CR_RESULT 0 4 0 0 1 2 1 1 1 1 1 1 1 0 0 0")):
            data = self.run_case(base, mode=mode)
            chunks = data.split(b"CR_RESULT ")
            self.assertEqual(len(chunks), 4)
            self.assertEqual(chunks[1], chunks[3])
            self.assertEqual(b"CR_RESULT " + chunks[2].rstrip(b"\n"), middle)
            self.assertTrue(chunks[1].endswith(base["expected"]["report"]))

    def test_uninstrumented_composition_and_unavailable_native_adapter(self):
        base = vectors.valid_cases()[0]
        expected = dict(base["expected"], calls={"policy": 0, "hash": 2, "inspect": 0, "format": 0})
        self.check(base, expected, binary=self.linked(base["policy"], "plain"))
        expected = {"return": 0, "cause": "POLICY", "policy_cause": "HASH_UNAVAILABLE", "length": 0,
                    "report": b"", "model": None, "summary": None, "cleared": True,
                    "output_disposition": "zeroed", "calls": {"policy": 1, "hash": 0, "inspect": 0, "format": 0}}
        self.check(base, expected, binary=self.linked(base["policy"], "native"))
        inert = {"name": "unchanged-inert-policy", "key": test_policy.KEY,
                 "definition": test_policy.DEFINITION, "payload": test_policy.PAYLOAD}
        expected = dict(expected, cause="STRUCTURE", policy_cause="NONE",
                        calls={"policy": 1, "hash": 2, "inspect": 1, "format": 0})
        self.check(inert, expected, binary=self.default)

    def test_production_linkage_symbols_and_scoped_storage_budgets(self):
        symbols = self.execute(["nm", "-u", str(self.plain)]).decode()
        # Sanitizer instrumentation is test-only and intentionally retained.
        names = {line.split()[-1] for line in symbols.splitlines()}
        self.assertLessEqual({x for x in names if "asan" not in x and "ubsan" not in x}, {
            "_bw_compiled_policy", "_bw_policy_bind", "_bw_payload_structure_review",
            "_snprintf", "_memcpy", "_memset", "_bzero", "___stack_chk_fail", "___stack_chk_guard"})
        makefile = (ROOT / "Makefile").read_text()
        self.assertEqual(next(x for x in makefile.splitlines() if x.startswith("PLAN_SOURCES =")),
                         "PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c")
        self.assertLessEqual(sum((ROOT / "src" / n).stat().st_size for n in
            ("content_review.c", "content_review.h", "private_span.h", "private_content_format.h")), 8192)
        self.assertLessEqual(sum(p.stat().st_size for p in (ROOT / "src").rglob("*") if p.suffix in (".c", ".h")), 256 * 1024)
