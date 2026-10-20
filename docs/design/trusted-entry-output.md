# Trusted entry and output integration

Status: source-only engineering continuation of issue #5 after PR #85,
2026-10-20. This refines the adopted [read-only scope](authenticated-plan.md),
[fixed I/O](authenticated-io.md), [material ownership](verifier-material.md) and
[fused review](authenticated-composition.md). Independent design/source review
precedes candidate execution. No new CLI, authentication/output capability,
publisher policy, extraction, native campaign or deployment is adopted here.

## Useful next result and fixed boundaries

The eventual trusted entry must own one complete process lifetime: identity and
hard process limits, original clock/cancellation, reservation, exact locked
confinement, capture, the real authenticated pipeline and complete fixed-fd output.
The existing internal memory result cannot be supplied to a separate writer as
authority. A returned signature/content observation or prior report is not an
input credential. No public deadline token, fd/key/path selector, callback or
authentication flag is introduced.

The production planner remains unchanged. Its `bw_start` has a five-second wall
and two-second CPU timer, `bw_write` has no phase clock/cancellation, and
`bw_confine` closes descriptors 3 and above and locks an empty whitelist. Reusing
them unchanged would destroy the new inherited inputs/reservation or miss the
adopted limits. Relaxing that qualified planner path is outside this continuation.

The fixed inherited roles remain definition0, review1, diagnostic2, payload3 and
signature4. The policy/key is compiled trusted data. Before reading, the complete
entry must reject elevated/set-ID execution, own the original signal flag and
hard timers, reserve only its private scratch, close unrelated descriptors while
preserving the reservation, and lock the exact reviewed OpenBSD whitelist. Capture
must retain its original input/output alias and metadata checks and consume each
owned input once. A descriptor is never reopened from a project filename.

The entry must carry one private original clock through reserve/capture/prepare.
Current capture starts a fresh two-second clock; the future entry must resolve
that gap rather than extend the reservation's conservative two-second allowance.
The overall fifteen-second hard wall, four-second CPU, eleven-second dependent
work stop and four-second cleanup reserve remain unchanged. The post-signature
two-second hash/inspection/render/output phase is shared, never renewed at an API
return. Exact clock/cancellation integration remains unimplemented.

## Closed output design and measured fit

A complete unexecuted candidate adds a closed fixed-fd1 sibling to the memory
entry. Both call one private core with destination mode fixed by their respective
compiled wrappers. The stdout sibling accepts only its own genuine RESERVED
material, original immutable snapshot and initially empty observation. It performs
its own preparation, signature stage, real binding/hash/parser/full inspector and
fixed rendering. It consumes no supplied report, prior verdict or deadline.

The trusted caller must retain exclusive ownership of fd1 and the original input
alias checks. Before preparation, the sibling requires writable `O_NONBLOCK` fd1
and a regular file, FIFO or terminal; sockets, block devices, directories and other
character devices refuse. It neither changes fd1 flags nor closes it. It writes
directly from private scratch, checking the same private phase/original clocks
and cancellation before writes and after complete publication. EINTR returns to
those checks; short writes advance only by the accepted count. EAGAIN, zero,
oversized return or other error stops without a fallback or fresh allowance.

Private observations retain whether a write was attempted and its known positive
byte count. Any later failure is UNCERTAIN after an attempt, including a zero-count
refusal. A late clock/cancellation failure can follow a complete external byte
sequence: memory clearing cannot retract those bytes. Counts prove neither remote
delivery nor durability. COMPLETE requires all bytes accepted and the final live
check; callers must also observe successful process completion. The report's
eighteen lines and zero-operation statements remain unchanged.

That complete candidate totals **81,747/81,920** whole-source bytes but
**12,771/11,264** fully charged fused bytes, an excess of **1,507**. All shared
headers are included; the other22 source files match PR #85. It is preserved
uncompiled and unexecuted, with independent fit review. It is not integrated or
adopted. No check/report is removed, no implementation moved out of accounting,
and no budget increase is selected to make it fit.

## Complete bounded prerequisite: process guard

The fitting independent slice is internal `bw_process_guard(void)`, unlinked from
the CLI. It establishes process ceilings and required signal/timer dispositions
only. Its return is neither authentication, sufficient-resource, fd ownership nor
confinement authority. It does not reserve, capture, close descriptors, write a
report, install cancellation handlers, lock unveil, fork or execute anything.
The caller must be a dedicated fresh single-threaded process with no children or
concurrent signal/timer owner. Any failure is terminal for that process; partial
setup is irreversible and must not be restored or retried into a continued entry.

1. The fixed native adapter must support OpenBSD and reject set-ID state. Reject
   root or mismatched real/effective uid/gid before process mutations. The normal
   Mac adapter refuses before rlimit, signal or timer changes.
