# Bounded payload authentication experiment

Accepted, 2026-10-12. Owner/acceptance authority: Cam.
Acceptance: Cam replied "yes,continue" after PR #43 and the explicit host-only
implementation/test-signing question. Implementation base:
d58418b832c21bba61fec84020e7f793b599898e, merged PR #43.
Base: 5b2526eacbed11976bfa292eac5dbbc737fbf096, merged PR #42.
Related [issue #5](https://github.com/gonz0w/bootweave/issues/5),
[trust requirements](../design/project-trust/PRD.md),
[engineering boundaries](../design/project-trust/EDD.md) and
[Test Design](../design/project-trust/TDD.md).

## Decision and intended result

Authorize one host-only experiment in research/payload: original Python development
code captures a bounded synthetic payload once, verifies its independently expected
identity and detached signature, then supplies those same immutable bytes to the
unchanged C inspector. Python remains host tooling. No installer runtime, public
API, profile grammar, production signer policy or privileged operation is adopted.

Approval also permits the exact existing signify port to generate two disposable
test key pairs and sign synthetic fixtures. This extends
[ADR 0002's verification-only use](../adr/0002-host-artifact-verifier.md) solely for
this experiment. No dependency installation, update or redistribution. Test keys
grant no production authority and never enter Git or PR evidence. Scope covers
implementation and host tests only, with no guest, disk, extraction, application,
hook, service or network execution. Native work needs a fresh reviewed plan and
exact-target authority; earlier targets were disposed.

## Source facts and prerequisites

The [OpenBSD 7.9 signify manual](https://man.openbsd.org/OpenBSD-7.9/signify.1),
read 2026-10-12, distinguishes detached verification from embedded-message output,
explicit public-key selection from comment-based hints, and signature integrity
from checksum-list verification. This experiment proposes detached verification
with an explicit key. A signature alone does not establish compatibility/freshness.

The local signify-osx 1.4 binary was rehashed and matched ADR 0002:
4b9b2069c5e5acda9d35ca898ecd5afc050094170cf3c73fa7cf9eb615530946.
Reviewed source commit: 9a654d698f5dc957ac407a3156a638bb3b77add2.
Local src/usr.bin/signify/signify.c SHA-256:
96006fa573cf1cdeae4b96682ced2ade7750830984346d009bd88e07e942d264.
Read-only inspection of xopen/readmsg/verifysimple found this port duplicates stdin
for message `-`, reads to EOF and uses separate key/signature files. Its own message
ceiling exceeds this scope; the controller must enforce its cap first. No signing
or key generation ran during proposal preparation. The accepted experiment's
[host outcomes](payload-authentication-host-evidence.md) are separate from these
source observations and do not establish native behavior.

Before implementation execution, recheck identities/provenance, review generation/
signing paths and retained file notices, and test detached stdin transport with
owned positive/negative fixtures. The port's ISC license and disabled pledge shim
retain ADR 0002's local-use limitations. Stop on mismatch, unsupported transport or
unexpected effects; no replacement download, custom cryptography or permissive retry.

## Requirements and engineering boundaries

Trusted harness context supplies immutable expected length/SHA-256 and pinned test
public-key bytes independently of archive/signature data. Never derive trust from
comments or archive metadata; a second valid signer rejects. These are fixture
expectations, not production compatibility/version policy.

Capture at most 1 MiB into one immutable Python bytes snapshot with bounded reads,
EOF and a ten-second deadline. Standard-library hashing compares length/digest to
trusted expectations; a digest alone grants no authentication. Reject arguments,
oversized/unfinished input, malformed context and missing/excessive key/signature
before inspection. Key and signature each have a 2 KiB cap.

Keys/signatures live only in exclusively owned 0700 temporary directories under
ignored .tools; files are created exclusively with mode 0600. Copy pinned public-key
bytes and the bounded signature into the controller's private files. Feed the
snapshot through a pipe to the exact verifier with fixed argument arrays, explicit
key/signature, message `-` and detached quiet verification. Trusted tooling owns
executable identity/environment. No shell, PATH search, comment-based key choice,
archive-derived filename, environment option, callback or network lookup.

Only complete successful verification permits the inspector child. Feed it the
same retained snapshot through a new pipe; never reopen the original payload path
or recapture stdin. Preserve inspector source/policy/deadlines/buffer limits.
Completion requires its exact fixed numeric summary, successful exit and cleanup.
Authenticated malformed archives still reject.

Supervise nonblocking I/O and reap every child. Per-child deadline: fifteen seconds;
whole workflow: forty-five seconds. Aggregate child stdout/stderr cap: 4 KiB. Keep
verifier diagnostics private; ordinary output uses fixed categories without comments,
names, content or key material. Failure, signal, timeout, broken output or cleanup
failure stops dependent execution without unsigned retry. Signing/generation use
equally bounded supervision.

Proposed host ceilings: controller RSS 64 MiB, each child 32 MiB, temporary allocation
20 MiB, private capture 1 MiB and original controller source 32 KiB. Independently
measure available RSS and disclose missing metrics. These do not revise inspector
limits or establish native footprint. Private key bytes never enter argv/environment/
logs; only owned key-file paths go to the signer. Disable owned-child core output
where supported. Dispose keys on success/failure/interruption. Deletion is lifetime
cleanup, not physical erasure or protection from host admins or same-user compromise.

Trusted host processes/private directories are the research boundary. This cannot
establish privileged key-file race resistance, immutable executable references,
OpenBSD pledge/unveil behavior, debugger/memory protection or safe extraction.
A production handoff needs separately scoped native design and evidence.

## Test Design and completion evidence

| Case | Independent observation |
|---|---|
| PAV-01: expected viewer, selected signer, correct signature | Verification precedes inspection; exact counts, same bytes and no member effects. |
| PAV-02: tampering, wrong expected length/digest, missing/corrupt/truncated signature, wrong signer | No inspector launch/success; fixed bounded failure and no fallback. |
| PAV-03: comment with controls, different key hint or shell fragment | Explicit key unchanged; supplied text never executes or appears in ordinary output. Malformed records reject first. |
| PAV-04: signed, identity-matched malformed archive | Authentication may pass; inspector rejects without extraction/completion. |
| PAV-05: original source replaced/mutated after snapshot/verification | Trusted barriers change the original; both children still receive retained bytes. Pre-capture mismatch rejects. |
| PAV-06: empty/exact/excessive/unfinished input, excessive key/signature, arguments/environment injection | Bounds/authority enforced before dependent execution; deadlines grant no completion. |
| PAV-07: controlled child failure, stalled/broken/full pipes, signal, excessive output | No false completion; children/pipes/fixtures cleaned, no secret/control-bearing diagnostic export. |
| PAV-08: canary, owned inventory and key lifetime | Canary unchanged, no member opened/executed; permissions checked and task-owned keys/files removed. |

Controlled process doubles are reviewed test code, never selected by payload input
or a runtime flag. Real signature cases use the exact verifier. Observe child
launch/order and received bytes independently of controller return values; mock
success is never cryptographic evidence. Preserve all twelve existing payload
groups and the exact 35 historical native fixtures.

Run focused tests and make verify on a clean signed/DCO candidate. Record commands,
versions, source/base/head/tree, tool digests, actual outcomes/resources/failures and
verified cleanup; publish sanitized evidence only. Host success neither completes
PTT-04/05 nor qualifies native aliases, signer rotation/revocation, freshness policy,
privileged input lifetime, extraction/races, readiness or production use.

The acceptance above authorizes this bounded experiment, without adopting its
mechanisms for production. Routine fixes within accepted scope need no repeated approval;
changes to effects, trust or bounds return to review.
