# SPDX-License-Identifier: BSD-3-Clause
"""Dormant SOURCE ONLY: fixed paused-QEMU custody, not execution admission.

No CLI, import effects, target/image creator, QMP sender, serial-command or secret
API. A separately reviewed coordinator must establish fresh exact authority,
create Campaign BEFORE any preparation effect, seal the role plan, inventory the
paused guest, and own all manual console/transport/closure proof. It must enforce
blocking-system-call and whole-campaign limits independently. See MISSING below.
"""
import hashlib
import errno
import json
import math
import os
from pathlib import Path
import re
import selectors
import signal
import socket
import stat
import subprocess
import sys
import threading
import time
from qualification.coordinator import Coordinator
from qualification.transport import Transport, CHANNELS, RAW_CAP, Refusal as TransportRefusal
from qualification.accounting import Accounting, RESERVATIONS

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
    "External supervisor bounding blocked coordinator operations before/after child birth",
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


def _verify(row, deadline, lifetime):
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
            if lifetime.cancelled or lifetime.now() >= deadline:
                raise Refusal("Binding read exceeded shared lifetime")
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
        self.failed, self.closed, self._write_sealed = False, False, False
        self.digests, self.lengths = {}, {}
        self.lock = threading.Lock()
        self.ledger = Transport(phase)
        self.allocation = {}
        self.identities = {}
        self.accounting = Accounting()
        self.allocation_role = "host." + phase + "-raw"
        self.ceiling = next(row.ceiling for row in RESERVATIONS if row.name == self.allocation_role)
        if self.ceiling != RAW_CAP:
            raise Refusal("Capture/allocation reservation mismatch")

    def open(self, directory):
        for name in ("stdout", "stderr", "qmp", "serial", "rss"):
            self.fds[name] = os.open(directory / (name + ".raw"), os.O_WRONLY | os.O_CREAT | os.O_EXCL | os.O_NOFOLLOW | os.O_CLOEXEC, 0o600)
            self.digests[name], self.lengths[name] = hashlib.sha256(), 0
            self.allocation[name] = 0
            seen = os.fstat(self.fds[name])
            if not stat.S_ISREG(seen.st_mode) or seen.st_size != 0 or seen.st_nlink != 1 or stat.S_IMODE(seen.st_mode) != 0o600 or seen.st_uid != os.getuid():
                self.failed = True
                raise Refusal("New exclusive capture identity refused")
            self.identities[name] = (seen.st_dev, seen.st_ino, seen.st_uid, seen.st_gid)

    def record(self, channel, data):
        """One raw-byte charge and one owned write; never retry a partial write.

        Enforces logical capture bytes. Actual allocation samples can stop future
        writes but cannot prove temporary peaks, filesystem metadata or global
        quotas. That distinct native allocation gate stays closed.
        """
        with self.lock:
            if self._write_sealed or self.closed or channel not in self.fds or type(data) is not bytes:
                self.failed = True
                raise Refusal("Raw capture incomplete or quota exhausted")
            try:
                ticket = self.ledger.raw_read(channel, self.lengths[channel], data)
                self.used = self.ledger.used
                try:
                    written = os.write(self.fds[channel], data)
                except OSError as error:
                    self.ledger.publish_result(ticket, -1, error.errno or 0)
                    raise
                if type(written) is not int or not 0 <= written <= len(data):
                    self.ledger.publish_result(ticket, -1, 0)
                    raise Refusal("Impossible write result")
                self.digests[channel].update(data[:written])
                self.lengths[channel] += written
                frames = self.ledger.publish_result(ticket, written, 0)
                seen = os.fstat(self.fds[channel])
                if not stat.S_ISREG(seen.st_mode) or seen.st_size != self.lengths[channel] or seen.st_nlink != 1 or stat.S_IMODE(seen.st_mode) != 0o600 or (seen.st_dev, seen.st_ino, seen.st_uid, seen.st_gid) != self.identities[channel]:
                    raise Refusal("Capture identity/length changed")
                self.allocation[channel] = max(self.allocation[channel], seen.st_size, seen.st_blocks * 512)
                if sum(self.allocation.values()) > self.ceiling:
                    raise Refusal("Observed capture allocation exceeds reservation; retain")
                return frames
            except TransportRefusal as error:
                self.used = self.ledger.used
                self.failed = True
                self._write_sealed |= str(error) != "JSON_FRAMING"
                raise
            except BaseException:
                self.used = self.ledger.used
                self.failed = self._write_sealed = True
                raise

    @property
    def readable(self):
        return not self.closed and not self._write_sealed

    def eof(self, channel):
        with self.lock:
            try:
                self.ledger.eof(channel, self.lengths[channel])
            except BaseException:
                self.failed = True
                raise

    def close(self):
        with self.lock:
            self.closed = True
            if self.fds:
                try:
                    for name, fd in self.fds.items():
                        seen = os.fstat(fd)
                        if not stat.S_ISREG(seen.st_mode) or seen.st_nlink != 1 or seen.st_size != self.lengths[name] or stat.S_IMODE(seen.st_mode) != 0o600 or (seen.st_dev, seen.st_ino, seen.st_uid, seen.st_gid) != self.identities[name]:
                            raise Refusal("Capture final identity/allocation uncertain")
                        self.allocation[name] = max(self.allocation[name], seen.st_size, seen.st_blocks * 512)
                    manifest = json.dumps({name: {"identity": self.identities[name],
                        "bytes": self.lengths[name], "sha256": self.digests[name].hexdigest(),
                        "allocated_charge": self.allocation[name]} for name in self.fds}, sort_keys=True).encode()
                    self.accounting.observe(self.allocation_role, sum(self.lengths.values()),
                        sum(self.allocation.values()), hashlib.sha256(manifest).hexdigest())
                except BaseException as error:
                    self.failed = True
                    self.errors.append(("final-allocation", str(error)))
            for name, fd in list(self.fds.items()):
                self.fds.pop(name)
                try:
                    os.close(fd)
                    self.ledger.close_result(name, 0, 0)
                except OSError as error:
                    try:
                        self.ledger.close_result(name, -1, error.errno or 0)
                    except BaseException as reported:
                        self.errors.append((name, str(reported)))
                    self.errors.append((name, str(error)))
                except BaseException as error:
                    self.errors.append((name, str(error)))


