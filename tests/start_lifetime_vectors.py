# SPDX-License-Identifier: BSD-3-Clause
"""Literal closed-start/lifecycle oracles frozen before implementation inspection.

Only semantic phases are specified. Test adapters may map these to static source
boundaries after source review; no measured candidate result defines an oracle.
No process, clock, filesystem, driver or candidate import.
"""
BASELINE = "1a8b5b25f4f95d86948f002286c926abc2ce017a"
TREE = "9bb1ed1930c3f177b506500668a2a749706cdce5"
CANCEL_SIGNALS = ("INT", "TERM", "HUP")
CLASSIC_SIGNALS = ("ALRM", "PROF", "XCPU", "XFSZ", "PIPE", "CHLD")
RESOURCE_ORDER = ("CPU", "DATA", "STACK", "NOFILE", "CORE", "FSIZE")
RESOURCE_CAPS = (4, 67108864, 2097152, 32, 0, 65536)
SEED = (1000, 1234)

# Setup failure is terminal, latched, and leaves EMPTY material untouched.
# First-clock cancellation is delivered by a retained flag-only callback after
# that sample but before REAL arm; a pending callback is delivered in UNBLOCK.
SETUP_FAILURES = (
    ("empty-action", "empty:0"), ("empty-unblock", "empty:1"),
    ("empty-cancel", "empty:2"), ("empty-reset", "empty:3"),
    ("block", "block"), ("unblock", "unblock"),
    ("origin-error", "origin"), ("origin-negative", "origin"),
    ("origin-invalid", "origin"), ("origin-negative-nsec", "origin"),
    ("unsupported", "supported"), ("root", "getuid"),
)
NEW_SIGNAL_FAILURES = (
    ("add-cancel", "cancel"), ("add-unblock", "unblock"), ("action-cancel", "action"),
)

