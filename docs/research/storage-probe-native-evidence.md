# Native read-only storage probe evidence

2026-10-11 local / 2026-10-12 UTC, bounded research for issues #3/#5/#10.
Cam explicitly accepted the [spike and exact build target](storage-probe-spike.md).
The accepted virtio amendment permits strictly read-only probe boots while the
sole writable build target remains IDE. Original and amended source identities
and observations are recorded separately below. No installer backend or supported
hardware profile follows.

## Original IDE source binding and build

Development base: 56feb68696800858651ccbeffff991d722551f13, merged PR #32.
Execution used worktree sources, not a clean signed candidate. The following
identities bind the original native run; later revisions require fresh evidence.
All original research implementation is BSD-3-Clause. Authenticated upstream
[source findings](storage-probe-source-findings.md) remain separate evidence.

| Executed input | Bytes | SHA-256 |
|---|---:|---|
| research/storage/probe.c | 4946 | 14b8e10c3ab402fe0b39ee76d6d8f8d19bb8e99050c566934b105dc2400a92f6 |
| research/storage/validate.c | 2960 | 9715b9b8b2779594b2a2f7cb98d3cf4a5d9e5640a691cdfce4156762fea00d8c |
| research/storage/validate.h | 811 | 37380cd2807695c4e053229b203abeb02b5e99d833901be08b3f3d30814574eb |
| research/storage/validator-test.c | 3859 | eb2a7d804a994a9027434927341adb9037a8829f6477ad498d74cbb219f20b71 |
| research/storage/build.sh | 3880 | 5b31eaedeca9e401525855396f471640e5611b3f89660dde4da28ccd13562301 |
| research/storage/profile | 437 | 092f20585c01ad04aed7e5c4374f58d0639c5fc5f9cf84ce04518e0967941b5f |
| Final read-only source ISO | 5027840 | bca4958de246217b0c804c612439a468adea5f02c30bf5287ce733aa1564ec3f |

The source ISO contained only independently frozen original sources, signed release
bsd.rd/manifest and authenticated ISO-bound bootloaders. Native sha256 -c checked
all ten inputs before execution, then the build checked its fixed source subset
and native signify verified bsd.rd with the installed 7.9 base release key.
The build ran in OpenBSD 7.9 GENERIC #399 amd64 with base Clang 19.1.7:

```sh
sh /mnt/storage-probe/build.sh
```

Warning-clean -std=c99 -Os -static / -Wl,-s compilation passed. The native
validator repeated all 44 synthetic checks with zero failures. No synthetic test
emulates the native disklabel ABI. Base rdsetroot/vnconfig worked only on the
owned extracted FFS; base mkhybrid created BIOS-only optical media. Build exit 0
and all four vnode devices unused were observed. Reentry refused existing output
with exit 1. No kernel rebuild or ramdisk enlargement occurred.

Stock reserve remained 3768320 bytes / 7360 sectors. FFS was 3535 KiB usable,
3288 KiB used and 247 KiB free before additions; afterwards it used 3446 KiB,
with 89 KiB free. Probe/profile total 159757 bytes. Existing /dev/rwd0c was a
character node with major/minor 3/2; no new node was required. Native readelf showed
PIE and no INTERP/DT_NEEDED; the map linked rcrt0.o, crtbegin.o, crtend.o and libc.a.
No compiler-runtime archive was observed. Full redistribution provenance remains
deferred; GPL base mkhybrid is a build tool outside the embedded filesystem.

| Native output | Bytes | SHA-256 |
|---|---:|---|
| Stripped static probe | 159320 | e62644e62b6c45b7e30c97a2918bb7c699f3b52a379c5a01202f628f468386e7 |
| Modified compressed bsd.rd | 4918442 | 858372aad1b7a784fc4f0c40b85268730504b872f08d70fedcb904d391200fde |
| BIOS probe ISO | 5083136 | 568d56b5ff6088baf36956de0b609879c53da8872fd31f9ec4c2f00ac1ebfdee |
| Native linker map | 124568 | 6c308565357cf46154066b8c1a12deeb3706955cdf96618c52313cf8a613fbb0 |

