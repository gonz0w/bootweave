# Documentation guide

Write for the person using, reviewing or maintaining Bootweave. Markdown beside
code is canonical; [PROJECT](../../PROJECT.md) records enduring engineering choices
and the pinned Agent Temper [documentation method](../../.agent-temper/docs/reference/documentation.md).
GitHub Issues alone own live work and continuation.

## Put information where readers expect it

| Document | Purpose |
| --- | --- |
| README | Purpose, actual maturity, setup prerequisites and routes to deeper guides |
| AGENTS and PROJECT | Agent entry instructions and persistent local decisions |
| PRD | Observable user outcomes and non-goals |
| EDD/design | Engineering contracts, authority, failure and compatibility boundaries |
| TDD (Test Design Document) | Acceptance-to-evidence mapping; not a claim that tests ran |
| ADR | An accepted consequential decision, alternatives, reason, date and authority |
| How-to/reference | A reader's task or stable interface, with relevant examples |
| Research/evidence | Dated sources, exact tested inputs, observations, failures and limitations |
| Issue/PR | Scope, dependencies, status, approach, acceptance, review and continuation |

Use the issue for small specifications; do not create a full document packet for
every edit. Nontrivial implementation still needs accepted scoped requirements,
engineering and Test Design, or an explicitly authorized bounded research spike.
Record acceptance and its source. Research/proposals never become production
contracts merely because their documents or code exist.

The [foundation readiness record](../foundation-status.md) describes the original
setup period, not current work status. Current accepted scope and superseding
owner decisions are in the relevant issue/design and PROJECT; retain historical
observations without refreshing them to current source.

## Write a usable procedure

Use STE-informed plain English, without claiming formal ASD-STE100 compliance.
Prefer concrete verbs, consistent terms, short sentences and one action per step.
State the reader's goal, prerequisites, working directory, exact command, expected
result and what to do on failure. Explain destructive effects before the action.
Keep examples synthetic, clearly labeled and free of private paths or credentials.

Separate a safe source-reading instruction from an execution recipe. Link current
[change-specific prerequisites](../engineering/development.md); old host behavior
commands are historical, not authorization. Do not hide uncertainty, imply general rollback,
or describe a source check as a native/install/release result.

Update affected user instructions with the behavior. Check source accuracy, links,
navigation, terminology, examples and a representative reader path. Executable
validators follow the applicable check environment; executable examples retain
their behavior and target-authority gates.
Record actual review/check results in the owning issue/PR, not a new status file.