# Exact nanosecond offsets from the original seed, never a renewed deadline.
# A late first private SV stamp tests the gap before its private phase starts.
# A cleanup jump is installed by the synthetic stop callback, after its private
# cleanup origin; clock errors/backward values must preserve CLOCK, not DEADLINE.
SIGNATURE_CASES = (
    {"name": "ordinary", "start": 1000000000, "work": 1000000000, "cleanup": 1000000000,
     "return": 1, "cause": "NONE", "budget": 0, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "original-eleven-before-start", "start": 11000000000, "work": 11000000000, "cleanup": 11000000000,
     "return": 0, "cause": "DEADLINE", "budget": 0, "forks": 0, "launch": 0, "stop": 0, "consumed": 0, "lost": 0},
    {"name": "original-fifteen-before-start", "start": 15000000000, "work": 15000000000, "cleanup": 15000000000,
     "return": 0, "cause": "DEADLINE", "budget": 1, "forks": 0, "launch": 0, "stop": 0, "consumed": 0, "lost": 0},
    {"name": "own-five", "start": 1000000000, "work": 6000000000, "cleanup": 6000000000,
     "return": 0, "cause": "DEADLINE", "budget": 0, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "original-eleven-work", "start": 10000000000, "work": 11000000000, "cleanup": 11000000000,
     "return": 0, "cause": "DEADLINE", "budget": 0, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "own-four-cleanup", "start": 1000000000, "work": 1000000000, "cleanup": 5000000000,
     "return": 0, "cause": "DEADLINE", "budget": 1, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "cleanup-fifteen", "start": 1000000000, "work": 1000000000, "cleanup": 15000000000,
     "return": 0, "cause": "DEADLINE", "budget": 1, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "work-before-private-origin", "start": 10000000000, "work": 9000000000, "cleanup": 9000000000,
     "return": 0, "cause": "CLOCK", "budget": 0, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "work-before-material-origin", "start": 1000000000, "work": -500000000, "cleanup": -500000000,
     "return": 0, "cause": "CLOCK", "budget": 1, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "cleanup-before-private-origin", "start": 1000000000, "work": 1000000000, "cleanup": 500000000,
     "return": 0, "cause": "CLOCK", "budget": 1, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "cleanup-before-material-origin", "start": 1000000000, "work": 1000000000, "cleanup": -500000000,
     "return": 0, "cause": "CLOCK", "budget": 1, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "work-clock-error", "start": 1000000000, "work": "error", "cleanup": "error",
     "return": 0, "cause": "CLOCK", "budget": 1, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "work-clock-invalid", "start": 1000000000, "work": "invalid", "cleanup": "invalid",
     "return": 0, "cause": "CLOCK", "budget": 1, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "cleanup-clock-error", "start": 1000000000, "work": 1000000000, "cleanup": "error",
     "return": 0, "cause": "CLOCK", "budget": 1, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "cleanup-clock-invalid", "start": 1000000000, "work": 1000000000, "cleanup": "invalid",
     "return": 0, "cause": "CLOCK", "budget": 1, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "unknown-close-late", "start": 1000000000, "work": 1000000000, "cleanup": 15000000000,
     "return": -1, "cause": "CLOSE", "budget": 1, "forks": 1, "launch": 0, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "failed-stop-late", "start": 1000000000, "work": 1000000000, "cleanup": 5000000000,
     "return": -1, "cause": "STOP", "budget": 1, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    # This jump occurs when the synthetic observer returns ECHILD, preserving
    # WAIT before any cleanup budget check; it is not installed at gate grant.
    {"name": "lost-wait-late", "start": 1000000000, "work": 15000000000, "cleanup": 15000000000,
     "return": -1, "cause": "WAIT", "budget": 1, "forks": 1, "launch": 1, "stop": 0, "consumed": 0, "lost": 1},
)


def cases():
    result = [{"name": "closed-normal", "kind": "normal"},
              {"name": "normal-host-unsupported", "kind": "normal-host"},
              {"name": "null-does-not-latch", "kind": "null"},
              {"name": "nonempty-does-not-latch", "kind": "nonempty"},
              {"name": "legacy-shared-latch", "kind": "legacy-first"}]
    for mode, terminal in SETUP_FAILURES:
        result.append({"name": "setup-" + mode, "kind": "setup", "mode": mode, "terminal": terminal})
    for mode, target in NEW_SIGNAL_FAILURES:
        for signal in CANCEL_SIGNALS:
            result.append({"name": mode + "-" + signal, "kind": "setup", "mode": mode,
                           "signal": signal, "terminal": ("action:" + signal if target == "action" else "add:" + signal + ":" + target)})
    for mode in ("pending-unblock", "cancel-at-origin", "cancel-after-real", "cancel-after-prof"):
        for signal in CANCEL_SIGNALS:
            result.append({"name": mode + "-" + signal, "kind": "cancel-setup", "mode": mode, "signal": signal})
    for phase in ("reserve", "capture", "prepare"):
        result.append({"name": "original-two-" + phase, "kind": "spent", "phase": phase})
    for phase in ("after-reserve", "after-capture", "after-prepare", "signature-gate", "after-signature"):
        for signal in CANCEL_SIGNALS:
            result.append({"name": "flag-" + signal + "-" + phase, "kind": "flag", "phase": phase, "signal": signal})
    for item in SIGNATURE_CASES:
        result.append(dict(item, kind="signature", name="signature-" + item["name"]))
    return tuple(result)

# Pre-dispatch addendum: SL-T05 needs fractional positive controls for each
# strict cap, so a uniformly rejecting combined predicate cannot satisfy the
# exact-boundary cases. Original 74 cases/fields above are preserved unchanged.
# The cold cleanup control advances the first cleanup stamp to just below+11s
# only after the synthetic observer has completed at original+10s. Its final
# sample is then below BOTH cleanup+4 and original+15. Exact completed-work
# boundaries separately refuse even when waitid reports successful completion.
SIGNATURE_POSITIVE_CONTROLS = (
    {"name": "signature-own-five-just-below", "kind": "signature", "start": 1000000000,
     "work": 5999999999, "cleanup": 5999999999, "return": 1, "cause": "NONE", "budget": 0,
     "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "signature-original-eleven-before-start-just-below", "kind": "signature", "start": 10999999999,
     "work": 10999999999, "cleanup": 10999999999, "return": 1, "cause": "NONE", "budget": 0,
     "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "signature-own-four-cleanup-just-below", "kind": "signature", "start": 1000000000,
     "work": 1000000000, "cleanup": 4999999999, "return": 1, "cause": "NONE", "budget": 0,
     "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "signature-original-fifteen-cold-cleanup-just-below", "kind": "signature", "start": 10000000000,
     "work": 10000000000, "observer": 10999999999, "cleanup": 14999999998,
     "return": 1, "cause": "NONE", "budget": 0, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
)
SIGNATURE_COMPLETED_WORK_BOUNDARIES = (
    {"name": "signature-work-completed-at-own-five", "kind": "signature", "start": 1000000000,
     "work": 1000000000, "observer": 6000000000, "cleanup": 6000000000,
     "return": 0, "cause": "DEADLINE", "budget": 0, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
    {"name": "signature-work-completed-at-original-eleven", "kind": "signature", "start": 10000000000,
     "work": 10000000000, "observer": 11000000000, "cleanup": 11000000000,
     "return": 0, "cause": "DEADLINE", "budget": 0, "forks": 1, "launch": 1, "stop": 1, "consumed": 1, "lost": 0},
)
CASE_COUNT = 80