Private UART exports decoded only complete, unique base64 records and matched
native byte counts/digests for the ISO, binary and map before any attachment.
No host-compiled binary was booted or artifact published. Compressed-kernel growth
against the signed stock bsd.rd was 70270 bytes; no reproducibility claim follows.

## Original native observations and incomplete checks

The installed GENERIC guest exposed fd0 without a backing floppy file. Its native
probe returned 2 with fixed incomplete-discovery text, rather than ignoring the
unexpected name. Probe getrusage and independent base time both reported 456 KiB
RSS. An initial attempt to execute the exported mode-444 artifact failed with
126; its time measurement was not probe evidence. A separate mode-555 copy with
identical bytes supplied the actual measurement. The embedded binary already
had mode 555; the image did not require correction.

The diskless guest had only the frozen read-only ISO, no target, NIC, sharing or
passthrough. Paused QMP block/PCI and process/open-file/endpoint checks passed
before continuation. OpenBSD 7.9 RAMDISK_CD #396 booted with root rd0a, invoked
the original probe directly, reported unknown dimensions and no fixture disk,
and returned 2. Application output was 140 bytes, printable ASCII plus CR/LF,
with lines at most 40 columns. Its RSS checkpoint was 616 KiB. Embedded binary
and profile SHA-256 matched the frozen inputs. Profile reentry reported already
attempted and did not invoke the probe again. No stock installer menu appeared.
Native sync/halt was observed before emulator quit; its sole block node reported
zero write operations/bytes. This qualifies only the recorded diskless case.

The original IDE disk-bearing boots were not attempted: the same pinned QEMU
already rejected strictly read-only ide-hd in the
[prior initialization test](../design/native-profile/startup-failure.md).
Before the amendment was accepted, the proposed virtio replacement initialized while paused with both raw target
and ISO read-only, no new ROM/NIC/IP endpoint, and quit without guest CPU execution.
Its 2 GiB target remained hash-identical and zero allocated. This host check cannot
qualify native virtio metadata, 8 GiB acceptance, wrong-capacity rejection or VGA.
The accepted amendment and fresh source-bound observations are recorded below.
Physical serial, other hardware, signal/output failure cases and runtime adoption
remain unqualified. Metadata never establishes host identity or write permission.

## Original provisioning, failures and checkpoint disposition

Only the confirmed new 8 GiB build.raw received guest writes. Manual stock
installation observed wd0, 8192 MiB / 16777216 sectors, bound to the host receipt.
Whole-disk MBR/auto layout created FFS root/usr/home and swap. SSH/X startup was
disabled; no NIC was present. Host-verified bsd/bsd.rd/base/comp/man plus signed
manifest were transferred read-only and verified natively before extraction.
Initial optical discovery and the later source mount each needed one bounded
read-only retry. Stock GENERIC daemons ran offline; firmware/syspatch DNS attempts
failed without a NIC. No host service or live network was modified.

The first QMP inspection raced paused-startup readiness; it was corrected at the
boot prompt before kernel execution/writes, with target allocation still zero.
An optical-swap controller initially used incorrect QMP argument names and made
no change; corrected device arguments/readback succeeded before partitioning.
Later starts explicitly waited for paused readiness and inspected attachments
before continuation. Stock installer relink finished after observed ctfconv
activity; it was not bypassed or interrupted.

Installer/build/diskless runs all synced and halted before quit, after 1384.79 /
487.78 / 247.37 seconds. The paused backend review quit after 43.05 seconds without
guest execution. All four owned emulator PIDs were absent, endpoints inactive,
and clients exited. Credentials were absent from every console capture.
At this checkpoint owned allocation was 3780792320 bytes; respective run captures
were 920726 / 7370782 / 891669 / 7984 bytes, below the 20 GiB / 10 MiB limits.

The sealed post-build 8 GiB target SHA-256 was
a329913d7c648499877f9ea581d496ffd6c1e50b367d389a5d0bee7eefb57ea2;
the read-only 2 GiB target remained
a7c744c13cc101ed66c29f672f92455547889cc586ce6d44fe76ae824958ea51.
Both were unchanged during diskless/paused-backend observations. At this original checkpoint, private prepared files remained owned and full fixture
disposal had not occurred. The final disposition is recorded separately below.
No destructive rollback is claimed.

## Accepted amendment: final source binding and native build

