# Generic project trust and privilege: engineering design

Status: Proposed, 2026-10-11. Owner: Cam; acceptance pending.
Related [PRD](PRD.md), [Test Design](TDD.md) and the
[staged adoption decision](ADOPTION.md).

## Boundaries and authority

| Input/boundary | Proposed enforcement owner |
|---|---|
| Definition bytes to validated project intent | Unprivileged loader with bounded parsing, structural/semantic validation and explicit compatibility rejection. |
| Intent/operator values to plan | Planner with no write capability; validators do not execute project code. |
| Plan to privilege | Trusted authorizer binds reviewed effects, target identity and input/artifact identities; executor independently revalidates. |
| Verified payload to filesystem | Trusted extraction boundary constrains target root, members, links, types, owners, permissions and resource use. |
| Reviewed bootstrap code to product state | Separately authorized adapter with explicit operation scope, resources and independent readiness checks. |
| Any text to terminal/diagnostics | Trusted renderer/log boundary escapes controls, suppresses secrets and bounds output. |

These are responsibilities, not selected processes, IPC messages, service names or
serialized schemas. Choose mechanisms only after runtime/native evidence and scoped
design review. A signed definition is authenticated input; it cannot widen authority.
Unknown/duplicate/conflicting fields, malformed encoding and unsupported versions
must not acquire meaning through defaults or coercion. Whether a chosen format can
meet these requirements is a parser research question, not a format decision here.

## Typed execution and target identity

Operation categories include OS installation, verified artifact placement and
reviewed product bootstrap. Their concrete types, allowed arguments and sequencing
need separate design. Never accept arbitrary command lines, shell fragments, tool
paths or executable validators from project input. Use trusted executable identities
and validated argument arrays if a subprocess is necessary; shell quoting alone is
not the privilege boundary. Validate paths, lengths, ranges and enumerated choices
again where authority is exercised. Reject unrecognized operations before effects.

Bind authorization to the exact validated plan, verified artifact identities and
stable disk attachment mapping. Re-observe identity immediately before destructive
use; mismatch or changed plan invalidates confirmation. Review an explicit effect
summary and require a fresh action; no timeout/default/paste can authorize it.
UI mode, debug flags and a project's signature cannot supply operator authorization.

Address target substitution and time-of-check/time-of-use races in the chosen
implementation, including reopened paths, changed mounts and symlinks. Use a
reviewed stable reference or equivalent revalidation supported by native evidence;
do not claim ordinary path normalization solves races. If identity cannot be proved,
stop without proceeding. Planning may collect read-only observations but cannot
invoke installer defaults that might write.

## Payload verification and extraction

Require explicitly reviewed signer/trust policy, expected artifact identity,
compatibility and resource limits. Authenticate before parsing/extracting payload
contents with privilege. Digest checks alone do not establish publisher trust.
Offline trust failures cannot trigger unsigned or public-network fallback.
Determine source/package signature mechanisms separately from this boundary design.

Before extraction, constrain destination-relative paths and reject traversal,
absolute members, target-root escapes, dangerous symlink/hardlink chains, duplicate
or conflicting entries and unsupported special file types. Ownership/mode effects,
including setuid/setgid and device nodes, need explicit scoped policy rather than
archive defaults. Re-check containment during effects to prevent races; bound
expanded bytes, member count and processing duration, not only compressed size.
A scan followed by unconstrained extraction is insufficient. Exact native extraction
mechanism and atomic staging feasibility remain unresolved. Failure may leave partial
files: observe/report them; do not claim transactional rollback.

## Secrets, output and executable extensions

Separate secret-bearing values from display/diagnostic fields before logging. Pass
secrets only through a reviewed channel to the authorized consumer; avoid argv,
environment leakage, xtrace and raw-answer capture. Necessary secret files require
restrictive ownership/mode, bounded lifetime, interruption cleanup and storage
limitations documented. Do not promise physical erasure from flash or snapshots.
Test memory/core-dump exposure and available native mitigations before making claims.
Use synthetic values in fixtures; no shared production password is defined.

Escape/reject untrusted terminal controls and bound text before presentation or
capture. Keep full target/effect information readable in plain-text and serial
modes. Support exports contain only selected safe fields after operator preview;
no telemetry/upload callback. Debugging cannot enable raw secret logs or bypass
validation, verification, confirmation or resource limits.

Build hooks and product bootstrap adapters are executable trusted code, separately
reviewed and pinned with file-level licenses/provenance. Definition signing does
not approve them. Their authority must enumerate accessible targets, effects,
inputs, resources and readiness observations. Chroot alone is not confinement.
Native privilege separation/confinement mechanisms remain to be evaluated; don't
substitute host behavior for OpenBSD evidence. DNS pairing/firewall policy and
product lifecycle remain within their owning adapters/projects.

## State and failure

Maintain distinct intended effects, actions actually submitted/completed and
postconditions independently observed. A successful subprocess or hook exit is
insufficient. Readiness tests must detect the intended installed artifact/configuration
and product-specific function rather than assume a generic daemon start means ready.
Mark observation failure or process loss as uncertainty, not success or automatic
retry. Receipt encoding and persistence authority require later design.

No concurrent target mutations are permitted by this proposed acceptance scope;
actual synchronization/exclusion must be designed before implementation. On failure,
stop dependent actions, bound diagnostics and preserve safe evidence. Do not resume,
retry destructive work or install onto an existing disk implicitly. Recovery explains
remaining observed/unknown state and manual inspection or fixture disposal choices.
No general rollback claim is made.

## Remaining design and adoption

Resolve parser/runtime/renderer, privilege transport, stable target handles,
confinement, extraction tooling, signing trust, secret delivery and receipt lifetime
with native evidence. Prefer base tools; new significant dependencies require license,
provenance, review and ADR. This PR adopts no dependency or copied upstream code.
Each implementation slice needs accepted scoped PRD/EDD/Test Design or an explicit
bounded spike, exact allowed effects and a disposable fixture. Public contracts and
migration rules require separate review before profile syntax is published.

The [first read-only project planner](../project-plan.md) and
[ADR 0003](../../adr/0003-read-only-project-planner.md) are accepted for that narrow
metadata feature; PR #75 completed its bounded native qualification and teardown.
They do not adopt parent privileged interfaces. The [staged adoption decision](ADOPTION.md)
and [authenticated read-only continuation](../authenticated-plan.md) now identify
the next contract for review without promoting a research writer.
