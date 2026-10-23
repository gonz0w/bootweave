# Prospective OpenBSD8.0 baseline

The [official release page](https://www.openbsd.org/80.html) identifies OpenBSD8.0,
released2026-10-20, and its distinct base/package/firmware/syspatch signify keys.
This is the selected new direction. Existing7.9 artifacts, source and receipts
are historical evidence with their original scope; no version substitution
qualifies them for8.0.

On2026-10-23, bounded HTTPS reads of official small metadata, with no binary
acquisition, returned these candidate identities:

| Official location/name | Published checksum |
| --- | --- |
| [8.0/amd64/SHA256](https://cdn.openbsd.org/pub/OpenBSD/8.0/amd64/SHA256), `install80.iso` | SHA256 hex `bc85d747c6ee4354d14fbfeee38087500386a2e9d86fa7b5ab13d840d902aa33` |
| Same list, `base80.tgz` | SHA256 hex `ec4957f6fd81756412265a6d2f394268d61927c953ef0677a8a2b7dc1cd6afd2` |
| Same list, `bsd.rd` | SHA256 hex `40cddcb7a5f4320588c4ec7f49ae28702e400fe49c8c78d94f40ddf76e1e45ce` |
| Same list, `comp80.tgz` | SHA256 hex `96b7bc6fbc1343383e643601a9a6bb25753cd3207add4e9a124f27dd44565b8f` |
| [8.0/packages/amd64/SHA256](https://cdn.openbsd.org/pub/OpenBSD/8.0/packages/amd64/SHA256), `haproxy-3.2.25.tgz` | Published SHA256 base64 `m9Vv6TgTe1iSfz6IV/GcQmK8+fqdMMonuLGB3zvg69w=` |

The official package directory also lists that HAProxy filename. Metadata
transfer limits were20s/4MiB; the package index was2097503 bytes and package
checksum list1052438 bytes. No media/package/key was acquired, installed or run.
These are **observed candidate metadata**, not adopted runtime pins or verified
file contents. Signed manifest/key binding, actual downloaded artifact hashes,
selected complete set/package/compiler/loader closure, licenses and exact
dependency/account/chroot compatibility remain to be established before pinning
or building an operational packet. Never treat an HTTPS checksum read as signify
verification or proof of the unacquired image.

The [installation FAQ](https://www.openbsd.org/faq/faq4.html) describes full
`install80.iso`, set verification and the separate SHA256/SHA256.sig files. Media
authentication must be explicit before use; an install ISO's lack of embedded
signature material cannot silently authorize an unverified-set fallback.
Exact8.0 firmware/storage/install answers, userland confinement/descriptor/Wait
semantics, service lifecycle and actual product functions still need current
source-bound native qualification. No native hold or consumed grant changes.
