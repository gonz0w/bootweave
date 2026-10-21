# SPDX-License-Identifier: BSD-3-Clause
"""Dormant SOURCE ONLY: fixed paused-QEMU custody, not execution admission.

No CLI, import effects, target/image creator, QMP sender, serial-command or secret
API. A separately reviewed coordinator must establish fresh exact authority,
create Campaign BEFORE any preparation effect, seal the role plan, inventory the
paused guest, and own all manual console/transport/closure proof. It must enforce
blocking-system-call and whole-campaign limits independently. See MISSING below.
"""
import hashlib
import json
import math
import os
from pathlib import Path
import re
import selectors
import signal
import stat
import subprocess
import sys
import threading
import time

STATUS = "SOURCE_ONLY_NOT_EXECUTION_READY_OR_AUTHORIZED"
CANONICAL = "92166f07fd79c9d356ef3e1ed704eaeeed8653e4"
FIXTURE = "bootweave-qualification-92166f07-runtime-a1"
CAPTURE = 10 * 1024 * 1024
RSS = 2 * 1024 * 1024 * 1024
PINS = {
    "qemu": (26006400, "25afcb2986e4a0f2c567de17273f55de3c99792b78a07c99e923bb62024c088f"),
    "bios": (262144, "ae6f6aa973aaccc143f57aa960fb035fd9de4daee4ad0cd713322f8c259e7650"),
    "vga": (39424, "e8fc9e55790dbe3cb31f019a3deb57206ba6c54f5e581adb2ab2677a9d391472"),
    "installer": (798625792, "7a4a92e953618035097c796a90b54424a0f3ae775552e1e7d102cf8a5130449f"),
}
MISSING = (
    "Fresh exact target/effects authority and independently sealed execution closure",
    "Coordinator source enforcing pre-target original10800s and phase scheduling",
    "Independent paused device/block/PCI/handle/listener/process inventory before cont",
    "Manual install, credential/private-input custody and later installed-state seals",
    "Reviewed QMP/serial transport recording EVERY byte through the shared raw budget",
    "Coordinator enforcing no new guest work after2700s; this module sends no commands",
    "Guest compiler bootstrap/supervisor/build slots and all independent native oracles",
    "Normal guest halt, stopped full disk readback and independent final closure proof",
)
_retained = []  # Never let Popen.__del__ poll a lost/unconsumed child.
_campaign_created = False


class Refusal(RuntimeError):
    pass


def _keys(value, names):
    if type(value) is not dict or set(value) != set(names):
        raise Refusal("Unexpected closed-schema fields")


def _path(value):
    if type(value) is not str or not re.fullmatch(r"/[A-Za-z0-9_./-]+", value):
        raise Refusal("Unsafe absolute role path")
    if "//" in value or any(part in (".", "..") for part in value.split("/")[1:]):
        raise Refusal("Noncanonical role path")
    return Path(value)


