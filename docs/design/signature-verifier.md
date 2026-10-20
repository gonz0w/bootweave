# Internal fixed detached-signature verification

Status: delegated source continuation of issue #5, 2026-10-20. Independent design
review precedes implementation. Refines [authenticated I/O](authenticated-io.md)
and consumes only [prepared material](verifier-material.md). This is one internal
signature stage, not complete policy/digest authentication, a CLI or write authority.

## Result and trusted caller

Add an internal single-threaded original-C function taking one exclusive READY
material object and one exclusive zero-initialized result. The cancellation flag
remains the material object's caller-owned flag. Buffers/state/flag must remain
valid and disjoint; no concurrent close/rebind, handler mutation or reentry.
Consume READY exactly once by latching a new VERIFYING state before effects.
The material release function accepts that internal state for the sole verifier's
controlled cleanup; it consumes all original handles once, preserving its existing
close uncertainty. Every return after accepting READY releases it; repeated use
cannot close reused numbers. An already-used result or non-READY material refuses
without modifying either. No callback, project pathname, executable or policy input.

The result is signature-accepted, rejected or uncertain with a fixed first-cause
enum and private bounded child/wait/close observations. Signature-accepted means
only that the fixed verifier accepted these prepared bytes with this supplied key,
and all required local postconditions completed. It cannot assert compiled policy,
definition/payload digest, project compatibility, archive safety or authorization.
The later trusted caller must independently enforce those adopted requirements.

This function is absent from the production link and CLI. Non-OpenBSD production
objects refuse before fork. On OpenBSD, the future trusted entry must already have
installed the adopted hard overall timers/resource limits and exact locked unveil
table before calling this internal function; that entry does not yet exist. This
helper cannot query/prove another function's unveil history. No native acceptance
or usable standalone entry follows from compiling or testing this module.

## Fixed capabilities and native adapter

Before any fork, require non-elevated identity, default SIGCHLD disposition without
SA_NOCLDWAIT and an already ignored parent SIGPIPE. The future trusted entry owns
that fixed SIGPIPE policy; this function installs no temporary parent handler.
Require supported non-reaping waitid flags, live cancellation/deadline, and
three distinct readonly regular CLOEXEC handles at offset zero. Check against
the material's exact device/inode/owner/mode/size and require link count zero.
Check both caller outputs exist and do not alias material; never write to them.
Probe waitid on the process's own PID: ECHILD is the only accepted no-child result;
unsupported flags or unexpected results refuse before creating a child.

The sole executable and argv remain `/usr/bin/signify` and
`signify -V -q -p /dev/fd/3 -x /dev/fd/4 -m /dev/fd/5`. Environment is exactly
LC_ALL=C, LANG=C, TZ=UTC and PATH empty. No shell, PATH search or alternate verifier.
Open only fixed `/dev/null` readonly/CLOEXEC for child stdin. Refine the future
entry's exact whitelist to include `/dev/null` read-only; no directory or arbitrary
path is added. Native qualification must cover that device and the actual fixed
executable/loader/library/fd whitelist. The current module adds no unveil table.

Create only two private diagnostic pipes and one parent-to-child launch gate,
all CLOEXEC. Parent read ends are nonblocking; child stdout/stderr stay private.
After fork, the child closes the inherited parent pipe ends, restores SIGPIPE to
default, and establishes its own group and a default unblocked five-second SIGALRM
before waiting on the gate. The parent must successfully establish that
same born child's PGID=PID, release the material's parent handles with known
cleanup, and drop its exec/path promises before writing the one-byte launch grant.
Any failure closes the gate and prevents exec. The parent never grants launch
based on a sampled PID/birth record.

The child duplicates its six fixed sources to private temporary fds >=6, maps
stdin/stdout/stderr/key/signature/definition to 0..5, then calls base closefrom(6).
No inherited caller output, payload, other descriptor or environment is handed
to the executable. Child limits remain CPU2, DATA32MiB, STACK2MiB, NOFILE16,
CORE0 and FSIZE0, respecting lower inherited ceilings and reading limits back.
Failure refuses. Before exec call the adopted child pledge with explicit exec
promises; the locked caller whitelist is inherited. After fork the parent drops
to stdio/proc. Native adapter code owns only fixed OS operations; it is replaced
at static link time in a separate test executable, never via runtime configuration.

## Custody, diagnostics and bounded cleanup

Parent verification work has a fixed five-second cooperative deadline; cleanup
has a separate fixed four-second reserve. Invalid/backward clocks refuse. The
future entry's hard overall wall/CPU timers remain required. Check cancellation
at every work boundary. Child alarm is established independently before gate wait,
mapping, limits and exec. Kernel calls and host scheduling remain native concerns.

