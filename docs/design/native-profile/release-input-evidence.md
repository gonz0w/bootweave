# Verified OpenBSD 7.9 release inputs

Observed 2026-10-11 for [issue #10](https://github.com/gonz0w/bootweave/issues/10).
Trust policy: [accepted ADR 0002](../../adr/0002-host-artifact-verifier.md).
Host artifact verification only; no media boot or native compatibility result.
Acceptance source: maintainer's “continue with issues” immediately following the
explicit host-verifier/official-HTTPS key-bootstrap adoption request.

## Verification inputs and results

Rechecked the exact host binary and key-file SHA-256 against the ADR identities
before use. Verified src.tar.gz and amd64 install79.iso separately using signify -C,
explicit public-key path, each release SHA256.sig and explicit selected filenames.
Both commands returned zero, verified the signature and reported the selected file OK.

| Artifact | Bytes | SHA-256 |
|---|---|---|
| Release src.tar.gz | 204451355 | fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b |
| Source SHA256.sig | 861 | 8c23e45e4c09939151f119eddc3619a7fa2f09c8c745e00bfa4dfe46ddb09900 |
| amd64 install79.iso | 798625792 | 7a4a92e953618035097c796a90b54424a0f3ae775552e1e7d102cf8a5130449f |
| amd64 SHA256.sig | 2144 | 99db2ba3d63cddeeb9a1166319e48bf4b22224c48bbdc4deff8ecadc0f7b2786 |

Source/media came from official 7.9 release HTTPS locations; the key-bootstrap
assumptions and exact key identity are in the ADR. These are not independent
out-of-band authentication or base-signify cross-check claims. Tool/compiler and
adapted regression details are in [host evidence](host-verifier-evidence.md).

Re-read the three source members listed in
[installer findings](../../research/installer-source-findings.md) from the verified
archive and compared exact bytes with the inspected temporary source files: all
matched. Those source observations now bind to verified release inputs under this
accepted policy; inferred behaviors still require native confirmation.

## Negative controls and storage

Changed a copy of the signed checksum payload: signature verification rejected it.
Created an owned copy-on-write clone of install79.iso, changed one byte and checked
it against the original signed checksum: rejected. Neither modified artifact was
executed or mounted. Temporary negative-control directories were removed.

The image stayed below the 2 GiB media limit. Source, original image, verifier and
private receipts remain in local ignored/temporary research storage, not Git.
No fixture target disk, guest endpoint or running emulator exists from this work.
Downloaded bytes must be revalidated immediately before future use, especially if
paths or identities change. Digest records alone do not authorize a launch.

## Remaining acceptance

Artifact-verification EDD exception is accepted narrowly. Native profile, exact
machine/firmware/device/console configuration, isolation, execution resource checks
and bounded non-install run authority remain pending. No source script, installer
or boot image has executed; no services/live networks changed. Source findings do
not establish renderer, payload containment, privileged operation or readiness tests.
