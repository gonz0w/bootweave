# ADR 0003: base-C read-only project planner

Status: **Proposed, 2026-10-15.** Decision owner: Cam.
Acceptance date/source: pending. Related [scoped PRD/EDD/Test Design](../design/project-plan.md)
and [issue #5](https://github.com/gonz0w/bootweave/issues/5).

## Problem and proposed decision

The installed-root research completed 1068 native checks and exact disposal, but
project authors still have no engine entry point. The research uses compiled
synthetic fixtures; it does not adopt a runtime, project definition or public API.
The next useful step is a real read-only loader/planner with a deliberately small
input contract, before attaching any write backend.

Propose original base C99 and OpenBSD base libc for `bootweave plan`, with the
version-1 restricted JSON definition in the scoped design. The planner reads a
bounded regular read-only standard-input file, validates metadata and emits a
plain-text unverified plan. It opens no payload/device path and grants no authority.
No third-party parser/runtime or new significant dependency is selected.

This adopts C only for this planner. It does not select the final renderer, service
adapter, privileged protocol, payload trust policy or persistent state format. The
JSON fields are project metadata for this read-only entry point, not an installer
execution schema. Changes to version-1 meaning require a reviewed version change.

## Evidence and limits

The [completed native research](../research/installed-payload-native-progress.md#completed-independent-readback-and-exact-disposal-2026-10-15)
establishes base-C dynamic/static builds, constrained processes, terminal handling
and bounded data processing under the exact OpenBSD 7.9 amd64 fixture. It supports
investigating this small engine implementation, not a JSON parser or production
planner qualification. Those new mechanisms need the design's separate tests.

JSON's syntax/reference is [RFC 8259](https://www.rfc-editor.org/rfc/rfc8259).
Duplicate rejection, exact fields, ASCII restrictions and limits are Bootweave
policy. The schema is intentionally smaller than the JSON value space. Implement
the reader originally under BSD-3-Clause; copy no external parser/code or RFC code
components. The native libc/API use adds no bundled library or capability database.

Native confinement is a proposed checked combination of base pledge/unveil, limits
and descriptor discipline, not a host stub or a claim of whole-session confinement.
No target provisioning or native execution follows from accepting this ADR; the
same feature draft must present its concrete implementation and exact native plan.

## Alternatives and consequences

Continuing only compiled fixtures postpones the project-author workflow. Base
shell would put parsing/process/error boundaries on the wrong side of untrusted
text. Python remains development-only. A third-party JSON library adds dependency,
provenance and maintenance scope; none is needed for this fixed bounded schema.
Another serialization format would still need ambiguity/version/resource rules.
JSON gives authors familiar syntax while keeping the accepted subset explicit.

An original parser is security-sensitive code. Keep it small, bounded and separate
from rendering; reject unknown/duplicate/coerced data and test independently before
native support. The ASCII and regular-file input limits are visible compatibility
limits, not silent fallback behavior. The planner cannot install or validate an
artifact merely because its project definition parsed.

Approval accepts this runtime/definition choice and implementation/host-test scope.
After approval, build in the same draft; review a larger contract change or the
fresh exact native target separately. Removing this planner requires no service,
network or product-state migration; no persistent state is introduced. Until
acceptance, existing proposed architecture and no-public-grammar status remain.
