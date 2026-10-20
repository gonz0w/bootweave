# Trusted entry and output integration

Status: source-only engineering continuation of issue #5 after PR #85,
2026-10-20. This refines the adopted [read-only scope](authenticated-plan.md),
[fixed I/O](authenticated-io.md), [material ownership](verifier-material.md) and
[fused review](authenticated-composition.md). Independent design/source review
precedes candidate execution. No new CLI, publisher policy, extraction, native
campaign, runtime/security activation or deployment is adopted here.

The owner subsequently approved the complete source-budget package on 2026-10-20:
96 KiB whole source, 16 KiB authenticated review/output, 10 KiB trusted entry and 8 KiB
capture. All other component and runtime ceilings remain unchanged. These limits
measure source text, not storage, resource sufficiency or native qualification.
Count every runtime C/header regardless of directory, shared headers once in the
whole tree and in full in every using component. No minification or hidden source.
The first complete implementation under this package is the closed, unlinked
fixed-fd1 output sibling below. Reservation-bound capture and the closed
startup/reservation lifetime below continue that direction; complete input
ownership and native confinement remain separate integration obligations.

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
The closed reservation-bound capture sibling now retains material's original
two-second clock and cancellation flag, including final close checks. Legacy
capture retains its fresh-clock behavior. The new sibling's latch and failure
poisoning prevent repeat input closure or preparation from failed capture; see
the [capture contract](authenticated-io.md#reservation-bound-capture-continuity).
The closed startup/reservation sibling now supplies that clock from immediately
before the first hard-wall arm and owns its permanent signal flag. Calling legacy
guard and reserve separately does not establish that relationship. Neither path
extends the reservation's conservative allowance.
The overall fifteen-second hard wall, four-second CPU, eleven-second dependent
work stop and four-second cleanup reserve remain unchanged. The post-signature
two-second hash/inspection/render/output phase is shared, never renewed at an API
return. Complete entry ownership/confinement and native qualification remain gated.

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

Before this approval, the complete candidate totaled **81,747/81,920** whole-source bytes but
**12,771/11,264** fully charged fused bytes, an excess of **1,507**. All shared
headers are included; the other22 source files match PR #85. It is preserved
uncompiled and unexecuted in the historical fit snapshot, with independent fit
review. That refusal remains preserved. The two-file output implementation is
now integrated onto PR86's process-guard source under the explicitly approved
96/16 KiB source limits. No check/report is removed or source hidden. The old
memory entry retains its exact report and refusal behavior. Both public siblings
select their destination by compiled wrapper constants; no caller flag is added.

## Legacy bounded prerequisite: process guard

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
was no whole-source headroom under the previous 80 KiB ceiling. Full output combined
with this guard exceeded that ceiling by2,439 bytes as well as the previous fused
cap. Those historical measurements do not represent the newly approved limits.

Guard locals and fixed tables are small and sequential with the later pipeline;
they fit within the existing conservative8KiB local allowance and2MiB application
accounting. No duplicate snapshot, allocator, worker, new external dependency or
new runtime allowance is introduced. This is source accounting, not native RSS,
hard-timer performance or confinement qualification.

## Closed startup and reservation lifetime

`bw_start_reserve(material)` accepts only a valid exclusive EMPTY material object.
It accepts no clock, cancellation pointer, token, path, descriptor selector or
callback. Invalid/null/nonempty storage refuses before the guard latch or effects.
The same private once-only guard core serves this compiled wrapper and legacy
`bw_process_guard`; calling either after a latched supported attempt cannot renew
timers or start another reservation. The legacy wrapper's existing six-signal,
resource and timer observations stay unchanged.

After identity and inactive inherited-timer checks, the new path builds the fixed
INT/TERM/HUP set and blocks it before installing handlers. Those three handlers
have zero flags, block all three cancellation signals during the handler, and
only assign one to a private static volatile sig_atomic_t. The flag is initially
zero by static initialization and is never reset. Preserve the existing four
fatal handlers, ignored PIPE and default CHLD with their empty handler masks.
After the unchanged six resource clamps/readbacks, unblock exactly these nine
signals. A pending cancellation delivered on unblocking refuses before sampling
the origin or arming a timer. Check cancellation before each arm, after the origin
sample and after the final readback. A cancellation can always arrive after a
check; the same flag remains live in every later component.

Sample one normalized CLOCK_MONOTONIC origin immediately before the first REAL15
arm. Clock failure or invalid fields refuses before that arm. The original REAL15
and PROF4 nonrepeating arms/readbacks remain unchanged and are never reset. The
closed wrapper passes its exact private origin and permanent flag through a
private material implementation bridge to the existing reserve core. That bridge
is declared only in private_start_material.h, never the public material API, and
accepts only trusted implementation-owned seed storage. It is not a caller
deadline interface. The core copies the seed without a replacement origin stamp;
its ordinary live observations charge all time since the pre-arm sample to the
same conservative two-second reserve/capture/prepare window. Legacy
`bw_verifier_reserve` still takes its own origin and caller flag.

| Stage | Clock and cancellation lifetime | Ownership and failure |
| --- | --- | --- |
| Guard setup/arms | Private pre-REAL origin and permanent INT/TERM/HUP flag; original hard15/CPU4 limits | No descriptor or namespace takeover; any partial guard failure is terminal and leaves material untouched; do not restore/retry setup |
| Reservation | Exact pre-arm origin, original2 window, same flag before namespace work | Only the exact private scratch directory; reservation failure performs its existing once-only cleanup and retains uncertainty; fixed inputs remain caller-owned |
| Reserved capture and prepare | Same origin/flag and joint2 window, never renewed | Existing input latch, once-only0/3/4 consumption, poisoned preparation, first cause and unknown-close uncertainty remain unchanged |
| Signature work | Its own5 phase cap AND original11; same parent flag | No channels/fork/grant after a failed live boundary; sole READY transfer/custody rules remain unchanged |
| Signature cleanup | Its own4 cleanup cap AND original15; same parent flag | Preserve first cause and independent fixed finalizers; lost wait, failed stop and unknown closure remain uncertainty; known complete but late cleanup is refusal |
| Content and publication | Original11 and the existing shared2 content/render/output phase; same flag | Late failure clears private publication; any external write attempt preserves UNCERTAIN; no new allowance on return |

The signature helper samples once per existing clock boundary and compares that
sample against both relevant origins. It rejects invalid fields, samples before
either origin and exact expired boundaries. These original11/15 ceilings apply
consistently to all genuine material, including legacy reservation; a delayed or
jumped signature start cannot renew dependent work. There is no entry-mode bit.
Capture/material clocks remain origin-relative, fused review retains its existing
last sample, and signature remains phase/original-relative. This does not claim
a global nondecreasing chronology across every syscall. After fork the parent's
flag storage is copied, not shared; the child keeps its existing independent hard
ALRM5 and the parent retains sole cancellation/custody responsibility.

Release is a cleanup observation, not entry success or authorization. Fixed
cleanup remains bounded by the known objects and the hard process lifetime;
neither a successful reserve nor a known release proves complete entry execution,
confinement or native resource sufficiency. Failed close/identity observations
must not be erased by a later known cleanup or converted into retry instructions.

Before execution the complete source is90,851/98,304 bytes. Entry C/header plus
the entire private_limits.h and private_start_material.h is5,743/10,240; material
plus the entire bridge header is16,712/18,432; signature plus its entire shared
limit helper is16,672/18,432. Capture remains8,093/8,192 and authenticated
review/output12,782/16,384. Count the bridge once in whole source and fully in both
consumers. All other runtime/component ceilings remain unchanged. A compile-time
8KiB guard workspace bound and existing sequential application accounting cover
the small private origin/flag and local masks; this is not measured native RSS.

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

For the newly integrated output sibling, independently freeze all 18 exact report
lines and test short writes, EINTR, EAGAIN, zero/oversized/error returns, every
original clock/cancellation boundary and final completion check. Literal tests
must verify that no output can precede this call's own signature/content acceptance
and that any refusal after a write attempt is UNCERTAIN. Sink preflight must refuse
before preparation, preserve fd1 flags/ownership and accept only the specified
regular/FIFO/terminal objects. Test the old memory API and all alias/reuse cases
unchanged. Model writes and descriptor/syscall failures; no live listener, native
verifier, real credential or host resource/security mutation is permitted.

For startup lifetime, freeze new literal oracles before reading implementation:

| ID | Required independent observation |
| --- | --- |
| SL-T01 | Exact private pre-REAL origin survives guard/reserve/capture/prepare; consumed guard time is charged; no replacement reserve origin, no timer renewal |
| SL-T02 | INT/TERM/HUP installed while blocked, exact handler masks/flags and nine-signal unblock; pending cancellation and each later phase use the same flag, which never resets on refusal/reuse |
| SL-T03 | Every new signal-set/block/action/unblock and origin-clock failure retains only its attempted prefix; terminal guard failure leaves material/inputs untouched; normal unsupported adapter changes no host state |
| SL-T04 | Exact original2 boundary before namespace, capture and prepare refuses; known finalizers still run, no input/scratch double close or erased uncertainty |
| SL-T05 | Signature jump before its own phase start still refuses at original11; own5/original11 and cleanup4/original15 exact/near boundaries are independently distinguished |
| SL-T06 | Cancellation and invalid/backward-to-origin clocks at work/cleanup boundaries retain first cause; unknown close/lostwait/failedstop remain uncertainty independently of lateness |
| SL-T07 | Empty/nonempty/null/reused material and cross-wrapper reuse cannot clear flag, renew guard or close rebound descriptors; legacy149 guard event oracles remain unchanged |
| SL-T08 | Public entry has no caller seed selector; private bridge fully charged to both consumers, normal symbols/link boundaries and unchanged planner/capture/fused regressions verified |

Use the unchanged owned finite launcher with modeled guard mutators and inert
records. Real private inputs/material/pipes are permitted; any modeled verifier
child must stay explicitly synthetic, with no actual fork/kill or fictional born
PID claim. No signer, credential, native verifier or real host setting mutation.

Independent source/fixture/envelope review precedes modeled execution. Focused
results, full clean-candidate `make verify`, trusted signatures/DCO, independent
terminal/source/canonical binding and normal PR gates precede eligible merge.
Reuse existing evidence custody; do not create a new validation framework.

## Remaining integration and native gates

The closed sibling establishes startup/reservation/capture origin and cancellation
continuity only. It does not choose the smallest exact native
loader whitelist or prove locked unveil removal, actual signify fd handoff,
hard timers, resource sufficiency or serial/plain output. Those remain explicit
engineering/native gates. Preserve all consumed fixture authority and research.
No new guest, key, download, security-setting change or deployment is requested.

Complete entry must adopt this closed lifetime and finish all pre-signature
failure ownership:
capture consumes inputs 0/3/4, preparation failure consumes material, and signature
entry transfers sole material/process cleanup. No public deadline token, caller
authentication flag or complete-entry success is implemented by these closed slices.