Cam accepted the read-only virtio amendment on 2026-10-11 local. Probe boots use
the exact legacy virtio-blk-pci settings in the accepted scope, with romfile empty,
discard/write-zeroes off, and raw candidate/ISO readonly=on. No overlay, new ROM
or writable probe attachment was introduced. Only the same confirmed IDE build
target received writes during the native rebuild.

The final run used worktree sources on the same development base. Their frozen
identities below must match the eventual signed candidate; clean host checks are
a separate gate. Six original source inputs were rechecked byte-for-byte after
execution. Host-prepared transfer media SHA-256 was
a63903ba0c721f9b2fc29b08437debce14a57dec39ac5a3b62cd98bed8bde999
(5027840 bytes). All ten transfer inputs passed native sha256 before execution;
native signify again authenticated the stock bsd.rd.

| Final executed source | Bytes | SHA-256 |
|---|---:|---|
| research/storage/probe.c | 4947 | a4fff56671a1f2d09b9bfc8e0a899d24c19c4baa36a984fa0ed1cdd4a69bb0e7 |
| research/storage/validate.c | 2963 | 9ad065cb560b402135450ebebc6340aad298eb0bb71aadad9f8a2b51061f5786 |
| research/storage/validate.h | 811 | 37380cd2807695c4e053229b203abeb02b5e99d833901be08b3f3d30814574eb |
| research/storage/validator-test.c | 3859 | e42e334f82d2d68eacd3b5ad922e0038c4996c5166cb9469fec0a0f84ffdd1a7 |
| research/storage/build.sh | 3882 | 8ab9e86a6e7c9a1796e4e32015fdab473f32deb3490c170021774b7edb498d24 |
| research/storage/profile | 437 | 092f20585c01ad04aed7e5c4374f58d0639c5fc5f9cf84ce04518e0967941b5f |

The same native OpenBSD 7.9 GENERIC #399 / base Clang 19.1.7 build passed with
44 validator checks and zero failures. Build exit 0, reentry refusal exit 1,
all four vnode devices unused, static PIE/no INTERP or DT_NEEDED, and linker
inputs rcrt0.o/crtbegin.o/crtend.o/libc.a were observed again. The existing
/dev/rsd0c was character major/minor 13/2; no node creation was needed.
Stock reserve and 3535 KiB FFS remained unchanged: 247 KiB free before additions,
89 KiB afterwards. No kernel rebuild or filesystem enlargement occurred.

| Final native output | Bytes | SHA-256 |
|---|---:|---|
| storage-probe | 159320 | 92e4f0cfa43074cff3ecc5e563403f6cea29d2f47de7258c10de03ec8b029ac7 |
| bsd.rd | 4918439 | 43251b717a648b1b9462b5bc1f2d1f2af6e4c2d87a6baf11f21f05178e994199 |
| storage.iso | 5083136 | 6724c12bdfbe453d44fb20917b1df40dc27bb11ce5403cd03c81112f8a69d494 |
| storage-probe.map | 124568 | 44ede0568f76f0d0e30f4d28ea82477e11bb8cdb042135ac4306f1afbbd323a3 |

Complete unique private UART exports of the ISO, binary and map matched native
byte counts/hashes before attachment. Kernel growth over the signed stock was
70267 bytes. The rebuilt guest synced/halted before quit, after 530.75 seconds.
Images are research artifacts, without reproducibility or redistribution claims.

## Fresh amended native observations

Each fresh guest was inspected paused before CPU execution: exact frozen raw
attachments, no backing overlay, candidate and ISO read-only, no NIC/sharing/
passthrough/IP endpoint. RAMDISK_CD #396 booted directly into the research profile
on rd0a, without a stock installer menu. Serial bootstrap observed the known VGA
boot prompt before selecting com0; this is virtual UART evidence, not physical
serial qualification. VGA used the default keyboard/display console, 80x25.

| Fresh case | Probe exit | Result | Direct RSS KiB |
|---|---:|---|---:|
| Serial, no disk | 2 | No fixture observed; unknown dimensions | 576 |
| Serial, 8 GiB | 0 | sd0, 8589934592 bytes, 512-byte sectors; unknown dimensions | 580 |
| Serial, 2 GiB | 2 | Label or fixture capacity rejected | 628 |
| VGA, 8 GiB | 0 | Same scoped capacity/sector observation, readable fixed text | 612 |
| VGA, 2 GiB | 2 | Same capacity rejection, readable fixed text | 616 |

