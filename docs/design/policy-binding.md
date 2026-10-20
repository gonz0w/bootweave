# Internal compiled-policy and digest binding

Status: delegated portable continuation of issue #5 after PR #82, 2026-10-20.
This refinement is reviewed before implementation under the adopted
[authenticated read-only scope](authenticated-plan.md). It adds no trusted entry,
CLI, verifier launch, archive inspection, rendering or write authority.

## Supported result and trust boundary

Add an internal bounded C component that compares an immutable retained public-key
span with one compiled policy, hashes the exact retained definition bytes, parses
those same bytes with the unchanged strict v1 parser, compares project id and
release/architecture, then hashes the complete retained payload against the parsed
payload digest. A policy match is only that component result. It is never a
signature acceptance, authenticated review, archive safety result, transferable
allow token or operation authorization. The independently required fixed signify
stage and complete structural inspector remain separate. Their future trusted
composition must hold the same immutable bytes throughout all stages; caller
supplied result objects cannot substitute for that ownership proof.

The request has only const byte spans and lengths for key, definition and payload.
No filename is opened; the parsed payload filename remains a label. The result
contains a fixed cause enum and the validated plan only after complete matching.
Rejection zeros the disjoint result before setting a fixed cause. Null request or
result, invalid positive-length null span, pointer-range overflow,
overlap among request/inputs/output, or overlap with the
compiled policy rejects without writing. The caller owns valid accessible storage,
exclusive output and immutable input lifetime; numeric range checks do not prove
mapped-memory validity or exclude concurrent mutation. No allocator, callback,
descriptor, syscall, environment, path or persistent state is introduced.
The future trusted entry copies the compiled key into a separate bounded 1 KiB
immutable span, then uses that exact copy for material preparation, signature
verification and this binding stage. Direct aliases into compiled policy are
rejected. This copy remains within the existing two-MiB application-storage cap.

One separately compiled const policy object pins an exact signify public-key
representation, definition SHA-256, project/system tuple and the fixed read-only
resource profile. The first anchor is visibly inert synthetic format-only public
material, with no private key or production signer. The compiled synthetic viewer
definition pins a known synthetic payload. Distribution/key enrollment, rotation,
real publisher selection and online freshness/revocation remain outside this
slice. There is no policy selector or runtime policy-file input. Tests replace the
const object only by linking a separate fixture; no injection API enters product
sources or production Makefile inputs.

## Hashing, bounds and failure

Use OpenBSD base libc SHA256Init/Update/Final in a fixed adapter, not a new SHA or
signature implementation, external command or dependency. The native adapter
returns unavailable on other hosts. Portable tests link a distinct Mac base
CommonCrypto adapter and compare its real SHA-256 results with independent Python
hashlib vectors. This models native adapter integration, never OpenBSD behavior.
The normal non-OpenBSD object must reject as unavailable. Neither adapter accepts
a runtime algorithm/tool choice. No Ed25519 implementation is added.
Reuse the existing pure record grammar through a format-only public-key wrapper
in auth_material.c; no dummy signature or duplicate grammar. That linked object
also contains dormant material I/O routines. Audit the binding object's actual
references and call sites rather than claiming the entire fixture has no I/O
symbols. The wrapper proves representation only, never cryptography or trust.

Enforce the adopted 1024-byte key, 16384-byte definition and 1048576-byte payload
ceilings before reading/hashing spans. Empty definition/payload/key rejects. The
compiled profile fixes the existing archive policy; no request can widen it.
Profile 1 means exactly: key1024, definition16384, archive1048576, members128,
total content524288, per-file262144, path255, depth16, directory0755,
regular0644, numeric UID/GID0, zero owner/group/link fields and complete restricted
ustar validation. Only the version1 profile discriminator is stored; no individual
limit is caller-configurable. A different profile, malformed compiled public-key
representation or noncanonical/unsupported tuple refuses before hashing. Digest
pins are fixed 32-byte arrays with no pointer or caller-selected digest length.
Order is argument/profile checks, exact key comparison, exact definition digest,
strict parse, tuple comparison, complete payload digest. All digest bytes and
claimed hex characters are compared; no prefix or reserialization acceptance.
Fixed causes distinguish key, definition digest, syntax, tuple, payload digest and
unavailable digest implementation. Failure exposes no partial plan. No cache or
earlier match is reused across calls. SHA-256 matching is relative identity, not
publisher trust or latest-release freshness.
The future trusted caller must check cancellation and its original remaining
overall budget immediately before and after this bounded pure stage; it installs
the already-required hard timers. This API performs no timer/cancellation syscall
and does not reset any deadline or publish a match after caller cancellation.

