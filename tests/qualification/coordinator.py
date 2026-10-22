# SPDX-License-Identifier: BSD-3-Clause
"""Dormant source model and stop gates; NOT an execution coordinator.

No process, file, socket, signal, media, credential, effect-callback or CLI API.
External records are reported evidence, not authenticated native facts. There is
no authority issuer here, and every Admission.native_admitted remains False.
Positive pure-model decisions neither authorize effects nor validate inventory.

The dormant host guard now borrows this exact lifetime, including cancellation;
blocking owner operations still need an external watchdog. Calling watchdog is
not itself an independent watchdog. No proof of fresh authority, native observation, aggregate
allocation, transport completeness or terminal closure is invented here.
"""
from dataclasses import dataclass, field, replace
import math
import re
from types import MappingProxyType
from qualification.lifetime import Lifetime

STATUS = "SOURCE_ONLY_NOT_EXECUTION_READY_OR_AUTHORIZED"
CANONICAL = "92166f07fd79c9d356ef3e1ed704eaeeed8653e4"
RUNTIME_TREE = "79060329e9d06283a95c247c9245bbdd60ac9081"
RUNTIME_BINDING_SHA256 = "6d537a176ef58cac2ea0ad4b91822dea32bf51928220c265cfb7bcc995bad797"
MAX_BINDINGS = 512
MAX_RECORDS = 32
MAX_POLLS = 131072
PHASES = ("install", "installed")
STAGES = MappingProxyType({
    "campaign": ("preflight", "prepared"),
    "install": ("paused-inventory", "guest-ready", "halted", "closed"),
    "installed": ("paused-inventory", "guest-ready", "build-seal", "cases-seal", "halted", "closed"),
})
REQUIRED_SOURCE = frozenset((
    "src/auth_input.c", "src/auth_input.h", "src/auth_material.c", "src/auth_material.h",
    "src/authenticated_review.c", "src/authenticated_review.h", "src/content_review.c",
    "src/content_review.h", "src/entry_cli.c", "src/entry_native.c", "src/entry_openbsd.c",
    "src/entry_platform.c", "src/main.c", "src/native.c", "src/payload_review.c",
    "src/payload_review.h", "src/plan.c", "src/plan.h", "src/plan_io.c", "src/policy_binding.c",
    "src/policy_binding.h", "src/policy_data.c", "src/policy_platform.c", "src/private_cli.h",
    "src/private_content_format.h", "src/private_entry.h", "src/private_limits.h",
    "src/private_span.h", "src/private_start_material.h", "src/signature_platform.c",
    "src/signature_verify.c", "src/signature_verify.h", "src/trusted_start.c", "src/trusted_start.h",
    "tests/qualification/coordinator.py", "tests/qualification/transport.py",
    "tests/qualification/accounting.py", "tests/qualification/lifetime.py",
    "tests/qualification/__init__.py",
    "tests/qualification/host_guard.py", "tests/qualification/prepare.py",
    "tests/qualification/compiler_guard.c", "tests/qualification/native_supervisor.c",
    "tests/qualification/entry_probe.c", "tests/qualification/limit_probe.c",
    "tests/qualification/forwarder.c", "tests/qualification/trace_limits.c",
    "tests/qualification/trace_limits.h", "tests/qualification/trace_model.c",
    "tests/qualification/runtime-bindings.json", "tests/qualification/oracles.json",
    "tests/qualification/trace_oracles.json",
))
MISSING = (
    "Fresh exact native execution authority and independently reviewed issuer",
    "External watchdog bounding blocked coordinator operations and effect quiescence",
    "Supported exact QMP inventory/resume semantics and independent paused inventory",
    "Authentication and independent interpretation of actual external stage seals",
    "Canonical34-source actual before/after byte proof against the frozen runtime binding",
    "Fixed provisioning/bootstrap/transport and aggregate allocation/high-water enforcement",
    "Actual native custody, normal halt, stopped image and independent final closure proof",
)
_created = False


class Refusal(RuntimeError):
    pass


class NativeAuthority:
    """Deliberately unissuable here. Plan/source approval is not this authority."""
    def __new__(cls, *args, **kwargs):
        raise Refusal("Fresh native authority integration is not implemented")


