# SPDX-License-Identifier: BSD-3-Clause
"""Finite qualification reservations and sampled accounting, SOURCE ONLY.

No filesystem/process/quota/credential API or admission capability. A sample or
manifest hash is caller-reported data; it cannot prove a native peak, complete
inventory, disjoint scopes or retained custody. Reservations never become free
after a smaller sample, failure, cancellation, close or apparent cleanup.
"""
from dataclasses import dataclass
import re

MIB = 1024 * 1024
HOST_CAP = 20 * 1024 * MIB
GUEST_CAP = 27 * MIB
MAX_OBSERVATIONS = 256
COMPILER_JOBS = ("bootstrap", *("unit-%02d" % i for i in range(20)),
                 "supervisor", "closed-dynamic", "limits-dynamic")


class Refusal(ValueError):
    pass


@dataclass(frozen=True)
class Reservation:
    name: str
    domain: str
    ceiling: int
    scope: str


# Explicitly disjoint proposed scopes; an actual complete inventory must still
# establish that disjointness. Metadata includes receipts, markers and overhead.
RESERVATIONS = (
    Reservation("host.target", "host", 8 * 1024 * MIB, "new build.raw"),
    Reservation("host.data", "host", 1024 * MIB, "new read-only data.iso"),
    Reservation("host.media-work", "host", 1024 * MIB, "new media working files"),
    Reservation("host.source", "host", MIB, "new sealed source bundle"),
    Reservation("host.install-raw", "host", 10 * MIB, "first-launch five raw channels"),
    Reservation("host.installed-raw", "host", 10 * MIB, "second-launch five raw channels"),
    Reservation("host.metadata", "host", MIB, "new host metadata and overhead"),
    Reservation("guest.source", "guest", MIB, "sealed source and derived input files"),
    Reservation("guest.objects", "guest", 20 * 262144, "twenty retained unit objects"),
    Reservation("guest.executables", "guest", 4 * 262144, "bootstrap and three linked executables"),
    Reservation("guest.maps", "guest", 4 * 262144, "bootstrap and three linker maps"),
    Reservation("guest.compiler-logs", "guest", len(COMPILER_JOBS) * 2 * 262144,
                "separate stdout/stderr for all twenty-four compiler calls"),
    Reservation("guest.compiler-tmp", "guest", MIB, "all compiler temporary high-water bytes"),
    Reservation("guest.cases", "guest", MIB, "all nine cases, file-size bytes and journal"),
    Reservation("guest.metadata", "guest", MIB, "all other guest metadata and overhead"),
)
_FIXED = {row.name: row for row in RESERVATIONS}


def plan():
    """Return a new data projection; unused space grants no allocation authority."""
    totals = {domain: sum(row.ceiling for row in RESERVATIONS if row.domain == domain)
              for domain in ("host", "guest")}
    if len(_FIXED) != len(RESERVATIONS) or totals["host"] > HOST_CAP or totals["guest"] > GUEST_CAP:
        raise Refusal("Fixed reservation plan exceeds the unchanged ceilings")
    return {"status": "SOURCE_ONLY_NOT_ENFORCEMENT_OR_AUTHORITY",
            "caps": {"host": HOST_CAP, "guest": GUEST_CAP}, "reserved": totals,
            "unreserved": {"host": HOST_CAP - totals["host"], "guest": GUEST_CAP - totals["guest"]},
            "compiler_calls": len(COMPILER_JOBS), "compiler_jobs": list(COMPILER_JOBS),
            "reservations": [{"name": row.name, "domain": row.domain,
                              "ceiling": row.ceiling, "scope": row.scope} for row in RESERVATIONS],
            "required_unestablished_proofs": [
                "complete disjoint native inventories and actual identity/custody",
                "host aggregate and guest temporary high-water enforcement",
                "no uncharged allocations outside the fixed reported scopes",
                "all native staged tool/executable/output and final stopped readbacks"],
            "execution_ready": False}


class Accounting:
    """Bounded append-only in-memory reports, never a native allocator or oracle."""
    def __init__(self):
        self._plan = plan()
        self._records = []
        self._failed = None
        self._last = {}
        self._maxima = {}
        self._decreases = set()

    def _refuse(self, reason):
        if self._failed is None:
            self._failed = reason
        raise Refusal(reason)

    def unknown(self, role):
        """A missing/uncertain measurement poisons closure; no repair/retry API."""
        if self._failed is not None:
            raise Refusal("Accounting already failed")
        self._refuse("Unknown allocation evidence" if type(role) is str and role in _FIXED
                     else "Unknown allocation scope")

    def observe(self, role, logical_bytes, allocated_bytes, manifest_sha256):
        """Record a reported aggregate sample, not verified contents or a peak.

        Charge max(logical, allocated) so sparse/rounded allocation observations
        cannot reduce the reservation. Hash syntax is a binding check only.
        """
        if self._failed is not None:
            raise Refusal("Accounting already failed")
        if type(role) is not str or role not in _FIXED:
            self._refuse("Unknown allocation scope")
        if len(self._records) >= MAX_OBSERVATIONS:
            self._refuse("Finite allocation observation count exhausted")
        if (type(logical_bytes) is not int or type(allocated_bytes) is not int or
                not 0 <= logical_bytes <= 2**63 - 1 or not 0 <= allocated_bytes <= 2**63 - 1):
            self._refuse("Invalid allocation byte count")
        if type(manifest_sha256) is not str or not re.fullmatch(r"[0-9a-f]{64}", manifest_sha256):
            self._refuse("Invalid reported inventory digest")
        charged = max(logical_bytes, allocated_bytes)
        row = {"role": role, "logical_bytes": logical_bytes, "allocated_bytes": allocated_bytes,
               "charged_bytes": charged, "manifest_sha256": manifest_sha256}
        self._records.append(row)  # Preserve even the reported over-cap sample.
        if role in self._last and charged < self._last[role]:
            self._decreases.add(role)
        self._last[role] = charged
        self._maxima[role] = max(charged, self._maxima.get(role, 0))
        if charged > _FIXED[role].ceiling:
            self._refuse("Reported allocation exceeded its fixed reservation")
        return len(self._records) - 1

    def report(self):
        reservations = {row.name: row.ceiling for row in RESERVATIONS}
        return {"status": "SOURCE_ONLY_SAMPLED_ACCOUNTING_NOT_PEAK_PROOF",
                "failed": self._failed, "observations": [dict(row) for row in self._records],
                "latest_charged_bytes": dict(self._last), "maximum_observed_bytes": dict(self._maxima),
                "sample_decreases": sorted(self._decreases), "reservations_held": reservations,
                "missing_scopes": sorted(set(_FIXED) - set(self._last)),
                "maximum_observed_by_domain": {
                    domain: sum(self._maxima.get(row.name, 0) for row in RESERVATIONS if row.domain == domain)
                    for domain in ("host", "guest")},
                "logical_or_sampled_fit_proves_high_water": False,
                "source_plan_only": True, "execution_ready": False,
                "required_unestablished_proofs": list(self._plan["required_unestablished_proofs"])}
