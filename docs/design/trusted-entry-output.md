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
This lifetime begins with the installed private handlers. A signal already
discarded by an inherited disposition before installation cannot be reconstructed.

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
Before successful work becomes cleanup, validate the existing cleanup-start
sample against work5/original11 too. A final WNOWAIT observation cannot cross a
work deadline and silently become a fresh cleanup phase. That check adds no
clock sample and never skips cleanup; cancellation is checked before its bounds.
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

Before execution the complete source is91,341/98,304 bytes. Entry C/header plus
the entire private_limits.h and private_start_material.h is5,743/10,240; material
plus the entire bridge header is16,712/18,432; signature plus its entire shared
limit helper is17,162/18,432. Capture remains8,093/8,192 and authenticated
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
| Production symbols and full source accounting | Guard and closed startup/reservation wrappers only; exact private material bridge dependency, no caller seed selector, shared source charged fully; planner/CLI and all old regressions preserved |

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
| SL-T05 | Signature jump before its own phase start and final observer completion still refuse at exact work boundaries; own5/original11 and cleanup4/original15 exact/just-below controls are independently distinguished |
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

## Closed complete-entry ownership coordinator

The next source-only continuation adopts, in the existing trusted-start module, a closed `bw_entry_output(void)` and a
const private-observation accessor, `bw_entry_observe(void)`. No caller supplies
storage, material, snapshot, clock, cancellation flag, descriptor, path, callback,
prior report or authentication assertion. Private static snapshot/material storage
keeps the existing application-storage ceiling; observations grant no operation,
retry or cleanup authority. The coordinator reuses `bw_authenticated_result`: its
existing NOT_TAKEN/UNCERTAIN/REJECTED/COMPLETE meanings cover this full fixed-output
lifetime. Entry state/cause remain distinct; no duplicate result domain is added. A dedicated fresh single-threaded caller exclusively
transfers its original fixed input roles0/3/4 on the first taken attempt; their
open-file descriptions must not be concurrently rebound, advanced or modified.
No output or ordinary diagnostic is written before alias/authentication checks.

Before taking inputs or changing the observation, refuse NOT_TAKEN if this entry
was already attempted or the shared private process guard was latched by another
wrapper. This protects scratch/material owned by a prior guard/reserve caller.
Otherwise latch the entry WORKING and its original input ownership before guard
setup. Unsupported, identity, inherited-timer and partial-setup guard refusal is
terminal after independently closing those original inputs once. It performs no
later confinement/final-pledge transition. This entry consumes its process lifetime
even when the unsupported/identity branch did not previously latch the guard.
The legacy guard and startup/reservation interfaces retain their own behavior.

After successful original-clock/private-flag guard setup, observe all five fixed
roles0..4 with F_GETFD and require each present before any namespace allocation.
Only0/3/4 transfer ownership; fd1/2 and their flags are preserved.
Missing/unobservable roles refuse and all original input close attempts still run
once; no replacement input is opened. Next call the fixed native closefrom adapter
with lower bound5, preserving roles0..4 and closing unrelated inherited descriptors
including numbers above the new NOFILE ceiling. This precedes reservation, avoiding
any scratch-fd exclusion or closefrom hole. A failed closefrom observation stops
before reservation and separately records uncertain unrelated closure. Individual
native close guarantees remain a qualification gate, not a portable proof.

Reserve with the exact private pre-first-REAL-arm origin and permanent flag, using
the existing private implementation bridge. Fixed-role presence checks and unrelated closure
consume the original two-second allowance; they cannot start another clock or
renew either hard timer. Check the same private flag and normalized original2 clock
before every F_GETFD, before and after closefrom, before the native boundary
and before authenticated-output handoff. Cancellation, invalid/backward clocks
and exact expiry refuse as separate entry causes before dependent work. Reserve failure has already consumed its own attempted
material cleanup and must not be released again by the coordinator.

The fixed private entry boundary adapter takes only the genuine reservation for a
future native whitelist. In this slice its normal implementation always refuses,
on every platform, and performs no security operation. Tests separately link an
inert adapter. This intentional refusal prevents normal capture/signify/output
before the exact locked whitelist is adopted and natively qualified. There is no
fallback, caller bypass, production test switch or CLI wiring.

After a test-modeled successful boundary, call real reservation-bound capture.
NOT_TAKEN retains entry input ownership; every taken capture return transfers all
three input closes permanently to capture, including rejected/uncertain returns.
Never close the role numbers again, even if a later allocation/rebind reuses them.
Capture does not own scratch: failure requires one independent RESERVED release.
A successful snapshot then goes directly to the real closed authenticated-output
sibling. Its NOT_TAKEN leaves material with the coordinator for one release; every
ordinary taken return transfers all material and final-boundary responsibility.
Preparation failure already consumed material; signature entry owns its sole
process/material cleanup. The coordinator must never release or reduce again.

