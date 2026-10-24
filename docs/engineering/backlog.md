# Work and delivery index

[PROJECT](../../PROJECT.md) records the Agent Temper method and local choices.
The existing Suite delivery and rollout extension remains pinned at
`4aeb70ae4d7423c4ae7d52c025f94af4fb53ff22`; see [exact sources and access](standards-sources.md).
It supplies Ready/Done, INVEST and WIP practices. No optional Agent Temper profile
is selected. Stronger local safety, review and native gates remain.

## Find the owning issue

[GitHub Issues](https://github.com/gonz0w/bootweave/issues) alone own live priority,
status, dependencies, acceptance, approach, results and handoff. This page is
navigation, not a queue snapshot. Read the selected issue and all its comments.

| Scope | Authoritative work |
| --- | --- |
| Method and delivery adoption | [#95](https://github.com/gonz0w/bootweave/issues/95) and its bounded [#102](https://github.com/gonz0w/bootweave/issues/102), one coherent documentation batch |
| Production architecture/trust | [#5](https://github.com/gonz0w/bootweave/issues/5) |
| Install, boot, use and restart a reference consumer | [#96](https://github.com/gonz0w/bootweave/issues/96), including its IM1–IM6 acceptance |
| Consumer outcomes | [Aegvium #97](https://github.com/gonz0w/bootweave/issues/97), [Nomvium #98](https://github.com/gonz0w/bootweave/issues/98), [Arxwall #99](https://github.com/gonz0w/bootweave/issues/99), [Vectivium #100](https://github.com/gonz0w/bootweave/issues/100) |

Stock installer/packages plus a generic bootstrap is the selected direction in
#5/#96; inspect their current scope and holds. Source availability is not installer
acceptance. Historical 7.9 evidence and held implementation/qualification branches
retain their original identity and ownership. Consumer profiles/setup/runtime stay
with consumers; no named-product dispatch or required appliance runtime belongs in
the generic engine. Helper counts and visual polish cannot substitute for usable
install/boot/use/restart outcomes.

## Select and finish a batch

Use the existing Backlog/Ready/In Progress/Blocked/Review/Done issue fields or board.
Before implementation establish owner, finite outcome, non-goals, acceptance,
dependencies, authority, checks and recovery. Small specifications stay in issues;
consequential contracts need accepted scoped designs or a bounded research mandate.

Normal WIP is two active batches per repository and one per implementer. In Progress,
Blocked and Review count; related issues in one coherent PR count once. #95 owns
the temporary adoption exception and live inherited-batch accounting. Reconcile
actual owner/count, reason, cap and expiry at handoff or changed blockers. Do not
start a new product batch while above the cap, hide held work in Backlog, interrupt
another owner or discard evidence. #102 continues the existing #95 batch.

Keep a feature or related issue slice, corrections, tests, native qualification and
teardown in the same branch/PR. Map each acceptance ID to actual evidence and held
lanes. Close an issue only after all its criteria and required integration gates
are met; reconcile dependent/parent issues without claiming operational acceptance
from source-only work.

## Checks and boundaries

Use [development](development.md), [task context](agent-tasks.md),
[review](review.md) and the applicable [native gate](../testing/native-gate.md).
The owner-approved documentation gate for #95/#102 uses existing supported-host
`make check lint secrets`, provenance/link, hook/signature/DCO checks and separate
review. Runtime/behavior/tooling changes retain OpenBSD and full `make verify`
gates. Tool/authentication prerequisites and missing results must be explicit. No GitHub
Actions, runners or hosted success publisher.

Method adoption grants no native/root/disk/image, credential, service/security,
deployment, disposal, denied observer retry or consumed-trial reuse authority.
Keep existing source budgets, signatures/DCO, protection/merge gates, exact squash
preflight and canonical verification. Revert or update the method through a finite
reviewed issue; retain earlier pins, decisions, worktrees and measured evidence.
