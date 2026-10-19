# Authenticated read-only payload review: scoped product design

Status: proposed for adoption with [the issue #5 decision](project-trust/ADOPTION.md),
2026-10-19. No source implementation, native run or new target is authorized by
this document alone. Engineering continuation is delegated; this is a concrete
scope to review rather than a request for new product direction.

## Requirements

An independent author can check that the exact bounded project definition and
archive match a trusted media policy, then inspect a complete plain-text review.
The existing unverified `bootweave plan` remains compatible. The new result says
what is authenticated, what was inspected and what remains unqualified. It cannot
create a target or authorize extraction.

| ID | Required behavior |
| --- | --- |
| AP-01 | Require a trusted policy binding one exact definition SHA-256, project id, release/architecture, explicit publisher key identity and resource policy. Project/operator/archive bytes cannot select or widen that policy. |
| AP-02 | Authenticate the retained exact definition bytes using an explicit-key OpenBSD base signify detached signature; require their pinned digest and strict existing v1 parsing/tuple agreement. No reserialization before signature verification. |
| AP-03 | Snapshot one bounded payload; compare full SHA-256 with authenticated payload.sha256, then inspect the same retained bytes. Filename is a label, never a path to open. |
| AP-04 | Validate the entire restricted archive and all limits before any successful review. Reject unsupported types, paths, aliases, owners/modes and trailers. No fallback extraction. |
| AP-05 | Emit a complete deterministic bounded plain-text review, distinguishing trusted-policy binding, signature/digest result, inspected contents and absent effects. Untrusted terminal controls/secret values cannot enter diagnostics. |
| AP-06 | Reject elevated execution, changed or unavailable input/context, unsupported host/confinement, timeout, verifier failure and incomplete output. No partial success or cached verification. |
| AP-07 | No target/device access, network, extraction, privilege backend, executable hook, secret prompt, persistent receipt or installation. A successful review cannot be replayed into future write authority. |
| AP-08 | Independent negative/control tests and later source-bound native evidence precede product acceptance. Preserve research and exhausted target approvals. |

## Engineering boundary

Retain the version-1 definition unchanged. Sign its exact original bytes, not a
new manifest grammar or a digest chosen by the untrusted caller. The trusted
policy pins those bytes in addition to the explicit publisher key and tuple, so
an older valid signature or a key shared by another project cannot substitute a
different definition. A new release requires a reviewed policy/media update.
This is offline identity relative to the selected policy; do not claim current
revocation, latest-version freshness or semantic safety of signed content.

The media builder owns that policy as reviewed trusted code/data, outside the
project definition. The first implementation uses one compiled synthetic policy
for tests. A public policy-file format, arbitrary key path, distribution service,
key enrollment/rotation and real production key selection are outside this slice.
No synthetic signer is a production trust anchor. Test-only policy injection must
be compiled into a separate driver and absent from the production entry.

Original C/base libc parses and retains bounded byte snapshots. Base signify
performs cryptography; do not implement Ed25519 or accept a configurable verifier.
The existing authentication research demonstrates a trusted explicit-key verifier
and retained-buffer flow. Reuse only reviewed components needed for this slice.
The verifier's fixed executable identity, scratch-file/descriptor handoff and
cleanup, exact CLI descriptor assignments and confinement transitions must be
specified and independently reviewed **before implementation of that I/O layer**.
These are engineering decisions under the above fixed authority limits. Do not
weaken confinement, create an arbitrary pathname interface or publish an invented
privilege/wire schema to work around them. Pure model/parser work can proceed
once this scope is adopted; this remaining I/O design is a concrete review gate.

Every opened input must be a held, read-only regular file at initial offset zero;
reject descriptor/output aliases and observed metadata changes. Caller-owned
output descriptors remain an explicit limit. Capture complete bounded bytes,
then close input descriptors; never reopen using a project filename. Bound the
signature/key representation and reject malformed, trailing or ambiguous records.
Do not let verifier output become terminal output. Snapshot/digest verification
establishes the retained bytes, not filesystem publication atomicity.

Use the researched uncompressed ustar subset as a conservative **read-only**
archive policy: 1 MiB complete archive, 128 members, 512 KiB content total,
256 KiB/file, 255-byte paths and depth 16. Require complete zero padding, two zero
trailer blocks and EOF. Reject links, special/extension types, traversal, absolute
paths, duplicate/conflicting trees and distinct wire aliases of one path. Fixed
accepted mode/owner encodings must be listed from the existing inspector's
source policy in the implementation review; none grants a filesystem effect.
Inspection can reject otherwise valid tar files. No decompression dependency.

Start with the existing 16 KiB definition and 256-byte fixed diagnostic ceilings;
permit at most 16 KiB signature and 1 KiB trusted public-key input. Whole review
output is capped at 64 KiB and must fit before first output. In the I/O design,
retain finite CPU/wall/read/verifier/output deadlines and explicit memory/fd/file
ceilings for every child; measure real OpenBSD RSS. Never silently inherit larger
limits from host tests. No native acceptance or production resource claim exists
until that concrete implementation and exact native plan are qualified.

## Behavioral Test Design

Tests specify outcomes independently of implementation functions. Use synthetic
unrelated application, DNS and firewall definitions with the same trust rules.

| Test | Stimulus and independent expected observation |
| --- | --- |
| AP-T01 | Valid exact signed metadata and payload: all policy/identity/content states complete; no target and zero operations. Reordered equivalent JSON with an old signature rejects unless newly signed and pinned. |
| AP-T02 | Wrong/untrusted key, cross-project/system tuple, changed definition, valid old signed definition, absent policy or changed policy: reject before successful review. |
| AP-T03 | Alter one payload byte, substitute after verification or mutate input during snapshot: mismatch/refusal; inspector consumes only the originally retained verified bytes. |
| AP-T04 | Existing boundary/alias archive corpus plus independent traversal/link/special/mode/size/depth/trailer controls: fail closed, outside sentinels unchanged and no writer invoked. |
| AP-T05 | Controls, huge fields, malformed signature/key, raw verifier errors, short/broken output: fixed bounded diagnostics; no completed review on failure. |
| AP-T06 | Wrong input descriptor type/access/offset, aliases, elevated execution, missing confinement, verifier exit-zero with wrong identity, signal/timeout: refuse without fallback; owned scratch/children handled and uncertainty explicit. |
| AP-T07 | Compare byte inventories/operation witnesses outside owned test scratch and syscall/process observations natively; no device/network/archive-member writes or privilege requests. |
| AP-T08 | Existing planner regressions unchanged. Complete exact-source portable checks and independent review; later native serial and keyboard/plain tests, resources and owned teardown under fresh authority. |

Portable tests use frozen read-only sources, synthetic credentials only if their
bounded test authority permits, private temporary state and no network/provider
access. A doc-only adoption runs repository checks; it does not need new signing
fixtures or a VM. Native evidence must distinguish legitimate verifier scratch
writes from forbidden target/member effects. No native test or real key is
requested now. Parent PT/PTT acceptance remains partial; no broad installer,
extraction, privilege, secret or bootstrap claim follows from this design.
