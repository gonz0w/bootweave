# SPDX-License-Identifier: BSD-3-Clause
"""Finite owned portable material fixtures; inert records, no verifier/key tool."""
import base64
import hashlib
import json
import os
from pathlib import Path
import resource
import shutil
import stat
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


def record(decoded, comment=b"inert format fixture"):
    return b"untrusted comment: " + comment + b"\n" + base64.b64encode(decoded) + b"\n"


KEY = record(b"Ed" + bytes(40))
SIGNATURE = record(b"Ed" + bytes(72))


def failure_fields(result):
    marker = b"\nMATERIAL_FAILURE "
    if marker not in result.stdout:
        return None
    parts = result.stdout.rsplit(marker, 1)[1].rstrip(b"\n").split(None, 10)
    names = ["state", "cause", "cancelled", "unknown_closes", "close_errors", "directory_lost",
             "member_opens", "writes", "directory_device", "directory_inode"]
    if len(parts) != 11:
        raise AssertionError("malformed fixture failure observation")
    return {**dict(zip(names, map(int, parts[:10]))), "path": os.fsdecode(parts[10])}


def fixture_limits():
    for which, cap in [(resource.RLIMIT_CORE, 0), (resource.RLIMIT_CPU, 2),
                       (resource.RLIMIT_NOFILE, 32), (resource.RLIMIT_FSIZE, 16384)]:
        soft, hard = resource.getrlimit(which)
        bounded = cap if hard == resource.RLIM_INFINITY else min(cap, hard)
        resource.setrlimit(which, (bounded if soft == resource.RLIM_INFINITY else min(soft, bounded), bounded))


class MaterialTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix="material-", dir=ROOT / ".tools")
        cls.addClassCleanup(cls.temp.cleanup)
        cls.directory = Path(cls.temp.name)
        ledger = tempfile.NamedTemporaryFile(prefix="material-observations-", suffix=".jsonl",
                                             dir=ROOT / ".tools", delete=False)
        cls.ledger = Path(ledger.name)
        ledger.close()
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1"}
        cls.flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic", "-I", str(ROOT / "src")]
        cls.normal = cls.directory / "normal.o"
        cls.injected = cls.directory / "injected.o"
        cls.binary = cls.directory / "material"
        replacements = ["clock_gettime", "mkdtemp", "open", "openat", "fcntl", "fstat", "lstat",
                        "fstatat", "write", "read", "close", "unlinkat", "rmdir", "getuid"]
        for path, defines in [(cls.normal, []), (cls.injected, [f"-D{x}=bw_m_{x}" for x in replacements])]:
            subprocess.run(cls.flags + defines + ["-c", str(ROOT / "src/auth_material.c"), "-o", str(path)],
                           check=True, capture_output=True, timeout=15, env=cls.env)
        subprocess.run(cls.flags + [str(cls.injected), str(ROOT / "src/auth_input.c"),
                                   str(ROOT / "tests/material_driver.c"), "-o", str(cls.binary)],
                       check=True, capture_output=True, timeout=15, env=cls.env)

    def fixture(self, key=KEY, signature=SIGNATURE, definition=b"exact\0definition\n", behavior="ok", fault="none", inspect=None):
        # Outside class scratch: an uncertain invocation cannot be erased by its cleanup.
        directory = Path(tempfile.mkdtemp(prefix="material-case-", dir=ROOT / ".tools"))
        result = error = None
        published = False
        inventory_error = None
        try:
            paths = [directory / name for name in ["key", "signature", "definition"]]
            for p, data in zip(paths, [key, signature, definition]):
                p.write_bytes(data)
            canary = directory / "canary"
            canary.write_bytes(b"outside fixture sentinel\n")
            before = {p.name: (p.stat().st_dev, p.stat().st_ino, hashlib.sha256(p.read_bytes()).digest())
                      for p in [*paths, canary]}
            try:
                result = subprocess.run([self.binary, directory, *paths, behavior, fault],
                                        capture_output=True, timeout=4, env=self.env, preexec_fn=fixture_limits)
            except BaseException as caught:
                error = caught
            finally:
                stdout = result.stdout if result else getattr(error, "stdout", None) or b""
                stderr = result.stderr if result else getattr(error, "stderr", None) or b""
                if isinstance(stdout, str): stdout = stdout.encode("utf-8", "replace")
                if isinstance(stderr, str): stderr = stderr.encode("utf-8", "replace")
                inventory = []
                try:
                    for p in sorted(directory.rglob("*")):
                        s = p.lstat()
                        row = {"path": str(p.relative_to(directory)), "device": s.st_dev, "inode": s.st_ino,
                               "mode": s.st_mode, "size": s.st_size, "links": s.st_nlink}
                        if stat.S_ISREG(s.st_mode) and s.st_size <= 1048576:
                            row["sha256"] = hashlib.sha256(p.read_bytes()).hexdigest()
                        if stat.S_ISLNK(s.st_mode): row["link"] = os.readlink(p)
                        inventory.append(row)
                except BaseException as observation_error:
                    inventory_error = type(observation_error).__name__ + ": " + str(observation_error)[:2048]
                fields = fields_error = None
                try:
                    if result: fields = failure_fields(result)
                except BaseException as parse_error:
                    fields_error = type(parse_error).__name__ + ": " + str(parse_error)[:2048]
                with self.ledger.open("a") as receipt:
                    receipt.write(json.dumps({"case": str(directory), "behavior": behavior, "fault": fault,
                        "exit": result.returncode if result else None,
                        "child_custody": "run returned after wait" if result else "uncertain: invocation raised",
                        "error_type": type(error).__name__ if error else None, "error": str(error)[:2048] if error else None,
                        "stdout_sha256": hashlib.sha256(stdout).hexdigest(), "stdout_bytes": len(stdout),
                        "stdout_base64": base64.b64encode(stdout[:65536]).decode("ascii"), "stdout_truncated": len(stdout)>65536,
                        "stderr_sha256": hashlib.sha256(stderr).hexdigest(), "stderr_bytes": len(stderr),
                        "stderr_base64": base64.b64encode(stderr[:65536]).decode("ascii"), "stderr_truncated": len(stderr)>65536,
                        "failure": fields, "failure_parse_error": fields_error,
                        "before": {name: [dev, ino, digest.hex()] for name, (dev, ino, digest) in before.items()},
                        "inventory": inventory, "inventory_error": inventory_error}, sort_keys=True) + "\n")
                published = True
            if error: raise error
            self.assertIsNone(inventory_error)
            self.assertIsNone(fields_error)
            self.assertLessEqual(self.ledger.stat().st_size, 2*1024*1024)
            after = {p.name: (p.stat().st_dev, p.stat().st_ino, hashlib.sha256(p.read_bytes()).digest())
                     for p in [*paths, canary]}
            self.assertEqual(before, after)
            self.assertEqual(result.stderr, b"")
            if inspect:
                inspect(directory, result)
            elif result.returncode in [0, 2]:
                self.assertFalse(list(directory.glob("verify.*")))
            return result
        finally:
            # No timeout/exception/unknown-custody fixture is automatically removed.
            if result is not None and published and inventory_error is None:
                shutil.rmtree(directory)

    def assert_ready(self, **kwargs):
        r = self.fixture(**kwargs)
        self.assertEqual((r.returncode, r.stdout),
                         (0, b"prepared-only\n" + kwargs.get("key", KEY) + kwargs.get("signature", SIGNATURE) +
                          kwargs.get("definition", b"exact\0definition\n")))

    def test_exact_caps_binary_readback_short_transfers_and_once_only_ownership(self):
        self.assert_ready()
        self.assert_ready(key=record(b"Ed"+bytes(40), b"x"*255),
                          signature=record(b"Ed"+bytes(72), b"y"*255), definition=bytes(range(256))*64)
        self.assert_ready(fault="short")
        self.assertEqual(self.fixture(behavior="abort").returncode, 0)

    def test_record_lines_comments_marker_base64_and_padding_are_pure(self):
        self.assertEqual(self.fixture(behavior="records").returncode, 0)
        for position, good, decoded in [(0, KEY, b"Ed"+bytes(40)), (1, SIGNATURE, b"Ed"+bytes(72))]:
            malformed = [b"", good[:-1], good+b"\n", good+b"trailing", good.replace(b"\n", b"\r\n"),
                         good.replace(b"untrusted", b"trusted", 1), record(decoded, b""), record(decoded, b"x"*256),
                         record(decoded, b"control\x1b"), record(decoded, b"nul\0"), record(decoded, b"high\xff"),
                         record(b"XX"+decoded[2:]), record(decoded[:-1]), record(decoded+b"x"),
                         good.replace(b"RW", b"!W", 1)]
            if position == 1:
                malformed += [good[:-3]+b"B=\n", good[:-2]+b"A\n", good[:-3]+b"===\n"]
            for bad in malformed:
                args = {"key" if position == 0 else "signature": bad}
                self.assertEqual(self.fixture(behavior="records", **args).returncode, 2)

    def test_bad_requests_never_open_or_write_members(self):
        for behavior in ["null-key", "null-signature", "null-definition", "null-request"]:
            result = self.fixture(behavior=behavior)
            self.assertEqual(result.returncode, 2)
            self.assertEqual((failure_fields(result)["member_opens"], failure_fields(result)["writes"]), (0, 0))
        for args in [{"definition": b""}, {"definition": b"x"*16385}, {"key": b"x"*1025},
                     {"signature": b"x"*16385}, {"signature": SIGNATURE+b"extra"}]:
            result = self.fixture(**args)
            self.assertEqual(result.returncode, 2)
            self.assertEqual((failure_fields(result)["member_opens"], failure_fields(result)["writes"]), (0, 0))

    def test_reservation_preserves_every_missing_fixed_role_and_capture_rejection(self):
        for hole in range(5):
            self.assertEqual(self.fixture(behavior=f"hole-{hole}").returncode, 0)

    def test_creation_privilege_clock_and_shared_deadline_refusals(self):
        for fault in ["root", "clock-error", "clock-invalid", "backward", "elapsed", "mkdir-error", "dup-error"]:
            result = self.fixture(fault=fault)
            self.assertEqual(result.returncode, 2, (fault, result.stdout))
        for fault in ["open-directory-error", "stat-directory-error", "close-original"]:
            self.assertEqual(self.fixture(fault=fault).returncode, 3, fault)

    def test_io_changes_bounds_and_finite_interruption_failures(self):
        for fault in ["write-error", "write-zero", "read-error", "read-truncate", "read-corrupt",
                      "write-eintr-forever", "read-eintr-forever", "changed-inode", "changed-mode",
                      "changed-owner", "changed-links", "changed-size"]:
            result = self.fixture(fault=fault)
            self.assertEqual(result.returncode, 2, (fault, result.stdout))
        for role in range(3):
            for prefix in ["open-write-error", "open-read-error"]:
                self.assertEqual(self.fixture(fault=f"{prefix}-{role}").returncode, 2)
            self.assertEqual(self.fixture(fault=f"stat-created-error-{role}").returncode, 3)

    def test_cancellation_every_lifecycle_phase_and_first_cause(self):
        for fault in ["cancel-before", "cancel-write", "cancel-read", "cancel-unlink", "cancel-rmdir", "cancel-close"]:
            result = self.fixture(fault=fault)
            self.assertEqual(result.returncode, 2, (fault, result.stdout))
            self.assertEqual(failure_fields(result)["cause"], 3)
        for fault in ["cancel-create", "cancel-write-close"]:
            result = self.fixture(fault=fault)
            self.assertEqual(result.returncode, 3)
            self.assertEqual(failure_fields(result)["cause"], 3)

    def test_cancel_and_expiry_during_directory_observation_prevent_member_effects(self):
        for fault, cause in [("cancel-observe", 3), ("cancel-named-observe", 3), ("elapsed-observe", 4)]:
            result = self.fixture(fault=fault)
            self.assertEqual(result.returncode, 2)
            fields = failure_fields(result)
            self.assertEqual((fields["cause"], fields["member_opens"], fields["writes"], fields["directory_lost"]),
                             (cause, 0, 0, 0))

    def test_each_close_role_uncertainty_and_no_success_after_final_deadline(self):
        for role in range(3):
            for prefix, code in [("close-writer", 3), ("close-reader", 3), ("ebadf-reader", 2)]:
                self.assertEqual(self.fixture(fault=f"{prefix}-{role}").returncode, code)
        for fault in ["close-directory", "close-deadline", "close-clock-error"]:
            self.assertEqual(self.fixture(fault=fault).returncode, 3 if fault == "close-directory" else 2)

    def test_failed_cleanup_and_unknown_names_are_quarantined(self):
        def remains(directory, result):
            self.assertEqual(result.returncode, 3)
            self.assertTrue(list(directory.glob("verify.*")))
        for fault in ["unlink-error-0", "unlink-error-1", "unlink-error-2", "rmdir-error", "extra-name", "replace-name",
                      "directory-zero-links", "directory-named-zero-links", "directory-reserve-zero-links"]:
            self.fixture(fault=fault, inspect=remains)
        def replaced(directory, result):
            remains(directory, result)
            self.assertEqual((directory / "moved/original-sentinel").read_bytes(), b"original")
            self.assertTrue(next(directory.glob("verify.*")).is_dir())
        self.fixture(fault="replace-directory", inspect=replaced)

    def test_source_budget_symbols_and_production_link_unchanged(self):
        self.assertLessEqual(sum((ROOT / "src" / p).stat().st_size for p in ["auth_material.c", "auth_material.h", "private_start_material.h"]), 18*1024)
        result = subprocess.run(["nm", "-u", self.normal], capture_output=True, check=True, timeout=3, env=self.env)
        for forbidden in [b"getenv", b"fork", b"exec", b"socket", b"system", b"bw_m_", b"malloc"]:
            self.assertNotIn(forbidden, result.stdout)
        self.assertNotIn("auth_material", (ROOT / "Makefile").read_text())