class _Held:
    def __init__(self, lifetime=None):
        self.proc, self.pid, self.consumed, self.lost = None, None, False, False
        self.sealed, self.finishing, self.exited = False, False, False
        self.errors, self.stops, self.data = [], [], {"stdout": bytearray(), "stderr": bytearray()}
        self.selector, self.streams = None, {}
        self.eof = {"stdout": False, "stderr": False}
        self.wait_pid, self.wait_status, self.wait_rusage = None, None, None
        self.lifetime = lifetime
        self._custody_lock = threading.RLock()
        self._stop_attempted = False

    def now(self):
        return _now() if self.lifetime is None else self.lifetime.now()

    def start(self, argv, env, cwd, deadline):
        if signal.getsignal(signal.SIGCHLD) != signal.SIG_DFL or not hasattr(os, "WNOWAIT") or not hasattr(os, "waitid"):
            raise Refusal("Default non-auto-reaping SIGCHLD/WNOWAIT required")
        # Reinstall the SAME default disposition before birth: getsignal alone
        # cannot exclude a native SA_NOCLDWAIT flag. Main coordinator thread and
        # no competing signal/child-reaper changes are mandatory preconditions.
        signal.signal(signal.SIGCHLD, signal.SIG_DFL)
        if self.now() >= deadline or self.lifetime is not None and self.lifetime.cancelled:
            raise Refusal("No child birth after shared admission closes")
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
        with self._custody_lock:
            return self._observe()

    def _observe(self):
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

    def stop(self):
        """Sole original direct-PID stop; no renewal after error or lost wait.

        The independent child watchdog and finalizer serialize this with the
        consuming wait. No sampled PID, process-group or descendant authority.
        """
        with self._custody_lock:
            if self.pid is None or self.sealed or self.consumed or self.lost or self._stop_attempted:
                return False
            self._stop_attempted = True
            try:
                os.kill(self.pid, signal.SIGKILL)
                self.stops.append("KILL")
            except BaseException as error:
                self.errors.append(("last-stop", str(error), self.exited))
            return True

    def pump(self, sink, eof=None):
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
                if eof is not None:
                    eof(key.data)
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
            live_time = self.now() < deadline
        except BaseException as error:
            self.errors.append(("final-clock", str(error)))
            live_time = False
        if not live_time:
            self.errors.append(("no-time-for-stop",))
        if live_time and not self.lost and not self.consumed:
            self.stop()
        with self._custody_lock:
            self.sealed = True  # Irrevocable BEFORE consuming-wait attempt.
        while not self.lost and not self.consumed:
            try:
                if self.now() >= deadline:
                    break
            except BaseException as error:
                self.errors.append(("wait-clock", str(error)))
                break
            try:
                with self._custody_lock:
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
                if self.now() >= deadline:
                    self.errors.append(("no-time-for-stream-finalization",))
                    break
                self.pump(sink)
                time.sleep(0.01)
            except BaseException as error:
                self.errors.append(("last-pump", str(error)))
                break