Native boot text independently marked sd0 read-only: 8192 MiB / 16777216 sectors
and 2048 MiB / 4194304 sectors respectively. The positive fixture retained its
previously installed partitions; success does not imply empty storage. The probe
validated repeated discovery/native-label DUID agreement and printed fixed-width
hexadecimal DUID; actual inventory values are excluded from public evidence.
Embedded binary/profile hashes matched the frozen receipts in all five cases,
including direct VGA hash readback. Profile reentry never reran the probe.

Serial application output was 140 / 240 / 150 bytes for diskless/valid/small,
printable ASCII plus CR/LF with lines at most 40 columns. The same valid guest
passed additional 80x24, 40x12 and unknown 0x0 cases (exit 0), and rejected known
30x12 dimensions (exit 2). Exact native stty -g states matched before/after every
case. Output was 211 / 211 / 240 / 101 bytes respectively. Observed serial RSS
checkpoints, including preliminary harness retries, ranged 576–644 KiB, below the
32768 KiB budget. The early narrow-console return emits no RSS checkpoint.

The first terminal-state comparison used cmp, absent from the reduced ramdisk,
and returned 127; the probe itself returned 0. The corrected private harness
compared exact before/after OpenBSD gfmt1 observations on the host. Its initial
hex-only parser rejected that format before correction; neither failed harness
attempt qualifies terminal restoration. All four corrected checks passed.
A diskless shutdown exceeded the harness's first 30-second observation window;
subsequent console inspection confirmed sync/halt, with no forced termination
or reboot. Remaining owned shutdown checks used a bounded 60-second observation.

Only recorded fixture behavior is qualified. Other hardware, physical serial,
hot-plug substitution, signal/output failure cases and runtime adoption remain
unqualified. No destructive installer or generic storage backend was built.

## Final disposition and review gate

All five final guests synced and halted before emulator quit. QMP block statistics
reported zero write operations/bytes for every probe attachment. Final run lengths
were 133.76 / 183.87 / 99.13 / 180.91 / 208.43 seconds for serial diskless, serial
valid, serial small, VGA valid and VGA small, within the 60-minute run budget.
Both whole raw-file hashes and allocated-byte counts remained unchanged from the
post-rebuild seal through all read-only cases:

| Candidate | Bytes | Allocated bytes | Before = after SHA-256 |
|---|---:|---:|---|
| build.raw | 8589934592 | 2414592000 | 68cc6d4942fe604e00d85730f9a716fc893e3d4f9ecc9335bcd0f69c000dff15 |
| small.raw | 2147483648 | 0 | a7c744c13cc101ed66c29f672f92455547889cc586ce6d44fe76ae824958ea51 |

Every owned emulator PID was absent, Unix endpoints refused connection, and lsof
found no open owned files before disposal. Serial/build clients had exited. The
stock upstream install ISO remained hash-identical. Credential bytes were absent
from every console/emulator capture; private input queues were disposed with the
fixture. All capture folders remained below 10 MiB:

| Run | Private capture bytes |
|---|---:|
| install | 920726 |
| build | 7370782 |
| initial-serial-none | 891669 |
| backend-review | 7984 |
| rebuild | 7368484 |
| serial-none | 894582 |
| serial-valid | 906849 |
| serial-small | 899910 |
| vga-valid | 2625262 |
| vga-small | 2626379 |

Owned host allocation was 3817263104 bytes before disposal, below 20 GiB.
After a sanitized source/artifact/result receipt was saved separately, the entire
owned fixture directory was removed: build/negative disks, credentials, captures,
input queues, modified images, binaries/maps and transfer media. Authenticated
upstream inputs remain separate; no research image is published. Cleanup and
whole-file comparisons establish this fixture's disposition, not general rollback.

The eventual PR records clean candidate/base/tree identities, trusted signature
and DCO verification, and make verify on the unchanged clean signed candidate.
Those local host results qualify tooling only; native qualification is limited
to the exact source-bound cases above. No GitHub Actions, runners or hosted status
publisher is used. The parent research issues remain open for their wider scope.