Keep the immutable fork PID and known group unreaped through the final positive
stop. Observe completion only with waitid WNOWAIT. Unsupported/failed/lost wait
authority revokes all further positive signals; observation never renews it.
If group establishment failed, only the still-owned unreaped direct child may be
stopped by PID. Otherwise make one final SIGKILL group attempt before actual wait.
Only success or ESRCH is known stop; EPERM and other errors remain uncertainty.
No ps invocation, numeric-ID recovery, signal after consumed/lost wait, or retry
of a close's numeric fd. An early child gate-reader death makes the ignored-SIGPIPE
parent observe EPIPE and perform controlled cleanup, never an implicit parent exit.
The fixed native executable's behavior and process tree
need separate source-bound native observations; this is no global cleanup proof.

After the final stop attempt, draining both pipes, actual wait4, and every owned
fd close are independent bounded stages. wait4 uses WNOHANG; latch consumption
immediately on the exact positive PID result, before interpreting status/RSS.
Signal, wait, drain or close uncertainty prevents signature-accepted. Preserve the
primary failure, and record cleanup uncertainty separately. Never signal after
that latch, including after later status/observation errors.
After the final custody/close stages, independently reduce the parent to stdio
only, including controlled rejection/uncertainty returns. A failed final reduction
prevents acceptance and preserves the primary cause and separate boundary flag;
it cannot skip any other close/wait stage. No process operation follows that drop.

Discard raw verifier output after counting at most 4096 combined bytes; use fixed
ordinary categories only. Both EOFs and exact reaped exit0 are required for
signature-accepted. Overflow, pipe/read failure, signal/nonzero status, cancellation,
deadline, failed boundary or incomplete cleanup refuse. Even after cancellation,
attempt all known owned closes once. Unknown closure is not deletion/retry authority.
Scratch namespace was already removed by preparation; this module creates no
member files, keys, credential, target or persistent receipt.

## Portable Test Design and budgets

Keep main/Makefile production linkage unchanged. New source/header/adapter ceiling
is 18KiB, whole src64KiB; retain planner24KiB, capture6KiB and material18KiB
ceilings separately. Fixed transient workspace <=8KiB, three pipes and one child.
The adopted native runtime limits above are unchanged and are not host claims.

Tests link the real common orchestration and material code with a separate static
native adapter and execve/syscall substitutions. Use inert format-valid records,
no key generation, private key, actual signify or project executable. A tiny fixed
in-child execve double checks actual 0..5 mappings, exact argv/environment and
retained bytes, then emits controlled private diagnostics/exits. Its exit0 models
acceptance; it is never cryptographic or OpenBSD evidence. Native DATA/STACK and
pledge/closefrom outcomes are explicitly modeled on this Mac; do not silently
claim those production limits were established. CPU/fd/file/alarm controls and
owned fork/wait/pipe behavior can be observed portably under a finite outer run.
Darwin zombie-only stop behavior is recorded separately if a test double models
the native stop result; production must not special-case that host permission error.

| Test | Independent required observation |
| --- | --- |
| SV-T01 | Exact prepared bytes reach child3/4/5 at offset0, argv/env are fixed, unrelated child fds absent and parent's outside sentinel unchanged; modeled exit0 plus known cleanup yields stage acceptance only. |
| SV-T02 | Wrong state, elevated/unsupported host, SIGCHLD/descriptor/type/alias/metadata/offset and WNOWAIT preflight failures produce no fork/exec; accepted READY still releases once. |
| SV-T03 | Child group/gate/dup/map/limit/pledge/exec and parent-boundary/final-drop failures never yield acceptance; no launch grant after parent cleanup failure; early gate-reader death gives finite EPIPE cleanup. |
| SV-T04 | Nonzero/signal exits, exact4096/overflow diagnostics, short/EINTR/error pipes, missing EOF and cancellation/deadline cause finite refusal; raw bytes never enter ordinary outputs. |
| SV-T05 | WNOWAIT-before-stop-before-single-consumed-wait ordering, lost/reaped authority, observer/status faults and per-owned-close errors preserve uncertainty and never signal/reclose reused IDs. |
| SV-T06 | Duplicate use leaves consumed objects/reused fds untouched; all normal-object/CLI regression and source budgets remain enforced. Actual child alarm has a bounded control independent of modeled parent deadline. |

Each test invocation has finite CPU/fd/file/timeout/output limits, explicit minimal
environment and independently retained failures/custody observations. One supervised
portable job at a time, bounded worker parallelism, no network or inherited
credentials. Review the exact launch envelope before running it. Full clean-candidate
make verify, independent review, signatures/DCO and canonical binding precede normal
merge. Exact native execution requires its own reviewed packet and applicable fresh
authority; consumed approvals and historical data remain unchanged.