class _ChildWatchdog:
    """Independent thread stops ONLY the original held child at work closure.

    This still cannot bound a blocked coordinator filesystem call, thread start,
    or pre-PID Popen. It grants no native execution/descendant signal authority.
    """
    def __init__(self, child, lifetime, window):
        self.child, self.lifetime, self.window = child, lifetime, window
        self.done = threading.Event()
        self.thread, self.errors = None, []

    def start(self):
        if self.thread is not None or self.done.is_set():
            raise Refusal("Watchdog is once-only")
        self.thread = threading.Thread(target=self._run, name="qualification-child-stop", daemon=True)
        self.thread.start()

    def _run(self):
        try:
            while not self.done.is_set():
                now = self.lifetime.now()
                if self.lifetime.cancelled or now >= self.window.work_end:
                    self.child.stop()
                    return
                self.done.wait(min(0.05, self.window.work_end - now))
        except BaseException as error:
            self.errors.append(("watchdog-unknown", str(error)))
            self.lifetime.cancel()
            self.child.stop()

    def close(self):
        self.done.set()
        if self.thread is not None:
            try:
                self.thread.join(max(0, min(1, self.window.end - self.lifetime.now())))
                if self.thread.is_alive():
                    raise Refusal("Watchdog did not join")
            except BaseException as error:
                self.errors.append(("watchdog-join-unknown", str(error)))
                self.lifetime.cancel()


_SOCKET_TYPE = socket.socket


def _socket_close(stream):
    stream.close()  # One library close, never retry the numeric descriptor.


def _close_sockets(sockets):
    closes, errors = {}, []
    for name, stream in list(sockets.items()):
        sockets.pop(name)  # Consume ownership before any close attempt.
        closes[name] = "unknown"
        if type(stream) is not _SOCKET_TYPE:
            errors.append(("not-owned-socket", name))
            continue  # Never invoke an arbitrary object's close callback.
        try:
            _socket_close(stream)
            closes[name] = "closed"
        except BaseException as error:
            errors.append(("socket-close", name, str(error)))
    return closes, errors