def _digest(value):
    return type(value) is str and re.fullmatch(r"[0-9a-f]{64}", value) is not None


@dataclass(frozen=True)
class Binding:
    kind: str
    name: str
    size: int
    sha256: str

    def __post_init__(self):
        if type(self.kind) is not str or self.kind not in ("source", "tool", "input", "recipe", "artifact") or \
                type(self.name) is not str or not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_./-]{0,191}", self.name) or \
                any(part in ("", ".", "..") for part in self.name.split("/")) or \
                type(self.size) is not int or not 0 <= self.size <= 20 * 1024**3 or not _digest(self.sha256):
            raise Refusal("BINDING")


def binding_set(values):
    """Pure exact-set representation; duplicates/partial comparisons are errors."""
    if type(values) is not tuple or not 0 < len(values) <= MAX_BINDINGS or \
            any(type(row) is not Binding for row in values):
        raise Refusal("BINDINGS")
    keys = [(row.kind, row.name) for row in values]
    if len(keys) != len(set(keys)):
        raise Refusal("DUPLICATE_BINDING")
    return frozenset(values)


def compare_bindings(expected, observed):
    """Both complete independently supplied sets must agree, including helpers.

    This comparison does not discover transitive dependencies or authenticate the
    producers. Independent review must establish the full expected closure.
    """
    if binding_set(expected) != binding_set(observed):
        raise Refusal("BINDING_MISMATCH")


def model_order(phase, accepted, stage):
    """Pure positive ordering oracle. It admits no effect or native fact."""
    if type(phase) is not str or phase not in STAGES or type(accepted) is not tuple or \
            accepted != STAGES[phase][:len(accepted)] or len(accepted) >= len(STAGES[phase]) or \
            stage != STAGES[phase][len(accepted)]:
        raise Refusal("ORDER")
    return accepted + (stage,)


@dataclass(frozen=True)
class Evidence:
    phase: str
    stage: str
    receipt_sha256: str
    bindings: tuple
    observed_at: float

    def __post_init__(self):
        if type(self.phase) is not str or self.phase not in STAGES or \
                type(self.stage) is not str or self.stage not in STAGES[self.phase] or \
                not _digest(self.receipt_sha256) or type(self.observed_at) not in (int, float) or \
                not math.isfinite(self.observed_at) or self.observed_at < 0:
            raise Refusal("EVIDENCE")
        binding_set(self.bindings)


def model_evidence(phase, stage, expected, evidence, oldest, now):
    """Pure record/bytes/time comparison, explicitly not proof authentication."""
    if type(evidence) is not Evidence or (evidence.phase, evidence.stage) != (phase, stage) or \
            not oldest <= evidence.observed_at <= now:
        raise Refusal("EVIDENCE_BINDING_OR_TIME")
    compare_bindings(expected, evidence.bindings)
    return True


@dataclass(frozen=True)
class Admission:
    model_eligible: bool
    work_closed: bool
    stop_required: bool
    cancelled: bool
    first_failure: str
    uncertain: bool
    phase: str
    missing: tuple = field(default=MISSING, init=False)
    native_admitted: bool = field(default=False, init=False)
    status: str = field(default=STATUS, init=False)

    def __bool__(self):
        raise TypeError("Inspect explicit fields; a source-model decision grants no authority")


