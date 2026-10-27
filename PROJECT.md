# Bootweave engineering policy

This file records enduring project choices. Read [README](README.md), the selected
[GitHub issue](https://github.com/gonz0w/bootweave/issues) and its comments before
work. [Issue #102](https://github.com/gonz0w/bootweave/issues/102) owns this method
adoption within the existing #95 documentation batch. Cam owns material policy
decisions; recorded answers persist across agents and sessions.

## Method and local contracts

Agent Temper supplies methodology, process and engineering practices. It selects
no operating system, implementation language, toolchain, version control system
or hosting service. The technology, platform, command and authority choices in
this file belong to Bootweave; they are not requirements for other adopters.
A product's installation/runtime target and its development/check environments
are separate local decisions. One does not determine the other automatically.

Apply the [Agent Temper core](.agent-temper/METHOD.md) at revision
`781a4b9f71d002b93bc4ad26669d01a3b37c482e`, with its
[source](.agent-temper/SOURCE.md), [checksums](.agent-temper/SHA256SUMS) and
[license](.agent-temper/LICENSE). Only the [Codex profile](.agent-temper/profiles/codex.md)
is selected. Copied GitHub and strict-test-first profiles remain inactive;
existing issue fields and coherent PR batches remain the local mapping.

| Local contract | Purpose and applicability |
| --- | --- |
| [Backlog navigation](docs/engineering/backlog.md) and [task context](docs/engineering/agent-tasks.md) | Existing Suite delivery extension and advisory compact context; exact identities and private-source retention are in [source provenance](docs/engineering/standards-sources.md). |
| [Product requirements](docs/product/PRD.md), scoped designs and ADRs | Generic installer outcomes, accepted interfaces and their explicit adoption boundaries. Research remains evidence until adopted. |
| [Threat model](docs/security/threat-model.md), [native gate](docs/testing/native-gate.md) and [debugging](docs/debugging.md) | Trust, destructive targets, secrets, uncertainty and scoped native qualification. |
| [Development](docs/engineering/development.md), [review](docs/engineering/review.md), [document lifecycle](docs/process/document-lifecycle.md) and [governance](GOVERNANCE.md) | Local tools, actual evidence, signed/DCO source, human documentation and maintainer integration. |

Agent Temper owns general engineering method; compatible stronger local contracts
remain. File order, a newer upstream revision or another agent's preference cannot
supersede an owner decision. Record an explicit replacement, scope and reason
before relying on it. Missing or conflicting required policy stops dependent work
for owner Q&A while independent preparation may continue. Routine implementation
details are delegated within accepted scope; product, trust, technology, support,
authority and consequential design changes need the owner.

## Product and technology

Read the canonical [product and compatibility contracts](docs/policy/product.md#product-and-technology).

## Work selection and completion

Read the canonical [work and delivery contracts](docs/policy/work.md#work-selection-and-completion).

## Bootweave verification and execution

Read the canonical [verification and execution contracts](docs/policy/verification.md#bootweave-verification-and-execution).

## Review, integration and external actions

Read the canonical [review and action authority](docs/policy/authority.md#review-integration-and-external-actions).

## Documentation and maintenance

Read the canonical [documentation and method maintenance](docs/policy/documentation.md#documentation-and-maintenance).

### Codex routing selection and policy change — 2026-10-24

Read the canonical [codex routing and resources](docs/policy/routing.md#codex-routing-selection-and-policy-change--2026-10-24).

### Finite verification decision for routing adoption #103

Read the canonical [historical finite decisions](docs/policy/history.md#finite-verification-decision-for-routing-adoption-103).

## Terra product routing update — 2026-10-27

Read the canonical [codex routing and resources](docs/policy/routing.md#terra-product-routing-update--2026-10-27).

## One-time Terra routing integration decision

Read the canonical [historical finite decisions](docs/policy/history.md#one-time-terra-routing-integration-decision).

## One-time Temper guidance integration — 2026-10-27

Read the canonical [historical finite decisions](docs/policy/history.md#one-time-temper-guidance-integration--2026-10-27).

## Explicit policy selection

The owner-selected modular adoption in [issue #109](https://github.com/gonz0w/bootweave/issues/109)
uses root [policy.toml](policy.toml) as the sole selected-module record. Read every
selected module and its declared prerequisites before work; linked local contracts
retain their existing applicability. PROJECT remains the entry point and retains
its prior heading anchors as pointers to the canonical extracted clauses.

Compatible stronger rules apply together. The owner controls required-policy,
selection, exception and authority changes through the existing work/review process.
The manifest describes composition; it does not authorize actions or supersede prose.
Historical finite approvals remain expired for this work; selection does not renew
WIP lanes, host permissions, check/hook waivers or operational authority.

The [manually invoked selection checker](tools/temper/SOURCE.md) reads inert declared data.
It is outside the retained method bundle and is not wired into CI, hooks or gates.
Mechanical success leaves policy meaning, source trust, approval, check applicability
and execution/integration authority unverified. Use only an already authorized executor;
missing required checks remain unavailable and block their dependent acceptance.

## One-time modular-policy integration — 2026-10-27

On 2026-10-27 the owner approved a one-time integration decision for this owning
modular-policy/checker adoption issue. For this exact separately reviewed patch,
the unchanged upstream checker, its inert synthetic contract cases, the upstream
read-only adoption auditor and trusted passive source/inventory/hash/link/role
checks may run on inert coordinator copies. No consumer module, existing validator,
hook, example command or product/native workload may execute there. No native
preimplementation red was run; exact reviewed upstream source plus meaningful
post-install positive/rejection evidence and separate source review substitute
for that missing adoption history. The checker behavior is not modified.

For this patch only, those source/configuration observations and separate review
replace unavailable mandatory BSD repository gates and commit hooks, which are
waived/not run, never passed. Existing SSH signatures, genuine author DCO, PRs,
separate review, manual title/DCO/signature checks and canonical verification remain.
Horvium's old pin-specific validator is not repaired or qualified by this decision.
Any base/head/source drift stops affected integration for renewed source review.
For this owning issue only, one temporary additional source-adoption WIP lane may
coexist with held/Review work; existing caps and native/product lanes otherwise
remain. This lane ends at this PR's merge or abandonment.
The decision ends at that same event; retain it as expired history thereafter. It is a finite integration allowance outside the standing selected
policy, and grants no future bypass, other WIP, retries, guest/runner startup,
tool installation, CI workflow/infrastructure change, model change, release,
deployment, key/trust change or control of unrelated work.
