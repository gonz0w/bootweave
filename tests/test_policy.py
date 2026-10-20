# SPDX-License-Identifier: BSD-3-Clause
"""Independent exact-byte oracles; no signer, archive effect or native authority."""
import base64
import copy
import hashlib
import json
import os
from pathlib import Path
import resource
import selectors
import signal
import struct
import subprocess
import sys
import tempfile
import time
import unittest
from unittest import mock

ROOT = Path(__file__).resolve().parents[1]
KEY = b"untrusted comment: Bootweave inert policy fixture\n" + base64.b64encode(b"Ed" + bytes(40)) + b"\n"
DEFINITION = (ROOT / "tests/fixtures/policy-definition.json").read_bytes()
PAYLOAD = (ROOT / "tests/fixtures/policy-payload.txt").read_bytes()
MODEL = json.loads(DEFINITION)


def encode(model):
    return json.dumps(model, separators=(",", ":")).encode("ascii")


def array(data):
    return "{" + (",".join(str(b) for b in data) or "0") + "}"


def limits(compile_job=False):
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    resource.setrlimit(resource.RLIMIT_CPU, ((20 if compile_job else 2),) * 2)
    resource.setrlimit(resource.RLIMIT_NOFILE, ((64 if compile_job else 32),) * 2)
    resource.setrlimit(resource.RLIMIT_FSIZE, ((16777216 if compile_job else 16384),) * 2)


class PolicyTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if sys.platform != "darwin":
            raise RuntimeError("Mac portable policy fixture only; no native dispatch")
        cls.directory = Path(tempfile.mkdtemp(prefix="policy-build-", dir=ROOT / ".tools"))
        cls.ledger = cls.directory / "observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1",
                   "ASAN_OPTIONS": "detect_leaks=0:abort_on_error=1"}
        cls.flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic", "-I", str(ROOT / "src")]
        cls.calls = cls.builds = 0
        cls.cache = {}
        cls.quarantine = []
        cls.objects = []
        for source in ["policy_binding", "plan", "auth_material", "policy_platform", "policy_data"]:
            target = cls.directory / (source + ".o")
            cls.execute(cls.flags + ["-c", str(ROOT / "src" / (source + ".c")), "-o", str(target)], compile_job=True)
            if source not in ("policy_platform", "policy_data"):
                cls.objects.append(str(target))
        cls.driver = cls.directory / "driver.o"
        cls.execute(cls.flags + ["-c", str(ROOT / "tests/policy_driver.c"), "-o", str(cls.driver)], compile_job=True)
        cls.default = cls.directory / "default"
        cls.execute(cls.flags + cls.objects + [str(cls.driver), str(cls.directory / "policy_data.o"), "-o", str(cls.default)], compile_job=True)
        cls.native = cls.directory / "native-unavailable"
        cls.execute(cls.flags + ["-DPB_NATIVE", str(ROOT / "tests/policy_driver.c")] + cls.objects +
                    [str(cls.directory / "policy_data.o"), str(cls.directory / "policy_platform.o"), "-o", str(cls.native)], compile_job=True)
        cls.sanitized = cls.directory / "sanitized"
        cls.execute(cls.flags + ["-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g"] +
                    [str(ROOT / "src" / (x + ".c")) for x in ["policy_binding", "plan", "auth_material", "policy_data"]] +
                    [str(ROOT / "tests/policy_driver.c"), "-o", str(cls.sanitized)], compile_job=True)

    @classmethod
    def execute(cls, command, data=None, compile_job=False, expected_failure=False):
        cls.calls += 1
        if cls.calls > 320:
            raise AssertionError("finite fixture invocation cap")
        case = cls.directory / f"case-{cls.calls:03d}"
        case.mkdir(mode=0o700)
        if data is not None:
            (case / "input.bin").write_bytes(data)
        record = {"command": list(map(str, command)), "compile": compile_job, "started": time.monotonic(),
                  "stream_cap": 16384, "wait_consumed": False, "wait_lost": False,
                  "pipes": {}, "errors": [], "expected_failure_control": expected_failure}
        process = None
        input_file = None
        selector = selectors.DefaultSelector()
        buffers = {"stdout": bytearray(), "stderr": bytearray()}
        streams = {}
        completed = False

        def error(message):
            if len(record["errors"]) < 12:
                record["errors"].append(str(message)[:512])

        def close_stream(name, eof=False):
            stream = streams.pop(name)
            disposition = {"eof": eof, "close": "attempted"}
            try:
                selector.unregister(stream)
            except BaseException as caught:
                error("unregister:" + repr(caught))
            try:
                stream.close()
                disposition["close"] = "closed"
            except BaseException as caught:
                disposition["close"] = "uncertain"
                error("pipe-close:" + repr(caught))
            record["pipes"][name] = disposition

        def observe():
            nonlocal completed
            if completed or record["wait_lost"]:
                return
            try:
                found = os.waitid(os.P_PID, process.pid, os.WEXITED | os.WNOHANG | os.WNOWAIT)
                if found is not None and found.si_pid:
                    if found.si_pid != process.pid:
                        raise RuntimeError("unexpected child identity")
                    completed = True
                    record["completion_observed"] = True
            except InterruptedError:
                return
            except BaseException as caught:
                record["wait_lost"] = True
                error("nonreaping-observation:" + repr(caught))

        def drain(timeout):
            for selected, _ in selector.select(timeout):
                name = selected.data
                try:
                    chunk = os.read(selected.fd, min(4096, 16384 - len(buffers[name]) + 1))
                except (InterruptedError, BlockingIOError):
                    continue
                except BaseException as caught:
                    error("pipe-read:" + repr(caught))
                    close_stream(name)
                    continue
                if not chunk:
                    close_stream(name, eof=True)
                else:
                    record[name + "_bytes_observed"] = record.get(name + "_bytes_observed", 0) + len(chunk)
                    if len(buffers[name]) + len(chunk) > 16384:
                        buffers[name].extend(chunk[:16384-len(buffers[name])])
                        record.setdefault("overflow", []).append(name)
                        error("live pipe output cap exceeded")
                        close_stream(name)
                    else:
                        buffers[name].extend(chunk)

        try:
            if signal.getsignal(signal.SIGCHLD) != signal.SIG_DFL:
                raise RuntimeError("exclusive default child-wait ownership required")
            if data is not None:
                input_file = (case / "input.bin").open("rb")
            process = subprocess.Popen(command, env=cls.env, cwd=case, stdin=input_file or subprocess.DEVNULL,
                                       stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=True,
                                       preexec_fn=lambda: limits(compile_job))
            record["pid"] = process.pid
            record["born_monotonic"] = time.monotonic()
            streams.update(stdout=process.stdout, stderr=process.stderr)
            for name, stream in streams.items():
                os.set_blocking(stream.fileno(), False)
                selector.register(stream, selectors.EVENT_READ, name)
            deadline = record["started"] + (20 if compile_job else 4)
            while streams or not completed:
                observe()
                if record["wait_lost"] or record["errors"]:
                    break
                if time.monotonic() >= deadline:
                    record["timeout"] = True
                    error("finite owned fixture work deadline")
                    break
                drain(.02)
        except BaseException as caught:
            error("fixture:" + repr(caught))
        finally:
            if process is not None:
                observe()
                if not completed and not record["wait_lost"]:
                    record["stop_attempted_before_wait"] = True
                    try:
                        os.kill(process.pid, signal.SIGKILL)
                        record["stop"] = "sent to born unreaped owned child"
                    except BaseException as caught:
                        record["stop"] = "unconfirmed"
                        error("stop:" + repr(caught))
                cleanup_deadline = time.monotonic() + 5
                while time.monotonic() < cleanup_deadline and (streams or not completed):
                    observe()
                    try:
                        drain(.02)
                    except BaseException as caught:
                        error("cleanup-drain:" + repr(caught))
                        break
                    if record["wait_lost"]:
                        break
                if completed and not record["wait_lost"]:
                    try:
                        while True:
                            try:
                                waited, status, _ = os.wait4(process.pid, os.WNOHANG)
                                break
                            except InterruptedError:
                                if time.monotonic() >= cleanup_deadline:
                                    raise RuntimeError("consuming wait deadline")
                        if waited:
                            record["wait_consumed"] = True
                            if waited != process.pid:
                                raise RuntimeError("unexpected consuming wait identity")
                            process.returncode = os.waitstatus_to_exitcode(status)
                    except BaseException as caught:
                        record["wait_lost"] = not record["wait_consumed"]
                        error("consuming-wait:" + repr(caught))
                if not record["wait_consumed"]:
                    error("unresolved child ownership; outer held group remains stop owner")
                    cls.quarantine.append(process)
            for name in list(streams):
                error("incomplete pipe EOF:" + name)
                close_stream(name)
            try:
                selector.close()
                record["selector_close"] = "closed"
            except BaseException as caught:
                error("selector-close:" + repr(caught))
            if input_file is not None:
                try:
                    input_file.close()
                    record["stdin_close"] = "closed"
                except BaseException as caught:
                    record["stdin_close"] = "uncertain"
                    error("stdin-close:" + repr(caught))
            for name, buffer in buffers.items():
                record[name + "_sha256"] = hashlib.sha256(buffer).hexdigest()
                record[name + "_bytes_retained"] = len(buffer)
                publication = {"status": "attempted"}
                try:
                    written = (case / name).write_bytes(buffer)
                    publication["bytes_written"] = written
                    if written != len(buffer):
                        raise RuntimeError("partial owned stream publication")
                    publication["status"] = "written"
                except BaseException as caught:
                    publication.update(status="unconfirmed", error=repr(caught)[:512])
                    error("stream-publication:" + name + ":" + repr(caught))
                record.setdefault("stream_files", {})[name] = publication
            record["elapsed"] = time.monotonic() - record["started"]
            if process is not None:
                record["returncode"] = process.returncode
            record["succeeded"] = bool(record["wait_consumed"] and not record["wait_lost"] and
                                       not record["errors"] and process.returncode == 0 and not buffers["stderr"])
            try:
                with cls.ledger.open("a") as ledger:
                    row = json.dumps(record) + "\n"
                    if ledger.write(row) != len(row):
                        raise RuntimeError("partial owned ledger publication")
                    ledger.flush()
            except BaseException as caught:
                raise AssertionError("owned ledger publication unconfirmed; case retained: " + repr(caught)[:512]) from caught
        if record["errors"] or not record["wait_consumed"] or process.returncode or buffers["stderr"]:
            raise AssertionError(f"fixture {case.name}: status={record.get('returncode')}, errors={record['errors']!r}, stderr={bytes(buffers['stderr'][:1024])!r}")
        return bytes(buffers["stdout"])

    @classmethod
    def linked(cls, definition=DEFINITION, **changes):
        policy = {"profile": 1, "key": KEY, "key_length": len(KEY), "digest": hashlib.sha256(definition).digest(),
                  "id": b"offline-viewer", "release": b"7.9", "arch": b"amd64"}
        policy.update(changes)
        source = '#include "policy_binding.h"\nconst struct bw_policy_definition bw_compiled_policy = {\n' + ",\n".join(
            [str(policy["profile"]), str(policy["key_length"])] +
            [array(policy[x]) for x in ["key", "digest", "id", "release", "arch"]]) + "\n};\n"
        identity = hashlib.sha256(source.encode()).hexdigest()
        if identity in cls.cache:
            return cls.cache[identity]
        cls.builds += 1
        if cls.builds > 64:
            raise AssertionError("finite link-time fixture cap")
        path = cls.directory / f"policy-{cls.builds:02d}.c"
        path.write_text(source)
        binary = path.with_suffix("")
        cls.execute(cls.flags + cls.objects + [str(cls.driver), str(path), "-o", str(binary)], compile_job=True)
        cls.cache[identity] = binary
        return binary

    def run_bytes(self, definition=DEFINITION, payload=PAYLOAD, key=KEY, binary=None, mode="normal"):
        frame = struct.pack(">III", len(key), len(definition), len(payload)) + key + definition + payload
        return self.execute([str(binary or self.default), mode], frame)

    def expect(self, cause=0, calls=2, **kwargs):
        data = self.run_bytes(**kwargs)
        first = data.splitlines()[0]
        self.assertEqual(first, f"PB_RESULT {int(cause == 0)} {cause} {calls} {int(cause != 0)}".encode())
        self.assertEqual(len(data.splitlines()), 2 if cause == 0 else 1)
        return data

    def test_exact_default_and_generic_use_are_identity_only(self):
        for project, name in [("offline-viewer", "Offline Viewer"), ("sample-dns", "Sample DNS"), ("sample-firewall", "Sample Firewall")]:
            model = copy.deepcopy(MODEL)
            model["project"] = {"id": project, "name": name}
            definition = DEFINITION if project == "offline-viewer" else encode(model)
            result = self.expect(definition=definition, binary=self.linked(definition, id=project.encode()))
            expected = "|".join([project, name, "7.9", "amd64", "synthetic.tar", model["payload"]["sha256"]]).encode()
            self.assertEqual(result.splitlines()[1], expected)
        # This payload is deliberately not a tar archive; matching cannot inspect it.
        self.assertEqual(hashlib.sha256(DEFINITION).hexdigest(), "028217bc0cc16683fb541ce2a568c82488404297a9df9ce20be581a62694fbb4")
        self.expect()

    def test_exact_key_and_original_definition_bytes(self):
        for key in [KEY[:-1], KEY + b"\n", KEY.replace(b"fixture", b"Fixture"), KEY[:-3] + b"AB\n"]:
            self.expect(3, 0, key=key)
        variants = [DEFINITION[:-1], b" " + DEFINITION, json.dumps(MODEL, sort_keys=True).encode(),
                    DEFINITION.replace(b"Viewer", b"viewer"), DEFINITION + b"\x00"]
        for definition in variants:
            self.expect(5, 1, definition=definition)
        for position in [0, 31]:
            digest = bytearray(hashlib.sha256(DEFINITION).digest()); digest[position] ^= 1
            self.expect(5, 1, binary=self.linked(digest=bytes(digest)))

    def test_strict_syntax_and_tuple_with_new_compiled_pins(self):
        variants = [b"", DEFINITION + b"x", DEFINITION.replace(b'"schema_version":1', b'"schema_version":1,"schema_version":1'),
                    DEFINITION.replace(b'"id":', b'"unknown":'), DEFINITION.replace(b"Viewer", b"View\x1ber"),
                    DEFINITION.replace(b"7.9", b"7.8"), DEFINITION.replace(b"amd64", b"arm64"),
                    DEFINITION.replace(MODEL["payload"]["sha256"].encode(), MODEL["payload"]["sha256"].upper().encode())]
        for definition in variants:
            self.expect(1 if not definition else 6, 0 if not definition else 1,
                        definition=definition, binary=self.linked(definition))
        self.expect(7, 1, binary=self.linked(id=b"different-project"))

    def test_payload_full_digest_and_boundary_bytes(self):
        for payload in [PAYLOAD[:-1], PAYLOAD + b"\0", bytes([PAYLOAD[0] ^ 1]) + PAYLOAD[1:], PAYLOAD[:-1] + b"X"]:
            self.expect(8, 2, payload=payload)
        for payload in [b"\0", b"a\0b\xff", bytes(range(256)) * 4096]:
            model = copy.deepcopy(MODEL); model["payload"]["sha256"] = hashlib.sha256(payload).hexdigest()
            definition = encode(model)
            self.expect(definition=definition, payload=payload, binary=self.linked(definition))
        for position in [0, 63]:
            model = copy.deepcopy(MODEL)
            claimed = list(model["payload"]["sha256"]); claimed[position] = "0" if claimed[position] != "0" else "1"
            model["payload"]["sha256"] = "".join(claimed)
            definition = encode(model)
            self.expect(8, 2, definition=definition, binary=self.linked(definition))

    def test_preflight_empty_exact_and_over_caps(self):
        for i in range(3):
            self.expect(1, 0, mode=f"empty-{i}")
        for changes in [{"key": KEY + bytes(1025-len(KEY))}, {"definition": DEFINITION + bytes(16385-len(DEFINITION))},
                        {"payload": bytes(1048577)}]:
            self.expect(1, 0, **changes)
        exact_definition = DEFINITION + b" " * (16384-len(DEFINITION))
        self.expect(definition=exact_definition, binary=self.linked(exact_definition))
        exact_key = b"untrusted comment: " + b"x" * 255 + b"\n" + KEY.splitlines()[1] + b"\n"
        # Valid representation has a tighter grammar than the outer 1KiB cap.
        self.expect(key=exact_key, binary=self.linked(key=exact_key, key_length=len(exact_key)))

    def test_invalid_linked_policy_refuses_before_hash(self):
        variants = [{"profile": 0}, {"profile": 2}, {"key_length": 0}, {"key_length": 1025},
                    {"key": b"not a public record", "key_length": 19}, {"key": KEY[:-1], "key_length": len(KEY)-1},
                    {"id": b""}, {"id": b"A"}, {"id": b"x\0y"}, {"id": b"a"*65},
                    {"release": b"7.8"}, {"release": b"7.9X"}, {"arch": b"arm64"}]
        for variant in variants:
            self.expect(2, 0, binary=self.linked(**variant))

    def test_all_aliases_invalid_ranges_and_no_write_canaries(self):
        modes = ["null-request", "null-output", "overflow-request", "overflow-output", "request-output",
                 "policy-request", "policy-output", "input-01", "input-02", "input-12"]
        modes += [f"{kind}-{i}" for kind in ["null", "overflow", "request", "output", "policy"] for i in range(3)]
        for binary in [self.default, self.sanitized]:
            for mode in modes:
                self.assertEqual(self.run_bytes(binary=binary, mode=mode), b"PB_NOWRITE 0 0 1 1\n", mode)

    def test_hash_failure_repeatability_and_host_unavailability(self):
        self.expect(4, 1, mode="hashfail1")
        self.expect(4, 2, mode="hashfail2")
        self.expect(4, 0, binary=self.native)
        repeated = self.run_bytes(mode="repeat").splitlines()
        self.assertEqual(repeated[0], b"PB_RESULT 1 0 2 0")
        self.assertEqual(repeated[2], b"PB_RESULT 0 8 4 1")
        self.assertEqual(repeated[3], b"PB_RESULT 1 0 6 0")
        self.assertEqual(repeated[1], repeated[4])

    def test_independent_hash_vectors_and_padding_boundaries(self):
        vectors = [(b"", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"),
                   (b"abc", "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"),
                   (b"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1")]
        vectors += [(bytes(range(256))[:n], hashlib.sha256(bytes(range(256))[:n]).hexdigest()) for n in [1, 55, 56, 63, 64, 65, 119, 120, 127, 128]]
        for payload, expected in vectors:
            self.assertEqual(self.run_bytes(payload=payload, mode="hash"), expected.encode()+b"\n")

    def test_objects_production_linkage_and_source_budgets(self):
        symbols = self.execute(["nm", "-u", str(self.directory / "policy_binding.o")]).decode()
        names = {line.split()[-1] for line in symbols.splitlines()}
        self.assertLessEqual(names, {"_bw_compiled_policy", "_bw_parse", "_bw_validate", "_bw_signify_public_key",
                                     "_bw_policy_sha256", "_memcmp", "_bcmp", "_memcpy", "_memset", "_bzero",
                                     "___stack_chk_fail", "___stack_chk_guard"})
        native_symbols = self.execute(["nm", "-u", str(self.directory / "policy_platform.o")])
        self.assertEqual(native_symbols, b"")
        makefile = (ROOT / "Makefile").read_text()
        self.assertEqual(next(line for line in makefile.splitlines() if line.startswith("PLAN_SOURCES =")),
                         "PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c")
        sources = [ROOT / "src" / (x + suffix) for x, suffix in [("policy_binding", ".c"), ("policy_binding", ".h"),
                                                               ("policy_data", ".c"), ("policy_platform", ".c")]]
        self.assertLessEqual(sum(p.stat().st_size for p in sources), 8*1024)

    def test_fixture_live_output_and_timeout_retention_controls(self):
        controls = ["import os; os.write(1,b'x'*32768)", "import os; os.write(2,b'x'*32768)",
                    "import time; time.sleep(30)", "import os; os.write(1,b'partial'); os._exit(7)"]
        for index, code in enumerate(controls):
            with self.assertRaises(AssertionError):
                self.execute([sys.executable, "-I", "-c", code], expected_failure=True)
            record = json.loads(self.ledger.read_text().splitlines()[-1])
            self.assertTrue(record["wait_consumed"])
            self.assertFalse(record["wait_lost"])
            self.assertFalse(record["succeeded"])
            self.assertLess(record["elapsed"], 10)
            self.assertTrue(all(p["close"] == "closed" for p in record["pipes"].values()))
            self.assertLessEqual(record["stdout_bytes_retained"] + record["stderr_bytes_retained"], 32768)
            case = self.directory / f"case-{self.calls:03d}"
            for stream in ["stdout", "stderr"]:
                self.assertEqual(record["stream_files"][stream]["status"], "written")
                self.assertEqual(hashlib.sha256((case / stream).read_bytes()).hexdigest(), record[stream + "_sha256"])
            if index < 2:
                stream = ["stdout", "stderr"][index]
                self.assertEqual(record["overflow"], [stream])
                self.assertEqual(record[stream + "_bytes_retained"], 16384)
                self.assertEqual(record[stream + "_bytes_observed"], 16385)
            elif index == 2:
                self.assertTrue(record["timeout"])
                self.assertEqual(record["returncode"], -signal.SIGKILL)
            else:
                self.assertEqual(record["returncode"], 7)
                self.assertEqual(record["stdout_bytes_retained"], 7)
        original_write = Path.write_bytes

        def refuse_stdout(path, data):
            if path.name == "stdout" and path.parent.parent == self.directory:
                raise OSError("injected owned stdout publication failure")
            return original_write(path, data)

        with mock.patch.object(Path, "write_bytes", refuse_stdout):
            with self.assertRaises(AssertionError):
                self.execute([sys.executable, "-I", "-c", "print('publication')"], expected_failure=True)
        record = json.loads(self.ledger.read_text().splitlines()[-1])
        self.assertTrue(record["wait_consumed"])
        self.assertFalse(record["succeeded"])
        self.assertEqual(record["returncode"], 0)
        self.assertEqual(record["stream_files"]["stdout"]["status"], "unconfirmed")
        self.assertEqual(record["stream_files"]["stderr"]["status"], "written")
        self.assertEqual(record["stdout_sha256"], hashlib.sha256(b"publication\n").hexdigest())
