# SPDX-License-Identifier: BSD-3-Clause
"""Portable source sealing/syntax/host-refusal only; zero native cases passed."""
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import socket
import stat
import sys
import tempfile
import threading
import time
import unittest
from unittest import mock

import test_policy

ROOT = Path(__file__).resolve().parents[1]
ORACLES_SHA256 = "610a161c0c0d64bbebdd35d1584a0ec5234186bec3b2c453f50ffbbf40381f73"
TRACE_ORACLES_SHA256 = "7684d2ddca3369af32ffecba33e3de53369dc9856a2a301433e6c38f8cf08b47"
ORACLE_GATES_SHA256 = "089022ea7d7ca37a903fcb7c0ef84e18c35762bb9eb6cb48813a2ecfdb140131"


class QualificationPreparationTests(unittest.TestCase):
    execute = classmethod(test_policy.PolicyTests.execute.__func__)

    @classmethod
    def setUpClass(cls):
        if sys.platform != "darwin":
            raise RuntimeError("Portable source preparation only; no native dispatch")
        spec = importlib.util.spec_from_file_location("nq_source_preparation", ROOT / "tests/qualification/prepare.py")
        cls.preparation = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(cls.preparation)
        cls.directory = Path(tempfile.mkdtemp(prefix="qualification-build-", dir=ROOT / ".tools"))
        cls.ledger = cls.directory / "observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1"}
        cls.calls = cls.builds = cls.invocations = 0
        cls.cache, cls.quarantine = {}, []
        cls.flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic", "-I", str(ROOT / "src")]
        cls.source_files, cls.preview = cls.preparation.sources()
        files = {**cls.source_files, "tests/test_qualification_preparation.py": Path(__file__).read_bytes()}
        (cls.directory / "source-manifest.json").write_text(json.dumps({name:
            {"bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()}
            for name, data in sorted(files.items())}, sort_keys=True) + "\n")

    def source_root(self, omit=None):
        root = Path(tempfile.mkdtemp(prefix="source-fixture-", dir=self.directory))
        (root / ".tools").mkdir(mode=0o700)
        for name, data in self.source_files.items():
            if name == omit:
                continue
            path = root / name
            path.parent.mkdir(mode=0o700, parents=True, exist_ok=True)
            path.write_bytes(data)
        return root

    def test_independent_literal_catalogue_and_runtime(self):
        data = self.source_files["tests/qualification/oracles.json"]
        self.assertEqual(hashlib.sha256(data).hexdigest(), ORACLES_SHA256)
        oracle = json.loads(data)
        self.assertEqual(oracle["case_count"], 9)
        self.assertEqual(len(oracle["cases"]), 9)
        self.assertEqual(len({row["id"] for row in oracle["cases"]}), 9)
        self.assertEqual(len(oracle["entry_record"]["bounded_numeric_fields"]), 29)
        self.assertEqual(oracle["authority"]["positive_authentication_cases"], 0)
        self.assertFalse(oracle["authority"]["trace_interposer"])
        binding = json.loads(self.source_files["tests/qualification/runtime-bindings.json"])
        self.assertEqual(len(binding["files"]), 34)
        self.assertEqual(sum(row["bytes"] for row in binding["files"].values()), 100932)
        self.assertLessEqual(binding["whole_runtime_bytes"], binding["whole_cap_bytes"])
        self.assertLessEqual(binding["entry_bytes_including_complete_shared_helpers"], binding["entry_cap_bytes"])
        self.assertIn("PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c\n", (ROOT / "Makefile").read_text())

    def test_seal_exact_private_source_and_data_only_build(self):
        root = self.source_root()
        before = {str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest()
                  for p in root.rglob("*") if p.is_file()}
        with mock.patch.object(self.preparation, "ROOT", root):
            bundle = self.preparation.prepare()
        manifest = json.loads((bundle / "SOURCE-MANIFEST.json").read_text())
        self.assertTrue(manifest["source_only"])
        expected = set(self.source_files) | {"BUILD-PREVIEW.json", "BUILD-JOBS.h", "ORACLE-GATES.h",
                                           "inputs/definition.bin", "inputs/signature.bin", "inputs/payload.bin"}
        self.assertEqual(set(manifest["files"]), expected)
        self.assertEqual(hashlib.sha256((bundle / "ORACLE-GATES.h").read_bytes()).hexdigest(),
                         ORACLE_GATES_SHA256)
        self.assertEqual({str(p.relative_to(bundle)) for p in bundle.rglob("*") if p.is_file()},
                         expected | {"SOURCE-MANIFEST.json"})
        for name, row in manifest["files"].items():
            path = bundle / name
            self.assertEqual(path.stat().st_size, row["bytes"])
            self.assertEqual(hashlib.sha256(path.read_bytes()).hexdigest(), row["sha256"])
            self.assertEqual(stat.S_IMODE(path.stat().st_mode), 0o600)
        self.assertTrue(all(stat.S_IMODE(p.stat().st_mode) == 0o700 for p in [bundle, *bundle.rglob("*")] if p.is_dir()))
        self.assertLessEqual(sum(p.stat().st_size for p in bundle.rglob("*") if p.is_file()), 1048576)
        for name, sha in before.items():
            self.assertEqual(hashlib.sha256((root / name).read_bytes()).hexdigest(), sha)
        build = json.loads((bundle / "BUILD-PREVIEW.json").read_text())
        self.assertEqual(len(build["jobs"]), 6)
        self.assertEqual(build["status"], "DATA_ONLY_NOT_EXECUTABLE_OR_AUTHORIZED")
        for job in build["jobs"]:
            self.assertEqual(job["argv"][0], "/usr/bin/cc")
            self.assertFalse(any(arg.startswith("-D") for arg in job["argv"]))
            self.assertNotIn("src/main.c", job["argv"])
            self.assertNotIn("src/entry_cli.c", job["argv"])
            if job["name"] not in ("closed-dynamic", "limits-dynamic"):
                self.assertEqual("src/entry_platform.c" in job["argv"], not job["name"].startswith("forwarded"))
            self.assertEqual("tests/qualification/forwarder.c" in job["argv"], job["name"].startswith("forwarded"))
        self.assertEqual(len(build["first_stage_jobs"]), 23)
        self.assertEqual(build["first_stage_jobs"][-3:], ["supervisor", "closed-dynamic", "limits-dynamic"])
        self.assertEqual(len(build["auxiliary_jobs"]), 21)
        by_name = {job["name"]: job for job in build["jobs"] + build["auxiliary_jobs"]}
        self.assertEqual(sum(by_name[name]["slot_seconds"] for name in build["first_stage_jobs"]), 950)
        self.assertEqual(build["first_stage_build_batch"]["file_ceiling_bytes"], 262144)
        self.assertEqual(120 + 950 + 130, build["first_stage_build_batch"]["wall_seconds"])
        self.assertEqual(len(build["trace_jobs"]), 5)
        for job in build["jobs"] + build["auxiliary_jobs"]:
            self.assertFalse(any(arg.startswith("-D") for arg in job["argv"]))
        renamed = [job for job in build["trace_jobs"] if "-Dgetrlimit=bw_nq_trace_getrlimit" in job["argv"]]
        self.assertEqual([job["name"] for job in renamed], ["trace-start", "trace-child"])
        wrapper = next(job for job in build["trace_jobs"] if job["name"] == "trace-limits")
        self.assertFalse(any(arg.startswith("-D") for arg in wrapper["argv"]))
        self.assertEqual((bundle / "inputs/signature.bin").read_bytes(), b"unsigned closed-entry fixture\n")

    def test_changed_runtime_refuses_before_output_namespace(self):
        root = self.source_root()
        path = root / "src/entry_platform.c"
        path.write_bytes(path.read_bytes() + b"\n")
        with mock.patch.object(self.preparation, "ROOT", root), self.assertRaisesRegex(ValueError, "Runtime bytes"):
            self.preparation.prepare()
        self.assertEqual(list((root / ".tools").iterdir()), [])

    def test_extra_runtime_refuses_before_output_namespace(self):
        root = self.source_root()
        (root / "src/extra.c").write_text("/* unexpected */\n")
        with mock.patch.object(self.preparation, "ROOT", root), self.assertRaisesRegex(ValueError, "Runtime closure"):
            self.preparation.prepare()
        self.assertEqual(list((root / ".tools").iterdir()), [])

    def test_symlink_helper_refuses_before_output_namespace(self):
        name = "tests/qualification/entry_probe.c"
        root = self.source_root(omit=name)
        (root / name).symlink_to(ROOT / name)
        with mock.patch.object(self.preparation, "ROOT", root), self.assertRaisesRegex(ValueError, "ordinary file"):
            self.preparation.prepare()
        self.assertEqual(list((root / ".tools").iterdir()), [])

    def test_oversized_helper_refuses_before_output_namespace(self):
        root = self.source_root()
        (root / "tests/qualification/entry_probe.c").write_bytes(b" " * 262145)
        with mock.patch.object(self.preparation, "ROOT", root), self.assertRaisesRegex(ValueError, "bounded ordinary file"):
            self.preparation.prepare()
        self.assertEqual(list((root / ".tools").iterdir()), [])

    def test_failed_output_retains_exact_partial_namespace(self):
        root = self.source_root()
        original_open, writes = self.preparation.os.open, []

        def interrupted(path, flags, *args):
            if flags & self.preparation.os.O_CREAT:
                writes.append(str(path))
                if len(writes) == 2:
                    raise OSError("synthetic source-write failure")
            return original_open(path, flags, *args)

        with mock.patch.object(self.preparation, "ROOT", root), mock.patch.object(self.preparation.os, "open", interrupted), \
                self.assertRaisesRegex(RuntimeError, "partial namespace retained"):
            self.preparation.prepare()
        bundles = list((root / ".tools").iterdir())
        self.assertEqual(len(bundles), 1)
        self.assertEqual([p.name for p in bundles[0].iterdir()], ["BUILD-JOBS.h"])
        self.assertEqual((bundles[0] / "BUILD-JOBS.h").read_bytes(),
                         self.preparation.jobs_header(self.preview))

    def test_native_body_syntax_and_unsupported_main_functions(self):
        objects = []
        wrapper = self.directory / "host-refusal-wrapper.c"
        wrapper.write_text('#include <stdio.h>\nint nq_probe_main(int,char **);\n'
                           'int main(int argc,char **argv) { int result=nq_probe_main(argc,argv);\n'
                           'printf("NQ_HOST_STATUS %d\\n",result); return result==77?0:1; }\n')
        for label in ("entry", "limit"):
            source = ROOT / ("tests/qualification/" + label + "_probe.c")
            self.execute(self.flags + ["-DBW_NQ_SOURCE_CHECK", "-fsyntax-only", str(source)], compile_job=True)
            obj = self.directory / (label + ".o")
            self.execute(self.flags + ["-Dmain=nq_probe_main", "-c", str(source), "-o", str(obj)], compile_job=True)
            objects.append(obj)
            binary = self.directory / label
            self.execute(self.flags + [str(obj), str(wrapper), "-o", str(binary)], compile_job=True)
            arguments = ([], ["extra"]) if label == "entry" else tuple([mode] for mode in (
                "parent-readback", "child-readback", "parent-file-size", "child-file-size", "unknown"))
            for args in arguments:
                self.assertEqual(self.execute([str(binary), *args]), b"NQ_HOST_STATUS 77\n")
        forward = self.directory / "forwarder.o"
        self.execute(self.flags + ["-c", str(ROOT / "tests/qualification/forwarder.c"), "-o", str(forward)], compile_job=True)
        objects.append(forward)
        for obj in objects:
            names = {row.split()[-1] for row in self.execute(["nm", "-u", str(obj)]).decode().splitlines()}
            self.assertLessEqual(names, {"___stack_chk_fail", "___stack_chk_guard"})
        check = self.directory / "host-forwarder.c"
        check.write_text('#include "private_entry.h"\n#include "private_start_material.h"\n#include <stdio.h>\n'
                         'int main(void) { int ready=bw_entry_available(); int result=bw_entry_boundary(0);\n'
                         'printf("NQ_FORWARD_REFUSAL %d %d\\n",ready,result); return ready==0&&result==-1?0:1; }\n')
        binary = self.directory / "forwarder"
        self.execute(self.flags + [str(check), str(forward), "-o", str(binary)], compile_job=True)
        self.assertEqual(self.execute([str(binary)]), b"NQ_FORWARD_REFUSAL 0 -1\n")

    def test_prepared_native_witnesses_refuse_on_host(self):
        (self.directory / "BUILD-JOBS.h").write_bytes(self.preparation.jobs_header(self.preview))
        header = self.preparation.oracle_header(self.source_files["tests/qualification/oracles.json"])
        self.assertEqual(hashlib.sha256(header).hexdigest(), ORACLE_GATES_SHA256)
        (self.directory / "ORACLE-GATES.h").write_bytes(header)
        flags = self.flags + ["-I", str(self.directory)]
        wrapper = self.directory / "guard-refusal-wrapper.c"
        wrapper.write_text('#include <stdio.h>\nint nq_probe_main(int,char **);\n'
                           'int main(int argc,char **argv) { int result=nq_probe_main(argc,argv);\n'
                           'printf("NQ_HOST_STATUS %d\\n",result); return result==77?0:1; }\n')
        for label, name in (("compiler", "compiler_guard.c"), ("supervisor", "native_supervisor.c")):
            source = ROOT / "tests/qualification" / name
            self.execute(flags + ["-DBW_NQ_SOURCE_CHECK", "-fsyntax-only", str(source)], compile_job=True)
            obj, binary = self.directory / (label + ".o"), self.directory / label
            self.execute(flags + ["-Dmain=nq_probe_main", "-c", str(source), "-o", str(obj)], compile_job=True)
            names = {row.split()[-1] for row in self.execute(["nm", "-u", str(obj)]).decode().splitlines()}
            self.assertLessEqual(names, {"___stack_chk_fail", "___stack_chk_guard"})
            self.execute(flags + [str(obj), str(wrapper), "-o", str(binary)], compile_job=True)
            self.assertEqual(self.execute([str(binary)]), b"NQ_HOST_STATUS 77\n")

    def test_dormant_host_role_plan_has_closed_selectors(self):
        # Pure data only: never construct Campaign, inspect target files or launch.
        spec = importlib.util.spec_from_file_location("nq_host_source", ROOT / "tests/qualification/host_guard.py")
        guard = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(guard)
        self.assertFalse(guard._campaign_created)
        directory = {"path": "/q", "device": 1, "inode": 2, "uid": 501, "gid": 20, "mode": 0o700}
        fixture = dict(directory, path="/q/" + guard.FIXTURE, inode=3)
        def file_row(path, size=1, digest="0" * 64):
            return {"path": path, "bytes": size, "sha256": digest, "device": 1, "inode": 4,
                    "uid": 501, "gid": 20, "mode": 0o600, "nlink": 1, "mtime_ns": 1, "ctime_ns": 1}
        files = {name: file_row("/tools/" + name, size, digest)
                 for name, (size, digest) in guard.PINS.items()}
        files.update(disk=file_row(fixture["path"] + "/build.raw", 8589934592,
            "ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25"),
            data=file_row(fixture["path"] + "/data.iso"), ps=file_row("/bin/ps"))
        plan = {"schema": "bootweave-host-role-plan-v1", "canonical": self.preparation.CANONICAL,
                "phase": "install", "workspace": directory, "fixture": fixture, "files": files}
        def validated(value):
            raw = json.dumps(value).encode()
            return guard.validate_plan(raw, hashlib.sha256(raw).hexdigest())
        checked = validated(plan)
        argv = guard.arguments(checked)
        for flag in ("-S", "-no-user-config", "-nodefaults", "-no-reboot"):
            self.assertIn(flag, argv)
        for flag, value in (("-machine", "pc-i440fx-11.1"), ("-cpu", "qemu64-v1"),
                            ("-smp", "1"), ("-m", "1024"), ("-nic", "none"), ("-net", "none")):
            self.assertEqual(argv[argv.index(flag) + 1], value)
        first = guard.endpoints(checked)
        self.assertEqual([p.name for p in first], ["qmp1.sock", "ser1.sock"])
        installed = validated(dict(plan, phase="installed"))
        self.assertEqual([p.name for p in guard.endpoints(installed)], ["qmp2.sock", "ser2.sock"])
        self.assertFalse(any("id=installer" in arg for arg in guard.arguments(installed)))
        for field, value in (("argv", ["/bin/sh"]), ("env", {"PATH": "/bin"}), ("phase", "third")):
            with self.assertRaises(guard.Refusal):
                validated(dict(plan, **{field: value}))
        for key, value in (("bytes", True), ("nlink", 2), ("mode", 0o666), ("sha256", "f" * 64)):
            broken = {**files, "qemu": dict(files["qemu"], **{key: value})}
            with self.assertRaises(guard.Refusal):
                validated(dict(plan, files=broken))
        raw = json.dumps(plan).encode()
        with self.assertRaises(guard.Refusal):
            guard.validate_plan(raw, "0" * 64)
        duplicate = b'{"schema":0,"schema":1}'
        with self.assertRaises(guard.Refusal):
            guard.validate_plan(duplicate, hashlib.sha256(duplicate).hexdigest())
        self.assertFalse(guard._campaign_created)
        self.assertEqual(guard._retained, [])

    def test_trace_pair_against_independent_literal_model(self):
        raw = self.source_files["tests/qualification/trace_oracles.json"]
        self.assertEqual(hashlib.sha256(raw).hexdigest(), TRACE_ORACLES_SHA256)
        oracle = json.loads(raw)
        self.assertEqual(oracle["schema"], "bootweave-qualification-trace-model-oracles-v1")
        self.assertEqual(oracle["case_count"], 64)
        self.assertEqual(oracle["driver_invocations"], 64)
        self.assertEqual(len(oracle["cases"]), 64)
        obj, binary = self.directory / "trace-pair.o", self.directory / "trace-model"
        renames = ["-Dgetrlimit=nq_model_getrlimit", "-Dsetrlimit=nq_model_setrlimit",
                   "-Dgetpid=nq_model_getpid", "-Dgetppid=nq_model_getppid"]
        self.execute(self.flags + renames + ["-c", str(ROOT / "tests/qualification/trace_limits.c"),
                     "-o", str(obj)], compile_job=True)
        names = {row.split()[-1] for row in self.execute(["nm", "-u", str(obj)]).decode().splitlines()}
        self.assertLessEqual(names, {"___error", "___stack_chk_fail", "___stack_chk_guard",
                                    "_nq_model_getrlimit", "_nq_model_setrlimit",
                                    "_nq_model_getpid", "_nq_model_getppid"})
        self.execute(self.flags + [str(ROOT / "tests/qualification/trace_model.c"), str(obj),
                     "-o", str(binary)], compile_job=True)
        linked = {row.split()[-1].lstrip("_").split("$", 1)[0] for row in
                  self.execute(["nm", "-u", str(binary)]).decode().splitlines()}
        self.assertFalse(linked & {"getrlimit", "setrlimit", "getpid", "getppid"})
        for index, case in enumerate(oracle["cases"]):
            self.assertEqual(case["index"], index)
            self.assertEqual(case["argv"], [str(index)])
            output = self.execute([str(binary), *case["argv"]])
            lines = output.splitlines(keepends=True)
            self.assertEqual(len(lines), 2)
            self.assertEqual(lines[0], case["expected_stdout"].encode())
            self.assertTrue(lines[1].startswith(b"NQ_TRACE_PID "))
            self.assertTrue(lines[1].endswith(b"\n"))
            pid = json.loads(lines[1][len(b"NQ_TRACE_PID "):])
            self.assertEqual(set(pid), {"case_index", "counts"})
            self.assertIs(type(pid["case_index"]), int)
            self.assertEqual(pid["case_index"], index)
            self.assertEqual(len(pid["counts"]), case["expected_pid_record_rows"])
            previous = [0, 0]
            for counts in pid["counts"]:
                self.assertIsInstance(counts, list)
                self.assertEqual(len(counts), 2)
                for column, value in enumerate(counts):
                    self.assertIs(type(value), int)
                    self.assertLessEqual(previous[column], value)
                    self.assertLessEqual(value, 256)
                previous = counts

    def support_module(self, name):
        spec = importlib.util.spec_from_file_location("nq_support_" + name,
            ROOT / "tests/qualification" / (name + ".py"))
        module = importlib.util.module_from_spec(spec)
        sys.modules[spec.name] = module
        spec.loader.exec_module(module)
        return module

    def coordinator_bindings(self, support):
        # Synthetic reported records exercise comparisons, never native facts.
        return tuple(support.Binding("source", name, len(data), hashlib.sha256(data).hexdigest())
                     for name, data in sorted(self.source_files.items())) + (
            support.Binding("tool", "model-tool", 1, "a" * 64),
            support.Binding("input", "model-input", 1, "b" * 64),
            support.Binding("recipe", "model-recipe", 1, "c" * 64))

    def test_allocation_fixed_reservations_and_build_count(self):
        support = self.support_module("accounting")
        data = support.plan()
        self.assertEqual(data["caps"], {"host": 20 * 1024**3, "guest": 27 * 1024**2})
        self.assertEqual(data["reserved"]["guest"], 23 * 1024**2)
        self.assertLess(data["reserved"]["host"], data["caps"]["host"])
        self.assertEqual(data["compiler_calls"], 24)
        self.assertEqual(data["compiler_jobs"], ["bootstrap", *self.preview["first_stage_jobs"]])
        self.assertFalse(data["execution_ready"])

    def test_allocation_samples_never_release_reservations_or_prove_peaks(self):
        support = self.support_module("accounting")
        ledger = support.Accounting()
        ledger.observe("guest.compiler-tmp", 100, 4096, "a" * 64)
        before = ledger.report()
        ledger.observe("guest.compiler-tmp", 0, 0, "b" * 64)
        after = ledger.report()
        self.assertEqual(after["latest_charged_bytes"]["guest.compiler-tmp"], 0)
        self.assertEqual(after["maximum_observed_bytes"]["guest.compiler-tmp"], 4096)
        self.assertEqual(after["sample_decreases"], ["guest.compiler-tmp"])
        self.assertEqual(before["reservations_held"], after["reservations_held"])
        self.assertFalse(after["logical_or_sampled_fit_proves_high_water"])
        self.assertFalse(after["execution_ready"])
        after["observations"][0]["logical_bytes"] = 999
        self.assertEqual(ledger.report()["observations"][0]["logical_bytes"], 100)

    def test_allocation_failures_are_sticky_and_oversize_is_retained(self):
        support = self.support_module("accounting")
        ledger = support.Accounting()
        with self.assertRaises(support.Refusal):
            ledger.observe("guest.cases", 1024**2 + 1, 0, "a" * 64)
        report = ledger.report()
        self.assertEqual(report["observations"][0]["charged_bytes"], 1024**2 + 1)
        self.assertTrue(report["failed"])
        with self.assertRaises(support.Refusal):
            ledger.observe("guest.cases", 0, 0, "b" * 64)
        self.assertEqual(len(ledger.report()["observations"]), 1)
        for role, logical, physical, digest in (("missing", 0, 0, "a" * 64),
                ("host.metadata", True, 0, "a" * 64), ("host.metadata", 0, -1, "a" * 64),
                ("host.metadata", 0, 0, "not-a-seal")):
            with self.subTest(role=role, logical=logical, physical=physical):
                rejected = support.Accounting()
                with self.assertRaises(support.Refusal):
                    rejected.observe(role, logical, physical, digest)
                self.assertTrue(rejected.report()["failed"])

    def test_allocation_unknowns_and_finite_record_count_do_not_recover(self):
        support = self.support_module("accounting")
        for role in ("guest.compiler-tmp", [], "unknown"):
            ledger = support.Accounting()
            with self.assertRaises(support.Refusal):
                ledger.unknown(role)
            self.assertTrue(ledger.report()["failed"])
            self.assertFalse(ledger.report()["execution_ready"])
        ledger = support.Accounting()
        for _ in range(256):
            ledger.observe("host.metadata", 0, 0, "a" * 64)
        with self.assertRaises(support.Refusal):
            ledger.observe("host.metadata", 0, 0, "a" * 64)
        self.assertEqual(len(ledger.report()["observations"]), 256)

    def test_transport_retains_exact_raw_prefixes_without_inventory_authority(self):
        support = self.support_module("transport")
        ledger = support.Transport("install")
        first, second = b'{"reported":', b'1}\n'
        ticket = ledger.raw_read("qmp", 0, first)
        self.assertEqual(ledger.publish_result(ticket, len(first), 0), ())
        ticket = ledger.raw_read("qmp", len(first), second)
        frames = ledger.publish_result(ticket, len(second), 0)
        self.assertEqual(len(frames), 1)
        self.assertEqual(frames[0].raw, first + second)
        self.assertEqual(frames[0].sha256, hashlib.sha256(first + second).hexdigest())
        opaque = b'\x00\xffprivate-console-control\x1b'
        ticket = ledger.raw_read("serial", 0, opaque)
        self.assertEqual(ledger.publish_result(ticket, len(opaque), 0), ())
        for channel in support.CHANNELS:
            offset = len(first + second) if channel == "qmp" else len(opaque) if channel == "serial" else 0
            ledger.eof(channel, offset)
            ledger.close_result(channel, 0, 0)
        record = json.loads(ledger.snapshot())
        self.assertTrue(record["reported_capture_complete"])
        self.assertFalse(record["actual_channel_transport_supplied"])
        self.assertEqual(record["charged"], len(first + second + opaque))
        self.assertFalse(record["qmp_semantics_supported"])
        report = ledger.inventory_report()
        self.assertFalse(report.supported or report.valid or report.resume_eligible or report.native_authority)

    def test_transport_partial_publication_and_close_never_restore_rights(self):
        support = self.support_module("transport")
        ledger = support.Transport("installed")
        ticket = ledger.raw_read("stdout", 0, b"observed")
        with self.assertRaises(support.Refusal):
            ledger.publish_result(ticket, 3, 0)
        record = json.loads(ledger.snapshot())
        self.assertEqual(record["channels"]["stdout"]["observed"], 8)
        self.assertEqual(record["channels"]["stdout"]["published"], 3)
        self.assertTrue(record["uncertain"])
        with self.assertRaises(support.Refusal):
            ledger.publish_result(ticket, 8, 0)
        with self.assertRaises(support.Refusal):
            ledger.raw_read("stderr", 0, b"later")
        ledger.eof("stdout", 8)
        with self.assertRaises(support.Refusal):
            ledger.close_result("stdout", -1, 4)
        with self.assertRaises(support.Refusal):
            ledger.close_result("stdout", 0, 0)
        self.assertEqual(json.loads(ledger.snapshot())["channels"]["stdout"]["close"], "unknown")
        invalid = support.Transport("install")
        ticket = invalid.raw_read("stdout", 0, b"x")
        with self.assertRaises(support.Refusal):
            invalid.publish_result(ticket, -1, 2**31)
        self.assertTrue(json.loads(invalid.snapshot())["uncertain"])
        invalid = support.Transport("install")
        invalid.eof("stdout", 0)
        with self.assertRaises(support.Refusal):
            invalid.close_result("stdout", -1, 2**31)
        record = json.loads(invalid.snapshot())
        self.assertEqual(record["channels"]["stdout"]["close"], "unknown")
        self.assertEqual(record["channels"]["stdout"]["close_errno"], 0)

    def test_transport_generic_json_errors_are_sticky_without_semantic_shortcuts(self):
        support = self.support_module("transport")
        for wire in (b'{"same":1,"same":2}\n', b'{"n":NaN}\n', b'[]\n', b'{"n":9223372036854775808}\n',
                     b'{"a":' + b'[' * 33 + b'0' + b']' * 33 + b'}\n', b'{"x":"\xff"}\n'):
            ledger = support.Transport("install")
            ticket = ledger.raw_read("qmp", 0, wire)
            with self.assertRaises(support.Refusal):
                ledger.publish_result(ticket, len(wire), 0)
            self.assertTrue(json.loads(ledger.snapshot())["framing_failed"])
            self.assertFalse(ledger.inventory_report().resume_eligible)
        ledger = support.Transport("install")
        ticket = ledger.raw_read("qmp", 0, b'{"cut":')
        ledger.publish_result(ticket, 7, 0)
        with self.assertRaises(support.Refusal):
            ledger.eof("qmp", 7)
        self.assertTrue(json.loads(ledger.snapshot())["failed"])

    def test_transport_caps_are_shared_across_channels_and_offsets_are_exact(self):
        support = self.support_module("transport")
        ledger = support.Transport("installed")
        chunk = b"x" * 65536
        offsets = {name: 0 for name in ("stdout", "serial")}
        for index in range(160):
            channel = "stdout" if index % 2 else "serial"
            ticket = ledger.raw_read(channel, offsets[channel], chunk)
            ledger.publish_result(ticket, len(chunk), 0)
            offsets[channel] += len(chunk)
        self.assertEqual(json.loads(ledger.snapshot())["charged"], 10 * 1024**2)
        with self.assertRaises(support.Refusal):
            ledger.raw_read("stdout", offsets["stdout"], b"x")
        self.assertEqual(json.loads(ledger.snapshot())["charged"], 10 * 1024**2 + 1)
        with self.assertRaises(support.Refusal):
            support.Transport("installed").raw_read("serial", True, b"x")

    def test_coordinator_exact_bindings_and_order_are_structural_only(self):
        support = self.support_module("coordinator")
        binding = support.Binding("source", "fixed.c", 1, "a" * 64)
        support.compare_bindings((binding,), (binding,))
        with self.assertRaises(support.Refusal):
            support.compare_bindings((binding,), (support.Binding("source", "fixed.c", 1, "b" * 64),))
        with self.assertRaises(support.Refusal):
            support.binding_set((binding, binding))
        accepted = ()
        for stage in support.STAGES["installed"]:
            accepted = support.model_order("installed", accepted, stage)
        with self.assertRaises(support.Refusal):
            support.model_order("installed", (), "closed")
        evidence = support.Evidence("campaign", "preflight", "c" * 64, (binding,), 10)
        self.assertTrue(support.model_evidence("campaign", "preflight", (binding,), evidence, 9, 11))
        with self.assertRaises(support.Refusal):
            support.model_evidence("campaign", "preflight", (binding,), evidence, 11, 12)
        with self.assertRaises(support.Refusal):
            support.NativeAuthority()

    def test_coordinator_single_lifetime_cancellation_and_unknown_custody(self):
        support = self.support_module("coordinator")
        with mock.patch("qualification.lifetime.time.monotonic", return_value=100):
            coordinator = support.Coordinator()
            with self.assertRaises(support.Refusal):
                support.Coordinator()
            self.assertFalse(coordinator.admission("prepare", {"approved": True}).native_admitted)
            coordinator.cancel()
            decision = coordinator.watchdog()
            self.assertTrue(decision.cancelled and decision.work_closed)
            self.assertFalse(decision.native_admitted)
            with self.assertRaises(TypeError):
                bool(decision)
            with self.assertRaises(support.Refusal):
                coordinator.begin_phase("install")
        other = self.support_module("coordinator")
        with mock.patch("qualification.lifetime.time.monotonic", return_value=100):
            coordinator = other.Coordinator()
            with self.assertRaises(other.Refusal):
                coordinator.report_custody("install", "a" * 64, consumed=True, lost=False, close_unknown=False)
            self.assertTrue(coordinator.watchdog().uncertain)
        other = self.support_module("coordinator")
        with mock.patch("qualification.lifetime.time.monotonic", return_value=100):
            coordinator = other.Coordinator()
            bindings = self.coordinator_bindings(other)
            coordinator.record(bindings, other.Evidence("campaign", "preflight", "a" * 64, bindings, 100))
            with self.assertRaisesRegex(other.Refusal, "RECEIPT_REUSE"):
                coordinator.record(bindings, other.Evidence("campaign", "prepared", "a" * 64, bindings, 100))
        for consumed, lost, stop in ((True, False, False), (False, True, False), (False, False, True)):
            other = self.support_module("coordinator")
            with mock.patch("qualification.lifetime.time.monotonic", return_value=100):
                coordinator = other.Coordinator()
                bindings = self.coordinator_bindings(other)
                for stage, digest in zip(other.STAGES["campaign"], ("a" * 64, "b" * 64)):
                    coordinator.record(bindings, other.Evidence("campaign", stage, digest, bindings, 100))
                coordinator.begin_phase("install")
                coordinator.report_custody("install", "c" * 64,
                    consumed=consumed, lost=lost, close_unknown=True)
                decision = coordinator.watchdog()
                self.assertTrue(decision.uncertain)
                self.assertEqual(decision.stop_required, stop)
                self.assertFalse(decision.native_admitted)

    def test_coordinator_unsupported_inventory_and_original_deadline_never_resume(self):
        support = self.support_module("coordinator")
        clock = [100.0]
        with mock.patch("qualification.lifetime.time.monotonic", side_effect=lambda: clock[0]):
            coordinator = support.Coordinator()
            bindings = self.coordinator_bindings(support)
            for stage, digest in zip(support.STAGES["campaign"], ("d" * 64, "f" * 64)):
                evidence = support.Evidence("campaign", stage, digest, bindings, clock[0])
                self.assertFalse(coordinator.record(bindings, evidence).native_admitted)
            coordinator.begin_phase("install")
            clock[0] += 1
            with self.assertRaisesRegex(support.Refusal, "INVENTORY_UNSUPPORTED"):
                coordinator.record(bindings, support.Evidence("install", "paused-inventory", "e" * 64, bindings, clock[0]))
            self.assertFalse(coordinator.admission("resume").native_admitted)
            self.assertEqual(coordinator.origin, 100)
        # Independent unpoisoned controls: unsupported inventory must not mask
        # the exact work/total deadline comparisons under test.
        fresh = self.support_module("coordinator")
        clock[0] = 100
        with mock.patch("qualification.lifetime.time.monotonic", side_effect=lambda: clock[0]):
            coordinator = fresh.Coordinator()
            bound = tuple(fresh.Binding(row.kind, row.name, row.size, row.sha256) for row in bindings)
            for stage, digest in zip(fresh.STAGES["campaign"], ("a" * 64, "b" * 64)):
                coordinator.record(bound, fresh.Evidence("campaign", stage, digest, bound, clock[0]))
            coordinator.begin_phase("install")
            clock[0] = 2799.999
            before = coordinator.watchdog()
            self.assertFalse(before.work_closed or before.stop_required)
            self.assertTrue(before.model_eligible)
            clock[0] = 2800
            exact = coordinator.watchdog()
            self.assertTrue(exact.work_closed and exact.stop_required)
            self.assertEqual(exact.first_failure, "")
            clock[0] = 3699.999
            self.assertEqual(coordinator.watchdog().first_failure, "")
            clock[0] = 3700
            self.assertEqual(coordinator.watchdog().first_failure, "TOTAL_DEADLINE")
        fresh = self.support_module("coordinator")
        clock[0] = 100
        with mock.patch("qualification.lifetime.time.monotonic", side_effect=lambda: clock[0]):
            coordinator = fresh.Coordinator()
            clock[0] = 10899.999
            self.assertFalse(coordinator.watchdog().work_closed)
            clock[0] = 10900
            self.assertTrue(coordinator.watchdog().work_closed)

    def test_shared_host_clock_cancellation_and_closed_native_admission(self):
        guard = self.support_module("host_guard")
        with mock.patch("qualification.lifetime.time.monotonic", return_value=100):
            coordinator = guard.Coordinator()
            with mock.patch.object(guard, "_now", side_effect=AssertionError("second origin")):
                campaign = guard.Campaign(coordinator)
            self.assertIs(campaign.lifetime, coordinator.lifetime)
            self.assertEqual(campaign.origin, coordinator.origin)
            campaign.cancel()
            self.assertTrue(coordinator.watchdog().cancelled)
            with mock.patch.object(guard, "validate_plan") as validator, \
                    mock.patch.object(guard.subprocess, "Popen") as birth, \
                    self.assertRaisesRegex(guard.Refusal, "admission remains unsupported"):
                campaign.launch(b"not a plan", "a" * 64)
            validator.assert_not_called()
            birth.assert_not_called()
            with self.assertRaises(guard.Refusal):
                guard.Campaign(coordinator)

    def test_shared_lifetime_exact_deadlines_and_failed_slots_cannot_renew(self):
        from qualification.lifetime import Lifetime, Refusal
        clock = [100.0]
        with mock.patch("qualification.lifetime.time.monotonic", side_effect=lambda: clock[0]):
            lifetime = Lifetime()
            window = lifetime.begin("install")
            self.assertEqual((window.origin, window.work_end, window.end), (100, 2800, 3700))
            clock[0] = 2799.99
            lifetime.require_work(window)
            clock[0] = 2800
            with self.assertRaises(Refusal):
                lifetime.require_work(window)
            lifetime.cancel()
            clock[0] = 3700
            with self.assertRaises(Refusal):
                lifetime.begin("installed")
            with self.assertRaises(Refusal):
                lifetime.begin("installed")
            self.assertEqual(lifetime.origin, 100)
            self.assertEqual(lifetime.end, 10900)
        clock[0] = 100
        with mock.patch("qualification.lifetime.time.monotonic", side_effect=lambda: clock[0]):
            lifetime = Lifetime()
            clock[0] = 99
            with self.assertRaises(Refusal):
                lifetime.now()
            clock[0] = 101
            with self.assertRaises(Refusal):
                lifetime.now()
            self.assertTrue(lifetime.cancelled)

    def test_child_stop_watchdog_and_finalizer_share_once_only_original_rights(self):
        guard = self.support_module("host_guard")
        held = guard._Held()
        held.pid = 42424242  # Synthetic only; all kill calls below are doubled.
        with mock.patch.object(guard.os, "kill") as stop:
            workers = [threading.Thread(target=held.stop) for _ in range(4)]
            for worker in workers:
                worker.start()
            for worker in workers:
                worker.join(1)
                self.assertFalse(worker.is_alive())
            stop.assert_called_once_with(42424242, guard.signal.SIGKILL)
            self.assertFalse(held.stop())
            for field in ("consumed", "lost", "sealed"):
                other = guard._Held()
                other.pid = 42424243
                setattr(other, field, True)
                self.assertFalse(other.stop())
            self.assertEqual(stop.call_count, 1)
        unknown = guard._Held()
        unknown.pid = 42424244
        with mock.patch.object(guard.os, "kill", side_effect=InterruptedError("synthetic")) as stop:
            self.assertTrue(unknown.stop())
            self.assertFalse(unknown.stop())
            self.assertEqual(stop.call_count, 1)
            self.assertTrue(unknown.errors)

    def test_child_watchdog_runs_independently_and_uses_shared_cancellation(self):
        from qualification.lifetime import Lifetime
        guard = self.support_module("host_guard")
        lifetime = Lifetime()
        window = lifetime.begin("install")
        stopped = threading.Event()
        child = mock.Mock()
        child.stop.side_effect = stopped.set  # No actual process or signal.
        watchdog = guard._ChildWatchdog(child, lifetime, window)
        try:
            watchdog.start()
            lifetime.cancel()
            self.assertTrue(stopped.wait(1))
        finally:
            watchdog.close()
        self.assertFalse(watchdog.thread.is_alive())
        self.assertEqual(watchdog.errors, [])
        child.stop.assert_called_once_with()
        with self.assertRaises(guard.Refusal):
            watchdog.start()

    def raw_fixture(self, label):
        guard = self.support_module("host_guard")
        directory = Path(tempfile.mkdtemp(prefix=label + "-", dir=self.directory))
        raw = guard._Raw("install")
        raw.open(directory)
        return guard, raw, directory

    def retain_io(self, directory, raw, extra=None):
        files = {p.name: {"bytes": p.stat().st_size, "sha256": hashlib.sha256(p.read_bytes()).hexdigest()}
                 for p in directory.glob("*.raw")}
        (directory / "portable-io-receipt.json").write_text(json.dumps({
            "portable_synthetic_only": True, "native_cases": 0,
            "transport": json.loads(raw.ledger.snapshot()), "allocation": raw.accounting.report(),
            "close_errors": raw.errors, "files": files, "extra": extra}, sort_keys=True) + "\n")

    def test_actual_anonymous_read_channels_retain_raw_bytes_eof_and_close(self):
        guard, raw, directory = self.raw_fixture("anonymous-read")
        peers, transferred, channels = {}, {}, None
        wire = {"qmp": b'{"reported":"synthetic"}\n', "serial": b"\x00\xffinert-console\x1b"}
        try:
            for name in ("qmp", "serial"):
                transferred[name], peers[name] = socket.socketpair(socket.AF_UNIX, socket.SOCK_STREAM)
                self.assertEqual(peers[name].send(wire[name]), len(wire[name]))
                peers[name].shutdown(socket.SHUT_WR)
            owned, transferred = transferred, {}
            channels = guard._ReadChannels(owned, raw)
            frames = []
            for _ in range(100):
                frames.extend(channels.pump())
                if channels.eof == {"qmp", "serial"}:
                    break
                time.sleep(0.001)
            self.assertEqual(channels.eof, {"qmp", "serial"})
            self.assertEqual([frame.raw for frame in frames], [wire["qmp"]])
            for name in ("stdout", "stderr", "rss"):
                raw.eof(name)
        finally:
            if channels is not None:
                channels.close()
            for stream in [*peers.values(), *transferred.values()]:
                stream.close()
            raw.close()
        for name, data in wire.items():
            self.assertEqual((directory / (name + ".raw")).read_bytes(), data)
        self.assertEqual(channels.closes, {"qmp": "closed", "serial": "closed"})
        self.assertEqual(channels.errors, [])
        report = json.loads(raw.ledger.snapshot())
        self.assertTrue(report["reported_capture_complete"])
        self.assertEqual(report["charged"], sum(map(len, wire.values())))
        self.assertFalse(raw.ledger.inventory_report().resume_eligible)
        self.retain_io(directory, raw, {"socket_closes": channels.closes, "socket_eof": sorted(channels.eof)})

    def test_actual_capture_partial_write_is_retained_without_retry(self):
        guard, raw, directory = self.raw_fixture("partial-write")
        original = guard.os.write
        try:
            with mock.patch.object(guard.os, "write", side_effect=lambda fd, data: original(fd, data[:2])) as writer:
                with self.assertRaisesRegex(RuntimeError, "PUBLICATION_PARTIAL_OR_ERROR"):
                    raw.record("stdout", b"observed")
                self.assertEqual(writer.call_count, 1)
            with mock.patch.object(guard.os, "write") as writer, self.assertRaises(guard.Refusal):
                raw.record("stdout", b"retry")
            writer.assert_not_called()
        finally:
            raw.close()
        self.assertEqual((directory / "stdout.raw").read_bytes(), b"ob")
        report = json.loads(raw.ledger.snapshot())
        self.assertEqual((report["channels"]["stdout"]["observed"], report["channels"]["stdout"]["published"]), (8, 2))
        self.assertTrue(report["uncertain"])
        self.retain_io(directory, raw)

    def test_actual_capture_close_uncertainty_consumes_descriptor_once(self):
        guard, raw, directory = self.raw_fixture("unknown-close")
        for name in guard.CHANNELS:
            raw.eof(name)
        target = raw.fds["stdout"]
        original, calls = guard.os.close, []
        def interrupted(fd):
            calls.append(fd)
            original(fd)
            if fd == target:
                raise InterruptedError(4, "synthetic unknown close after actual close")
        with mock.patch.object(guard.os, "close", side_effect=interrupted):
            raw.close()
            raw.close()
        self.assertEqual(calls.count(target), 1)
        self.assertEqual(raw.fds, {})
        self.assertTrue(raw.errors)
        self.assertEqual(json.loads(raw.ledger.snapshot())["channels"]["stdout"]["close"], "unknown")
        self.retain_io(directory, raw, {"target_close_attempts": calls.count(target)})

    def test_actual_capture_overquota_stops_but_framing_failure_retains_tails(self):
        guard, raw, directory = self.raw_fixture("malformed-wire")
        try:
            with self.assertRaisesRegex(RuntimeError, "JSON_FRAMING"):
                raw.record("qmp", b'{"same":1,"same":2}\n')
            raw.record("serial", b"forensic tail")
        finally:
            raw.close()
        self.assertEqual((directory / "qmp.raw").read_bytes(), b'{"same":1,"same":2}\n')
        self.assertEqual((directory / "serial.raw").read_bytes(), b"forensic tail")
        self.assertTrue(raw.failed)
        self.assertTrue(json.loads(raw.ledger.snapshot())["framing_failed"])
        self.assertFalse(raw.ledger.inventory_report().resume_eligible)
        self.retain_io(directory, raw)
        guard, raw, directory = self.raw_fixture("oversize-before-write")
        try:
            with mock.patch.object(guard.os, "write") as writer, self.assertRaisesRegex(RuntimeError, "RAW_CAP"):
                raw.record("stdout", b"x" * 65537)
            writer.assert_not_called()
        finally:
            raw.close()
        self.assertEqual((directory / "stdout.raw").stat().st_size, 0)
        self.assertEqual(json.loads(raw.ledger.snapshot())["charged"], 65537)
        self.retain_io(directory, raw)

    def test_read_channel_constructor_failure_closes_only_transferred_sockets(self):
        guard, raw, directory = self.raw_fixture("constructor-close")
        unknown = mock.Mock()
        with self.assertRaises(guard.Refusal):
            guard._ReadChannels({"qmp": unknown, "serial": unknown}, raw)
        unknown.close.assert_not_called()
        left, right = socket.socketpair(socket.AF_UNIX, socket.SOCK_STREAM)
        try:
            with self.assertRaises(guard.Refusal):
                guard._ReadChannels({"qmp": left, "serial": left}, raw)
            self.assertGreaterEqual(left.fileno(), 0)  # Invalid aliases never transfer.
        finally:
            left.close()
            right.close()
        streams, peers = {}, {}
        for name in ("qmp", "serial"):
            streams[name], peers[name] = socket.socketpair(socket.AF_UNIX, socket.SOCK_STREAM)
        calls = []
        def close(stream):
            calls.append(stream)
            stream.close()
            if stream is streams["qmp"]:
                raise InterruptedError(4, "synthetic unknown close")
        receipt = None
        try:
            with mock.patch.object(guard.selectors, "DefaultSelector", side_effect=OSError("synthetic selector birth")), \
                    mock.patch.object(guard, "_socket_close", side_effect=close), \
                    self.assertRaises(guard.Refusal) as failure:
                guard._ReadChannels(streams, raw)
            receipt = failure.exception
            self.assertEqual(receipt.socket_closes, {"qmp": "unknown", "serial": "closed"})
            self.assertEqual(calls, [streams["qmp"], streams["serial"]])
            self.assertTrue(receipt.close_errors)
            self.assertEqual([s.fileno() for s in streams.values()], [-1, -1])
        finally:
            for peer in peers.values():
                peer.close()
            raw.close()
        self.retain_io(directory, raw, {"constructor_closes": receipt.socket_closes,
                                       "constructor_errors": receipt.close_errors})

    def test_read_channel_metadata_and_socket_birth_cannot_renew_work_window(self):
        from qualification.lifetime import Lifetime
        from types import SimpleNamespace
        guard = self.support_module("host_guard")
        for failure_mode in ("deadline", "cancel"):
            clock = [100.0]
            with mock.patch("qualification.lifetime.time.monotonic", side_effect=lambda: clock[0]):
                lifetime = Lifetime()
                window = lifetime.begin("install")
                def delayed_metadata():
                    if failure_mode == "deadline":
                        clock[0] = window.work_end
                    else:
                        lifetime.cancel()
                    return SimpleNamespace(st_mode=stat.S_IFSOCK | 0o600, st_uid=os.getuid())
                endpoint = mock.Mock()
                endpoint.lstat.side_effect = delayed_metadata
                with mock.patch.object(guard, "endpoints", return_value=(endpoint, endpoint)), \
                        mock.patch.object(guard.socket, "socket") as factory, \
                        self.assertRaisesRegex(guard.Refusal, "WORK_CLOSED"):
                    guard._ReadChannels.connect({}, mock.Mock(), lifetime, window)
                factory.assert_not_called()
        clock = [100.0]
        stream = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        with mock.patch("qualification.lifetime.time.monotonic", side_effect=lambda: clock[0]):
            lifetime = Lifetime()
            window = lifetime.begin("install")
            endpoint = mock.Mock()
            endpoint.lstat.return_value = SimpleNamespace(st_mode=stat.S_IFSOCK | 0o600, st_uid=os.getuid())
            def delayed_socket(*_):
                clock[0] = window.work_end
                return stream
            with mock.patch.object(guard, "endpoints", return_value=(endpoint, endpoint)), \
                    mock.patch.object(guard.socket, "socket", side_effect=delayed_socket), \
                    self.assertRaisesRegex(guard.Refusal, "WORK_CLOSED") as failure:
                guard._ReadChannels.connect({}, mock.Mock(), lifetime, window)
            self.assertEqual(failure.exception.socket_closes, {"qmp": "closed"})
        self.assertEqual(stream.fileno(), -1)
        lifetime = Lifetime()
        window = lifetime.begin("install")
        endpoint = mock.Mock()
        endpoint.lstat.return_value = SimpleNamespace(st_mode=stat.S_IFSOCK | 0o600, st_uid=os.getuid())
        pending = mock.MagicMock()
        pending.__enter__.return_value = pending
        pending.select.return_value = [(None, 2)]
        closed = []
        def uncertain_close(stream):
            closed.append(stream)
            stream.close()
            if len(closed) == 1:
                raise InterruptedError(4, "synthetic first close unknown")
        with mock.patch.object(guard, "endpoints", return_value=(endpoint, endpoint)), \
                mock.patch.object(guard, "_identity", return_value={"synthetic": True}), \
                mock.patch.object(socket.socket, "connect_ex", side_effect=(0, OSError("synthetic second connect"))), \
                mock.patch.object(socket.socket, "getsockopt", return_value=0), \
                mock.patch.object(guard.selectors, "DefaultSelector", return_value=pending), \
                mock.patch.object(guard, "_socket_close", side_effect=uncertain_close), \
                self.assertRaises(guard.Refusal) as failure:
            guard._ReadChannels.connect({}, mock.Mock(), lifetime, window)
        self.assertEqual(failure.exception.socket_closes, {"qmp": "unknown", "serial": "closed"})
        self.assertEqual(len(closed), 2)
        self.assertEqual([stream.fileno() for stream in closed], [-1, -1])

    def test_terminal_read_channels_drain_large_tails_through_real_eof(self):
        from qualification.lifetime import Lifetime
        guard, raw, directory = self.raw_fixture("terminal-tail")
        wire = {"qmp": (b'{"n":"' + b"x" * 1000 + b'"}\n') * 130,
                "serial": b"z" * 131072}
        streams, peers, workers, failures = {}, {}, [], []
        for name in ("qmp", "serial"):
            streams[name], peers[name] = socket.socketpair(socket.AF_UNIX, socket.SOCK_STREAM)
            peers[name].settimeout(1)
        channels = guard._ReadChannels(streams, raw)
        def send(name):
            try:
                peers[name].sendall(wire[name])
            except BaseException as error:
                failures.append(str(error))
            finally:
                peers[name].close()
        lifetime = Lifetime()
        window = lifetime.begin("install")
        try:
            for name in ("qmp", "serial"):
                worker = threading.Thread(target=send, args=(name,))
                worker.start()
                workers.append(worker)
            channels.drain(lifetime, window)
            self.assertEqual(channels.eof, {"qmp", "serial"})
            self.assertEqual(channels.errors, [])
            for name in ("stdout", "stderr", "rss"):
                raw.eof(name)
        finally:
            channels.close()
            for worker in workers:
                worker.join(2)
                self.assertFalse(worker.is_alive())
            raw.close()
        self.assertEqual(failures, [])
        for name, data in wire.items():
            self.assertGreater(len(data), 65536)
            self.assertEqual((directory / (name + ".raw")).read_bytes(), data)
        self.assertTrue(json.loads(raw.ledger.snapshot())["reported_capture_complete"])
        self.retain_io(directory, raw, {"socket_closes": channels.closes, "socket_eof": sorted(channels.eof)})

    def test_terminal_read_clock_failure_preserves_missing_eof_and_both_closes(self):
        from qualification.lifetime import Lifetime, Refusal
        guard, raw, directory = self.raw_fixture("terminal-clock")
        streams, peers = {}, {}
        for name in ("qmp", "serial"):
            streams[name], peers[name] = socket.socketpair(socket.AF_UNIX, socket.SOCK_STREAM)
        channels = guard._ReadChannels(streams, raw)
        lifetime = Lifetime()
        window = lifetime.begin("install")
        try:
            with mock.patch.object(lifetime, "now", side_effect=Refusal("synthetic clock failure")):
                channels.drain(lifetime, window)
        finally:
            channels.close()
            for peer in peers.values():
                peer.close()
            raw.close()
        self.assertEqual(channels.eof, set())
        self.assertEqual(channels.closes, {"qmp": "closed", "serial": "closed"})
        self.assertTrue(channels.errors)
        self.assertFalse(json.loads(raw.ledger.snapshot())["reported_capture_complete"])
        self.retain_io(directory, raw, {"socket_closes": channels.closes, "socket_eof": [],
                                       "drain_errors": channels.errors})

    def test_terminal_recv_error_stops_work_and_other_channel_still_drains(self):
        from qualification.lifetime import Lifetime
        guard, raw, directory = self.raw_fixture("one-channel-read-error")
        streams, peers = {}, {}
        wire = {"qmp": b'{"unobserved":"read fails"}\n', "serial": b"retained independent tail"}
        for name in ("qmp", "serial"):
            streams[name], peers[name] = socket.socketpair(socket.AF_UNIX, socket.SOCK_STREAM)
            self.assertEqual(peers[name].send(wire[name]), len(wire[name]))
            peers[name].shutdown(socket.SHUT_WR)
        channels = guard._ReadChannels(streams, raw)
        lifetime = Lifetime()
        window = lifetime.begin("install")
        original_recv = socket.socket.recv
        def read(stream, size):
            if stream is streams["qmp"]:
                raise OSError(5, "synthetic recv failure")
            return original_recv(stream, size)
        try:
            with mock.patch.object(socket.socket, "recv", new=read):
                channels.drain(lifetime, window)
            self.assertEqual(channels.eof, {"serial"})
            self.assertFalse(raw.failed)  # Gate must inspect channel errors too.
            with self.assertRaisesRegex(guard.Refusal, "Transport capture/read failed"):
                channels.require_healthy()
        finally:
            channels.close()
            for peer in peers.values():
                peer.close()
            raw.close()
        self.assertEqual((directory / "qmp.raw").read_bytes(), b"")
        self.assertEqual((directory / "serial.raw").read_bytes(), wire["serial"])
        self.assertEqual(channels.closes, {"qmp": "closed", "serial": "closed"})
        self.assertFalse(json.loads(raw.ledger.snapshot())["reported_capture_complete"])
        self.retain_io(directory, raw, {"socket_closes": channels.closes, "socket_eof": sorted(channels.eof),
                                       "drain_errors": channels.errors})

    def test_original_stop_serializes_with_consuming_wait_and_cannot_return(self):
        from types import SimpleNamespace
        guard = self.support_module("host_guard")
        held = guard._Held()
        held.pid, held.proc = 42424245, SimpleNamespace(returncode=None)
        entered, release, late_stop = threading.Event(), threading.Event(), []
        def wait(*_):
            entered.set()
            if not release.wait(1):
                raise RuntimeError("synthetic wait release missing")
            return held.pid, 0, None
        with mock.patch.object(guard, "_now", return_value=100), \
                mock.patch.object(guard.os, "waitid", return_value=None), \
                mock.patch.object(guard.os, "wait4", side_effect=wait) as waited, \
                mock.patch.object(guard.os, "kill") as killed:
            owner = threading.Thread(target=held.finish, args=(200, lambda *_: None))
            owner.start()
            contender = None
            try:
                self.assertTrue(entered.wait(1))
                contender = threading.Thread(target=lambda: late_stop.append(held.stop()))
                contender.start()
            finally:
                release.set()
                owner.join(2)
                if contender is not None:
                    contender.join(2)
            self.assertFalse(owner.is_alive())
            self.assertFalse(contender.is_alive())
            self.assertTrue(held.consumed and held.sealed)
            self.assertFalse(held.lost)
            self.assertEqual(late_stop, [False])
            self.assertEqual(held.errors, [])
            waited.assert_called_once_with(42424245, guard.os.WNOHANG)
            killed.assert_called_once_with(42424245, guard.signal.SIGKILL)
