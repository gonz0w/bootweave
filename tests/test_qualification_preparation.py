# SPDX-License-Identifier: BSD-3-Clause
"""Portable source sealing/syntax/host-refusal only; zero native cases passed."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import sys
import tempfile
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