class Coordinator:
    """One model lifetime, created BEFORE target/media effects; no caller clock.

    All methods are dormant data/state operations. No returned value is an effect
    capability. The corresponding real native admission remains unavailable.
    Serialized calls, one normally loaded module and no competing owner/reload
    are required. Python introspection is not a hostile-caller security boundary.
    """
    def __init__(self):
        global _created
        if _created:
            raise Refusal("ONCE")
        _created = True  # Even clock failure cannot renew this process lifetime.
        self._lifetime = Lifetime()
        self._origin = self._lifetime.origin
        self._end = self._lifetime.end
        self._cancelled, self._failure, self._uncertain = False, "", False
        self._phase, self._phase_origin, self._work_end, self._phase_end = "campaign", None, None, None
        self._attempts, self._accepted, self._tried, self._records = (), (), (), ()
        self._reports = ()
        self._receipts, self._evidence_time = frozenset(), self._origin
        self._baseline, self._custody = None, None
        self._custodies = ()
        self._polls = 0

    @property
    def origin(self):
        """Read-only clock witness; never accepted as an origin selector."""
        return self._origin

    @property
    def lifetime(self):
        """Same object borrowed by the host guard, never a copied clock seed."""
        return self._lifetime

    def _fail(self, reason, uncertain=False):
        if not self._failure:
            self._failure = reason
        self._uncertain |= uncertain

    def _now(self):
        try:
            now = self._lifetime.now()
            if now >= self._end:
                self._fail("CAMPAIGN_DEADLINE")
            return now
        except BaseException:
            self._fail("CLOCK", True)
            raise

    def cancel(self):
        self._lifetime.cancel()
        self._cancelled = True  # No clear/reset method, including between phases.
        self._fail("CANCELLED")

    def _require_live(self, now, closure=False):
        if now >= self._end or (self._phase_end is not None and now >= self._phase_end):
            self._fail("TOTAL_DEADLINE")
        if not closure and (self._failure or self._cancelled or
                            self._work_end is not None and now >= self._work_end):
            self._fail("WORK_CLOSED")
        if self._failure and not closure:
            raise Refusal(self._failure)
        if now >= min(self._end, self._phase_end or self._end):
            raise Refusal(self._failure)

    def record(self, expected, evidence):
        """Retain externally reported seals; malformed/missing/order failures stick.

        New stage hashes must be supplied from actual independent observations.
        Empty/unknown hashes refuse; do not fill them with planned values. A
        caller-created Evidence is never promoted to authenticated native proof.
        """
        try:
            if type(evidence) is Evidence:
                if len(self._reports) >= MAX_RECORDS:
                    raise Refusal("REPORT_BOUND")
                self._reports += (evidence,)  # Retain rejected well-typed reports too.
            now = self._now()
            if type(evidence) is not Evidence or evidence.phase != self._phase:
                raise Refusal("EVIDENCE_PHASE")
            key = (evidence.phase, evidence.stage)
            if key in self._tried or len(self._tried) >= MAX_RECORDS:
                raise Refusal("RECORD_ONCE")
            self._tried += (key,)  # Refusal consumes the stage, never retry.
            if evidence.receipt_sha256 in self._receipts:
                raise Refusal("RECEIPT_REUSE")
            self._receipts |= {evidence.receipt_sha256}
            closure = evidence.stage in ("halted", "closed")
            self._require_live(now, closure)
            # Failure finalizers remain independently reportable. Their receipts
            # never fill a missing normal prefix or enable the next phase.
            accepted = self._accepted if closure and self._failure else \
                model_order(self._phase, self._accepted, evidence.stage)
            oldest = max(self._evidence_time, self._phase_origin or self._origin)
            model_evidence(self._phase, evidence.stage, expected, evidence, oldest, now)
            base = frozenset(row for row in evidence.bindings if row.kind != "artifact")
            if evidence.stage == "preflight":
                if {row.kind for row in base} != {"source", "tool", "input", "recipe"} or \
                        not REQUIRED_SOURCE <= {row.name for row in base if row.kind == "source"}:
                    raise Refusal("INCOMPLETE_SHARED_CLOSURE")
                manifest = next(row for row in base if row.kind == "source" and
                                row.name == "tests/qualification/runtime-bindings.json")
                if manifest.sha256 != RUNTIME_BINDING_SHA256:
                    raise Refusal("RUNTIME_MANIFEST_CHANGED")
                self._baseline = base
            elif self._baseline is None or base != self._baseline:
                raise Refusal("BASELINE_CHANGED")
            if evidence.stage == "paused-inventory":
                raise Refusal("INVENTORY_UNSUPPORTED")
            if evidence.stage == "closed" and (self._custody is None or
                                               self._custody[1:] != (True, False, False)):
                raise Refusal("CUSTODY_UNRESOLVED")
            self._accepted = accepted
            self._evidence_time = evidence.observed_at
            self._records += (evidence,)
        except BaseException as error:
            self._fail(str(error) if type(error) is Refusal else "RECORD_ERROR", type(error) is not Refusal)
            raise
        return self.watchdog()

    def begin_phase(self, phase):
        """Consume a modeled launch slot, NOT a QEMU launch/resume admission."""
        try:
            now = self._now()
            if self._failure or self._cancelled or now >= self._end:
                raise Refusal(self._failure or "CAMPAIGN_DEADLINE")
            if phase not in PHASES or self._attempts != PHASES[:len(self._attempts)] or \
                    len(self._attempts) >= len(PHASES) or phase != PHASES[len(self._attempts)] or \
                    self._accepted != STAGES[self._phase]:
                raise Refusal("PHASE_ORDER")
            self._attempts += (phase,)  # No repeat or renewal after preparation failure.
            window = self._lifetime.begin(phase)
            self._phase, self._phase_origin = phase, window.origin
            self._phase_end, self._work_end = window.end, window.work_end
            self._accepted, self._custody = (), None
            if now >= self._work_end:
                raise Refusal("NO_WORK_WINDOW")
        except BaseException as error:
            self._fail(str(error) if type(error) is Refusal else "PHASE_ERROR", type(error) is not Refusal)
            raise
        return self.watchdog()

    def report_custody(self, phase, receipt_sha256, *, consumed, lost, close_unknown):
        """External original-owner report only; no PID argument or resampling.

        Clear reported flags are necessary model data, never actual wait/close
        proof or regained rights. Uncertainty remains sticky through closure.
        """
        if phase != self._phase or phase not in PHASES or self._custody is not None or \
                not _digest(receipt_sha256) or any(type(v) is not bool for v in (consumed, lost, close_unknown)):
            self._fail("CUSTODY_RECORD", True)
            raise Refusal("CUSTODY_RECORD")
        self._custody = (receipt_sha256, consumed, lost, close_unknown)
        self._custodies += ((phase, self._custody),)
        if not consumed or lost or close_unknown:
            self._fail("CUSTODY_UNCERTAIN", True)

    def admission(self, action, authority=None):
        """No native issuer/verification bridge exists: never grant admission."""
        if type(action) is not str or action not in ("prepare", "launch", "resume", "work", "finalize"):
            self._fail("ACTION")
            return replace(self.watchdog(), model_eligible=False)
        # Neither a dictionary/plan/source approval nor a forged type substitutes
        # for the missing authenticated issuer. No type is accepted as authority.
        decision = self.watchdog()
        structure = {
            "prepare": self._phase == "campaign" and self._accepted == ("preflight",),
            "launch": self._accepted == STAGES[self._phase] and len(self._attempts) < 2,
            "resume": False,  # Exact inventory semantics remain unsupported.
            "work": self._phase in PHASES and "guest-ready" in self._accepted,
            "finalize": self._phase in PHASES,
        }.get(action, False)
        return replace(decision, model_eligible=decision.model_eligible and structure)

    def watchdog(self):
        """Data only. An independent external watchdog must enforce these stops."""
        self._polls += 1
        if self._lifetime.cancelled:
            self.cancel()
        if self._polls > MAX_POLLS:
            self._fail("POLL_BOUND", True)
        try:
            now = self._now()
        except BaseException:
            now = None
        active = self._phase in PHASES and self._accepted != STAGES[self._phase]
        work_closed = self._cancelled or bool(self._failure) or now is None or \
            active and self._work_end is not None and now >= self._work_end
        total_closed = now is None or now >= self._end or \
            active and self._phase_end is not None and now >= self._phase_end
        if total_closed:
            self._fail("TOTAL_DEADLINE", now is None)
        rights_sealed = self._custody is not None and (self._custody[1] or self._custody[2])
        return Admission(not work_closed and not total_closed, bool(work_closed),
                         bool(active and not rights_sealed and (work_closed or total_closed)), self._cancelled,
                         self._failure, self._uncertain, self._phase)

    def snapshot(self):
        """Immutable bounded reported evidence; no diagnostic file is written."""
        return (STATUS, self._origin, self._end, self._phase, self._phase_origin,
                self._work_end, self._phase_end, self._attempts, self._accepted,
                self._tried, self._records, self._reports, self._custody, self._custodies, self._cancelled,
                self._failure, self._uncertain, self._receipts, self._evidence_time)
