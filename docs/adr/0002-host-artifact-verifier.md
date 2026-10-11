# ADR 0002: host-only OpenBSD artifact verification

Status: Accepted for host artifact verification, 2026-10-11. Decision owner: Cam.
Related [issue #10](https://github.com/gonz0w/bootweave/issues/10),
[host-verifier review](../design/native-profile/host-verifier-review.md) and
[build/test evidence](../design/native-profile/host-verifier-evidence.md).
Acceptance date/source: 2026-10-11, maintainer replied “continue with issues”
immediately after the explicit ADR host-verifier/official-HTTPS key-bootstrap
approval request. This accepts the host artifact-verification EDD exception and
the stated HTTPS trust assumptions; it does not authorize guest execution.

## Problem and proposed decision

The current native-profile EDD requires an independently trusted OpenBSD base signify
before boot. No such environment is available in this session. A pinned macOS port
has completed a bounded local build and adapted synthetic regression experiment.
Propose allowing that exact reviewed host binary solely to verify artifact signatures
and checksums, with OpenBSD base signify remaining preferred when available.

Candidate: signify-osx 1.4 at source commit
9a654d698f5dc957ac407a3156a638bb3b77add2, archive SHA-256
5aa954fe6c54f2fc939771779e5bb64298e46d0a4ae3d08637df44c7ed8d2897.
Observed local binary SHA-256
4b9b2069c5e5acda9d35ca898ecd5afc050094170cf3c73fa7cf9eb615530946.
No moving HEAD, global install, guest/media inclusion or custom cryptographic code.

## Provenance, license and maintenance

Port LICENSE is ISC; upstream files retain individual notices. Reviewed build/test
inputs and representative notices do not establish exhaustive redistribution
clearance. Use locally only; redistribute neither tool nor bundled source in media.
The host shim disables pledge, so its behavior is not OpenBSD confinement evidence.
Compiler warnings and the OpenSSL checksum-label regression adaptation are recorded
in the experiment evidence; no verifier source changed to make tests pass.

Keep source/tool/receipts in ignored owned development storage. Updates require
new source/binary identities, build review and positive/negative tests; never update
silently. Remove this isolated tool directory to retire the dependency and return
to base-signify-only verification. No runtime architecture depends on it.
The actual local experiment allocation was under 1 MiB, not a general footprint
promise. Adoption must specify how complete file-level review is satisfied or bounded
local-use limitations are accepted.

## Separate key-bootstrap decision

Candidate OpenBSD 7.9 base public-key record:
RWTSdNN9A3yvWNn7mUjXwv9DOCOUnyfuV+mq1iGPIfD+NhN8EYnEQ1at

Fetched key-file SHA-256:
b7ee8e7981549a1f707a4ed05115a99959a5f9e34b9bed9f9a098be17dd7e4b1.
The key record matched the [official release page](https://www.openbsd.org/79.html)
and [official mirror key file](https://ftp.openbsd.org/pub/OpenBSD/7.9/openbsd-79-base.pub)
when inspected 2026-10-11. These are distinct official endpoints, not independent
organizational trust roots. Proposed bootstrap would explicitly trust official
OpenBSD HTTPS publishing and the host TLS/certificate infrastructure. It is weaker
than comparison against a previously trusted OpenBSD installation or an independently
held key. The verifier and key both still require acceptance; matching endpoints
alone cannot authorize privileged use.

Do not adopt bundled tool keys or a media-mirror key silently. Prefer a previously
trusted key if the maintainer supplies one; mismatch stops verification and needs
investigation. Any accepted key change must be reviewed as a new trust decision.
Public key material is public reference data; no private key is needed or created
for release verification.

## Actual consistency observations

Using the built binary and candidate key, performed a host-only read-only consistency
check of the already downloaded release source. Commands used signify -V -e with an
explicit key/signature/output, then signify -C with the explicit src.tar.gz selection.
Both returned success; the selected archive reported OK. A changed signed payload
returned nonzero and its temporary fixture was removed. No guest or source code ran.

Signature-file SHA-256:
8c23e45e4c09939151f119eddc3619a7fa2f09c8c745e00bfa4dfe46ddb09900.
Selected source SHA-256:
fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b.
These observations establish cryptographic consistency under an unaccepted candidate
key. They do not upgrade source findings to authenticated trust, verify install media,
compare base signify, or pass a native test.

## Alternatives, consequences and acceptance

Alternative: retain the existing requirement and arrange a trusted OpenBSD verifier
and key comparison. This avoids the host exception but needs an available independent
system. Booting unverified media to create the verifier would be circular and is
excluded. No third-party prebuilt binary or weak digest-only route is proposed.

If explicitly accepted, record the scope/date/source, amend EDD to permit this exact
host verification route, and rerun source/media checks under the accepted trust policy.
Host artifact checks remain separate from native compatibility. Guest launches require
frozen configuration, isolation and bounded-run authorization; disk installation
requires reviewed disposable-target confirmation. Adoption of this ADR grants none
of those effects. Until acceptance, the existing EDD remains in force.

Acceptance supersedes the proposal-time pending language above. Earlier consistency
observations remain historical; accepted-policy checks are recorded in the
[release-input evidence](../design/native-profile/release-input-evidence.md).
