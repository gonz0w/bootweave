# First native qualification profile: Test Design

Status: Proposed, 2026-10-11. Owner: Cam. Acceptance pending; no native runs.
Related [PRD](PRD.md) and [EDD](EDD.md).

## Acceptance mapping

| Requirement | Test | Fixture/action | Expected result and gate |
|---|---|---|---|
| NP-01 | NPT-01 | Review frozen media/source/verifier/firmware/backend identities | Verified release inputs and exact configuration; stop before boot if incomplete. |
| NP-01 | NPT-02 | Verify a deliberately corrupted copy of the media offline | Verification fails; corrupt copy never boots. Requires approved verifier setup. |
| NP-02 | NPT-03 | Inspect effective attachments, open files and guest device inventory | Only new disk/read-only media; no network/sharing/passthrough; stop if uncertain. |
| NP-03 | NPT-04 | Boot VGA run and type non-destructive stock menu input | Readable prompt and correct keyboard response within budget; no install choice. |
| NP-03 | NPT-05 | Fresh fixture, release-specific serial boot configuration | Prompt/input observed over private serial endpoint; no graphical input needed. |
| NP-03 | NPT-06 | Observe stock text with color disabled and 80x24 console | Required stock prompts remain readable; log limitations, no Bootweave UI claim. |
| NP-04 | NPT-07 | Hash complete fresh target before/after non-install interaction | Disk bytes unchanged; any change is failure/uncertainty requiring investigation. |
| NP-04 | NPT-08 | Review substituted path, backing chain and extra-device configurations without launching them | Preflight rejects each; no dangerous configuration executed. |
| NP-05 | NPT-09 | Stop on unexpected prompt or lost console; exercise capture cap using synthetic output | No further input; bounded escaped evidence; guest terminates and cleanup verified. |
| NP-05 | NPT-10 | Finish each run and inspect fixture/process/endpoint state | Owned files/processes/listeners gone; failures explicitly recorded. |
| NP-06 | NPT-11 | Review sanitized evidence against frozen profile and source | Every outcome has identity and limits; no unsupported runtime/media claim. |

## Levels and limitations

NPT-01/02/08/11 include manual review and host tooling only. NPT-03 through NPT-07
need actual OpenBSD guest observations; host mocks cannot satisfy them. NPT-09/10
need both host cleanup observations and native termination evidence where launched.
No harness implementation or redundant unit tests are included in this scope.
Full-disk byte hashing establishes the non-install target postcondition, but does
not by itself prove isolation or every internal installer operation was read-only.
NPT-03 must independently establish attachments. Media remains read-only.

This baseline does not test Bootweave boot-to-installer, parser, renderer parity,
payload trust/extraction, installed accounts/services, reboot readiness, interrupted
writes, recovery or generic application integration. Those remain required by the
[full native gate](../../testing/native-gate.md) and later scoped Test Designs.

## Procedure and resource collection

Before every launch: approved run scope; clean candidate and resolved base; verified
inputs; frozen backend/console settings; available resource capacity; reviewed
attachment inventory; capture limits; known termination and cleanup procedure.
Create a fresh disk per console run. Record complete-disk digest before and after.
Initial scope stops at stock installer menu inspection. Never select install or
upgrade to collect a baseline. A separately approved installation test needs new
requirement mappings, exact target confirmation and installed postconditions.

Apply PRD limits and record actual media bytes, disk logical/allocated bytes,
boot elapsed time, total duration, peak host process memory, available guest memory
observations and capture size. Record method/tool version and missing metrics.
Timeouts are failed observations, not native support disqualification for all
hardware. Manual console response timings include operator delay explicitly.

For failure injection, use only corrupt media copies, synthetic capture input and
unlaunched unsafe configurations. Do not inject malicious host paths into an active
emulator. A naturally unexpected installer prompt stops the run; describe it without
attempting undocumented answers. Forced interruption targets only the owned guest.

## Evidence and review

Report each test as passed, failed, skipped or not run with observations and reason.
Evidence binds exact repository head/tree/base, release/source/media, emulator,
firmware, console, commands and tool versions. Retain private receipts outside Git;
review excerpts for secrets, private host/network information and escape sequences.
Confirm teardown before declaring a run complete. Cam reviews actual evidence and
any compatibility change. Passing local make verify validates these documents and
foundation tooling only. Native outcome as of 2026-10-11: all tests not run.

Current bounded execution evidence: [diskless VGA](diskless-evidence.md) and
[corrected serial](serial-evidence.md). Earlier no-run/acceptance-pending statements
describe the original full disk-bearing proposal; execution authority was limited
to the accepted non-install diskless research, not the complete profile.
