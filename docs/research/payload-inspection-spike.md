# Bounded payload inspection: next implementation scope

Phase A accepted, 2026-10-12. Owner and acceptance authority: Cam.
Acceptance: Cam replied "yes,continue" to the explicit Phase A implementation
question after PR #38. Implementation base:
028a2e951cb1e42c6a0e44f51a674f46bea10b96 (merged PR #38).
Related [issue #5](https://github.com/gonz0w/bootweave/issues/5),
[trust requirements](../design/project-trust/PRD.md),
[engineering boundaries](../design/project-trust/EDD.md) and
[parent Test Design](../design/project-trust/TDD.md).
Base: ec43b2268b293659a9ca12d1f235f2e54a459e54, merged PR #37.
The implementation's [source binding](payload-source-findings.md) and
[host outcomes](payload-host-evidence.md) are recorded separately.

## Outcome and decision requested

Implement an original, removable C archive inspector in research/payload, with
synthetic host tests. It accepts only a bounded subset of uncompressed ustar and
reports whether the supplied bytes meet this experiment's structural/path policy.
An unrelated document-viewer fixture supplies static content; no DNS, firewall or
product runtime policy enters the experiment.

This is the first payload component, before authentication and extraction are
integrated. An accepted result means inspection passed, never trusted payload,
safe extraction, installed application or readiness. Approval is for Phase A
implementation and host checks only. It adopts no public payload/profile format,
runtime language, parser interface or privileged operation contract.

## Source orientation and implementation prerequisites

The release-specific [OpenBSD 7.9 tar manual](https://man.openbsd.org/OpenBSD-7.9/tar.1)
documents ustar output selection, duplicate-member overwrites during extraction
and partial files after interruption. The
[7.9 pax manual](https://man.openbsd.org/OpenBSD-7.9/pax.1) describes listing separately
from extraction and recovery attempts on damaged input. These were read on
2026-10-12; no tar/pax operation or archive extraction ran for this proposal.

Inference: a human-readable listing or exit status alone is insufficient for the
proposed strict admission boundary. The experiment will inspect raw headers and
content framing directly, without interpreting an archiver's rendered filenames.
This does not claim a defect in either base utility or select a replacement extractor.

Before coding the byte reader, reverify the 7.9 source archive using the
[accepted verifier/key policy](../adr/0002-host-artifact-verifier.md), inspect the
selected bin/pax format definitions/readers as bounded regular-file members and
record their exact digests, revisions and field layout. Do not copy or execute
upstream code. Stop on an unavailable or conflicting format definition; do not
infer offsets from another platform's tar. C/base libc are research candidates
already used in this repository; no dependency installation is requested.

## Phase A requirements and engineering bounds

The inspector reads stdin only and rejects caller arguments. It has no filesystem
output, path-opening, subprocess, shell, socket, plugin, decompressor or extraction
backend. Archive member names are data, never paths passed to an operation.
Use fixed diagnostic categories and a fixed numeric summary; do not print names,
content, owner names or raw malformed bytes. Exit 0 means the complete supplied
stream meets this policy; nonzero means rejected/incomplete, with no success
summary before the final input and trailer checks.

| Boundary | Proposed experimental rule |
|---|---|
| Framing | Validate complete 512-byte ustar headers, magic/version, unsigned checksum and declared content/padding. Require two zero end blocks and only bounded zero padding through EOF; reject truncation, concatenated archives and nonzero trailing bytes. |
| Numeric/text fields | Bounded octal parsing with checked arithmetic and reviewed terminators; reject binary/negative/overflow encodings and unsupported header metadata. Validate fixed fields before use. |
| Member types | Regular files and directories only; directories have zero content. Reject links, devices, FIFOs, sparse/extension records, compressed input and unknown types. |
| Paths | Combine validated ustar prefix/name once. Require relative ASCII components using letters, digits, dot, underscore and hyphen; reject empty, dot/dot-dot components, leading slash, controls, backslash and duplicate separators. A directory may have one trailing slash, removed for collision checks. Maximum combined path is 255 bytes. |
| Tree conflicts | Reject duplicate canonical paths and file/directory conflicts in either order, including a regular file used as another member's ancestor. Implicit parent directories are permitted; no host path resolution is performed. |
| Ownership/modes | Require numeric uid/gid zero, empty textual owner/group names and no link target/device metadata. Regular files use 0644, directories 0755; other modes reject. These are fixture header constraints, not authority to set ownership or permissions. |
| Work/resources | At most 1 MiB total input including headers/padding, 128 members, 512 KiB aggregate regular-file content, 256 KiB per file and 64 KiB application buffers. Reject empty archives and require at least one regular file. |
| Time/output | Monotonic ten-second whole-input deadline, bounded read/poll and five-second output deadline; at most 4 KiB trusted diagnostics. Never wait for an oversized declared body after detecting a bound violation. |

Freeze exact accepted field encodings after the authenticated source review, within
these rules. Compatibility widening, new member types, extraction, compression,
dependencies or public contracts require further review. Do not quietly accept
extensions because the host fixture generator supports them.

Compile with installed cc, C99, Wall/Wextra/Werror/pedantic and no third-party
libraries. Host Python generates owned synthetic archive bytes and supervises fixed
argument arrays; it is not a runtime dependency or the inspector's implementation.
Tests may create only owned temporary binaries, fixture bytes and canaries under
ignored .tools or the approved temporary directory. Proposed per-case process RSS
ceiling is 32 MiB, private capture 1 MiB, temporary allocation 20 MiB and supervisor
deadline 15 seconds. Measure actual peaks where available; record unavailable
metrics rather than infer a pass. Source ceiling is 32 KiB.

## Phase A Test Design

| Case | Independent expected observation |
|---|---|
| PAY-01: valid viewer fixture, reordered members, implicit/explicit directories | Complete admission, correct fixed counts/byte totals; no extracted files or name/content output. |
| PAY-02: checksum/magic/version/numeric mutations and truncated header/body/trailer | Nonzero exit, fixed bounded diagnostics, no admission summary or effects. |
| PAY-03: traversal/absolute/control/high-bit/backslash paths, prefix escape | Reject before admission; private/shell-fragment sentinels never appear in output or execute. |
| PAY-04: duplicate names, trailing-slash aliases, ancestor/type conflicts in both orders | Whole archive rejects; no last-entry-wins or path normalization escape. |
| PAY-05: every excluded type, link metadata, uid/gid/owner/mode mutations | Reject; no links, special files, privilege bits or executable effects created. |
| PAY-06: exact/below/above path, member, content and archive bounds; arithmetic extremes | Accepted boundaries where otherwise valid; bounded rejection for excess without allocation/body-read from untrusted size. |
| PAY-07: extra zero padding, one zero end block, concatenation and trailing garbage | Only complete reviewed framing admits; no unread second archive. |
| PAY-08: empty stream/archive, stalled input, partial writes, signals and broken/blocked output | Bounded incomplete/failure result, no false completion; owned endpoints/processes cleaned. |
| PAY-09: generated headers plus independently constructed byte fixtures | Format/framing checks are not tested only through the same encoder; inspection leaves input and owned canaries unchanged. |

Run all existing checks plus the new host groups through make verify. Record clean
base/head/tree, original source digests, compiler/interpreter identities, commands,
actual outcomes/resource limits and cleanup. No existing native result qualifies
this new reader. Parent PTT-04/05 remain incomplete: authenticity, extraction,
containment during effects, races and readiness have not been tested by Phase A.

## Later gates and stop conditions

Phase B needs a separately reviewed native build/read-only execution plan with
fresh exact fixture authority; previous targets were disposed. No guest launch,
media change, guest disk write or native qualification follows from Phase A.
The [prepared Phase B proposal](payload-native-plan.md) now identifies its one new
empty build target, frozen data and requested effects; acceptance remains pending.
Authentication/trust policy and immutable verified input lifetime precede any
future privileged parsing/use. Extraction additionally needs an accepted scoped
mechanism, target-root identity/authority, containment throughout effects, observed
ownership/modes and interruption/failure cases. Admission followed by unconstrained
tar extraction is explicitly insufficient.

Stop on unreviewed format behavior, inability to enforce bounds, unexpected effects,
cleanup failure or private data exposure. Do not retry through a permissive parser
or extractor. No hook, application execution, service, network, OS installation,
destructive confirmation, resume or rollback is included. Acceptance must be
recorded explicitly here before implementation; routine corrections inside an
accepted Phase A need no additional permission.
