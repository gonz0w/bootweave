# SPDX-License-Identifier: BSD-3-Clause
"""Portable planner oracles; these do not qualify OpenBSD confinement."""

import copy
import itertools
import json
import os
from pathlib import Path
import platform
import pty
import random
import resource
import signal
import socket
import subprocess
import tempfile
import time
import unittest

ROOT = Path(__file__).resolve().parents[1]
from plan_cases import BASE, encode, expected


class PlannerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        (ROOT / ".tools").mkdir(exist_ok=True)
        cls.temp = tempfile.TemporaryDirectory(prefix="planner-", dir=ROOT / ".tools")
        cls.directory = Path(cls.temp.name)
        cls.driver = cls.directory / "driver"
        cls.production = cls.directory / "bootweave"
        cls.unit = cls.directory / "unit"
        cls.flags = ["cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-pedantic",
                     "-O2", "-I", str(ROOT / "src")]
        cls.core = [str(ROOT / "src/plan.c"), str(ROOT / "src/plan_io.c"),
                    str(ROOT / "src/native.c")]
        for entry, output in [("tests/plan_driver.c", cls.driver),
                              ("src/main.c", cls.production),
                              ("tests/plan_unit.c", cls.unit)]:
            subprocess.run(cls.flags + cls.core + [str(ROOT / entry), "-o", str(output)],
                           check=True, capture_output=True)

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def run_bytes(self, data, binary=None, **kwargs):
        with tempfile.NamedTemporaryFile(dir=self.directory) as named:
            named.write(data)
            named.flush()
            with open(named.name, "rb") as readonly:
                return subprocess.run([str(binary or self.driver)], stdin=readonly,
                                      capture_output=True, timeout=7, **kwargs)

    def rejected(self, data):
        result = self.run_bytes(data)
        self.assertEqual(result.returncode, 3, data[:100])
        self.assertEqual(result.stdout, b"")
        self.assertLessEqual(len(result.stderr), 256)
        self.assertRegex(result.stderr, rb"^bootweave: (input rejected|definition rejected at byte [0-9]+)\n$")

    def test_all_orders_and_generic_projects(self):
        for identity, name in [("offline-viewer", "Offline Viewer"),
                               ("dns", "DNS"), ("firewall", "Firewall")]:
            model = copy.deepcopy(BASE)
            model["project"] = {"id": identity, "name": name}
            for root_order in itertools.permutations(model):
                for reversals in itertools.product([False, True], repeat=3):
                    ordered = {}
                    for key in root_order:
                        value = model[key]
                        if isinstance(value, dict):
                            reverse = reversals[["project", "system", "payload"].index(key)]
                            value = dict(list(value.items())[:: -1 if reverse else 1])
                        ordered[key] = value
                    result = self.run_bytes(encode(ordered))
                    self.assertEqual(result.returncode, 0)
                    self.assertEqual(result.stdout, expected(model))
                    self.assertEqual(result.stderr, b"")

    def test_whitespace_escapes_and_literal_metacharacters(self):
        model = copy.deepcopy(BASE)
        model["project"]["name"] = 'Viewer %s %n $(id); `id` \\ " /'
        data = json.dumps(model, indent=2).encode().replace(b"Viewer", b"\\u0056iewer")
        result = self.run_bytes(b" \t\r\n" + data + b"\r\n ")
        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, expected(model))

    def test_required_unknown_duplicate_and_wrong_types(self):
        wrong = [None, True, False, 0, 1, 1.5, [], {}, ""]
        for group in [None, "project", "system", "payload"]:
            target = BASE if group is None else BASE[group]
            for key in target:
                model = copy.deepcopy(BASE)
                node = model if group is None else model[group]
                del node[key]
                self.rejected(encode(model))
                for value in wrong:
                    if type(value) is type(target[key]) and value == target[key]:
                        continue
                    model = copy.deepcopy(BASE)
                    node = model if group is None else model[group]
                    node[key] = value
                    self.rejected(encode(model))
                raw = encode(BASE)
                needle = json.dumps(key).encode() + b":"
                # Duplicate immediately before the original, preserving original value.
                value = encode(target[key])
                self.rejected(raw.replace(needle, needle + value + b"," + needle, 1))
                escaped = b'"\\u%04x' % ord(key[0]) + key[1:].encode() + b'":'
                self.rejected(raw.replace(needle, escaped, 1))
            model = copy.deepcopy(BASE)
            node = model if group is None else model[group]
            node["unknown"] = "x"
            self.rejected(encode(model))

    def test_each_field_boundary(self):
        cases = [
            ("project", "id", ["a", "a" * 63, "a" * 64],
             ["", "a" * 65, "A", "1a", "-a", "a_b", "a.b"]),
            ("project", "name", ["X", "X" * 79, "X" * 80],
             ["", " ", " " * 80, "X" * 81]),
            ("payload", "file", ["a", "A" * 127, "A" * 128, "a._-9"],
             ["", "A" * 129, ".a", "../a", "/a", "a/b", "a b"]),
            ("payload", "sha256", ["0123456789abcdef" * 4],
             ["a" * 63, "a" * 65, "A" * 64, "g" * 64]),
            ("system", "release", ["7.9"], ["7.8", "7.90", "", "7.9 "]),
            ("system", "architecture", ["amd64"], ["arm64", "AMD64", "amd64 "]),
        ]
        for group, key, accepted, rejected in cases:
            for value in accepted + rejected:
                model = copy.deepcopy(BASE)
                model[group][key] = value
                if value in rejected:
                    self.rejected(encode(model))
                else:
                    result = self.run_bytes(encode(model))
                    self.assertEqual(result.returncode, 0)
                    self.assertEqual(result.stdout, expected(model))

    def test_control_unicode_and_json_rejections(self):
        raw = encode(BASE)
        for ch in list(range(32)) + [127, 128, 255]:
            self.rejected(raw.replace(b"Offline Viewer", bytes([ch])))
        for value in ["\\u0000", "\\u001b", "\\u007f", "\\u0080", "\\ud800",
                      "\\udfff", "\\ud800\\udc00", "\\n", "\\b", "\\f", "\\r", "\\t", "\\x41"]:
            self.rejected(raw.replace(b"Offline Viewer", value.encode()))
        for version in [b"0", b"2", b"-1", b"+1", b"01", b"1.0", b"1e0", b"1" * 1000]:
            self.rejected(raw.replace(b'"schema_version":1', b'"schema_version":' + version))
        for invalid in [b"", b"{}", b"[]", b"null", b"\xef\xbb\xbf" + raw,
                        raw + b"x", raw + b"{}", b"// comment\n" + raw,
                        raw[:-1] + b",}", b"{" * 1000, b" " * 16384,
                        raw.replace(b"Offline Viewer", "caf\u00e9".encode("utf-8"))]:
            self.rejected(invalid)
        for end in range(len(raw)):
            self.rejected(raw[:end])

    def test_input_byte_ceiling(self):
        raw = encode(BASE)
        for length in [16383, 16384]:
            result = self.run_bytes(raw + b" " * (length - len(raw)))
            self.assertEqual(result.returncode, 0)
            self.assertEqual(result.stdout, expected(BASE))
        self.rejected(raw + b" " * (16385 - len(raw)))

    def test_descriptor_refusals(self):
        result = subprocess.run([self.driver], input=encode(BASE), capture_output=True, timeout=2)
        self.assertEqual((result.returncode, result.stdout), (3, b""))
        with open(os.devnull, "rb") as device:
            result = subprocess.run([self.driver], stdin=device, capture_output=True, timeout=2)
            self.assertEqual((result.returncode, result.stdout), (3, b""))
        with tempfile.TemporaryFile(dir=self.directory) as readwrite:
            readwrite.write(encode(BASE))
            readwrite.seek(0)
            result = subprocess.run([self.driver], stdin=readwrite, capture_output=True, timeout=2)
            self.assertEqual((result.returncode, result.stdout), (3, b""))
        with tempfile.NamedTemporaryFile(dir=self.directory) as named:
            named.write(encode(BASE))
            named.flush()
            with open(named.name, "rb") as source:
                source.seek(1)
                result = subprocess.run([self.driver], stdin=source, capture_output=True, timeout=2)
                self.assertEqual((result.returncode, result.stdout), (3, b""))
        a, b = socket.socketpair()
        try:
            result = subprocess.run([self.driver], stdin=a, capture_output=True, timeout=2)
            self.assertEqual((result.returncode, result.stdout), (3, b""))
        finally:
            a.close()
            b.close()
        master, slave = pty.openpty()
        try:
            result = subprocess.run([self.driver], stdin=slave, capture_output=True, timeout=2)
            self.assertEqual((result.returncode, result.stdout), (3, b""))
        finally:
            os.close(master)
            os.close(slave)
        fifo = self.directory / "owned-fifo"
        os.mkfifo(fifo)
        fd = os.open(fifo, os.O_RDONLY | os.O_NONBLOCK)
        try:
            result = subprocess.run([self.driver], stdin=fd, capture_output=True, timeout=2)
            self.assertEqual((result.returncode, result.stdout), (3, b""))
        finally:
            os.close(fd)
            fifo.unlink()

    def test_output_alias_and_canary_invariants(self):
        with tempfile.TemporaryDirectory(dir=self.directory) as owned:
            directory = Path(owned)
            source = directory / "definition"
            canary = directory / "outside-canary"
            source.write_bytes(encode(BASE))
            canary.write_bytes(b"independent canary\n")
            def inventory():
                return {p.name: (p.read_bytes(), p.stat().st_mode, p.stat().st_uid,
                                 p.stat().st_gid, p.stat().st_mtime_ns, p.stat().st_ctime_ns)
                        for p in directory.iterdir()}
            before = inventory()
            with source.open("rb") as input_file:
                result = subprocess.run([self.driver], stdin=input_file, capture_output=True,
                                        cwd=directory, timeout=2)
            self.assertEqual(result.returncode, 0)
            self.assertEqual(before, inventory())
            for output_fd in ["stdout", "stderr"]:
                with source.open("rb") as input_file, source.open("ab") as output_file:
                    options = {"stdin": input_file, "stdout": subprocess.PIPE,
                               "stderr": subprocess.PIPE, output_fd: output_file}
                    result = subprocess.run([self.driver], **options, timeout=2)
                self.assertEqual(result.returncode, 3)
                self.assertEqual(before, inventory())

    def test_broken_output_and_stderr_failure(self):
        read_fd, write_fd = os.pipe()
        os.close(read_fd)
        try:
            with tempfile.NamedTemporaryFile(dir=self.directory) as source:
                source.write(encode(BASE))
                source.flush()
                with open(source.name, "rb") as input_file:
                    result = subprocess.run([self.driver], stdin=input_file,
                                            stdout=write_fd, stderr=subprocess.PIPE, timeout=2)
                self.assertEqual(result.returncode, 5)
                source.seek(0)
                source.truncate()
                source.write(b"invalid")
                source.flush()
                with open(source.name, "rb") as input_file:
                    result = subprocess.run([self.driver], stdin=input_file,
                                            stdout=subprocess.PIPE, stderr=write_fd, timeout=2)
                self.assertEqual((result.returncode, result.stdout), (5, b""))
        finally:
            os.close(write_fd)

    def test_blocked_output_deadline_with_inherited_signal_mask(self):
        import fcntl
        read_fd, write_fd = os.pipe()
        try:
            fcntl.fcntl(write_fd, fcntl.F_SETFL, os.O_NONBLOCK)
            while True:
                try:
                    os.write(write_fd, b"X" * 4096)
                except BlockingIOError:
                    break
            fcntl.fcntl(write_fd, fcntl.F_SETFL, 0)
            with tempfile.NamedTemporaryFile(dir=self.directory) as source:
                source.write(encode(BASE))
                source.flush()
                started = time.monotonic()
                with open(source.name, "rb") as input_file:
                    result = subprocess.run(
                        [self.driver], stdin=input_file, stdout=write_fd,
                        stderr=subprocess.PIPE, timeout=7,
                        preexec_fn=lambda: signal.pthread_sigmask(signal.SIG_BLOCK, {signal.SIGALRM}))
                self.assertEqual(result.returncode, 4)
                self.assertGreater(time.monotonic() - started, 4)
                self.assertLess(time.monotonic() - started, 7)
        finally:
            os.close(read_fd)
            os.close(write_fd)

    def test_deterministic_mutations(self):
        raw = encode(BASE)
        generator = random.Random(20261015)
        for _ in range(256):
            # Every mutation inserts a forbidden raw byte; validity is independently known.
            at = generator.randrange(len(raw) + 1)
            mutation = raw[:at] + bytes([generator.choice([0, 1, 27, 127, 128, 255])]) + raw[at:]
            self.rejected(mutation)

    def test_cpu_deadline_with_inherited_signal_mask(self):
        stimulus = self.directory / "cpu-stimulus"
        subprocess.run(self.flags + self.core + [str(ROOT / "tests/plan_cpu.c"),
                                                "-o", str(stimulus)],
                       check=True, capture_output=True)
        before = resource.getrusage(resource.RUSAGE_CHILDREN)
        started = time.monotonic()
        result = subprocess.run(
            [stimulus], capture_output=True, timeout=7,
            preexec_fn=lambda: signal.pthread_sigmask(signal.SIG_BLOCK, {signal.SIGPROF}))
        elapsed = time.monotonic() - started
        after = resource.getrusage(resource.RUSAGE_CHILDREN)
        cpu = after.ru_utime + after.ru_stime - before.ru_utime - before.ru_stime
        self.assertEqual((result.returncode, result.stdout, result.stderr), (4, b"", b""))
        # Host scheduling/accounting only; the stricter native 2.1 s gate is unchanged.
        self.assertGreater(cpu, 1.5)
        self.assertLess(cpu, 2.5)
        self.assertLess(elapsed, 5)

    def test_model_api_and_production_cli(self):
        subprocess.run([self.unit], check=True, capture_output=True, timeout=2)
        for argument in ["--help", "--version"]:
            result = subprocess.run([self.production, argument], capture_output=True, timeout=2)
            self.assertEqual(result.returncode, 0)
            self.assertTrue(result.stdout)
            self.assertEqual(result.stderr, b"")
        for arguments in [[], ["unknown"], ["plan", "extra"], ["--help", "extra"]]:
            result = subprocess.run([self.production, *arguments], capture_output=True, timeout=2)
            self.assertEqual((result.returncode, result.stdout), (2, b""))
            self.assertNotIn(b"unknown", result.stderr)
        if platform.system() != "OpenBSD":
            result = subprocess.run([self.production, "plan"], capture_output=True, timeout=2)
            self.assertEqual((result.returncode, result.stdout), (77, b""))
        source_bytes = sum(p.stat().st_size for p in (ROOT / "src").iterdir()
                           if p.suffix in {".c", ".h"})
        self.assertLessEqual(source_bytes, 72 * 1024)
        planner_bytes = sum((ROOT / path).stat().st_size for path in
                            ["src/main.c", "src/plan.c", "src/plan_io.c", "src/native.c", "src/plan.h"])
        self.assertLessEqual(planner_bytes, 24 * 1024)

    def test_io_faults_and_short_transfers(self):
        fault_object = self.directory / "fault-io.o"
        fault_driver = self.directory / "fault-driver"
        subprocess.run(self.flags + ["-Dread=bw_test_read", "-Dwrite=bw_test_write",
                                    "-Dfstat=bw_test_fstat", "-Dsetitimer=bw_test_setitimer",
                                    "-Dsigaction=bw_test_sigaction", "-c",
                                    str(ROOT / "src/plan_io.c"), "-o", str(fault_object)],
                       check=True, capture_output=True)
        subprocess.run(self.flags + [str(ROOT / "src/plan.c"), str(fault_object),
                                    str(ROOT / "tests/plan_driver.c"),
                                    str(ROOT / "tests/plan_faults.c"), "-o", str(fault_driver)],
                       check=True, capture_output=True)
        for mode, status in [("short", 0), ("changed", 3), ("read-error", 3),
                             ("write-zero", 5), ("write-error", 5),
                             ("timer-real", 4), ("timer-cpu", 4), ("sigprof", 4)]:
            result = self.run_bytes(encode(BASE), fault_driver,
                                    env={**os.environ, "BW_TEST_FAULT": mode})
            self.assertEqual(result.returncode, status, mode)
            self.assertEqual(result.stdout, expected(BASE) if status == 0 else b"")

    def test_sanitized_bounded_corpus(self):
        sanitized = self.directory / "sanitized-driver"
        sanitized_unit = self.directory / "sanitized-unit"
        flags = self.flags + ["-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g"]
        for entry, output in [("tests/plan_driver.c", sanitized),
                              ("tests/plan_unit.c", sanitized_unit)]:
            subprocess.run(flags + self.core + [str(ROOT / entry), "-o", str(output)],
                           check=True, capture_output=True)
        environment = {**os.environ, "ASAN_OPTIONS": "detect_leaks=0:halt_on_error=1",
                       "UBSAN_OPTIONS": "halt_on_error=1:print_stacktrace=0"}
        subprocess.run([sanitized_unit], env=environment, check=True,
                       capture_output=True, timeout=7)
        raw = encode(BASE)
        corpus = [(raw, 0), (raw + b" " * (16384 - len(raw)), 0),
                  (raw + b" " * (16385 - len(raw)), 3), (b"{" * 16384, 3)]
        generator = random.Random(20261015)
        for _ in range(64):
            at = generator.randrange(len(raw) + 1)
            corpus.append((raw[:at] + b"\x00" + raw[at:], 3))
        for data, status in corpus:
            result = self.run_bytes(data, sanitized, env=environment)
            self.assertEqual(result.returncode, status)
            self.assertNotIn(b"Sanitizer", result.stderr)
            self.assertEqual(result.stdout, expected(BASE) if status == 0 else b"")


if __name__ == "__main__":
    unittest.main()
