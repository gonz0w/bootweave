# Backlog and delivery index

Authority: [Suite-Standards delivery](https://github.com/gonz0w/suite-standards/blob/4aeb70ae4d7423c4ae7d52c025f94af4fb53ff22/docs/backlog-delivery.md)
and [rollout](https://github.com/gonz0w/suite-standards/blob/4aeb70ae4d7423c4ae7d52c025f94af4fb53ff22/docs/backlog-delivery-rollout.md)
at `4aeb70ae4d7423c4ae7d52c025f94af4fb53ff22`. Owner: Cam (@gonz0w),
Bootweave owning lane; adoption story [#95](https://github.com/gonz0w/bootweave/issues/95).
This pointer adopts that delivery contract prospectively, not a new runtime,
architecture, CI service or unrelated standards profile. Existing scope, reviews
and safety rules remain authoritative. The central document defines flow,
INVEST, Ready/Done and proportional checks; do not duplicate it locally.

## Prioritized outcomes

Issues contain live acceptance, status, dependencies and Ready/Done. This is a
dated navigation snapshot (2026-10-23), not a second board or completion ledger.

| Order | Issue / user outcome | State and next action |
| --- | --- | --- |
| P0 | [#95 central consumer adoption](https://github.com/gonz0w/bootweave/issues/95) | In Progress: finish reviewed pointer/issue mapping and applicable checks |
| P1 | [#5 production trust/architecture decision](https://github.com/gonz0w/bootweave/issues/5) | Blocked for material decision: compare existing typed engine with proposed stock-first path; do not adopt either changed guarantee silently |
| P1 | [#96 install, boot, use and restart one reference appliance](https://github.com/gonz0w/bootweave/issues/96) | Blocked: #5, selected reference product, complete authenticated8.0 closure/layout, actual native authority and containment |
| P2 | [#97 Aegvium](https://github.com/gonz0w/bootweave/issues/97), [#98 Nomvium](https://github.com/gonz0w/bootweave/issues/98), [#99 Arxwall](https://github.com/gonz0w/bootweave/issues/99), [#100 Vectivium](https://github.com/gonz0w/bootweave/issues/100) | Backlog: later required profile outcomes; each depends on #96 and its product-owned contracts/function/qualification |

The MVP denominator is #96's IM1–IM6 outcome, not helper/test/PR count. Four-profile
coverage is later required completion, not a prerequisite for unrelated product
development. Preserve generic extensibility and each product's independent runtime.
Stock-first simplification and a larger production-payload contract remain
proposals. Existing archive caps, authority and failure guarantees stay effective.
Historical7.9 research under #2/#3/#4/#10 is retained with its scope; it is not8.0
installer acceptance. Visual polish is not a substitute for the usable workflow.

## WIP and existing work

Local policy follows the central default:2 active batches/repo,1/implementer;
In Progress, Blocked and Review count. Related stories in one coherent batch
count once. At adoption this repository already has2 held batches:

- [PR92](https://github.com/gonz0w/bootweave/pull/92): qualification/authentication
  preparation; current execution/signing/retention and containment gates remain.
- [PR94](https://github.com/gonz0w/bootweave/pull/94): installer design/source;
  published7637b88 and preserved uncommitted UNTESTED owned-controller/placement
  continuation. Architecture reassessment pauses new custom lifecycle work.

This owning implementer also has2 inherited Vectivium batches. Temporary exception
for one coherent documentation-only consumer-adoption batch across both repos:
maximum3 batches here and5/implementer including the4 inherited batches. Owner:
Cam/owning lane; reason: requested central rollout while preserving held work;
review by2026-10-24 or a blocker/adoption transition, whichever first. No new
product batch may be pulled while inherited WIP exceeds its normal cap. Finish,
unblock or assist existing review; do not hide paused work in Backlog or delete
evidence to free capacity. Reconcile counts at handoff. No other owner's work is
interrupted or reclassified by this exception.

## Commands, acceptance and holds

Use existing [development](development.md), [task index](agent-tasks.md),
[review](review.md), [testing/native gate](../testing/native-gate.md), root
instructions and issue-specific test design. Focused document checks are
`make check lint secrets` and `git diff --check`; the existing required final
`make verify` requirement is unchanged. Current genuine-authentication fixture
effects needed for that aggregate command remain held; a focused pass does not
waive it or establish a clean full pass. No new test/validator framework is added.

No GitHub Actions, runners or hosted success publisher. No native/root/disk/image,
credential, service/security, deployment, disposal, denied observer retry or
consumed-trial reuse is authorized by this adoption. Exact approved source budgets
apply to their owning feature; a text allowance is not a runtime capacity proof.
Signed commits, genuine DCO, independent review, applicable protection/merge gates,
exact proposed squash-message preflight and canonical verification remain required.

PRs may batch related stories but must identify acceptance IDs, actual current-head
results and held/unrun lanes. Source implementation, native qualification and
usable release remain distinct. Close #95 only after its own reviewed adoption
acceptance; keep #96/profile stories open until their real outcomes pass.
Reverting adoption uses a reviewed pointer change and preserves issues, old pins,
historical failures, active worktrees and product evidence.
