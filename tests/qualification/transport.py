# SPDX-License-Identifier: BSD-3-Clause
"""SOURCE ONLY: bounded raw evidence accounting, never a transport sender.

The exclusive single-threaded caller owns reads, capture files, clocks and one
instance per launch. This module opens/closes/connects nothing. Publication/close
outcomes are data, not independently observed syscalls or descriptor authority.
All five channels share one ten-MiB budget. Never also debit these same bytes in
another transport ledger: a coordinator must integrate a single charging owner.

No installed official QMP/QAPI schema was available. JSON-object framing below
is generic forensic framing, NOT a QMP protocol or inventory validator. Command,
reply, event and ID/order semantics remain unsupported; no record can authorize
resume. Serial bytes stay opaque; there is no command, credential or argv API.
"""
from dataclasses import dataclass
import errno
import hashlib
import json
import math


STATUS = "SOURCE_ONLY_NOT_EXECUTION_READY_OR_AUTHORIZED"
CHANNELS = ("stdout", "stderr", "qmp", "serial", "rss")
RAW_CAP = 10 * 1024 * 1024
CHUNK_CAP = 65536
CHUNK_COUNT_CAP = 16384
LINE_CAP = 16384
RECORD_CAP = 1024
DEPTH_CAP = 32
NODE_CAP = 4096
ERRNO_MAX = 2147483647
QEMU_DOCUMENT = (
    "/opt/homebrew/Cellar/qemu/11.1.1/README.rst", 5579,
    "d9e0384f45d047137f74f38b32dec1f31488fdbe2557820c8934e1fbc8fc2efd",
)
MISSING = (
    "Pinned official QEMU11.1.1 QMP/QAPI greeting/command/reply/event/ID schemas",
    "Exact fixed query schedule, IDs and paused device/block/PCI whitelist",
    "Independent host process/handle/listener inventory and original PID custody",
    "Reviewed coordinator clocks, fresh exact authority and actual resume owner",
)


class Refusal(RuntimeError):
    pass


@dataclass(frozen=True)
class Frame:
    """Exact original JSON-object line, including its LF; no semantic verdict."""
    index: int
    raw: bytes
    sha256: str


@dataclass(frozen=True)
class InventoryReport:
    phase: str
    stage: str
    supported: bool
    valid: bool
    resume_eligible: bool
    native_authority: bool
    receipt_sha256: str
    bindings: tuple
    missing: tuple


class _Channel:
    def __init__(self):
        self.observed = self.published = 0
        self.observed_hash, self.published_hash = hashlib.sha256(), hashlib.sha256()
        self.observed_hash_complete = True
        self.eof = False
        self.close = "not-attempted"
        self.close_errno = 0


