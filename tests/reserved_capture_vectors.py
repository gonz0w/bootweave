# SPDX-License-Identifier: BSD-3-Clause
"""Literal reservation-bound capture requirements, frozen before execution.

No candidate, driver, clock, process or filesystem imports. Causes/ownership are
from the reviewed reservation-continuity contract, not measured output.
"""
BASELINE = "7550f2e93dfa05d4f35591323f19fb7daa37de26"
ROLE_NAMES = ("definition", "payload", "signature")
CAPS = (16384, 1048576, 16384)
CASE_COUNT = 116


def expected(result, cause="NONE", *, reads="none", errors=0, unknown=0):
    return {"return": result, "cause": cause, "reads": reads,
            "capture_close_errors": errors, "capture_unknown_closes": unknown,
            "uncertain": int(bool(unknown)), "taken": int(result != -2),
            "snapshot": "unchanged" if result == -2 else "exact" if result == 1 else "zero",
            "input_closes": (0, 0, 0) if result == -2 else (1, 1, 1),
            "prepare_result": None if result == -2 else 1 if result == 1 else -1 if unknown else 0,
            "prepare_members": result == 1, "repeat_unchanged": True,
            "original_origin_and_flag": True, "no_input_or_output_writes": True}


def cases():
    result = []

    def add(name, mode, e, *, index=0, phase="none", action="prepare"):
        result.append({"name": name, "mode": mode, "index": index, "phase": phase,
                       "action": action, "expected": e})

    for mode in ("normal", "short", "atime", "near-deadline"):
        add("capture-prepare-" + mode, mode, expected(1, reads="all"))
    add("capture-release", "normal", expected(1, reads="all"), action="release")
    for phase in ("preflight", "read", "eintr", "final-close"):
        for mode, cause in (("cancel", "CANCEL"), ("clock-error", "DEADLINE"),
                            ("clock-invalid", "DEADLINE"), ("clock-backward", "DEADLINE"),
                            ("deadline", "DEADLINE")):
            add(mode + "-" + phase, mode,
                expected(0, cause, reads="none" if phase == "preflight" else "some"), phase=phase)
    for role in range(3):
        add("close-eintr-" + ROLE_NAMES[role], "close-eintr",
            expected(-1, "CLOSE", reads="all", errors=1 << role, unknown=1 << role), index=role)
        add("close-ebadf-" + ROLE_NAMES[role], "close-ebadf",
            expected(0, "CLOSE", reads="all", errors=1 << role), index=role)
        add("read-primary-plus-unknown-close-" + ROLE_NAMES[role], "read-error-close-eintr",
            expected(-1, "IO", reads="some", errors=1 << role, unknown=1 << role), index=role)
        add("unknown-close-release-" + ROLE_NAMES[role], "close-eintr",
            expected(-1, "CLOSE", reads="all", errors=1 << role, unknown=1 << role), index=role, action="release")
    for mode in ("reserve-identity", "reserve-stat-error"):
        add(mode, mode, expected(0, "OWNERSHIP"))
    for role in range(3):
        for mode in ("empty", "overcap", "nonregular", "readwrite", "offset", "missing", "flags-error"):
            add(mode + "-" + ROLE_NAMES[role], mode,
                expected(0, "IO", errors=(1 << role) if mode == "missing" else 0), index=role)
    for mode, index in (("input-alias", 1), ("input-alias", 2), ("input-alias-last", 2),
                        ("output-alias", 1), ("output-alias", 2), ("output-stat-error", 1), ("output-stat-error", 2)):
        add(mode + "-" + str(index), mode, expected(0, "IO"), index=index)
    for mode in ("read-error", "shrink", "grow", "changed-offset", "changed-device", "changed-inode",
                 "changed-mode", "changed-uid", "changed-gid", "changed-link", "changed-size",
                 "changed-mtime", "changed-ctime"):
        add(mode, mode, expected(0, "IO", reads="some"))
    for mode in ("null-material", "null-snapshot", "null-flag", "overflow-material", "overflow-snapshot",
                 "overflow-flag", "material-snapshot", "material-flag", "snapshot-flag",
                 "state-empty", "state-working", "state-ready", "state-closed", "state-rejected",
                 "state-uncertain", "state-verifying", "taken", "cause", "uncertain", "close-errors",
                 "unknown-closes", "capture-close-errors", "capture-unknown-closes", "directory-low",
                 "directory-unknown", "directory-lost", "created", "known", "writer", "ready-fd",
                 "path-empty", "started-negative", "started-invalid", "identity-type", "identity-mode",
                 "identity-links"):
        add("no-effects-" + mode, mode, expected(-2, "unchanged"), action="release")
    return tuple(result)
