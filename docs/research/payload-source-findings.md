# Payload header source binding

2026-10-12, accepted [Phase A](payload-inspection-spike.md). The existing host
verifier, public key and signed manifest matched the identities in
[ADR 0002](../adr/0002-host-artifact-verifier.md) before use. `signify -C` verified
the signature and src.tar.gz, returning zero. The archive retained SHA-256
fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b.
Only bounded regular-file members were read into fixed private review filenames;
no general extraction or source execution occurred. No upstream code is copied.

| Release member | Revision | Bytes | SHA-256 |
|---|---|---:|---|
| bin/pax/tar.h | 1.9, 2014-01-08 | 5529 | ccf342e3293fea9c460aff53ee46c314fd6cfbbba3bbac393c39b74f0c8c70e9 |
| bin/pax/tar.c | 1.87, 2025-07-06 | 42611 | a82cc48dd4268768153a723b17347f84be01f4adde38cd4dc21b76c814d06a1a |
| bin/pax/gen_subs.c | 1.34, 2024-04-27 | 9707 | 7cf2bee797fb23dda622da3da8dbba5cb254a37535eefbcfd17fae31f837a281 |

tar.h defines 512-byte framing, two zero trailer blocks, checksum offset 148/length
8, ustar magic/version and HD_USTAR fields. tar.c 272–305 sums unsigned bytes with
eight spaces replacing the checksum. Its reader joins prefix/name, supports
additional member types/extensions and strips a directory's final slash. Its
writer zero-fills headers and uses octal numeric fields; ul_oct/ull_oct show
NUL/space termination variants. gen_subs.c contains bounded-field copying and
permissive numeric readers. Those functions remain upstream administrative tools;
their broader behavior is not this experiment's admission policy.

## Frozen experimental encodings

Offsets are byte positions, independent of compiler struct layout:

| Field | Offset | Length |
|---|---:|---:|
| name | 0 | 100 |
| mode, uid, gid | 100, 108, 116 | 8 each |
| size, mtime | 124, 136 | 12 each |
| checksum, type, linkname | 148, 156, 157 | 8, 1, 100 |
| magic, version | 257, 263 | 6, 2 |
| uname, gname | 265, 297 | 32 each |
| devmajor, devminor | 329, 337 | 8 each |
| prefix, unused header tail | 345, 500 | 155, 12 |

Require exact `ustar` plus NUL and version `00`. Numeric fields have one or more
octal digits followed by at least one NUL/space, with only NUL/space through the
field end; no leading space, suffix garbage or binary encoding. Check arithmetic
even though these fixed widths cannot represent a uint64 overflow. Device fields
may instead be all zero bytes; any represented device value must be zero. mtime is
validated but never applied. Accept regular type `0` or NUL and directory type `5`.

Name/prefix fields can fill their complete width; when NUL terminated, every byte
after the first NUL must be zero. Name is nonempty; prefix is optional. Joined
path length must be at most 255 before the allowed final directory slash is
removed. All other path/type/mode/ownership rules are in the accepted spike.
Linkname, owner/group names and the unused header tail are all-zero bytes. Content
is opaque and not displayed; content padding and complete trailing blocks are
zero. Require a complete second zero trailer block and EOF after bounded zero
blocks. These conservative restrictions reject some valid broader archives;
there is no format widening or general tar compatibility claim.

The reader will be original BSD-3-Clause C/base-libc research, with no archived
paths opened or member effects performed. This review establishes source facts
under the accepted host trust assumptions, not native execution or containment.
