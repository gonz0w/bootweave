# Native boundary and CLI: source-only integration

Status: finite engineering continuation after PR90, 2026-10-20. The owner approved
100 KiB whole runtime source and16 KiB fully charged entry; all other source and
all runtime/resource/timer limits remain unchanged. This approves source and
portable models only. No native/security activation or unqualified CLI link/run.

## Closed architecture and qualification barrier

Keep production PLAN_SOURCES unchanged. The normal bw_entry_boundary remains an
unconditional refusal on every platform. A new fixed bw_entry_available returns0
on every platform, with no environment, argument, file or runtime override.
The candidate native boundary and candidate dispatcher remain outside that link.
Readiness requires a later reviewed source change after exact native qualification;
a test linker substitution is never a product qualification mechanism.

Refactor the existing main body into bw_legacy_cli with its original code and one
thin main wrapper. Its output, timers, branches, statuses and production sources
remain equivalent. Candidate bw_entry_cli delegates all invocations except exact
argc2 review to that same legacy function. For review it checks fixed availability
first: false returns4 quietly, before bw_start, bw_entry_output, output, descriptor
queries/close or security calls. True in a separate synthetic fixture calls the
closed bw_entry_output once; COMPLETE maps0, REJECTED3, UNCERTAIN5 and NOT_TAKEN or
unexpected result4. No legacy fallback after taking the review branch and no
separate diagnostic writer. Existing help/version bytes stay unchanged; review is
not advertised or exposed by the production main. The inherited0/1/2/3/4 roles,
exclusive open-file descriptions and writable O_NONBLOCK fd1 remain prerequisites.
No file-opening/media/console launcher or real publisher policy is introduced.

## Fixed candidate boundary

Add one portable orchestration body and one fixed OpenBSD syscall adapter, private
to entry. Expose the existing private entry_live body as a closed no-argument
implementation bridge; it retains the original started/flag and same failure
cause. Call it before each candidate work observation/transition and after final
success. No copied clock, caller seed, callback or fresh allowance.

The candidate accepts only its caller's genuine private RESERVED material. Before
any unveil call check supported ordinary OpenBSD identity, valid reserved shape,
known directory>=5, NONE cause, no uncertainty/close/capture-attempt masks, valid
original flag/origin, no members/writers/read handles, and the exact fixed generated
/tmp/bootweave-verify. prefix plus10 ASCII alphanumeric suffix bytes and NUL.
Observe held/named directories with fstat/lstat and require device/inode/mode/uid/
gid identity against the reservation, exact0700, same effective uid and positive
links. No unknown/replaced path can receive authority. The outer entry remains
sole cleanup/final-boundary owner; this borrowed boundary closes/removes nothing.

Unveil the exact scratch first with rwc, then only this fixed provisional profile:
/usr/bin/signify x; /usr/libexec/ld.so r; /usr/lib/libc.so.103.0 r;
/usr/lib/libutil.so.21.0 r; /dev/null r; /dev/fd/3 r; /dev/fd/4 r; /dev/fd/5 r.
Then lock unveil and apply stdio rpath wpath cpath proc exec. Every call result and
original live boundary is mandatory; failure stops dependent transitions and does
not retry/widen/restore anything. No whole /tmp, /dev/fd or library directory.

This is an unqualified source candidate for the existing7.9 amd64 contract, not
adoption of actual loader dependencies/permissions or a native whitelist. The
libc/libutil names come from historical committed evidence, not current image
inspection. No hints/config path is added speculatively. If exact locked accesses
fail qualification, stop: do not widen or use a named-file/static-verifier fallback.
Native acceptance must prove fd aliases across later3/4/5 remapping, exact scratch
removal after lock, actual loader/signify accesses and all pledge transitions.
Reuse the existing signature argv/env/mapping/gate/custody; do not implement a
second verifier. Fixed non-OpenBSD adapter refuses without any security syscall.

## Portable Test Design

Freeze independent literal syscall sequences and outcomes before implementation
inspection or candidate execution. Model only fixed platform/security calls and
private clock boundaries in separately linked test executables; no actual unveil,
pledge, rlimit, timer, credential, verifier or native fixture action in new cases.