def _pairs(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError("Duplicate JSON key")
        result[key] = value
    return result


def _integer(text):
    if len(text) > 20:
        raise ValueError("Integer outside bounded generic profile")
    value = int(text)
    if not -(2 ** 63) <= value < 2 ** 63:
        raise ValueError("Integer outside bounded generic profile")
    return value


def _float(text):
    value = float(text)
    if not math.isfinite(value):
        raise ValueError("Nonfinite JSON number")
    return value


def _constant(_):
    raise ValueError("Nonfinite JSON constant")


def _object_line(raw):
    # Bound nesting before invoking the JSON parser; this changes no host limit.
    depth, quoted, escaped = 0, False, False
    for byte in raw:
        if quoted:
            if escaped:
                escaped = False
            elif byte == 92:
                escaped = True
            elif byte == 34:
                quoted = False
        elif byte == 34:
            quoted = True
        elif byte in (91, 123):
            depth += 1
            if depth > DEPTH_CAP:
                raise ValueError("JSON depth cap")
        elif byte in (93, 125):
            depth -= 1
            if depth < 0:
                raise ValueError("Unbalanced JSON")
    if quoted or escaped or depth:
        raise ValueError("Incomplete JSON object")
    value = json.loads(raw.decode("utf-8", errors="strict"), object_pairs_hook=_pairs,
                       parse_int=_integer, parse_float=_float, parse_constant=_constant)
    if type(value) is not dict:
        raise ValueError("Top-level JSON object required")
    nodes, pending = 0, [value]
    while pending:
        nodes += 1
        if nodes > NODE_CAP:
            raise ValueError("JSON node cap")
        item = pending.pop()
        if type(item) is dict:
            pending.extend(item.values())
        elif type(item) is list:
            pending.extend(item)
    # Preserve raw bytes, never reserialize them as the actual wire evidence.


class Transport:
    """Pure observations for one coordinator-owned install/installed launch.

    raw_read reserves bytes once. The caller then makes ONE capture-file write
    and passes its actual outcome to publish_result. No write or close retry is
    supplied. A parser refusal remains sticky while bounded raw retention of
    later bytes may continue. Quota/publication uncertainty seals further reads;
    EOF and independent close outcomes can still be recorded for finalization.
    """
    def __init__(self, phase):
        if type(phase) is not str or phase not in ("install", "installed"):
            raise Refusal("Fixed launch phase required")
        self.phase = phase
        self.used = self.chunks = 0
        self.failed = self.uncertain = self._sealed = False
        self.cause = None
        self._channels = {name: _Channel() for name in CHANNELS}
        self._pending = None
        self._qmp = bytearray()
        self._qmp_failed = False
        self._frames = []

    def _fail(self, cause, uncertain=False, seal=False):
        self.failed = True
        self.uncertain |= uncertain
        self._sealed |= seal
        if self.cause is None:
            self.cause = cause
        raise Refusal(cause)

    def _channel(self, name):
        if type(name) is not str or name not in self._channels:
            self._fail("UNKNOWN_CHANNEL", seal=True)
        return self._channels[name]

    def raw_read(self, channel, offset, data):
        """Accept observed bytes, not a read instruction or a sending request."""
        state = self._channel(channel)
        if self._sealed or self._pending is not None:
            self._fail("CAPTURE_SEALED_OR_PENDING", uncertain=True, seal=True)
        if state.eof or state.close != "not-attempted":
            self._fail("READ_AFTER_TERMINAL", uncertain=True, seal=True)
        if type(offset) is not int or offset != state.observed or type(data) is not bytes or not data:
            self._fail("READ_OFFSET_OR_TYPE", uncertain=True, seal=True)
        size = len(data)
        self.used += size
        state.observed += size
        self.chunks += 1
        if size > CHUNK_CAP or self.used > RAW_CAP or self.chunks > CHUNK_COUNT_CAP:
            # The first externally supplied overrun is counted, never concealed.
            # It is not fully hashed/retained, so no complete-capture claim exists.
            state.observed_hash_complete = False
            self._fail("RAW_CAP", uncertain=True, seal=True)
        state.observed_hash.update(data)
        self._pending = (self.chunks, channel, data)
        return self.chunks

    def publish_result(self, ticket, count, error):
        """Record one caller-owned raw capture write, never a socket write."""
        pending, self._pending = self._pending, None  # Consume before validation.
        if pending is None or type(ticket) is not int or ticket != pending[0]:
            self._fail("PUBLICATION_TICKET", uncertain=True, seal=True)
        _, channel, data = pending
        state = self._channels[channel]
        if (type(count) is not int or type(error) is not int or
                not 0 <= error <= ERRNO_MAX or not -1 <= count <= len(data)):
            self._fail("PUBLICATION_OUTCOME", uncertain=True, seal=True)
        if count > 0:
            state.published_hash.update(data[:count])
            state.published += count
        if count != len(data) or error:
            self._fail("PUBLICATION_PARTIAL_OR_ERROR", uncertain=True, seal=True)
        if channel != "qmp" or self._qmp_failed:
            return ()
        self._qmp.extend(data)  # At most one64KiB chunk plus one16KiB pending line.
        frames = []
        try:
            while True:
                end = self._qmp.find(b"\n")
                if end < 0:
                    break
                if end + 1 > LINE_CAP or len(self._frames) >= RECORD_CAP:
                    raise ValueError("JSON line/count cap")
                raw = bytes(self._qmp[:end + 1])
                del self._qmp[:end + 1]
                _object_line(raw)
                digest = hashlib.sha256(raw).hexdigest()
                frame = Frame(len(self._frames), raw, digest)
                self._frames.append(digest)
                frames.append(frame)
            if len(self._qmp) > LINE_CAP:
                raise ValueError("JSON line cap")
        except (ValueError, UnicodeError, RecursionError):
            self._qmp_failed = True
            self._fail("JSON_FRAMING")
        return tuple(frames)

    def eof(self, channel, offset):
        state = self._channel(channel)
        if type(offset) is not int or offset != state.observed or state.eof or state.close != "not-attempted":
            self._fail("EOF_ORDER", uncertain=True)
        if self._pending is not None and self._pending[1] == channel:
            self._fail("EOF_BEFORE_PUBLICATION", uncertain=True)
        state.eof = True
        if channel == "qmp" and self._qmp:
            self._qmp_failed = True
            self._fail("TRUNCATED_JSON_EOF")

    def close_result(self, channel, result, error):
        """Record independent once-only closes; EBADF is known failure only."""
        state = self._channel(channel)
        if state.close != "not-attempted":
            self._fail("CLOSE_REPEATED", uncertain=True)
        state.close = "unknown"  # Never restore numeric close authority.
        if type(result) is not int or type(error) is not int or not 0 <= error <= ERRNO_MAX:
            self._fail("CLOSE_OUTCOME", uncertain=True)
        state.close_errno = error
        if result == 0 and error == 0:
            state.close = "closed"
        elif result == -1 and error == errno.EBADF:
            state.close = "bad-fd"
            self._fail("CLOSE_BAD_FD")
        else:
            self._fail("CLOSE_UNKNOWN", uncertain=True)
        if not state.eof or (self._pending is not None and self._pending[1] == channel):
            self._fail("CLOSE_BEFORE_COMPLETE_INPUT", uncertain=True)

    def snapshot(self):
        """Immutable evidence bytes; never an authority or independent syscall proof."""
        channels = {name: {
            "observed": s.observed, "published": s.published,
            "observed_sha256": s.observed_hash.hexdigest(),
            "observed_hash_complete": s.observed_hash_complete,
            "published_sha256": s.published_hash.hexdigest(),
            "eof": s.eof, "close": s.close, "close_errno": s.close_errno,
        } for name, s in self._channels.items()}
        complete = (not self.failed and self._pending is None and not self._qmp and
                    all(s.eof and s.close == "closed" for s in self._channels.values()))
        value = {"schema": "bootweave-raw-transport-observation-v1", "status": STATUS,
                 "phase": self.phase, "charged": self.used, "cap": RAW_CAP,
                 "chunks": self.chunks, "failed": self.failed, "uncertain": self.uncertain,
                 "first_cause": self.cause, "capture_sealed": self._sealed,
                 "pending_publication": self._pending is not None, "channels": channels,
                 "framed_record_sha256": tuple(self._frames), "unframed_bytes": len(self._qmp),
                 "framing_failed": self._qmp_failed, "reported_capture_complete": complete,
                 "actual_channel_transport_supplied": False,
                 "qmp_semantics_supported": False, "native_authority": False}
        return json.dumps(value, sort_keys=True, separators=(",", ":")).encode()

    def inventory_report(self):
        """Explicit unsupported result; digest/record shape cannot unlock resume.

        Bindings name observed wire prefixes, not complete retained files. No
        reported device is confused with a host handle, listener, original PID
        right or global absence. Unknown replies/IDs/events cannot be accepted
        through this fallback: there is no supported semantic path.
        """
        receipt = self.snapshot()
        bindings = tuple(("observed", name + ".observed-prefix", state.observed,
                          state.observed_hash.hexdigest())
                         for name, state in self._channels.items()
                         if state.observed_hash_complete)
        return InventoryReport(self.phase, "paused-inventory", False, False, False,
                               False, hashlib.sha256(receipt).hexdigest(), bindings, MISSING)