On an earlier controlled failure, independently attempt every still-owned original
input close, known RESERVED release and, only after successful guard and while
still owned, final parent reduction once. EBADF is known absence but an invalid
close observation; other close failures record unknown closure and UNCERTAIN.
Latch each attempted role before closing; never retry a number. Preserve first
entry cause, separate capture/material close masks, uncertainty and final-boundary
failure. Entry input-close masks describe the sole closer: NOT_TAKEN
(including capture not called) retains entry ownership; a taken capture result
transfers it to capture. Copy capture's role0/1/2 masks after takeover. Material
state/cause/close masks remain in the original private object, not duplicated in
the entry view; test-only bridge witnesses observe that actual object. A later cleanup success cannot erase a prior unknown close, failed stop
or lost wait. No path after final reduction performs an input, namespace, process
or output operation; only bounded private observation/copy/clearing remains.

COMPLETE requires this call's real fused-output COMPLETE, never an observed prior
report. Preserve its same original11 work stop, shared post-signature two-second
phase, cancellation and post-write uncertainty. No API return creates another
allowance. The normal boundary currently makes COMPLETE unreachable. The snapshot
is cleared on controlled return; no raw bytes or private scratch path are exposed
by the const accessor. Refused repeats leave terminal observation, rebound input
canaries and every boundary unchanged. Existing149 guard oracles and all legacy
capture/material/fused/planner regressions remain mandatory.

| ID | Independent required observation |
| --- | --- |
| EO-T01 | First taken call owns original0/3/4; guard precedes all five presence checks, closefrom5 precedes reserve, reserve precedes boundary/capture; original origin/flag and hard timers never renew. |
| EO-T02 | Unsupported/identity/active-timer/partial-guard failures close original inputs once with no later boundary; prior latched legacy guard/start refuses entry with no takeover/effects. |
| EO-T03 | Missing/query-failed fixed roles0..4 stop before closefrom/reserve; high unrelated descriptors use exact fixed closefrom5; refusal is uncertain and cannot reserve. No replacement role or caller-selected descriptor. |
| EO-T04 | Every reserve failure/uncertain cleanup is consumed once; normal boundary always refuses before reads/verifier/output, then independent input/material cleanup and one eligible final reduction. |
| EO-T05 | Capture NOT_TAKEN versus taken success/refusal/uncertainty transfers input closes correctly; rebound role canaries survive all subsequent material/signature cleanup and repeated entry. |
| EO-T06 | Real capture/prepare/fused signature/content/output use exact retained inputs, compiled policy and same origin/flag. Preparation failure, signature rejection/stop/wait/close uncertainty, policy/content/output failures preserve sole cleanup ownership and no premature publication. |
| EO-T07 | Entry/capture/material close EBADF versus unknown failure, earlier primary error, failed final reduction and combined failures preserve independent attempts, masks and uncertainty without retry. |
| EO-T08 | Shared exact/just-below deadlines, cancellation before/after handoffs, original final completion checks, partial/zero/late output and reused observation cannot establish false COMPLETE or renew a phase. |
| EO-T09 | Public symbols have no caller selector; normal boundary is an inert refusal; full helper/whole/component accounting stays within approved caps and production planner/CLI/native sources retain their qualified behavior. |

Independent literal oracles are frozen before new coordinator implementation
inspection. Reuse the existing finite owned portable launcher and evidence custody;
new fixtures use inert records, modeled guard/security/process adapters and owned
private files only. No actual new child verifier, signer, native activation or
security mutation is authorized. Review source/fixtures/envelope before execution,
then focused/full exact clean-source checks, independent terminal review, genuine
source signatures/DCO, exact squash preflight and actual canonical verification.

### Measured coordinator source fit

The unexecuted current coordinator totals **95,788/98,304** runtime C/header
bytes. Trusted entry is **10,190/10,240**, including trusted_start.c/.h,
private_limits.h, the entire private_start_material.h and entry_platform.c.
The entire bridge is also charged to material: **16,610/18,432**. Signature
remains **17,162/18,432**, capture **8,093/8,192** and review/output
**12,782/16,384**; all other runtime files and ceilings are unchanged.
No implementation is excluded by directory, helper sharing or declaration naming.

The initial unexecuted draft exceeded the entry cap and remains preserved outside
the repository. The fitting source consolidates startup interfaces and the result
domain, references the normative contract instead of duplicating its prose, and
keeps material observations in their owning private object. No check, failure
record, finalizer, legacy oracle or runtime ceiling is removed. Entry has only
50 source bytes of headroom; further native-boundary implementation requires
another measured fit review under the same approved limits, not an assumed
increase. The exact native whitelist remains unadopted and unqualified.

The compile-time application bound includes the private static snapshot,
material and entry observation plus131,072 bytes for all remaining bounded
component workspace. This conservatively covers the existing fused, material,
signature, capture, guard, parser and inspector workspace bounds; it adds no
runtime allowance and makes no RSS or hard syscall-latency claim.

This source coordinator completes the ownership composition only. Activation
must adopt it together with the exact native boundary and inherited fd1 contract.
It does not require a second ownership implementation. The remaining native/CLI
gates below still apply, and the original planner remains unchanged.

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