| ID | Required observation |
| --- | --- |
| NE01 | Genuine reserved object: support/identity/shape, each original liveness check, held/named identities, scratch-first exact8-path sequence, lock, initial pledge and final live check; no close/write/child/output. |
| NE02 | Unsupported/root/set-ID/id mismatch, null/nonreserved/unknown/malformed reservation, reservation occupying fixed roles, members or bad path: refuse before first unveil and preserve storage/ownership. |
| NE03 | Each held/named stat error, changed dev/inode/type/mode/uid/gid/owner/link state: no authority for the bad path, no cleanup or retry. |
| NE04 | Cancellation/clock/deadline before each observation/transition and after success; every unveil/lock/pledge failure stops at its literal prefix; no renewal or later transition. |
| NE05 | Normal boundary still refuses and normal readiness0 has no native/security references; normal non-OpenBSD candidate adapter refuses. Product PLAN_SOURCES unchanged; no candidate link. |
| CL01 | Exact review unavailable: quiet4, zero legacy/guard/entry/output effects, descriptor canaries unchanged. No caller switch can enable availability. |
| CL02 | Modeled availability: one actual dispatcher call to modeled closed entry; exact0/3/5/4 results; no fallback or diagnostic writer. Nonreview argv delegates once to actual legacy function with unchanged argc/argv. |
| CL03 | Actual legacy help/version/invalid/plan regressions, production link and fixed refusal remain unchanged. Future CLI helper/main implementation is fully counted in entry and planner. |

Use one finite owned launcher, replaced environment and existing pinned tools.
Independent fixture/envelope/source review must precede one focused/full dispatch.
Preserve raw streams, actual consuming waits, terminal absence and all uncertainty;
model PIDs/security observations cannot supply native custody or authority.

## Source accounting and acceptance gates

Count every runtime C/header including dormant native candidate and private
interfaces, regardless of directory. Charge full shared main.c and CLI-private
header to both entry and planner, whole once. Charge every private helper fully
to every consumer; material also charges the entire startup/material bridge.
Entry cap16384, whole102400; signature/material18432, capture8192 and review/output
16384 stay unchanged. No source is hidden in fixtures, build rules or generated
runtime data. Measure the complete candidate before execution; no code golfing.

Normal signed/DCO draft PR, portable checks and independent actual acceptance may
complete this source-only slice. Mark/merge only when its agreed source deliverable
and applicable gates pass; exact proposed squash/DCO and canonical verification
are mandatory. Native/CLI activation remains blocked by exact profile adoption,
fresh native authority, source/tool/media/policy bindings, hard timer/resource
sufficiency, fixed descriptor/child custody and serial/plain/output qualification.
No old fixture/approval or later passing run repairs historical missing proof.

## Unexecuted implementation fit

The complete first implementation was preserved uncompiled: whole101,112/102,400,
fully charged entry16,522/16,384 (excess138). No fixture dispatch followed that
failed fit. A base-libc strspn over an explicit62-character ASCII set replaces the
manual suffix classifier only after checking the fixed terminal NUL, so the scan
is bounded by10 and every malformed suffix still refuses. Removing an unused
include and redundant identity locals preserves all checks and saves180 bytes.
The full candidate is now whole100,932/102,400 and entry16,342/16,384: margins1,468
and42. Capture8,093, signature17,162 and review/output12,782 remain unchanged;
material16,635 includes the full enlarged bridge. Planner14,708 includes complete
main.c and private_cli.h. No source or runtime ceiling was silently raised.

The full entry charge includes the still-refusing normal adapter AND dormant
native core/adapter, candidate dispatcher, both private interfaces, main.c and
both original complete shared helpers. Those dormant sources are not free.
The original legacy function body is byte-identical, with only its new name and
thin production wrapper outside that body. Private entry_live's implementation,
origin and cancellation remain unchanged. The small native glue adds no allocator
or retained buffer and fits the existing conservative sequential8KiB workspace
allowance within the existing2MiB application accounting; actual native resources
remain unqualified. A later qualification fix must still respect all caps; this
measured source fit does not promise that an unsupported native profile can be
repaired within them or authorize a wider path/fallback.