class _ReadChannels:
    """Own two nonblocking Unix streams; raw publication precedes framing use.

    Invalid shape/types/aliases refuse BEFORE transfer: caller retains ownership.
    Once validated, constructor failure attempts both closes, including selector
    allocation failure. Read-only: no sender, inventory verdict or resume method.
    Socket closes are distinct from capture-file closes and never imply EOF.
    """
    def __init__(self, sockets, raw):
        if type(sockets) is not dict or set(sockets) != {"qmp", "serial"}:
            raise Refusal("Exact two owned read channels required")
        if any(type(stream) is not _SOCKET_TYPE or stream.family != socket.AF_UNIX or
               stream.type != socket.SOCK_STREAM or stream.fileno() < 0
               for stream in sockets.values()) or len({s.fileno() for s in sockets.values()}) != 2:
            raise Refusal("Distinct ordinary owned Unix stream sockets required; no transfer")
        self.sockets = dict(sockets)
        self.raw, self.selector = raw, None
        self.eof, self.closes, self.errors = set(), {}, []
        try:
            self.selector = selectors.DefaultSelector()
            for name, stream in self.sockets.items():
                stream.setblocking(False)
                self.selector.register(stream, selectors.EVENT_READ, name)
        except BaseException as error:
            self.close()
            refused = Refusal("Channel construction failed: " + str(error))
            refused.socket_closes, refused.close_errors = dict(self.closes), tuple(self.errors)
            raise refused from error

    @classmethod
    def connect(cls, plan, raw, lifetime, window):
        """Dormant exact-endpoint connection; no QEMU executed by this module."""
        sockets = {}
        try:
            for name, path in zip(("qmp", "serial"), endpoints(plan)):
                lifetime.require_work(window)
                before = path.lstat()
                if not stat.S_ISSOCK(before.st_mode) or before.st_uid != os.getuid():
                    raise Refusal("Owned fixed socket endpoint missing")
                lifetime.require_work(window)  # lstat may consume/cancel work.
                stream = sockets[name] = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
                stream.setblocking(False)
                lifetime.require_work(window)  # No connect after delayed birth.
                result = stream.connect_ex(str(path))
                if result not in (0, errno.EINPROGRESS, errno.EWOULDBLOCK, errno.EALREADY):
                    raise Refusal("Fixed endpoint connection failed")
                with selectors.DefaultSelector() as pending:
                    pending.register(stream, selectors.EVENT_WRITE)
                    while not pending.select(0):
                        lifetime.require_work(window)
                        time.sleep(0.01)
                lifetime.require_work(window)
                if stream.getsockopt(socket.SOL_SOCKET, socket.SO_ERROR) or _identity(path.lstat()) != _identity(before):
                    raise Refusal("Socket identity/connect result uncertain")
                lifetime.require_work(window)
            owned, sockets = sockets, {}
            return cls(owned, raw)
        except BaseException as error:
            closes, errors = _close_sockets(sockets)
            if hasattr(error, "socket_closes"):
                closes.update(error.socket_closes)
                errors.extend(error.close_errors)
            refused = Refusal("Socket connection refused: " + str(error))
            refused.socket_closes, refused.close_errors = closes, tuple(errors)
            raise refused from error

    def pump(self):
        if self.selector is None:
            raise Refusal("Read channels already closed")
        frames = []
        for key, _ in self.selector.select(0):
            if not self.raw.readable:
                self.errors.append(("capture-sealed-before-read", key.data))
                self.selector.unregister(key.fileobj)
                continue
            try:
                data = key.fileobj.recv(65536)
            except (BlockingIOError, InterruptedError):
                continue
            except BaseException as error:
                self.errors.append(("socket-read-unknown", key.data, str(error)))
                self.selector.unregister(key.fileobj)
                continue
            try:
                if data:
                    frames.extend(self.raw.record(key.data, data))
                else:
                    self.eof.add(key.data)
                    self.selector.unregister(key.fileobj)
                    self.raw.eof(key.data)
            except BaseException as error:
                self.errors.append(("channel-capture", key.data, str(error)))
                # Framing failure is sticky but permits bounded forensic tails.
                # Publication/quota failure seals reads without inventing EOF.
        return tuple(frames)

    def drain(self, lifetime, window):
        """Bound both channels by original end and finite polls, reserving close.

        Failure of one channel does not skip the other. This cannot interrupt a
        blocking clock/selector/read syscall; the external watchdog gate remains.
        """
        for _ in range(4096):
            try:
                if self.selector is None or not self.selector.get_map():
                    break
                now = lifetime.now()
                if now >= window.end - 1:
                    self.errors.append(("socket-drain-deadline",))
                    break
                self.pump()
                if self.selector.get_map():
                    time.sleep(min(0.01, window.end - 1 - now))
            except BaseException as error:
                self.errors.append(("socket-drain-unknown", str(error)))
                break
        else:
            self.errors.append(("socket-drain-poll-bound",))
        if self.eof != {"qmp", "serial"}:
            self.errors.append(("socket-eof-incomplete", tuple(sorted(self.eof))))

    def require_healthy(self):
        """Negative-only work stop gate; never native admission."""
        if self.raw.failed or self.errors:
            raise Refusal("Transport capture/read failed")

    def close(self):
        closes, errors = _close_sockets(self.sockets)
        self.closes.update(closes)
        self.errors.extend(errors)
        if self.selector is not None:
            owned, self.selector = self.selector, None
            try:
                owned.close()
            except BaseException as error:
                self.errors.append(("socket-selector-close", str(error)))


