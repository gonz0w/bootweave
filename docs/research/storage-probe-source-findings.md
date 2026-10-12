# Read-only storage source findings

Read-only inspection of authenticated OpenBSD 7.9 source, under the
[accepted probe scope](storage-probe-spike.md). These findings do not adopt a
runtime, storage API or future configuration format. No upstream implementation
was copied into the original research sources or executed from the archive.

The accepted signify/key route reverified src.tar.gz and sys.tar.gz before
fixed, bounded regular members were read. Archive SHA-256:

- src.tar.gz: fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b
- sys.tar.gz: c9ef294021ef7aafd5f18ffe8ebfed63394e5a86d65d7436d6db78551f9d57f1

| Member | SHA-256 |
|---|---|
| sys/sys/disklabel.h | 29212a079676a6e34fc8e2a79e4eb8f9a2bc4ae264b04ef3e06fba11fe787ecb |
| sys/sys/dkio.h | 73b4bcc75ddeec561323baa3190dd3962e017cb55bdd01f043d90868e23192a6 |
| sys/sys/sysctl.h | f7b6e6f36cf847837cd415a6b7ac8a241cc53eb4dfd0ec5620dde6fa6783fbe2 |
| sys/arch/amd64/include/disklabel.h | 14b69a792960ec97f339d20c403afe301d2eeb1c062db17d1691d8f5ae26de2f |
| sys/arch/amd64/amd64/conf.c | cf0ca3b13b8dc36f8c183b5a9e75b278c460d175877f60b6deab9d3722c8dc68 |
| sys/arch/amd64/conf/RAMDISK_CD | 7b9c3b64e95725518a92992b9ecd57f21f8adb710dc0e4e20c768096bde4a1aa |
| sys/kern/subr_disk.c | 3696f57abafb8b7138893f91926d9babb205e6b5293ce16a35788e9baa3f767e |
| sys/kern/kern_sysctl.c | fe9fc1849806cdba8cef53e6dd1d11789edd0dd0892de0a94414f6b74ca8df11 |
| sys/dev/ata/wd.c | b4d73477dbe3fba5ec1698349d41f246b42f9ecabe57d88ded2f8cdd5956784e |
| sys/dev/pv/vioblk.c | 51a37425fd452547fad2474ba89a02c622c68f92213590f6a8ed93667a64f2a6 |
| sys/dev/isa/fd.c | 0c1e175c7372e72b0e424228c40680fa884de8eacae77c11b576bc617b8ff8f3 |
| lib/libc/sys/getrusage.2 | c1f85f6ab38dc76372b776d0a349e8eb69dd2ffb04d2ca68af05d1839e7b32d6 |

HW_DISKNAMES remains available in SMALL_KERNEL and exposes comma-separated names
and DUIDs. Reading it can initialize cached labels; it is an observation, not a
freshness or host-attachment proof. DIOCGDINFO is a read ioctl returning the native
disklabel. The amd64 label limit is 16 partitions, raw partition c has index 2,
and disk-size/partition macros combine native low/high fields into 48-bit sector
counts. Both label magic fields, version, counts, sector-size bounds, overflow and
partition extents need checking before numeric display. A synthesized default
label on empty media can be valid; metadata cannot establish emptiness/readiness.

The amd64 character switch assigns wd major 3 and sd major 13; unit zero/raw c
has minor 2. Discovery must never construct a privileged path or command. The
research probe fixes its path independently and checks the descriptor's type and
major/minor after a no-follow read-only open. Repeated discovery/DUID agreement
detects some changed observations; it is not a hot-plug identity guarantee.

RAMDISK_CD configures virtio/vioblk/sd. vioblk maps the negotiated RO feature to
SDEV_READONLY. This supported the accepted read-only backend amendment; source inspection and
paused initialization were preparation evidence. Actual native observations are
recorded separately in the [native evidence](storage-probe-native-evidence.md).
The installed GENERIC guest additionally exposed fd0 without attached floppy
media; the strict original probe returned incomplete discovery instead of ignoring
that unexpected device name. Its behavior is separate from RAMDISK_CD evidence.

The base getrusage manual specifies ru_maxrss in KiB. The probe's checkpoint and
independent native time measurement agreed for the installed incomplete run. This
is an observation, not a hard allocation limit or proof of all future runs.

Headers and base libc are native build inputs. Original probe sources use the
repository's BSD-3-Clause license; no new third-party runtime dependency follows.
Object-level provenance and full image redistribution closure remain deferred
before release or runtime adoption.
