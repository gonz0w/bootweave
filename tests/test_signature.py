# SPDX-License-Identifier: BSD-3-Clause
"""Finite inert fixed-verifier tests; native confinement remains unqualified."""
import base64
import errno
import hashlib
import json
import os
from pathlib import Path
import stat
import subprocess
import sys
import tempfile
import unittest

import test_material as fixtures

ROOT = Path(__file__).resolve().parents[1]
FIELDS = ("result state cause pid group launch stop consumed lost bytes close_errors "
          "unknown_closes material_close_errors material_unknown_closes budget_failed "
          "final_failed stop_errno modeled_stop fake waits signals final_calls bad_order bad_close").split()


def witnesses(stdout):
    """Recover valid identity rows even when another row/result is malformed."""
    born, groups, setgroups, failedgroups, errors = [], [], [], [], []
    for raw in stdout.splitlines():
        parts = raw.split()
        if not parts or parts[0] not in (b"SV_BORN", b"SV_GROUP", b"SV_SETGROUP", b"SV_SETGROUP_FAILED"):
            continue
        try:
            expected = {b"SV_BORN": 6, b"SV_GROUP": 4, b"SV_SETGROUP": 3, b"SV_SETGROUP_FAILED": 5}[parts[0]]
            if len(parts) != expected:
                raise ValueError("truncated identity row")
            values = list(map(int, parts[1:]))
            if values[0] <= 0 or values[1] == 0 or values[1] < -1:
                raise ValueError("invalid identity")
            if parts[0] == b"SV_SETGROUP_FAILED":
                if values[1] != values[0] or values[2] <= 0 or values[3] not in (0, 1):
                    raise ValueError("invalid failed setpgid observation")
                failedgroups.append(values)
            elif parts[0] == b"SV_GROUP":
                if (values[1] < 0 and values[2] <= 0) or (values[1] > 0 and values[2] != 0):
                    raise ValueError("invalid group observation")
                groups.append(values)
            elif parts[0] == b"SV_SETGROUP":
                if values[1] != values[0]:
                    raise ValueError("unexpected setpgid target")
                setgroups.append(values)
            else:
                if (values[1] < 0 and values[4] <= 0) or (values[1] > 0 and values[4] != 0):
                    raise ValueError("invalid initial group observation")
                if values[2] <= 0 or not 0 <= values[3] < 1000000000:
                    raise ValueError("invalid monotonic birth witness")
                born.append(values)
        except (ValueError, OverflowError) as caught:
            errors.append(str(caught))
    return born, groups, setgroups, failedgroups, errors


def parse(stdout):
    result = None
    for row in stdout.decode("ascii").splitlines():
        parts = row.split()
        sizes = {"SV_BORN": 6, "SV_GROUP": 4, "SV_SETGROUP": 3, "SV_SETGROUP_FAILED": 5}
        if parts[0] in sizes and len(parts) == sizes[parts[0]]:
            list(map(int, parts[1:]))
        elif parts[0] == "SV_SETUP" and len(parts) == 6:
            list(map(int, parts[2:]))
        elif parts[0] == "SV_RESULT" and len(parts) == len(FIELDS) + 1 and result is None:
            result = dict(zip(FIELDS, map(int, parts[1:])))
        else:
            raise AssertionError("unexpected private fixture output")
    return result


class SignatureTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if sys.platform != "darwin":
            raise RuntimeError("inert portable verifier harness requires Darwin; never launch the native adapter")
        # Retain build/cases/ledger even on success; no automatic evidence deletion.
        cls.directory = Path(tempfile.mkdtemp(prefix="signature-build-", dir=ROOT / ".tools"))
        cls.ledger = cls.directory / "observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1"}
        flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
                 "-I", str(ROOT / "src")]
        replacements = ["clock_gettime", "open", "pipe", "fcntl", "lseek", "fstat", "close",
                        "fork", "setpgid", "dup2", "write", "read", "poll", "waitid", "wait4",
                        "kill", "getuid", "execve"]
        sources = [
            ("normal", "src/signature_verify.c", []),
            ("injected", "src/signature_verify.c", [f"-D{x}=bw_v_{x}" for x in replacements]),
            ("platform", "src/signature_platform.c", []),
            ("material", "src/auth_material.c", ["-Dmkdtemp=bw_sv_test_mkdtemp", "-Dclose=bw_v_close"]),
            ("capture", "src/auth_input.c", []),
        ]
        for name, source, defines in sources:
            subprocess.run(flags + defines + ["-c", str(ROOT / source), "-o", str(cls.directory / (name + ".o"))],
                           check=True, capture_output=True, timeout=20, env=cls.env)
        for name, common, extra, defines in [
                ("normal", "normal.o", ["platform.o"], ["-DTEST_REAL_PLATFORM"]),
                ("modeled", "injected.o", [], [])]:
            subprocess.run(flags + defines + [str(cls.directory / common),
                           *[str(cls.directory / p) for p in extra],
                           str(cls.directory / "material.o"), str(cls.directory / "capture.o"),
                           str(ROOT / "tests/signature_driver.c"), "-o", str(cls.directory / name)],
                           check=True, capture_output=True, timeout=20, env=cls.env)
        cls.normal = cls.directory / "normal"
        cls.modeled = cls.directory / "modeled"

    def fixture(self, fault="none", normal=False):
        directory = Path(tempfile.mkdtemp(prefix="signature-case-", dir=ROOT / ".tools"))
        paths = [directory / x for x in ("key", "signature", "definition", "canary")]
        for p, data in zip(paths, [fixtures.KEY, fixtures.SIGNATURE, b"exact\0definition\n", b"outside sentinel\n"]):
            p.write_bytes(data)

        def identity(p):
            s = p.lstat()
            return [s.st_dev, s.st_ino, s.st_mode, hashlib.sha256(p.read_bytes()).hexdigest()]

        before = {p.name: identity(p) for p in paths}
        result = error = parsed = parse_error = inventory_error = None
        born, groups, setgroups, failedgroups, witness_errors = [], [], [], [], []
        inventory = []
        try:
            result = subprocess.run([self.normal if normal else self.modeled, directory, *paths[:3], fault],
                                    env=self.env, capture_output=True, timeout=15,
                                    preexec_fn=fixtures.fixture_limits)
        except BaseException as caught:
            error = caught
        finally:
            stdout = result.stdout if result is not None else getattr(error, "stdout", None) or b""
            stderr = result.stderr if result is not None else getattr(error, "stderr", None) or b""
            born, groups, setgroups, failedgroups, witness_errors = witnesses(stdout)
            try:
                parsed = parse(stdout)
            except BaseException as caught:
                parse_error = type(caught).__name__ + ": " + str(caught)[:1024]
            try:
                for p in sorted(directory.rglob("*")):
                    s = p.lstat()
                    item = {"path": str(p.relative_to(directory)), "device": s.st_dev, "inode": s.st_ino,
                            "mode": s.st_mode, "size": s.st_size, "links": s.st_nlink}
                    if stat.S_ISREG(s.st_mode) and s.st_size <= 65536:
                        item["sha256"] = hashlib.sha256(p.read_bytes()).hexdigest()
                    if stat.S_ISLNK(s.st_mode):
                        item["link"] = os.readlink(p)
                    inventory.append(item)
            except BaseException as caught:
                inventory_error = type(caught).__name__ + ": " + str(caught)[:1024]
            with self.ledger.open("a") as log:
                log.write(json.dumps({"case": str(directory), "fault": fault, "normal": normal,
                    "exit": result.returncode if result is not None else None,
                    "custody": "driver waited" if result is not None else "uncertain: invocation raised",
                    "error_type": type(error).__name__ if error else None,
                    "error": str(error)[:2048] if error else None,
                    "stdout_base64": base64.b64encode(stdout[:65536]).decode("ascii"),
                    "stderr_base64": base64.b64encode(stderr[:65536]).decode("ascii"),
                    "stdout_bytes": len(stdout), "stderr_bytes": len(stderr),
                    "stdout_sha256": hashlib.sha256(stdout).hexdigest(),
                    "stderr_sha256": hashlib.sha256(stderr).hexdigest(),
                    "born": born, "groups": groups, "setgroups": setgroups,
                    "failedgroups": failedgroups, "witness_errors": witness_errors,
                    "result": parsed, "parse_error": parse_error,
                    "before": before, "inventory": inventory, "inventory_error": inventory_error},
                    sort_keys=True) + "\n")
        if error:
            raise error
        self.assertEqual(result.returncode, 0, (fault, result.stdout, result.stderr))
        self.assertEqual(result.stderr, b"")
        self.assertIsNone(parse_error)
        self.assertEqual(witness_errors, [])
        self.assertIsNone(inventory_error)
        self.assertIsNotNone(parsed)
        self.assertLessEqual(len(stdout) + len(stderr), 4096)
        self.assertLessEqual(self.ledger.stat().st_size, 2 * 1024 * 1024)
        self.assertEqual(before, {p.name: identity(p) for p in paths})
        self.assertEqual(len(inventory), 4)
        self.assertEqual((parsed["state"], parsed["bad_order"], parsed["bad_close"]), (2, 0, 0))
        self.assertEqual(parsed["fake"], int(fault.startswith("modeled-")))
        self.assertEqual(len(born), int(parsed["pid"] > 0 and not parsed["fake"]))
        if born:
            self.assertEqual(born[0][0], parsed["pid"])
            self.assertGreater(born[0][2], 0)
            self.assertTrue(groups)
            self.assertTrue(all(group[0] == parsed["pid"] for group in groups))
            if parsed["group"]:
                self.assertIn([parsed["pid"], parsed["pid"]], setgroups)
        if parsed["pid"] and not parsed["lost"]:
            self.assertEqual((parsed["stop"], parsed["consumed"], parsed["signals"]), (1, 1, 1))
        return parsed

    def assert_rejection(self, r, cause):
        # An early failure can race Darwin's refused group signal before the
        # fixed child exits. Preserve actual stop uncertainty; never model it
        # away without the driver's positive unreaped completion witness.
        refused = r["stop_errno"] == errno.EPERM and not r["modeled_stop"]
        self.assertEqual((r["result"], r["cause"]), (-1 if refused else 0, cause))
        self.assertEqual((r["unknown_closes"], r["material_unknown_closes"], r["budget_failed"]), (0, 0, 0))
        if refused:
            self.assertEqual((r["stop"], r["consumed"], r["lost"]), (1, 1, 0))

    def test_fixed_arguments_environment_exact_retained_bytes_and_private_output(self):
        for fault in ["none", "short", "read-eintr", "exact-output"]:
            with self.subTest(fault=fault):
                r = self.fixture(fault)
                self.assertEqual((r["result"], r["cause"], r["group"], r["launch"], r["final_calls"]),
                                 (1, 0, 1, 1, 1))
                self.assertEqual(r["bytes"], 4096 if fault == "exact-output" else 2)

    def test_normal_host_unsupported_before_fork(self):
        r = self.fixture(normal=True)
        self.assertEqual((r["result"], r["cause"], r["pid"], r["final_calls"]), (0, 1, 0, 0))

    def test_preflight_refusals_consume_material_without_child(self):
        groups = {1: ["unsupported"], 2: ["root"], 3: ["sigchld-ignore", "sigchld-no-wait", "sigpipe-default"],
                  4: ["access", "cloexec-missing", "offset", "metadata", "links", "owner", "output-alias", "input-alias"],
                  5: ["cancel-before"], 6: ["clock-error", "clock-invalid"], 15: ["unsupported-wait"]}
        for cause, faults in groups.items():
            for fault in faults:
                with self.subTest(fault=fault):
                    r = self.fixture(fault)
                    self.assertEqual((r["result"], r["cause"], r["pid"], r["launch"]), (0, cause, 0, 0))

    def test_channel_and_fork_failures_no_launch(self):
        for fault in ["null-error", *[f"pipe-{x}" for x in range(3)],
                      *[f"cloexec-{x}" for x in range(1, 7)], "fork-error"]:
            with self.subTest(fault=fault):
                r = self.fixture(fault)
                self.assertEqual((r["result"], r["cause"], r["pid"], r["launch"]),
                                 (0, 9 if fault == "fork-error" else 8, 0, 0))

    def test_child_setup_and_exit_rejections_are_reaped(self):
        for fault in ["child-dup", "child-map", "child-limit", "child-closefrom", "child-boundary",
                      "exec-error", "child-reject", "child-signal"]:
            with self.subTest(fault=fault):
                r = self.fixture(fault)
                self.assert_rejection(r, 13)
                self.assertEqual(r["launch"], 1)

    def test_parent_group_boundary_and_dead_reader_no_launch(self):
        for fault, cause in [("group-error", 10), ("parent-boundary", 11), ("gate-error", 12),
                             ("gate-short", 12), ("gate-reader-death", 12)]:
            with self.subTest(fault=fault):
                r = self.fixture(fault)
                self.assert_rejection(r, cause)
                self.assertEqual(r["launch"], 0)

    def test_diagnostics_io_and_cancellation_fail_closed(self):
        for fault, cause in [("overflow", 14), ("poll-error", 8),
                             ("cancel-work", 5), ("cancel-close", 5), ("cancel-wait", 5),
                             ("cancel-final", 5), ("final-boundary", 11)]:
            with self.subTest(fault=fault):
                r = self.fixture(fault)
                self.assert_rejection(r, cause)
                self.assertLessEqual(r["bytes"], 4096)
        self.assertEqual(self.fixture("cancel-work-close")["result"], -1)
        r = self.fixture("read-error")
        self.assertEqual((r["result"], r["cause"]), (-1, 8))

    def test_shared_cleanup_deadline_includes_close_wait_and_final_boundary(self):
        for fault in ["deadline-work", "deadline-close", "deadline-wait", "deadline-final", "clock-final-error"]:
            with self.subTest(fault=fault):
                r = self.fixture(fault)
                self.assertNotEqual(r["result"], 1)
                self.assertEqual(r["cause"], 6 if fault == "clock-final-error" else 7)
                if fault != "deadline-work":
                    self.assertEqual(r["budget_failed"], 1)

    def test_close_failures_consumed_once_and_never_accepted(self):
        for prefix, outcome, field in [("close", -1, "unknown_closes"), ("ebadf", 0, "close_errors")]:
            for fd in range(7):
                with self.subTest(prefix=prefix, fd=fd):
                    r = self.fixture(f"{prefix}-{fd}")
                    if outcome == 0:
                        self.assert_rejection(r, 17)
                    else:
                        self.assertEqual((r["result"], r["cause"]), (outcome, 17))
                    self.assertTrue(r[field] & (1 << fd))
        for fd in range(3):
            with self.subTest(material=fd):
                r = self.fixture(f"material-close-{fd}")
                self.assertEqual((r["result"], r["cause"]), (-1, 17))
                # Prepared readonly handles retain material-role bits3..5.
                self.assertTrue(r["material_unknown_closes"] & (1 << (fd + 3)))

    def test_lost_wait_uses_only_nonspawning_syscall_models(self):
        for fault, signals, waits in [("modeled-observer-loss", 0, 0), ("modeled-wait-error", 1, 1)]:
            with self.subTest(fault=fault):
                r = self.fixture(fault)
                self.assertEqual((r["result"], r["cause"], r["lost"], r["consumed"],
                                  r["signals"], r["waits"]), (-1, 15, 1, 0, signals, waits))
        r = self.fixture("modeled-status")
        self.assertEqual((r["result"], r["cause"], r["lost"], r["consumed"]), (0, 13, 0, 1))

    def test_stop_uncertainty_and_child_hard_alarm(self):
        r = self.fixture("stop-error")
        self.assertEqual(r["result"], -1)
        r = self.fixture("child-alarm")
        self.assertEqual((r["result"], r["cause"], r["consumed"]), (0, 13, 1))

    def test_source_budget_and_production_link_boundary(self):
        files = list((ROOT / "src").glob("*.c")) + list((ROOT / "src").glob("*.h"))
        self.assertLessEqual(sum(p.stat().st_size for p in files), 72 * 1024)
        self.assertLessEqual(sum((ROOT / "src" / p).stat().st_size for p in
                                 ["signature_verify.c", "signature_verify.h", "signature_platform.c"]), 18 * 1024)
        makefile = (ROOT / "Makefile").read_text()
        self.assertIn("PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c\n", makefile)
        symbols = subprocess.run(["nm", "-u", str(self.directory / "normal.o")],
                                 env=self.env, capture_output=True, check=True, timeout=5).stdout.decode()
        for name in ["_system", "_popen", "_malloc", "_getenv", "_execvp"]:
            self.assertNotIn(name, symbols)

    def test_partial_output_preserves_independent_identity_witnesses(self):
        output = b"SV_BORN 123 100 400 5 0\nSV_SETGROUP 123 123\nSV_GROUP 123 123 0\nSV_GROUP 123 -1 3\nSV_BORN 456 -1 401 6 3\nSV_SETGROUP_FAILED 456 456 1 0\nSV_SETGROUP_FAILED 789 789 1 1\nSV_RESULT malformed\nSV_BORN 789"
        born, groups, setgroups, failedgroups, errors = witnesses(output)
        self.assertEqual(born, [[123, 100, 400, 5, 0], [456, -1, 401, 6, 3]])
        self.assertEqual(groups, [[123, 123, 0], [123, -1, 3]])
        self.assertEqual(setgroups, [[123, 123]])
        self.assertEqual(failedgroups, [[456, 456, 1, 0], [789, 789, 1, 1]])
        self.assertTrue(errors)
        with self.assertRaises(AssertionError):
            parse(output)
        for malformed in [b"SV_BORN 456 -1 401 6 0\n", b"SV_GROUP 123 -1 0\n",
                          b"SV_GROUP 123 -2 3\n", b"SV_SETGROUP 123 456\n",
                          b"SV_SETGROUP_FAILED 123 123 0 0\n", b"SV_SETGROUP_FAILED 123 123 1 2\n"]:
            self.assertTrue(witnesses(malformed)[4])

    def test_rejection_exception_is_only_recorded_host_stop_refusal(self):
        r = {"result": 0, "cause": 14, "stop_errno": 0, "modeled_stop": 0,
             "stop": 1, "consumed": 1, "lost": 0, "unknown_closes": 0,
             "material_unknown_closes": 0, "budget_failed": 0}
        self.assert_rejection(r, 14)
        r.update(result=-1, stop_errno=errno.EPERM)
        self.assert_rejection(r, 14)
        for key, value in [("unknown_closes", 1), ("material_unknown_closes", 1), ("budget_failed", 1),
                           ("result", 0), ("modeled_stop", 1), ("stop_errno", errno.EIO),
                           ("stop", 0), ("consumed", 0), ("lost", 1), ("cause", 16)]:
            with self.subTest(field=key):
                with self.assertRaises(AssertionError):
                    self.assert_rejection({**r, key: value}, 14)
