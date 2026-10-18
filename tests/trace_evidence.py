# SPDX-License-Identifier: BSD-3-Clause
"""Independent, fail-closed checks of bounded native kdump syscall evidence."""

import re

RECORD = re.compile(r"^\s*(\d+)\s+(\S+)\s+(CALL|RET|PSIG)\s+(.+)$")
CALL = re.compile(r"(\w+)\((.*)\)$")
FORBIDDEN = {"open", "openat", "execve", "fork", "vfork", "tfork", "socket", "socketpair"}
LIMITS = ("DATA", "STACK", "NOFILE", "CORE", "FSIZE", "CPU")


def check_trace(decoded, receipts, supervisor, positive=False):
    if len(decoded.encode("ascii")) > 1048576 or len(receipts) != (2 if positive else 46):
        raise ValueError("trace bounds/count")
    children = {r[1]: [] for r in receipts}
    if len(children) != len(receipts) or supervisor in children:
        raise ValueError("trace PID uniqueness")
    parent = []
    for line in decoded.splitlines():
        if not line.strip():
            continue
        match = RECORD.fullmatch(line)
        if match is None:
            raise ValueError("unknown or malformed trace record")
        pid, _, kind, body = match.groups()
        pid = int(pid)
        if pid in children:
            children[pid].append((kind, body))
        elif pid == supervisor:
            parent.append((kind, body))
        else:
            raise ValueError("uncorrelated trace process")
    forked = set()
    for kind, body in parent:
        match = re.match(r"fork (\d+)(?:/0x[0-9a-f]+)?$", body)
        if kind == "RET" and match:
            forked.add(int(match[1]))
    if forked != set(children):
        raise ValueError("parent fork correlation")
    results = []
    for index, receipt in enumerate(receipts):
        case, pid, status, *_ = receipt
        if case != index:
            raise ValueError("case sequence")
        records = children[pid]
        if not records or ("RET", "execve JUSTRETURN") not in records:
            raise ValueError("missing successful executable transition")
        exec_at = records.index(("RET", "execve JUSTRETURN"))
        runtime = records[exec_at + 1:]
        calls = []
        for offset, (kind, body) in enumerate(runtime):
            if kind == "CALL":
                match = CALL.fullmatch(body)
                if match is None:
                    raise ValueError("malformed syscall")
                calls.append((offset, *match.groups()))
        edge = index % 23
        confined = positive or edge < 15
        no_input = not positive and (edge < 10 or edge >= 15)
        if no_input and any(name == "read" and re.match(r"0,", args) for _, name, args in calls):
            raise ValueError("read from disallowed input")
        if confined:
            def checked(name, argument=None):
                found = []
                for offset, syscall, args in calls:
                    if syscall != name or (argument is not None and not args.startswith(argument)):
                        continue
                    if offset + 1 >= len(runtime) or runtime[offset + 1] != ("RET", name + " 0"):
                        raise ValueError("missing successful checked syscall return")
                    found.append(offset)
                return found

            close = checked("closefrom", "3")
            limits = [checked("setrlimit", "RLIMIT_" + name + ",") for name in LIMITS]
            unveil = checked("unveil")
            pledge = checked("pledge")
            timers = [checked("setitimer", timer + ",") for timer in ("ITIMER_REAL", "ITIMER_PROF")]
            if len(close) != 1 or any(len(x) != 1 for x in limits + timers) or len(unveil) != 2 or len(pledge) != 1:
                raise ValueError("incomplete confinement sequence")
            boundary = pledge[0]
            if not (max(x[0] for x in timers) < close[0] < limits[0][0] and
                    [x[0] for x in limits] == sorted(x[0] for x in limits) and
                    limits[-1][0] < unveil[0] < unveil[1] < boundary):
                raise ValueError("confinement order")
            if any(offset <= boundary and name == "read" and re.match(r"0,", args)
                   for offset, name, args in calls):
                raise ValueError("input before confinement")
            if any(offset > boundary and name in FORBIDDEN for offset, name, _ in calls):
                raise ValueError("forbidden post-confinement syscall attempt")
        if status & 127:
            if status & 127 != 15 or not any(kind == "PSIG" and body.startswith("SIGTERM SIG_DFL")
                                           for kind, body in runtime):
                raise ValueError("missing terminal signal evidence")
        else:
            exits = [(offset, args) for offset, name, args in calls if name == "exit"]
            if len(exits) != 1 or int(exits[0][1], 0) != status >> 8 or exits[0][0] != len(runtime) - 1:
                raise ValueError("missing complete terminal exit evidence")
        results.append({"case": index, "pid": pid, "confinement_checked": confined,
                        "disallowed_input_read_absent": no_input,
                        "complete_terminal_evidence": True})
    return results