New C/header sources are capped at 8 KiB and whole src remains at 64 KiB; prior planner,
capture, material and signature ceilings remain separately enforced. New explicit
application workspace is capped at 2 KiB, excluding the existing caller-owned
snapshot and parser workspace. The parent two-MiB application-storage and all
native runtime/timer/resource ceilings stay unchanged. This pure bounded increment
adds no native RSS or wall-clock qualification claim.

## Behavioral Test Design

| ID | Independent stimulus and required result |
| --- | --- |
| PB-T01 | Exact viewer, DNS and firewall fixtures: complete plan equals independently parsed values; matching does not inspect an invalid archive or assert signature/operation success. |
| PB-T02 | Reordered/whitespace-equivalent metadata, one-byte definition change, old pin, key content/comment/length change: refuse at exact identity boundary. |
| PB-T03 | Separately compiled pins for malformed/duplicate/unknown/trailing/control-byte definitions and cross-project/release/architecture tuples: strict parser/tuple refusal, no partial plan. Existing v1 accepts only7.9/amd64, so another requested system tuple is a syntax/semantic-parse refusal; an unsupported compiled system tuple refuses before hashing. |
| PB-T04 | Payload changes, truncated suffix, extra zero, first/last digest-byte differences, full-cap payload and binary NUL bytes: compare full bytes/digest with hashlib. Uppercase claimed digest digits are existing-parser syntax refusal, never normalization. |
| PB-T05 | Null/empty/over-cap spans, pointer overflow, all request/input/result overlaps, compiled-policy overlap, unsupported profile and hash failure: fixed rejection, canaries and aliased storage unchanged, no attempted hash for preflight failures. |
| PB-T06 | Known SHA-256 empty/abc/multi-block vectors and padding boundaries through the linked Mac adapter; normal non-OpenBSD adapter reports unavailable. No key generation, signify launch, path lookup or network. |
| PB-T07 | Repeated match/refusal/match on the same process produces complete independent results; product binary/Makefile remain unchanged and binding objects have only reviewed library/parser/hash symbols. |

The new owned fixture retains builds, inputs, partial outputs and results. It uses
finite compile/case counts, per-child wall/CPU/fd/core/file limits, a replaced
minimal environment and bounded output. Full make verify remains mandatory in a
fresh finite owned envelope with no inherited credentials; pre-existing legacy
authentication tests retain their separately documented synthetic-fixture scope.
The fixture admits at most64 linked policies and320 invocations. Compiler children
have CPU20/fd64/16MiB-file/20-second limits; byte drivers have CPU2/fd32/16KiB-file/
4-second limits, CORE0 throughout, with a five-second owned cleanup reserve.
Nonblocking drains retain at most16KiB per stream/32KiB combined; overflow closes
the pipe and fails. Exact non-reaping completion precedes one consuming wait;
any needed direct-child stop occurs while that born child remains unreaped.
Failure persists bounded streams, hashes, pipe dispositions and exit/uncertainty.
Unresolved children cannot pass and remain for the outer held group's final stop.
Known output-overflow, timeout and partial-nonzero controls test the launcher;
their intentionally failed rows remain labeled separately from successful cases.
Independent design, source and terminal review, signed/DCO contributor identity,
exact candidate/base/tree checks and normal PR/canonical gates precede merging
this portable source increment. Native and full authenticated-product acceptance
remain unrun and need fresh exact authority.