2. Latch this guard once. Observe both REAL and PROF timers. Require zero value
   and interval; unavailable, invalid or active inherited timers refuse without
   resetting them. This conservative entry does not lengthen an inherited deadline.
3. Install fixed fatal handlers for ALRM, PROF, XCPU and XFSZ. They terminate with
   fixed status4, or5 for file-size exhaustion; they perform no cleanup/output.
   Ignore PIPE and establish default CHLD with zero flags/mask. Check every
   signal-set/disposition operation and prepare exactly those six signals for
   unblocking after the resource ceilings are applied.
4. Apply CPU4, DATA64MiB, STACK2MiB, NOFILE32, CORE0 and FSIZE65536. Clamp each soft
   and hard ceiling down to the smaller inherited value; never raise either.
   Preserve the existing verifier adapter's nonzero checks for its first four
   resources and permit zero CORE/FSIZE. Check set results and exact readback of
   both bounds, then unblock the prepared six-signal set. Success does not promise that a lower inherited allowance is
   sufficient for later work; native operation may still refuse or terminate.
5. Arm nonrepeating REAL15 and PROF4 timers, then read each back. Require a
   positive valid remaining value no greater than its cap and zero interval.
   Setup failure or invalid/absent readback refuses. No timer is reset afterward.

The fixed six-resource clamp is shared privately with the existing signature
platform adapter. Its kind order, child caps2/32MiB/2MiB/16/0/0, nonzero checks,
lower-bound preservation, failure order and readback behavior remain unchanged.
The helper has no exported symbol or runtime policy selector. Charge its complete
header to both consumers. Planner, capture and fused source remain unchanged.

The complete guard candidate totals **81,920/81,920** whole-source bytes. Its C,
header and fully charged shared helper total3,239 bytes; the signature component
including that helper totals16,245/18,432. The fused component remains10,332/11,264
and earlier limits remain unchanged. No new component budget is invented. There
is no whole-source headroom for further implementation. Full output combined with
this guard would exceed the whole ceiling by2,439 bytes as well as the fused cap.

Guard locals and fixed tables are small and sequential with the later pipeline;
they fit within the existing conservative8KiB local allowance and2MiB application
accounting. No duplicate snapshot, allocator, worker, new external dependency or
new runtime allowance is introduced. This is source accounting, not native RSS,
hard-timer performance or confinement qualification.

## Behavioral Test Design

Use the existing finite owned launcher and replaced environment. Test-only static
syscall substitution models limits, signal installation and timers; it must not
change real Mac security/resource settings. A separately linked normal adapter
must refuse without process mutations. Freeze expected fixed caps, handler/mask
roles and refusal observations independently before candidate execution.

| Stimulus | Independent required observation |
| --- | --- |
| Supported fresh ordinary identity | Exact six caps, four fatal handlers, ignored PIPE/default CHLD, exact unblocked set, REAL15/PROF4 nonrepeating timers; guard succeeds only after verified readback |
| Unsupported/set-ID/root or uid/gid mismatch | Refusal before process mutation; no namespace, input, output, child or confinement action |
| Active/periodic/invalid/unavailable inherited REAL or PROF | Refusal before signal/limit/timer mutation; inherited timer not reset |
| Lower finite soft/hard bounds | Never raised; zero first-four bound refuses, zero CORE/FSIZE preserved; infinity is clamped |
| Every resource observation/set/readback failure or mismatched bound | Refusal with exact already-attempted prefix retained; no restoration or later stage |
| Every signal-set, disposition or unblock failure | Refusal, no later stage; earlier setup reported as partial, never rolled back |
| Either timer arm/readback failure; zero, invalid, over-cap or periodic readback | Refusal without retry, reset or claimed guard success |
| Every fatal handler | Fixed exit classification, no diagnostic/cleanup action |
| Reuse after a latched attempt | No further signal, resource or timer mutation; no deadline renewal |
| Production symbols and full source accounting | Only closed guard exported; no fd/path/write/fork/exec/unveil dependency; shared source charged fully; planner/CLI and all old regressions preserved |

Independent source/fixture/envelope review precedes modeled execution. Focused
results, full clean-candidate `make verify`, trusted signatures/DCO, independent
terminal/source/canonical binding and normal PR gates precede eligible merge.
Reuse existing evidence custody; do not create a new validation framework.

## Remaining integration and native gates

This guard does not establish one coherent private startup/reservation/capture
origin or cancellation lifetime. It does not choose the smallest exact native
loader whitelist or prove locked unveil removal, actual signify fd handoff,
hard timers, resource sufficiency or serial/plain output. Those remain explicit
engineering/native gates. Preserve all consumed fixture authority and research.
No new guest, key, download, security-setting change or deployment is requested.
