# Minimal fused authenticated review contract and fit gate

Status: **Approved source-only engineering continuation**, 2026-10-20.
Source basis 72592b7f608100e1a4b60476d5d078cbbed504dc. Refines the adopted
[read-only intent](authenticated-plan.md), [original I/O budgets](authenticated-io.md),
[material ownership](verifier-material.md), [fixed signature stage](signature-verifier.md)
and [unauthenticated content review](content-review.md). No CLI, new policy/key
selection, output descriptor, extraction, privilege, native or deployment authority.

The owner approved the scoped **81,920-byte whole-source and 11,264-byte fused
component ceilings** on 2026-10-20 after the preserved failed-fit assessment. The
complete proposed common source is 79,308 bytes, leaving 2,612 bytes whole and
932 bytes in this fully charged component. These are source-text review budgets;
all runtime, workspace, I/O and earlier component ceilings remain unchanged.
Independent exact source/fixture review and bounded checks precede adoption.
Never shorten a report, remove a check, hide source or create a flag/token stub
to meet a budget. Preserve the original failed-fit candidate and evidence.

The later approved complete entry package supersedes those source-only ceilings:
96 KiB whole and 16 KiB fully charged authenticated review/output, with 10 KiB trusted
entry and 8 KiB capture. Earlier component and all runtime/workspace/I/O limits stay
unchanged. The closed fixed-fd1 sibling is specified in
[trusted entry/output](trusted-entry-output.md); the memory API below retains its
original behavior. Historical 80/11 KiB candidate measurements remain evidence.

## Closed entry, input and output

Propose one unlinked internal `bw_authenticated_review(material, snapshot, text,
capacity, observation)` entry. Inputs are a genuine exclusive RESERVED material
capability created by real trusted startup before capture/locked unveil; one
immutable complete bw_auth_snapshot; disjoint caller output/result; and the
original cancellation flag held by material. No READY material, prior signature
or content result, verifier path, alternate key, callback or acceptance boolean.
The result is initially zero/EMPTY and one-shot: EMPTY → WORKING → FINISHED.
Return distinguishes NOT_TAKEN, REJECTED, UNCERTAIN and COMPLETE. COMPLETE is
relative identity/content review only, never a future write capability.

Observation contains fixed cause, material cause, private bounded signature
observation and completed content model/counts/length. Failure retains only
state/fixed causes/private signature uncertainty; plan/counts/text length are zero.
Signature PIDs/close observations never grant retry/signal/reclose authority.
No raw verifier bytes, key, signature or archive member data enters output.

Before reading members, establish mapped numeric ranges for material, full
snapshot, result, output and compiled policy; require pairwise disjointness.
Then establish the original cancellation-flag span and all 15 disjoint pairs.
Overflow, positive-length NULL, alias, NULL required object, non-EMPTY result or
non-RESERVED material returns NOT_TAKEN without writes or resource effects.
Caller retains its original capability on NOT_TAKEN and must perform its own
once-only release; this result never transfers ownership or revives stale state.
As in current APIs, caller supplies mapped storage, immutable/exclusive lifetime,
single-threaded use and no reentry; only the signal flag may change asynchronously.

After legitimate RESERVED ownership is taken, latch WORKING before fallible work.
Clear bounded outputs/model. Capacity 1..65536 includes NUL; invalid capacity is
ordinary rejection, clears result but leaves that invalid-capacity output untouched.
Input lengths remain nonzero definition/signature <= 16384 and payload <= 1048576.
All ordinary exits consume the legitimate reservation exactly once. Invalid input
never leaks a taken reservation merely because no verifier was launched.
NULL text with zero capacity passes numeric preflight and is an ordinary taken
ARGUMENT refusal (clear result, leave text untouched, consume reservation).
NULL text with positive capacity is NOT_TAKEN with no writes or resource effects.

## Closed provenance and cleanup sequence

1. Validate the compiled profile and key length before copying. Make one private
   disjoint <=1024-byte copy directly from bw_compiled_policy. No caller key copy
   or duplicate payload snapshot. Preserve original retained definition/signature.
2. Call real bw_verifier_prepare with that key and the original snapshot spans.
   It performs original anonymous readback/namespace removal and consumes on
   failure. Before signature entry, release a remaining RESERVED/READY capability
   once on cancellation/budget/ordinary failure; preserve uncertainty and first cause.
3. Only its own successful preparation may reach real bw_signature_verify with
   an internally zeroed private signature observation. Never substitute a supplied
   result. That common function owns material, group/gate/pipes/stop/wait/close and
   final stdio reduction. Its acceptance plus known cleanup is required.
4. After signature return, never release material again or regain process/path
   promises. Refusal/uncertainty stops dependent work. On success, call the real
   bw_content_review using the same key copy, original definition and payload
   pointer/length. Its unauthenticated text is private intermediate scratch only.
   Its real binder/hash/parser/full inspector is recomputed; no cached flags.
5. A private static formatter renders from that newly completed model after
   this call's signature acceptance. Keep the existing unauthenticated entry/API
   and exact text unchanged. No exported renderer accepts asserted authentication.
6. Check original budget/cancellation, copy complete text/model, check again, and
   clear the complete bounded publication on a late refusal before returning.
   Exclusive output is consumed only after COMPLETE. No partial/stale result.

