# Closed entry ownership: portable evidence

This records the focused source-only continuation of [PR #90](https://github.com/gonz0w/bootweave/pull/90)
and its [complete lifecycle contract](../design/trusted-entry-output.md).
It composes the existing startup, material, capture and authenticated-output
stages with one owner per original input/material/final-boundary lifetime.
The production planner and CLI are unchanged. The normal entry boundary always
refuses before capture; no native confinement or product acceptance follows.

## Exact focused result

Clean candidate 716cf050ab9121d221ffdf09151d0b67f05cc39b, tree
5e3dd843df9221c02d4de9a26db6448fc0fa8b9b, base
e300bb46eb69ca5a3e218d8e41c62fa3d2c2b2cb: both source commits have trusted
SSH signatures and genuine author DCO. All 312 tracked files were unchanged.
The first focused attempt passed 2 unittest groups, 139 independently frozen
literal cases and 155 launches (13 compile/link, 139 driver, 3 symbol queries),
with zero skips/failures. It ran 2026-10-20T21:44:47.017078Z through
21:45:00.596832Z in 13.579036 seconds. Raw [focused log](entry-ownership-evidence/focused.log)
and [bounded results](entry-ownership-evidence/results.json) are committed here.

Independent actual-result acceptance: accepted, no blockers; retained receipt SHA256
`d4bb04d7c7fcac7b3a4ca500861a49d57376cabc6303f1beb2db9d577bd2b041`.
This committed record covers focused checks; full clean-candidate verification
and merge acceptance are recorded separately in the PR and retained coordinator
receipts. Adding this evidence changes only documentation/README, not runtime or
fixture bytes.

## Ownership observations and proof limits

The actual entry body was used with separately compiled guard/security adapters.
It latches original 0/3/4 ownership before guard, refuses shared-latch reuse before
takeover, requires all roles 0..4 before allocation, and calls fixed closefrom(5)
before original-clock reservation. Guard failure closes inputs once and performs
no later pledge. Later early failures independently close owned inputs, release
only acquired RESERVED material and reduce once. Capture NOT_TAKEN retains input
ownership; every taken capture transfers it permanently. Auth NOT_TAKEN retains
material/boundary ownership; every taken auth call transfers both. Rebound canaries
and immutable first-attempt observations survive refusals/repeats.

Real owned input/scratch files, capture, preparation, fused review, policy/hash,
parser/inspector and fixed output were exercised. Test-only NOT_TAKEN controls
exercise outer handoff responsibilities. The verifier fork/kill/wait adapters use
only synthetic PID 12345, never an actual child or born/group claim. Inert Ed records
establish format/model flow, not cryptographic or production-trust acceptance.
The new unsupported-entry stimulus is modeled; actual normal Mac unsupported
legacy guard/start checks remain separate inherited regressions. The new normal
boundary case executes the real always-refusing entry_platform.c after a modeled
guard. Neither guard nor security adapters mutate host settings.

Known owned unrelated fd 5 was closed by the model. Descriptor 70 is an explicitly
synthetic above-new-NOFILE witness: no actual high descriptor or native closefrom
qualification is claimed. Exact native closure/confinement remains a gate.
The original two-second deadline, cancellation, all five presence failures, closefrom,
reserve/capture/preparation/signature/content/output errors and failed independent
finalizers preserve first cause and unknown closure. The first reserve-ownership
mismatch correctly retains UNCERTAIN scratch evidence; no cleanup was authorized
by its passing oracle.

Original 94 oracles were preserved before implementation inspection. R2 added 45
explicit prefix controls and selected-closer masks; its independent missing-role
EBADF correction is recorded. R3 corrected only the first reserve-ownership case
from known refusal to UNCERTAIN using the existing material contract before any
execution. All revisions remain preserved. Public redundant summaries were
consolidated; actual material witnesses and close journals retain their provenance.
Old 149 guard and 80 startup golden drivers/vectors remain byte-identical.

## Budgets, custody and remaining gates

Whole runtime source is 95,788/98,304 bytes; entry 10,190/10,240, material 16,610/18,432,
signature 17,162/18,432, capture 8,093/8,192 and review/output 12,782/16,384. Every shared
header is fully charged to its consumers. Original over-budget unexecuted drafts
remain retained. No runtime, source or resource ceiling grew.

The owned envelope retains the original leader/group through its last stop,
then records actual consuming wait and terminal absence. Its fixed work/total
limits are 270/300 seconds; child/stream/file limits and replaced private environment
are unchanged. Darwin refused the final stop only while the held original leader
was a zombie; the consuming wait and empty group were recorded independently.
No numeric identity was signaled after that wait. Synthetic process observations
are never treated as custody evidence. This is cooperative portable supervision,
not hard network/memory isolation or OpenBSD qualification.

Native activation still requires an adopted exact locked executable/loader/library/
devnull/fd/scratch whitelist, actual signify handoff and hard-timer/resource/serial/
plain-output qualification. CLI activation must adopt this coordinator and inherited
fd1 contract without weakening the planner. Entry has 50 source bytes of headroom;
further native-boundary work needs measured fit under the approved cap. No new guest,
credential, security-setting change, deployment or retained-attempt reuse occurred.
Historical failed sequential-run evidence and its custody gap remain preserved;
this passing run does not repair that earlier missing custody proof.
