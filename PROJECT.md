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
`4e6804a2c10be6e9ffc9fd057a7d109dcc167504`, with its
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

Bootweave is a generic third-party OpenBSD installer, independent of the OpenBSD
project and every appliance it can install. Product setup, private inputs, roles,
service policy and runtime belong to consumers. Do not add named-product dispatch
or a required appliance/suite runtime dependency.

The owner selected stock OpenBSD 8.0 installer/packages plus a small reviewed
generic bootstrap in [#5](https://github.com/gonz0w/bootweave/issues/5) and
[#96](https://github.com/gonz0w/bootweave/issues/96). That is an adopted direction,
not completed product acceptance. The separately held stock-source and OpenBSD
tooling changes remain with their owning PR/issue; this method change imports none
of them. Existing planner/custom trust source and 7.9 research retain their exact
scope and evidence. No production installer, broad hardware or release is claimed.

Keep existing C99 implementation contracts, POSIX shell and bounded inert metadata.
Python 3.14.7 and the pinned lint/secret/YAML tools are development inputs, not
selected installed-system dependencies. Prefer OpenBSD base tools where suitable.
Do not treat a host prototype or proposed parser/UI as a native runtime choice.
New languages, dependencies, services, abstractions or maintained upstream patches
need a present requirement, simpler-alternative comparison, provenance/license and
maintenance/removal rationale. Keep the BSD-3-Clause license and imported notices.

Project definitions, payloads, operator strings and terminal output are untrusted.
Preserve bounded parsing, escaping, explicit trust roots and source authenticity;
digests alone are not authentication. Never interpolate untrusted input into shell
commands. Package scripts and build hooks are reviewed executable code. Preserve
the adopted installation/trust boundary, explicit operation authorization and
consumer ownership; debug mode cannot bypass safety.

Show the exact destructive target and effects, require fresh operator confirmation,
and invalidate it on changed inputs. Planned, performed and observed states differ.
Unknown or partial results need recovery instructions, not an invented rollback or
automatic destructive retry. Keyboard, serial, low-color and plain-text paths remain
required; decoration cannot obscure prompts or enable control-sequence injection.

## Work selection and completion

GitHub Issues alone own live scope, priority, dependencies, ownership, progress,
approach, results and continuation. The backlog document links issues; it is not
another queue. Use the existing Backlog/Ready/In Progress/Blocked/Review/Done
states in each issue's status field or existing board. The optional GitHub profile
does not impose a new label scheme. Keep current issue bodies and historical
decisions; state a supersession explicitly instead of deleting inconvenient facts.

Select a finite ready outcome before implementation. Name owner, scope/non-goals,
observable acceptance, dependencies, proportional checks, authority and recovery.
A broad milestone or implementation proposal is not an executable assignment.
Use INVEST and Ready/Done as existing refinement aids, without adding ceremonies.
Small specifications stay in the issue; consequential contracts use scoped design
documents or ADRs. Record unrelated discoveries before implementing them. Changing
accepted outcomes or guarantees requires an owner decision.

Normal WIP is two active batches per repository and one per implementer; Blocked
and Review count. Related issues in one coherent batch count once. The existing
#95 documentation exception and inherited batches are tracked in that issue;
#102 continues that same method/delivery batch. Review its owner, reason, cap,
expiry and actual count at handoff. A branch is not a way to hide work or obtain
extra capacity. Preserve other owners, dirty files, held fixtures and failed evidence.

Keep one PR for a complete feature or closely related slice, including its fixes,
checks, native evidence and teardown when applicable. A phase transition does not
need another PR. Map each issue to actual acceptance and evidence; use closing
references only when every criterion is satisfied. Source-only work does not close
installer/use/restart or other operational acceptance. After accepted integration,
verify canonical source and reconcile issues, dependencies and parent outcomes.

## Bootweave verification and execution

These are local Bootweave requirements, not Agent Temper requirements. The method
asks for appropriate, honest verification; it does not prescribe an OS or a command.

Bootweave's runtime, installer builds, behavior tests and cross-build validation
remain OpenBSD-only under the October 23 decision in #5/#96. Historical Mac/Linux
LAB results remain historical. A product target does not automatically determine
the host for source-only checks. No VM or native authority follows from adoption.

On October 24 the owner directed completion of #95/#102 with verification
appropriate to the changes. For this finite documentation/method-metadata batch,
use the existing supported development host and unchanged `make check lint secrets`,
content/secret and commit-message hooks, signature/DCO checks, exact bundle/source/
license/link validation and separate reader/policy review. This replaces the
previous blanket OpenBSD/full-suite gate only for this adoption. Full `make verify`
and native installer tests are not applicable to its unchanged executable source;
record them as not run, never as passed. No hook/signing bypass is needed or allowed.

Behavior, tool, test, hook, build or dependency changes still require the relevant
OpenBSD checks and full `make verify` on the exact candidate. The held OpenBSD tool
adaptation and genuine authentication prerequisites remain with their owning work;
this adoption neither imports nor qualifies them. Follow
[development](docs/engineering/development.md). Never substitute fake crypto
positives, skip required assertions or call source review an executed check.

Before behavior changes define allowed/forbidden outcomes and meaningful positive,
negative and failure cases. Prefer test-first changes and reproduce defects before
repair, or record unavailable reproduction and the alternative evidence. Here TDD
in design filenames means **Test Design Document**; it is not evidence that a failing
test ran. Prose uses source, policy, link and reader checks, not artificial red tests.

Record exact source/base/tree, tool/OS identities, commands, exits and limitations.
Keep passed, failed, skipped and unavailable distinct. Preserve original attempts,
pins and failed/UNKNOWN evidence; never rewrite old hashes as fresh results. A
focused check cannot grant native or release acceptance. Exact disposable-target,
input, resource/process, credential, security, cleanup and native-execution gates
remain separate. Routine fixes/recovery on an already confirmed target stay within
its accepted scope and limits; a new target or expanded effects needs new authority.

## Review, integration and external actions

Use `codex/` branches, signed Conventional Commits and genuine author DCO sign-offs.
Preserve configured identity/signing and existing hooks. Verify the actual commit
range, trusted signatures and author trailers; do not invent sign-offs, transfer
private signing keys into guests, disable checks or silently bypass hooks.

The current maintainer policy remains: agents prepare scoped PRs against main;
Cam deliberately reviews and authorizes merging. General agent merge authority
has not been granted for Bootweave. Leave a PR open until that authority and its
actual gates are satisfied. The owner authorized bounded read-only agent reviews
of accepted work on 2026-10-24 in #102, replacing the blanket no-spawn restriction
for that scope. The October 24 routing decision below additionally permits
bounded implementation delegation; native execution and merging retain their
existing authority gates. An implementer's second pass is not independent review, and peer review
cannot replace a required specialist or native witness.

Review current head/base/tree, acceptance, effects, source, evidence and exact
proposed squash author/message/DCO before integration. Reconcile changed inputs
with affected checks/review and verify the canonical commit afterward. Preserve
PR/signature/linear-history/conversation protections and deliberate maintainer
review. No GitHub Actions, runners, hosted success publishers or bypass actors.

Release, deployment, service/live-network operations, new credentials, repository
security changes, fixture disposal and expanded native effects need their existing
explicit authority. Public source work never implies those actions. Publish only
reviewed, sanitized evidence; use SECURITY.md for sensitive reports. Keep private
Suite documents, operator inventories, keys and raw private transcripts outside Git.

## Documentation and maintenance

Write reader-oriented Markdown and STE-informed plain English without claiming
formal ASD-STE100 compliance. README explains purpose, maturity, setup and routes;
AGENTS directs agents; PROJECT owns lasting choices; product docs specify behavior;
ADRs record decisions; issues own work; evidence describes dated observations.
Procedures need prerequisites, working directory, commands, expected results and
failure/recovery instructions. Preserve useful canonical manuals and examples.

Keep the imported method unchanged and local rules outside its directory. Updates
need a finite issue, immutable source/license/inventory/hash/link checks in the
applicable check environment, compatibility and separate review. Walk a representative item from selection to
completion. Classify findings under the [extension rules](.agent-temper/docs/reference/extensions.md);
use sanitized examples for public upstream proposals. No automatic pin upgrades,
new policy engine or source cleanup follows merely from adopting this method.

### Codex routing selection and policy change — 2026-10-24

The owner explicitly selected model-routed subagent implementation and directed
all existing Temper adopters to be ready for it in
[issue #103](https://github.com/gonz0w/bootweave/issues/103). This replaces the
previous read-only-review-only delegation allowance with bounded implementation
delegation within already owner-accepted task scope. The lead must supply explicit
file ownership, acceptance, policy, evidence, stop conditions and resource limits.
The reason is that owner-selected workflow; separate review and existing native,
operational and per-PR maintainer merge approval remain binding. This does not
select a new product/platform/toolchain or expand other-resource authority.

| Role | Assignment | Model | Effort |
| --- | --- | --- | --- |
| `temper_workflow` | Bounded read-only inspection and evidence summaries | `gpt-6-luna` | `medium` |
| `temper_product` | Bounded documentation, visual checks, authorized browser tasks and small specified UI fixes | `gpt-6.1-terra` | `medium` |
| `temper_implementer` | Accepted implementation, tests and ordinary debugging | `gpt-6.1-sol` | `high` |
| `temper_analyst` | Difficult analysis or separate complex review | `gpt-6-astra` | `high` |
| `temper_reviewer` | Separate final review of work it did not author | `gpt-6.1-sol` | `high` |

The lead owns tracker writes, integration and returned-evidence verification.
Use at most three concurrent delegated agents, excluding the lead, within existing
resource and WIP limits. No nested helpers without an explicit lead assignment.
Keep tiny or tightly coupled work with the lead when appropriate. No role changes
permissions or authorizes native/operational execution by itself.

If a role or model is unavailable, report it. The lead may do the bounded work
with its current model only when adequate and permitted under existing policy
and limits; otherwise resolve the missing choice with the owner. Never silently
substitute a model or claim that requested settings actually ran. The owner
approves model/effort/role/limit changes; routine routing within this table is
delegated to the lead. Preserve independent final review.

Local `.codex/agents/` files are adapted project configuration outside the pinned
bundle. No global/default model, permission or primary-chat setting changes.
Where native role discovery is unavailable but explicit spawn settings are
supported, use this same table with a complete fresh-context brief and record
observed settings, or follow the fallback. Existing chats do not switch models
when files change. Record client/version and observed role/model/effort metadata
in the owning issue; parsing proves syntax only. Native discovery/routing remain
unverified until actually observed in a fresh supporting session. Earlier active
work retains accepted scope and evidence; future delegation follows this policy.

For this one owner-selected adoption item, Cam authorizes one temporary extra
WIP lane alongside the two existing held product batches on 2026-10-24. The
reason is fleet routing readiness; the exception ends at this item's merge or
abandonment. It does not relabel, stop or enlarge other work. The old #95/#102
supported-host documentation exception does not cover this configuration change:
required OpenBSD/full `make verify` checks remain outstanding before acceptance.
The owner subsequently authorized bounded validation for this exact routing
candidate in an isolated disposable OpenBSD guest, with existing resource,
isolation and teardown limits. Coordinate the shared executor before startup.
This permits the required checks/hooks, not production, new credentials,
permission/protection changes or unrelated effects. The owner also authorized
this issue's gated integration after checks and separate review. This finite
approval does not grant general future merge or native-operation authority.

### Finite verification decision for routing adoption #103

On 2026-10-24 the owner explicitly approved scoped Bootweave verification for
[#103](https://github.com/gonz0w/bootweave/issues/103). This replaces the
OpenBSD/full-suite acceptance requirement only for the thirteen-file adoption
based on `2142175dc0ff86f09593fbbcb8296053e3aa0fe3`, tree
`acfec5d0a49da7c9ed2eab81cdbaed27ea514711`, with sorted path+NUL+bytes+NUL
fingerprint `438e57075da16c370beeb8487701fc1d50d80f0c00320660028e2a500fef18ea`,
plus only this separately approved PROJECT/owning-issue decision delta. Any other
source change requires fresh applicability, checks and review.

For this exact adoption, unchanged pre-commit/commit-message hooks and
`make check lint secrets` must pass in the admitted isolated OpenBSD guest.
Exact upstream bundle/license/inventory/hashes, affected links, TOML/role mapping,
signed actual-author-DCO source and separate exact-candidate review must also pass.
Verify the public signed commit again on BSD and the canonical source after
integration. A failure of check, lint, secrets, either hook, provenance, signature,
DCO or required review remains blocking.

Only for this exact adoption, the unchanged `test`, `test-ui`, `test-storage`,
`test-handoff`, `test-payload` and `test-plan` targets are not applicable to
adoption acceptance. This is an explicit six-target applicability decision for
unchanged product/test/hook/tool/dependency sources, not a blanket exemption
inferred from one Darwin-only guard. Retain the unchanged full `make verify`
attempt, actual exit, every failure and existing skip. A failed aggregate stays
FAILED and is never relabeled as passing. Until execution occurs, the predicted
Darwin-only-harness failure is a source observation, not execution history. Its
unresolved full-suite/native/authentication acceptance stays open under its
original ownership.

This finite replacement changes only #103's source/configuration verification
applicability. It does not qualify product behavior, native authentication,
runtime project-role discovery or measured model benefit. Actual role/model
metadata remain unverified until observed. All behavior, build, tool, hook, test,
dependency and broader policy changes retain their existing OpenBSD/full-suite gates.

Preserve held work, resource/isolation/teardown, credential/access, signing and
publication boundaries. Gated merge permission applies only after all required
scoped gates above and separate review; no general future merge or operational
authority follows. The exact owner-approved proposal is retained in #103; this
decision delta requires separate review before final payload or signing.

## Terra product routing update — 2026-10-27

The owner authorized this workflow/model-routing update and integration after
review in [the owning issue](https://github.com/gonz0w/bootweave/issues/105). Select `temper_product` with
`gpt-6.1-terra` / `medium` from the recorded upstream pin. This adds product-facing
work to the selected routes; the existing workflow, implementer, analyst and
reviewer settings remain unchanged. The local role is
[temper_product](.codex/agents/temper_product.toml).

Use the product role only for accepted documentation, visual inspection, explicitly
authorized bounded browser tasks and small UI fixes with specified behavior and
observable acceptance. Substantial implementation stays with the implementer;
ambiguous design goes to the analyst. Existing risk, platform, execution and
separate-review requirements override a routine product assignment. The author
cannot supply its own required separate review.

For this role only, the lead may explicitly select `temper_implementer` with
`gpt-6.1-sol` / `high` as the one approved fallback when Terra is unavailable or
cannot perform the task with available tools. Record that selection and confirm
capability, tool availability, existing permissions and task/resource limits
before dispatch. Missing tools are not installed by choosing another model.
Retain stricter dispatch/risk rules and stop if the fallback is unsuitable. Other
routes keep their existing fallback policy; no automatic substitution, recursive
delegation, extra retry or higher limit follows.

Terra requires a supporting client/account; Codex CLI requires 0.159.0 or newer.
Start a fresh consumer session and inspect actual role/model/effort metadata
before claiming observed routing. Configuration and source review do not prove
runtime discovery, model execution or measured quality/cost benefit. These remain
unverified until observed. This selection changes no primary-chat model, CI,
hooks, trust, permissions, product/platform or consequential-action authority.

## One-time Terra routing integration decision

On 2026-10-27, Cam approved the reviewed one-time Terra routing integration
proposal for [this owning issue](https://github.com/gonz0w/bootweave/issues/105). Separate exact-source review supplies
acceptance for the declarative workflow/model/provenance patch. Required native
BSD checks and commit hooks are waived/not run for this patch only; SSH signatures,
genuine author DCO, PRs and separate review remain. One additional bounded routing
batch is permitted for this repository, ending at merge or abandonment. This
decision expires at that PR's merge or abandonment and authorizes no CI
infrastructure, product, release, deployment, other permission/protection change,
future bypass or runtime-routing claim.

No VM, runner, publisher, tool installation, extra retry or infrastructure startup
is included. Existing failed evidence remains failed; earlier exceptions stay
expired outside this exact Terra patch and policy recording.

## One-time Temper guidance integration — 2026-10-27

On 2026-10-27 the owner approved a one-time integration decision for this owning
Temper guidance-refresh issue. For this exact reviewed pin/provenance patch only,
the upstream read-only auditor and trusted passive source/inventory/hash/link/
role-preservation checks may run on inert coordinator copies. Separate source
review and those checks replace unavailable required BSD gates and commit hooks,
which are waived/not run, never passed. Existing SSH signatures, actual-author
DCO, PRs, separate review and canonical verification remain. No consumer check,
module, hook or product/native workload may execute on that host. The decision
ends at this PR's merge or abandonment and grants no future bypass, CI workflow/
infrastructure change, executor startup, model change, release or deployment.

Owning work: [issue #107](https://github.com/gonz0w/bootweave/issues/107).