Preparation failure is terminal and already consumes material; never release it
again. Preparation success transfers READY material back to this fused call,
which releases it once if signature entry is skipped. On signature entry, hand
sole ownership to bw_signature_verify and never release material afterward.
Do not retry unknown closes or use an observed PID to recover authority. Any
preparation quarantine, lost wait, failed stop/boundary, unknown close or incomplete
cleanup prevents COMPLETE. Keep primary failure separate from cleanup uncertainty.

For every taken-reservation path that never enters signature verification, this
fused call attempts the existing fixed bw_sv_parent_boundary(1) only after its
once-only material cleanup. It records a separate final-boundary-failed flag,
preserving the primary failure and any cleanup uncertainty. No path/process action
or cleanup retry follows that final reduction, even if cleanup is quarantined or
the reduction fails. Once signature verification is entered, that function owns
its final reduction. NOT_TAKEN leaves both capability and boundary responsibility
with the caller. The normal unsupported Mac adapter always refuses and proves no
native confinement. Tests must cover early final reduction, its failure, and
cleanup-before-reduction ordering separately from signature-stage reduction.

## Frozen authenticated report

Exactly 18 LF-terminated lines, no blank line. Reuse the content report's exact
field order, labels, model and three final zero-operation lines, changing only:

```text
AUTHENTICATED CONTENT REVIEW - NO OPERATIONS AUTHORIZED
...
Signature authentication: accepted by fixed verifier
```

The ellipsis above is explanatory, never output: lines 2–5 and 7–18 are exactly the
existing independently frozen REPORT_TEMPLATE in tests/content_vectors.py.
Line 1 and line 6 are the two literals above. The key/pins are compiled media trust,
not freshness, revocation, semantic safety, a selected target or write authority.
Tests use a modeled fixed exec double; their COMPLETE report is not evidence that
the real native verifier accepted a cryptographic signature.

## Original deadlines, storage and source fit

Use material.started as the original reserve/startup origin; never renew it.
The closed [startup sibling](trusted-entry-output.md#closed-startup-and-reservation-lifetime)
supplies its pre-first-REAL-arm origin and permanent private cancellation flag.
Signature work/terminal checks now also enforce original11/15 alongside phase5/4;
they cannot renew the lifetime after a clock jump between component boundaries.
Reserve→capture→prepare retains the existing conservative 2-second material window.
Require cancellation/valid nonbackward monotonic clock before/after dependent
stages. Signature work 5s/cleanup 4s starts only inside that original 2s window.
All dependent work stops before original 15s minus 4s cleanup reserve (11s).
The pure hash/inspection/render/publication phase has its own 2s ceiling starting
after signature, bounded also by original 11s. Invalid/backward clock or late
cancellation clears publication. No stage sum or fresh clock extends the budget.
This API publishes memory only. Eventual descriptor output must share this same
original rendering/output phase deadline and remaining allowance; it cannot start
a fresh two-second write window after return. Carrying that private deadline
through the eventual trusted entry remains a CLI integration gate. Do not add a
deadline token or new output API in this source-fit slice.
The eventual trusted entry still installs the hard 15s/CPU/memory/fd/output limits
and exact locked whitelist; this unlinked source does not prove native confinement.

No duplicate 1 MiB snapshot, allocator or new OS adapter. One key copy, private
<=2048-byte report scratch and fixed state only. Compile-time account nested
material 8 KiB/signature 8 KiB/content 4 KiB/policy 2 KiB/inspector 34 KiB allowances plus
snapshot/output, retained planner 24 KiB and new locals within unchanged 2 MiB
application storage. Bound new locals, nested preflight and scalar allowance at
8 KiB; bound C/header plus both private shared headers at 16 KiB independently
of the approved 96 KiB whole-source ceiling. Parent and
child native limits remain unchanged and unqualified.

Permitted fit investigation: extract unchanged numeric range/disjoint helpers
into a private static header shared by existing policy/content/new entry, and
the unchanged formatter into a private static header shared only by the two
closed entries. Only internal call flow selects fixed authenticated literals;
each translation unit selects its two report literals at compile time, without
a runtime authentication flag or caller-provided string. There is no externally
linked generic renderer. Charge every shared header in
full to every using component as well as once to whole-source accounting.
Keep every old guard, cause/zeroing/no-write classification and report byte.
Measure the full proposed delta before product source execution; incomplete
stubs or removing safety behavior cannot constitute a fit result.

## Independent portable checks

Independent vectors freeze exact authenticated viewer/DNS/firewall reports;
all 15 new aliases/overflow/NULL/cap/state/refusal ownership cases; prepared-fd
key/definition/signature bytes and original hash/inspection spans; changed bytes
at each phase; wrong pin/tuple/key, malformed archive and late format failure;
original 2/11/2s clock/cancellation boundaries; unsupported normal Mac adapter;
same-object reuse, once-only cleanup and complete late-publication clearing.
Use real common preparation/signature orchestration plus a fixed statically linked
exec double and real Mac hash/parser/inspector. No signer/key generation, actual
signify, runtime callback, network or native run. Reuse the bounded reviewed owned
launcher/custody behavior; review exact finite fixture/envelope before dispatch.
All old content/policy/material/signature/CLI and source-budget checks remain.
Full clean-candidate make verify and independent source/terminal/canonical review
precede eligible merge. Source fit failure stops implementation and returns the
precise cap/refactoring tradeoff; it is not a reason to add scaffolding or a VM.