def _pairs(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise Refusal("Duplicate plan key")
        result[key] = value
    return result


def validate_plan(raw, expected_sha256):
    """Pure structural check; a coordinator-provided hash is not native approval."""
    if type(raw) is not bytes or len(raw) > 65536 or type(expected_sha256) is not str or not re.fullmatch(r"[0-9a-f]{64}", expected_sha256):
        raise Refusal("Invalid sealed plan")
    if hashlib.sha256(raw).hexdigest() != expected_sha256:
        raise Refusal("Plan hash mismatch")
    plan = json.loads(raw, object_pairs_hook=_pairs,
                      parse_constant=lambda _: (_ for _ in ()).throw(Refusal("Nonfinite JSON")))
    _keys(plan, ("schema", "canonical", "phase", "workspace", "fixture", "files"))
    if (plan["schema"], plan["canonical"]) != ("bootweave-host-role-plan-v1", CANONICAL):
        raise Refusal("Wrong role plan")
    if plan["phase"] not in ("install", "installed"):
        raise Refusal("Only the two fixed first-stage phases exist")
    for role in ("workspace", "fixture"):
        _keys(plan[role], ("path", "device", "inode", "uid", "gid", "mode"))
        _path(plan[role]["path"])
        if any(type(plan[role][key]) is not int or plan[role][key] < 0
               for key in ("device", "inode", "uid", "gid", "mode")):
            raise Refusal("Invalid directory identity")
    fixture = _path(plan["workspace"]["path"]) / FIXTURE
    if _path(plan["fixture"]["path"]) != fixture or plan["fixture"]["mode"] != 0o700:
        raise Refusal("Only the fixed new fixture namespace is allowed")
    _keys(plan["files"], ("qemu", "bios", "vga", "installer", "disk", "data", "ps"))
    fields = ("path", "bytes", "sha256", "device", "inode", "uid", "gid", "mode", "nlink", "mtime_ns", "ctime_ns")
    for role, row in plan["files"].items():
        _keys(row, fields)
        path = _path(row["path"])
        if any(type(row[key]) is not int or row[key] < 0 for key in fields if key not in ("path", "sha256")):
            raise Refusal("Invalid file identity")
        if type(row["sha256"]) is not str or not re.fullmatch(r"[0-9a-f]{64}", row["sha256"]) or row["nlink"] != 1:
            raise Refusal("Invalid file binding")
        if row["mode"] & 0o6022:
            raise Refusal("Elevated or group/other-writable file refused")
        if role in PINS and (row["bytes"], row["sha256"]) != PINS[role]:
            raise Refusal("Tool or stock bytes differ from reviewed pins")
        if role in ("disk", "data"):
            if path != fixture / {"disk": "build.raw", "data": "data.iso"}[role] or row["mode"] != 0o600 or row["uid"] != plan["fixture"]["uid"]:
                raise Refusal("Wrong fixed fixture file")
        if role == "disk" and row["bytes"] != 8589934592:
            raise Refusal("Wrong raw disk size")
        if role == "data" and not 0 < row["bytes"] <= 1073741824:
            raise Refusal("Wrong source-media size")
        if role == "ps" and path != Path("/bin/ps"):
            raise Refusal("Only the fixed base RSS reader exists")
    if plan["phase"] == "install" and plan["files"]["disk"]["sha256"] != "ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25":
        raise Refusal("First disk must have the reviewed complete-zero digest")
    qmp, serial = endpoints(plan)
    if max(len(str(qmp).encode()), len(str(serial).encode())) > 103:
        raise Refusal("Private Unix socket path exceeds Darwin capacity")
    return plan


def endpoints(plan):
    number = 1 if plan["phase"] == "install" else 2
    root = Path(plan["fixture"]["path"])
    return root / ("qmp%d.sock" % number), root / ("ser%d.sock" % number)


def arguments(plan):
    """No argv/machine/resource/environment fields are accepted from the plan."""
    p = {name: row["path"] for name, row in plan["files"].items()}
    qmp, serial = endpoints(plan)
    argv = [p["qemu"], "-S", "-no-user-config", "-nodefaults", "-machine", "pc-i440fx-11.1",
            "-accel", "tcg,thread=single,tb-size=128", "-cpu", "qemu64-v1", "-smp", "1", "-m", "1024",
            "-no-reboot", "-display", "none", "-vga", "none", "-monitor", "none", "-parallel", "none",
            "-nic", "none", "-net", "none", "-bios", p["bios"], "-device",
            "VGA,id=video,bus=pci.0,addr=0x2,romfile=" + p["vga"], "-device",
            "ich9-ahci,id=ahci,bus=pci.0,addr=0x4", "-drive",
            "file=" + p["disk"] + ",if=none,id=target,format=raw,readonly=off", "-device",
            "virtio-blk-pci,id=qualification-disk,drive=target,bus=pci.0,addr=0x3", "-drive",
            "file=" + p["data"] + ",if=none,id=data,format=raw,media=cdrom,readonly=on", "-device",
            "ide-cd,id=data-cd,drive=data,bus=ahci.1,unit=0", "-qmp",
            "unix:" + str(qmp) + ",server=on,wait=off", "-chardev",
            "socket,id=console,path=" + str(serial) + ",server=on,wait=off", "-device",
            "isa-serial,chardev=console,index=0"]
    if plan["phase"] == "install":
        argv += ["-drive", "file=" + p["installer"] + ",if=none,id=installer,format=raw,media=cdrom,readonly=on",
                 "-device", "ide-cd,id=installer-cd,drive=installer,bus=ahci.0,unit=0", "-boot",
                 "once=d,order=c,menu=off,strict=on"]
    else:
        argv += ["-boot", "order=c,menu=off,strict=on"]
    return argv


def _now():
    value = time.monotonic()
    if not math.isfinite(value) or value < 0:
        raise Refusal("Invalid original clock")
    return value


def _identity(seen):
    return {"device": seen.st_dev, "inode": seen.st_ino, "uid": seen.st_uid, "gid": seen.st_gid,
            "mode": stat.S_IMODE(seen.st_mode), "nlink": seen.st_nlink, "bytes": seen.st_size,
            "mtime_ns": seen.st_mtime_ns, "ctime_ns": seen.st_ctime_ns}


def _directory(row):
    path = _path(row["path"])
    for ancestor in reversed((path, *path.parents)):
        if not stat.S_ISDIR(ancestor.lstat().st_mode):
            raise Refusal("Symlink or non-directory ancestor")
    seen = path.lstat()
    if any(_identity(seen)[key] != row[key] for key in row if key != "path"):
        raise Refusal("Directory binding changed")
    if seen.st_uid != os.getuid():
        raise Refusal("Directory is not owned")


def _verify(row, deadline):
    path = _path(row["path"])
    for parent in path.parents:
        if not stat.S_ISDIR(parent.lstat().st_mode):
            raise Refusal("File ancestor changed")
    before = path.lstat()
    if not stat.S_ISREG(before.st_mode):
        raise Refusal("Not an ordinary bound file")
    fd = os.open(path, os.O_RDONLY | os.O_NOFOLLOW | os.O_CLOEXEC)
    try:
        opened = os.fstat(fd)
        expected = {key: row[key] for key in _identity(opened)}
        if _identity(before) != expected or _identity(opened) != expected:
            raise Refusal("File identity differs from seal")
        digest, count = hashlib.sha256(), 0
        while True:
            if _now() >= deadline:
                raise Refusal("Binding read exceeded original time")
            block = os.read(fd, min(4 * 1024 * 1024, row["bytes"] - count + 1))
            if not block:
                break
            count += len(block)
            if count > row["bytes"]:
                raise Refusal("Bounded file grew")
            digest.update(block)
        if count != row["bytes"] or digest.hexdigest() != row["sha256"] or _identity(os.fstat(fd)) != expected or _identity(path.lstat()) != expected:
            raise Refusal("Full stable binding read failed")
    finally:
        os.close(fd)  # One attempt; a close error refuses, never retries.


class _Raw:
    def __init__(self, phase):
        self.phase = phase
        self.used, self.errors, self.fds = 0, [], {}
        self.failed, self.closed = False, False
        self.digests, self.lengths = {}, {}
        self.lock = threading.Lock()

    def open(self, directory):
        for name in ("stdout", "stderr", "qmp", "serial", "rss"):
            self.fds[name] = os.open(directory / (name + ".raw"), os.O_WRONLY | os.O_CREAT | os.O_EXCL | os.O_NOFOLLOW | os.O_CLOEXEC, 0o600)
            self.digests[name], self.lengths[name] = hashlib.sha256(), 0

    def record(self, channel, data):
        """Future reviewed transport must route every QMP/serial raw byte here."""
        with self.lock:
            if self.failed or self.closed or channel not in self.fds or type(data) is not bytes or self.used + len(data) > CAPTURE:
                self.failed = True
                raise Refusal("Raw capture incomplete or quota exhausted")
            self.used += len(data)
            try:
                written = os.write(self.fds[channel], data)
                self.digests[channel].update(data[:written])
                self.lengths[channel] += written
                if written != len(data):
                    raise Refusal("Partial raw capture; no retry")
            except BaseException:
                self.failed = True
                raise

    def close(self):
        with self.lock:
            self.closed = True
            for name, fd in list(self.fds.items()):
                self.fds.pop(name)
                try:
                    os.close(fd)
                except BaseException as error:
                    self.errors.append((name, str(error)))


class _Held:
    def __init__(self):
        self.proc, self.pid, self.consumed, self.lost = None, None, False, False
        self.sealed, self.finishing, self.exited = False, False, False
        self.errors, self.stops, self.data = [], [], {"stdout": bytearray(), "stderr": bytearray()}
        self.selector, self.streams = None, {}
        self.eof = {"stdout": False, "stderr": False}
        self.wait_pid, self.wait_status, self.wait_rusage = None, None, None

    def start(self, argv, env, cwd, deadline):
        if signal.getsignal(signal.SIGCHLD) != signal.SIG_DFL or not hasattr(os, "WNOWAIT") or not hasattr(os, "waitid"):
            raise Refusal("Default non-auto-reaping SIGCHLD/WNOWAIT required")
        # Reinstall the SAME default disposition before birth: getsignal alone
        # cannot exclude a native SA_NOCLDWAIT flag. Main coordinator thread and
        # no competing signal/child-reaper changes are mandatory preconditions.
        signal.signal(signal.SIGCHLD, signal.SIG_DFL)
        if _now() >= deadline:
            raise Refusal("No child birth after its original admission deadline")
        self.proc = subprocess.Popen(argv, cwd=cwd, env=env, stdin=subprocess.DEVNULL,
                                     stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                     close_fds=True, start_new_session=True)
        self.streams = {"stdout": self.proc.stdout, "stderr": self.proc.stderr}
        _retained.append(self.proc)
        if type(self.proc.pid) is not int or self.proc.pid <= 0:
            raise Refusal("No usable original born PID")
        self.pid = self.proc.pid
        self.selector = selectors.DefaultSelector()
        for name, stream in self.streams.items():
            os.set_blocking(stream.fileno(), False)
            self.selector.register(stream, selectors.EVENT_READ, name)

    def observe(self):
        if self.pid is None or self.consumed or self.lost or self.sealed:
            return self.exited
        try:
            seen = os.waitid(os.P_PID, self.pid, os.WEXITED | os.WNOHANG | os.WNOWAIT)
            if seen is not None:
                if seen.si_pid != self.pid:
                    raise Refusal("Wrong wait identity")
                self.exited = True
        except InterruptedError:
            pass
        except BaseException as error:
            self.lost = True
            self.errors.append(("observe-lost", str(error)))
        return self.exited

    def pump(self, sink):
        if self.selector is None:
            return
        for key, _ in self.selector.select(0):
            try:
                data = os.read(key.fileobj.fileno(), 65536)
            except (BlockingIOError, InterruptedError):
                continue
            if not data:
                self.eof[key.data] = True
                self.selector.unregister(key.fileobj)
            else:
                sink(key.data, data)

    def finish(self, deadline, sink):
        if self.finishing:
            return
        self.finishing = True
        try:
            self._finish_child(deadline, sink)
        finally:
            for name, stream in self.streams.items():
                try:
                    stream.close()
                except BaseException as error:
                    self.errors.append(("stream-close", name, str(error)))
            if self.selector is not None:
                try:
                    if self.selector.get_map():
                        self.errors.append(("stream-incomplete",))
                except BaseException as error:
                    self.errors.append(("selector-state", str(error)))
                try:
                    self.selector.close()
                except BaseException as error:
                    self.errors.append(("selector-close", str(error)))

    def _finish_child(self, deadline, sink):
        if self.pid is None:
            return  # Pipe handles still close; no PID signal authority invented.
        self.observe()
        try:
            live_time = _now() < deadline
        except BaseException as error:
            self.errors.append(("final-clock", str(error)))
            live_time = False
        if not live_time:
            self.errors.append(("no-time-for-stop",))
        if live_time and not self.lost and not self.consumed:
            try:
                os.kill(self.pid, signal.SIGKILL)  # Last ORIGINAL direct-PID stop.
                self.stops.append("KILL")
            except BaseException as error:
                self.errors.append(("last-stop", str(error), self.exited))
        self.sealed = True  # Irrevocable, BEFORE the first consuming-wait attempt.
        while not self.lost and not self.consumed:
            try:
                if _now() >= deadline:
                    break
            except BaseException as error:
                self.errors.append(("wait-clock", str(error)))
                break
            try:
                pid, status, usage = os.wait4(self.pid, os.WNOHANG)
                if pid:
                    self.consumed = True  # Latch BEFORE identity/status validation.
                    self.wait_pid, self.wait_status, self.wait_rusage = pid, status, usage
                    if pid != self.pid:
                        self.lost = True
                        self.errors.append(("consumed-wrong-wait-identity", pid))
                        break
                    try:
                        self.proc.returncode = os.waitstatus_to_exitcode(status)
                    except BaseException as error:
                        self.errors.append(("consumed-status-decode", str(error)))
                    break
            except InterruptedError:
                continue
            except BaseException as error:
                self.lost = True
                self.errors.append(("wait-lost", str(error)))
                break
            try:
                self.pump(sink)
            except BaseException as error:
                self.errors.append(("final-pump", str(error)))
            try:
                time.sleep(0.01)
            except BaseException as error:
                self.errors.append(("wait-pause", str(error)))
                break
        while self.selector is not None and self.selector.get_map():
            try:
                if _now() >= deadline:
                    self.errors.append(("no-time-for-stream-finalization",))
                    break
                self.pump(sink)
                time.sleep(0.01)
            except BaseException as error:
                self.errors.append(("last-pump", str(error)))
                break


def _rss(pid, raw, deadline, env, cwd, observations):
    query = _Held()
    origin = _now()
    end = min(deadline, origin + 2)
    def retain(name, data):
        if len(query.data[name]) + len(data) > 4096:
            raise Refusal("Bounded RSS reader overflow")
        raw.record("rss", data)
        query.data[name].extend(data)
    try:
        query.start(["/bin/ps", "-p", str(pid), "-o", "rss="], env, cwd, end)
        while _now() < end - 0.5 and not query.observe() and not query.lost:
            query.pump(retain)
            time.sleep(0.01)
    finally:
        query.finish(end, retain)
        try:
            if _now() >= end:
                query.errors.append(("query-deadline",))
        except BaseException as error:
            query.errors.append(("query-final-clock", str(error)))
        observations.append({"pid": query.pid, "subject_pid": pid, "origin": origin,
            "deadline": end, "consumed": query.consumed, "lost": query.lost,
            "wait_pid": query.wait_pid, "wait_status": query.wait_status,
            "wait_rusage": query.wait_rusage,
            "returncode": None if query.proc is None else query.proc.returncode,
            "stops": query.stops, "errors": list(query.errors), "eof": dict(query.eof),
            "streams": {name: {"bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()}
                        for name, data in query.data.items()}})
    if not query.consumed or query.lost or query.errors or query.proc.returncode or query.data["stderr"]:
        raise Refusal("RSS observation uncertain")
    text = bytes(query.data["stdout"])
    if not re.fullmatch(rb"\s*[0-9]+\s*", text):
        raise Refusal("RSS observation malformed")
    return int(text) * 1024


class Campaign:
    """Create once BEFORE any external target/media effect. No caller clock/reset."""
    def __init__(self):
        global _campaign_created
        if _campaign_created:
            raise Refusal("Campaign origin is once-only in this coordinator process")
        _campaign_created = True
        self.origin, self.attempts, self.previous = _now(), [], None
        self.raw = None
        self.anchor = None

    def record_transport(self, phase, channel, data):
        raw = self.raw
        if channel not in ("qmp", "serial") or raw is None or raw.phase != phase:
            raise Refusal("No active fixed transport channel")
        raw.record(channel, data)

    def launch(self, sealed_plan, expected_sha256):
        """Dormant effect body; caller must supply missing reviewed orchestration.

        Does not grant execution admission. No QMP cont/quit or serial bytes sent.
        A returned receipt is partial custody evidence, never campaign acceptance.
        """
        if sys.platform != "darwin":
            raise Refusal("Connected Mac only")
        host = os.uname()
        if (host.sysname, host.release, host.machine) != ("Darwin", "27.0.0", "arm64") or os.getuid() == 0 or os.getuid() != os.geteuid() or os.getgid() != os.getegid():
            raise Refusal("Reviewed unprivileged Mac identity required")
        origin = _now()
        plan = validate_plan(sealed_plan, expected_sha256)
        phase = plan["phase"]
        if self.attempts != ([] if phase == "install" else ["install"]):
            raise Refusal("No repeat, reordered or third launch")
        if phase == "installed" and (self.previous is None or self.previous["errors"] or not self.previous["consumed"] or self.previous["returncode"] != 0):
            raise Refusal("Prior stopped launch is not eligible even for fresh sealing")
        anchor = {"workspace": plan["workspace"], "fixture": plan["fixture"],
                  "files": {role: {key: value for key, value in row.items()
                    if role != "disk" or key not in ("sha256", "mtime_ns", "ctime_ns")}
                    for role, row in plan["files"].items()}}
        if self.anchor is not None and self.anchor != anchor:
            raise Refusal("No replacement target, tools or media between phases")
        self.anchor = anchor
        self.attempts.append(phase)  # Failed preparations consume the phase.
        end = min(origin + 3600, self.origin + 10800)
        work = min(origin + 2700, end - 900)
        child, errors, peaks, queries, work_closed = _Held(), [], [], [], False
        root = Path(plan["fixture"]["path"])
        env = {"LC_ALL": "C", "LANG": "C", "TZ": "UTC", "PATH": "",
               "HOME": str(root / "home"), "TMPDIR": str(root / "tmp")}
        try:
            if end - origin < 900 or origin < self.origin:
                raise Refusal("Original campaign time exhausted or moved backward")
            _directory(plan["workspace"])
            _directory(plan["fixture"])
            for directory in (root / "home", root / "tmp"):
                seen = directory.lstat()
                if not stat.S_ISDIR(seen.st_mode) or seen.st_uid != os.getuid() or stat.S_IMODE(seen.st_mode) != 0o700:
                    raise Refusal("Wrong private environment directory")
            for row in plan["files"].values():
                _verify(row, work)
            for endpoint in endpoints(plan):
                if os.path.lexists(endpoint):
                    raise Refusal("Fresh endpoint already exists; never unlink")
            directory = root / phase
            os.mkdir(directory, 0o700)  # Exclusive phase/capture latch, never reused.
            self.raw = _Raw(phase)
            self.raw.open(directory)
            if _now() >= work:
                raise Refusal("No launch after work admission closes")
            child.start(arguments(plan), env, root, work)
            next_rss = _now()
            while _now() < end - 5 and not child.observe() and not child.lost:
                child.pump(self.raw.record)
                if self.raw.failed:
                    raise Refusal("Independent transport capture failed")
                now = _now()
                if now < origin:
                    raise Refusal("Original launch clock moved backward")
                work_closed |= now >= work
                if now >= next_rss:
                    if len(queries) >= 3600:
                        raise Refusal("Finite RSS query count exhausted")
                    sampled = _rss(child.pid, self.raw, end - 5, env, root, queries)
                    peaks.append(sampled)
                    if sampled > RSS:
                        raise Refusal("Sampled emulator RSS ceiling exceeded")
                    next_rss = now + 1
                time.sleep(0.01)
        except BaseException as error:
            errors.append(str(error))
        finally:
            try:
                child.finish(end, self.raw.record if self.raw is not None else lambda *_: None)
            except BaseException as error:
                errors.append("finalization uncertain: " + str(error))
            finally:
                if self.raw is not None:
                    try:
                        self.raw.close()
                    except BaseException as error:
                        errors.append("raw close uncertain: " + str(error))
                    errors.extend(self.raw.errors)
                    if self.raw.failed:
                        errors.append("raw capture failed; retained prefix is incomplete")
        try:
            finished = _now()
            if finished < origin or finished >= end:
                errors.append("original total deadline/clock failed during finalization")
        except BaseException as error:
            finished = None
            errors.append("final clock uncertain: " + str(error))
        result = {"status": STATUS, "phase": phase, "source": CANONICAL,
                  "plan_sha256": expected_sha256, "pid": child.pid,
                  "consumed": child.consumed, "lost": child.lost,
                  "wait_pid": child.wait_pid, "wait_status": child.wait_status,
                  "wait_rusage": child.wait_rusage,
                  "returncode": None if child.proc is None else child.proc.returncode,
                  "stops": child.stops, "errors": errors + child.errors, "eof": child.eof,
                  "origin": origin, "campaign_origin": self.origin, "deadline": end,
                  "elapsed": None if finished is None else finished - origin, "work_closed": work_closed,
                  "sampled_peak_rss": max(peaks, default=None),
                  "rss_queries": queries,
                  "raw_quota_charged": 0 if self.raw is None else self.raw.used,
                  "raw_known_writes": {} if self.raw is None else {
                      name: {"bytes": self.raw.lengths[name], "sha256": digest.hexdigest()}
                      for name, digest in self.raw.digests.items()},
                  "execution_ready": False, "missing": MISSING}
        self.raw, self.previous = None, result
        return result
