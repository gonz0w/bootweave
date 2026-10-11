# Native tooling provenance and media trust review

Status: Partial review, 2026-10-11. Owner: Cam. Related
[issue #10](https://github.com/gonz0w/bootweave/issues/10) and [preflight](preflight.md).
No dependency adoption, trust bootstrap or guest execution is approved here.

## Installed package findings

The local package receipt records QEMU 11.1.1, ARM64, stable homebrew/core,
built as a bottle and poured from a bottle. The receipt has no tap Git revision.
This establishes the package manager's local account of origin, not independent
verification of the original bottle or its firmware/source correspondence. Raw
receipt paths/host details remain private. Reinstallation and package changes are
outside this review.

The installed LICENSE states that the emulator is GPL version 2 overall, with
file-specific compatible licenses, and that bundled firmware consists of separate
programs with separate licensing. TCG component licensing does not relicense the
whole emulator. No QEMU code or binary is copied into Bootweave or its media.
The [upstream license overview](https://www.qemu.org/docs/master/about/license.html)
corroborates the emulator license; its moving documentation version is not an
installed-version provenance record.

| Installed record | SHA-256 |
|---|---|
| LICENSE | 139c64b7a9ae62858ff969aae622ecfcf4f1519655bd92d2e70a1a2fb615690e |
| COPYING | dd3ce02338c3a48abb6ba59b48809f7108a8bd242cb0cc8be90daafa30707c28 |
| COPYING.LIB | 31c90ce76b6f5aab90a205851e71d5c27e31c0aa3d7017a4383b98a6fe3f1faa |

These hashes identify reviewed local records; they are not publisher authentication.
The existing executable/firmware byte identities are recorded in [preflight](preflight.md).
The exact source revision, build inputs and firmware-specific notices corresponding
to those bytes remain unresolved. Upstream SeaBIOS and version-tagged firmware source
retrieval attempts did not yield usable records during this review. Do not infer a
firmware license from filenames or the emulator's GPL notice.

Before approving the local backend: bind the installed bottle to reviewed package
metadata/source provenance, identify the selected firmware source revision and
license notices, and document its local-only use/removal path. Using an existing
emulator for disposable research does not select an installer runtime dependency.
Any future redistribution requires a separate artifact-specific license review.

## Media verification routes for review

The accepted procedure has not yet been recorded. The current proposed EDD requires
OpenBSD base signify in an independently trusted OpenBSD environment before booting
downloaded media. A verifier was not found on the ordinary host PATH. No trusted
OpenBSD environment/access has been supplied in this session.

Route A preserves that requirement: use an existing independently trusted OpenBSD
system and its established release-key provenance. Verify exact downloaded media
with base signify against signed release checksums, retain sanitized verifier/key/
artifact identities and outcomes, and ensure transferred local bytes match the
verified digest. A negative test uses a corrupted copy which never boots. Do not
publish access credentials, private host identities or raw transfer logs.

Route B would revise the design: review a host verifier's source, dependency/license,
provenance, key bootstrap and signature/checksum behavior, then amend EDD/Test Design
and record explicit acceptance before using it. No verifier is selected, installed
or implemented by this document. A host verifier result would qualify media trust
only; it would not establish OpenBSD runtime compatibility. Do not use ad hoc custom
cryptographic code as a shortcut.

The [7.9 release record](https://www.openbsd.org/79.html) identifies release keys and
[7.9 signify(1)](https://man.openbsd.org/OpenBSD-7.9/signify.1) documents signature
and checksum verification. These are reference inputs, not an independently accepted
trust root. A key fetched alongside an image from the same mirror is insufficient
by itself. Review trusted key acquisition separately. No media/signature was
verified in this review; key strings are deliberately not adopted here.

## Next evidence and stop conditions

Resolve the verifier route and key provenance, then verify exact install79.iso and
signed checksums. Freeze local bytes after transfer. Complete package/firmware
review, explicit machine/CPU/devices, VGA/serial endpoints, effective attachment
inspection, limits and cleanup. Record explicit bounded non-install execution
acceptance. Remaining fields cannot be filled with inferred successes.

No guest, fixture disk, service or network change occurred. No native teardown was
needed. Local documentation verification does not satisfy native test cases.
