# Generic project trust and privilege: requirements

Status: Proposed, 2026-10-11. Owner and acceptance authority: Cam.
[Issue #5](https://github.com/gonz0w/bootweave/issues/5), [EDD](EDD.md),
[Test Design](TDD.md). Acceptance date/source: pending explicit review.
This scope defines proposed boundaries, not configuration syntax or permission to
implement or execute privileged operations.

The narrower [read-only metadata planner](../project-plan.md) is now accepted and
implemented with portable host evidence. Its limited version-1 definition does not
adopt the broader profile, extension or privileged-operation contracts below.

## Author and operator outcomes

Independent OpenBSD projects can supply branding, bounded questions, verified
payloads and product-owned bootstrap without forking the engine. Operators can
inspect the exact proposed effects and authorize a specific target while rejected
inputs and failed operations produce safe, honest diagnostics.

Use an unrelated offline document-viewer application as the primary synthetic
fixture: project branding, a bounded content choice, a verified static payload and
an independent readiness observation. DNS peer pairing and firewall interface roles
are contrasting author scenarios only; their policies, credentials and ongoing
state belong to those products. No shared DNS/firewall assumptions may enter the
engine. No actual service names, configuration grammar or wire protocol are defined.

## Requirements and acceptance

| ID | Outcome |
|---|---|
| PT-01 | Invalid, ambiguous, oversized or unsupported definitions reject before effects; authenticity alone grants no execution authority. |
| PT-02 | Planning has no write authority and presents full target/effect summaries; changes invalidate prior confirmation. |
| PT-03 | Privileged effects use explicitly authorized typed operations with revalidated arguments and stable target identity, never input-to-shell interpolation. |
| PT-04 | Verify payload identity/trust before use and constrain extraction to the authorized target, paths, types, owners and permissions. |
| PT-05 | Secret input is confined to its authorized consumer/lifetime and excluded from argv, ordinary logs, summaries and evidence. |
| PT-06 | Untrusted text cannot emit terminal controls or spoof prompts; diagnostics are bounded and export requires safe preview. |
| PT-07 | Report planned, performed and independently observed state, including uncertain completion and failed readiness. |
| PT-08 | Executable build/bootstrap extensions require separate code review, provenance and bounded authority; definitions cannot smuggle executable hooks. |
| PT-09 | Generic, DNS and firewall author scenarios use the same proposed engine boundaries; runtime product policy remains external. |
| PT-10 | Native safety evidence, accepted scoped designs and dependency decisions precede implementation/support claims; debug mode changes no controls. |

## Scope and budgets

This design covers trust boundaries and acceptance goals. Parser format/versioning,
runtime language, privileged transport, concurrency model, package policy, product
adapter interface and persistent receipt format are unresolved. Do not implement
these interfaces from conceptual operation descriptions alone. Upgrade, automated
resume, fleet management and general disk rollback are outside this scope.

Proposed test ceilings: 1 MiB definition input, 128 questions, 1 KiB per ordinary
field, 64 KiB rendered diagnostic buffer and 10 MiB private capture. These bound
future stress tests, not an adopted schema or measured runtime promise. Payload
size/extracted bytes, file count and operation deadlines must be explicitly budgeted
per accepted payload fixture before use; absent limits reject it. Retain the native
profile's guest/disk/run limits and measure actual resources.

## Evidence and acceptance gates

Native execution depends on the [first profile](../native-profile/PRD.md), target
confirmation and the [installer research](../../research/installer-integration-plan.md).
Console behavior coordinates with [UI research](../../research/text-ui-runtime-plan.md).
These documents remain proposed; merging documentation does not authorize disks,
services or networks. Acceptance must identify the exact implementation scope and
remaining design decisions. No runtime code or native tests exist for this design.