def _rss(pid, raw, deadline, env, cwd, observations, lifetime):
    query = _Held(lifetime)
    origin = lifetime.now()
    end = min(deadline, origin + 2)
    def retain(name, data):
        if len(query.data[name]) + len(data) > 4096:
            raise Refusal("Bounded RSS reader overflow")
        raw.record("rss", data)
        query.data[name].extend(data)
    try:
        query.start(["/bin/ps", "-p", str(pid), "-o", "rss="], env, cwd, end)
        while lifetime.now() < end - 0.5 and not lifetime.cancelled and not query.observe() and not query.lost:
            query.pump(retain)
            time.sleep(0.01)
    finally:
        query.finish(end, retain)
        try:
            if lifetime.now() >= end:
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
    """Borrow the once-only Coordinator lifetime; never capture another origin."""
    def __init__(self, coordinator):
        global _campaign_created
        if _campaign_created:
            raise Refusal("Campaign origin is once-only in this coordinator process")
        _campaign_created = True
        if type(coordinator) is not Coordinator:
            raise Refusal("Exact shared Coordinator required")
        coordinator.lifetime.claim_host()
        self.coordinator, self.lifetime = coordinator, coordinator.lifetime
        self.origin, self.attempts, self.previous = self.lifetime.origin, [], None
        self.raw = None
        self.anchor = None

    def cancel(self):
        self.coordinator.cancel()

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
        if not self.coordinator.admission("launch").native_admitted:
            raise Refusal("Native admission remains unsupported")
        if sys.platform != "darwin":
            raise Refusal("Connected Mac only")
        host = os.uname()
        if (host.sysname, host.release, host.machine) != ("Darwin", "27.0.0", "arm64") or os.getuid() == 0 or os.getuid() != os.geteuid() or os.getgid() != os.getegid():
            raise Refusal("Reviewed unprivileged Mac identity required")
        plan = validate_plan(sealed_plan, expected_sha256)
        phase = plan["phase"]
        window = self.lifetime.window(phase)
        self.lifetime.require_work(window)
        origin = window.origin
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
        end, work = window.end, window.work_end
        child, errors, peaks, queries, work_closed = _Held(self.lifetime), [], [], [], False
        watchdog = _ChildWatchdog(child, self.lifetime, window)
        channels = None
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
                _verify(row, work, self.lifetime)
            for endpoint in endpoints(plan):
                if os.path.lexists(endpoint):
                    raise Refusal("Fresh endpoint already exists; never unlink")
            directory = root / phase
            os.mkdir(directory, 0o700)  # Exclusive phase/capture latch, never reused.
            self.raw = _Raw(phase)
            self.raw.open(directory)
            if self.lifetime.now() >= work:
                raise Refusal("No launch after work admission closes")
            child.start(arguments(plan), env, root, work)
            watchdog.start()
            channels = _ReadChannels.connect(plan, self.raw, self.lifetime, window)
            next_rss = self.lifetime.now()
            while self.lifetime.now() < end - 5 and not child.observe() and not child.lost:
                child.pump(self.raw.record, self.raw.eof)
                channels.pump()
                channels.require_healthy()
                now = self.lifetime.now()
                if now < origin:
                    raise Refusal("Original launch clock moved backward")
                work_closed |= now >= work or self.lifetime.cancelled
                if work_closed:
                    raise Refusal("Shared work lifetime closed")
                if now >= next_rss:
                    if len(queries) >= 3600:
                        raise Refusal("Finite RSS query count exhausted")
                    sampled = _rss(child.pid, self.raw, end - 5, env, root, queries, self.lifetime)
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
                watchdog.close()
                errors.extend(watchdog.errors)
                if self.raw is not None:
                    if channels is not None:
                        try:
                            channels.drain(self.lifetime, window)
                        except BaseException as error:
                            errors.append("last channel read uncertain: " + str(error))
                        channels.close()
                        errors.extend(channels.errors)
                    for name in ("stdout", "stderr"):
                        state = json.loads(self.raw.ledger.snapshot())["channels"][name]
                        if child.eof[name] and not state["eof"]:
                            try:
                                self.raw.eof(name)
                            except BaseException as error:
                                errors.append("stream EOF uncertain: " + str(error))
                    try:
                        self.raw.eof("rss")  # No later RSS reader can be born.
                    except BaseException as error:
                        errors.append("RSS EOF uncertain: " + str(error))
                    try:
                        self.raw.close()
                    except BaseException as error:
                        errors.append("raw close uncertain: " + str(error))
                    errors.extend(self.raw.errors)
                    if self.raw.failed:
                        errors.append("raw capture failed; retained prefix is incomplete")
        try:
            finished = self.lifetime.now()
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
                  "transport_receipt": None if self.raw is None else json.loads(self.raw.ledger.snapshot()),
                  "socket_eof": [] if channels is None else sorted(channels.eof),
                  "socket_closes": {} if channels is None else dict(channels.closes),
                  "observed_capture_allocations": {} if self.raw is None else dict(self.raw.allocation),
                  "capture_accounting": None if self.raw is None else self.raw.accounting.report(),
                  "raw_known_writes": {} if self.raw is None else {
                      name: {"bytes": self.raw.lengths[name], "sha256": digest.hexdigest()}
                      for name, digest in self.raw.digests.items()},
                  "execution_ready": False, "missing": MISSING}
        self.raw, self.previous = None, result
        return result
