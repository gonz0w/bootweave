# Native UI ramdisk build and boot evidence

2026-10-11 local / 2026-10-12 UTC, issues #4 / UI-10 and #10. Cam authorized
this bounded [ramdisk experiment](ramdisk-ui-build.md), the inventoried new
8 GiB disposable build target, and routine decisions. No installer engine,
runtime adoption or supported hardware profile follows. Artifacts are private
research output; no binaries, images, credentials or raw captures are published.

## Source binding and native build

Development base: 355617eb64d576145f93b2ce3b3c8a17ac55b991 (merged PR #31).
Execution used worktree sources, not a clean signed candidate. The signed PR
retains the exact executed source bytes; its clean host/signature/DCO evidence
is separate. Original research sources are BSD-3-Clause. No upstream source
implementation or build hook was copied or executed.

| Final input | Bytes | SHA-256 |
|---|---:|---|
| research/ui/plain-ui.c | 9116 | 8bdeaed352dabc532c46d12fcb9a7b5fa7e3ab2363058ba24196e99b87094dde |
| research/ui/native-smoke.sh | 1705 | 38b36b51b3acadab2f25b01ccf8cccc49c8b992ef1414ea13087a99db94ee879 |
| research/ui/native-pty.c | 6939 | 38eea7ddfaaa6a73a7f32696e60094724978aedb6a6fca90437b32304536f505 |
| research/ramdisk/build.sh | 3927 | 04b393fbbe008f3ad74296a5ee2c0dc689ac503e4367ba1724d4dfebd53a0d45 |
| research/ramdisk/profile | 582 | 1bf66a1ae447acf9eec8ff14cdc8c71cedf0aa79997d89779f726c499960d900 |
| research/ramdisk/smoke.sh | 1724 | a41e56cbdcca6bf83978f862f83ff09d1b6981ee97361c628554f19b96f88634 |
| Read-only final source ISO | 5033984 | 75dec716c3d7836140b4a205c599a8331e9b980cf623214d5ac9a46ad96aa087 |

The host froze only these sources, signed bsd.rd/manifest and authenticated
ISO-bound bootloaders in a nonbootable data ISO. Native sha256 -c verified all
ten inputs before execution; native signify verified bsd.rd against the installed
7.9 base release key. Input sizes/digests are in the scope record. The build ran
inside OpenBSD 7.9 GENERIC #399 amd64, base Clang 19.1.7, with no package or NIC:

```sh
sh /mnt/ramdisk-ui/build.sh
```

The script uses warning-clean base cc with -std=c99 -Os -static and -Wl,-s,
base rdsetroot/vnconfig on only its owned extracted root.fs, then base mkhybrid
for BIOS optical media. Final build reruns all seven pipes and twelve native
PTY cases before modifying the private filesystem. Mount and vnode detachment
are checked before re-embedding; an existing output path is refused.

## Failures and footprint

The initial unstripped static build passed 19 cases but failed installing the UI
with ENOSPC. It emitted no successful candidate. Independent inspection found
no remaining owned mount/vnode/output or temporary build directory. Stripping
symbols was a routine correction: neither kernel reservation nor runtime source
changed. The next image booted directly into the serial simulation and completed,
but the reduced ramdisk shell lacked printf: the exit-status line failed and the
smoke test returned 1. Native probing established echo/print availability. The
final profile uses echo; its smoke test uses the reduced shell's print builtin.
The first image remains failed evidence, rather than a passed final candidate.

Stock embedded reserve is 3768320 bytes / 7360 sectors. Native extraction and
FFS inspection found 3535 KiB usable, 3288 KiB used / 247 KiB free before adding
the UI; the first stripped image used 3449 KiB with 86 KiB free. No kernel rebuild
or filesystem enlargement occurred. The final UI/profile/smoke inputs total
160586 bytes, below the 16 MiB growth ceiling. Remaining capacity is limited;
this does not establish room for an installer engine or other renderers.

Native readelf found a static PIE (ELF64 ET_DYN) with no INTERP or DT_NEEDED.
The linker map showed /usr/lib/rcrt0.o, crtbegin.o, crtend.o and libc.a; no
compiler runtime archive was linked. Base mkhybrid is a GPL build tool and is
not included in the ramdisk. Full object-level license/provenance closure and
redistribution review remain deferred before adoption or release.

## Final native artifacts

| Output | Bytes | SHA-256 |
|---|---:|---|
| Static stripped UI | 158280 | aa56fa421fbc1a97b07aab912c1293b7e864ba442f79256792b729954f75ecd0 |
| Modified compressed bsd.rd | 4918846 | b7440866e1d61a2868ca006b1719d007400154db0f193a698414fb1d0565bd48 |
| BIOS UI ISO | 5083136 | a3096495e786889c9324acb3cb6f1a024246a7a649dd397a0bc37df174a6ff94 |
| Native linker map | 127320 | 1dcf820b42aa51d6fbcb02390898ec6638d8665e874a58fbf5187ff3b6257f74 |

Native build exited 0. A second invocation refused the existing output with exit 1;
all four vnode devices were unused after the successful build. The native-built
BIOS control ISO using unchanged stock bsd.rd was 5013504 bytes (SHA-256
cc343e48badd24f0ff41c6a9cd78dae8e853b7cf7ea9552e6a368d98c2144884):
final ISO growth is 69632 bytes and compressed-kernel growth 70674 bytes. This
reconstructed control was measured, not booted; authenticated stock install79.iso
was the separately observed native boot/control. No reproducibility claim follows.

Final static PTY peak RSS was 124–140 KiB. Idle timeout took 10080 ms; the other
cases took 7–233 ms. Captures were bounded; forced-kill recovery belonged to the
owned PTY supervisor. Installed-guest PTYs do not qualify ramdisk signal behavior.

## Final console results and cleanup

Host private UART export decoded only complete base64 records and matched native
artifact byte counts and SHA-256 before attachment. No host binary was booted.
Final fixtures each had only one read-only ISO, no writable disk, NIC, sharing or
passthrough. Paused QMP block inspection and process/open-file/endpoint checks
preceded execution. Both booted OpenBSD 7.9 RAMDISK_CD #396 with root on rd0a.

| Actual final check | Outcome |
|---|---|
| Direct serial startup | UI completed Example / alpha with status 0; initially zero dimensions were reported unknown |
| Seven pipes inside the ramdisk | Valid, CRLF, cancel, incomplete/EOF, ESC, NUL and help/back all passed; smoke exit 0 |
| Embedded files | Native SHA-256 matched the final UI/profile/smoke inputs; root had 86 KiB free |
| Serial 80x24 and 40x12 | Completion exit 0, exact stty -g before/after match, no application ESC bytes |
| Serial cancel/control/1025-byte overflow/idle | Exit 2, no completion, exact stty restoration, no application ESC bytes |
| Profile reentry | Reported already attempted and did not invoke the UI again |
| VGA pc0 keyboard | Direct UI startup; example / alpha completion and status 0 visibly observed; native tty/framebuffer 80x25 |

Control and oversized serial lines were rejected and reprompted, then cancelled
by inactivity; observed case times were 10.328 / 10.523 seconds. Explicit q took
0.321 seconds and idle took 10.765 seconds, including controller overhead. The
VGA controller matched the native label prompt at 34.96 seconds from QMP continue,
including firmware/boot countdown/kernel initialization. These are fixture elapsed
observations, not independent input-to-redraw or UI-only startup measurements.
Serial speed was declared 9600; Unix socket transport is not physical baud evidence.

The build-guest PTY supervisor was not included in the stock image, and direct
ramdisk signal/resize/forced-kill restoration was not exercised. The 19 static
build-guest cases remain separate evidence. Idle CPU, broken console behavior,
full license closure, physical serial, EFI, other hardware, ANSI/curses comparison,
secret fields and destructive confirmation remain unqualified. The simulation
and research shell grant no installer authority.

## Provisioning and disposition

The sole confirmed synthetic target was wd0, 8192 MiB / 16777216 sectors, bound
to the private host inode/device/size receipt. Manual stock installation used
whole-disk MBR/auto layout, FFS root/usr/home and swap. SSH/X startup was disabled,
with synthetic private credentials and no NIC. Stock ISO sets lacked SHA256.sig;
signature bypass was refused. Host-verified sets plus the signed manifest were
transferred on read-only media, then native signature/set verification passed.
Initial optical discovery needed one bounded read-only retry. A later source
swap initially reported no medium and then mounted read-only on retry.

Installer sync/halt was observed before emulator quit (808.65 seconds). The first
build run and failed-image serial run likewise synced/halted before quit (693.07 /
126.27 seconds). Rebuild startup found all installed filesystems clean. Ordinary
stock daemons ran offline; firmware/update DNS attempts failed with no NIC.
Rebuild shutdown reported reorder_kernel failure; its cause was not inspected or
asserted. Compilation and signed input bsd.rd verification had passed independently;
the experiment does not qualify the installed GENERIC kernel profile. Rebuild
sync/halt was observed before quit (412.51 seconds). Final serial synced/halted
before quit (196.92 seconds). Fixture limits were 60 minutes per run, 10 MiB per
private capture and 20 GiB owned host allocation. No destructive rollback is claimed.

Final VGA sync/halt was visibly observed before quit (229.41 seconds). All six
owned emulator PIDs were absent, all private Unix endpoints refused connections,
console clients exited and no fixture files remained open before disposal. Total
owned allocation was 3884457984 bytes; run captures/metadata were respectively
923744 / 7158431 / 887951 / 7144922 / 895016 / 865024 bytes for install, first build,
failed-image serial, rebuild, final serial and VGA. The credential was absent from
all console captures. Final target allocated 2484600832 bytes, SHA-256
80bd94c484b67ae71762332d754fdf830887559f9ded1b16a8b1a4f1a87fb89e.
Stock install79.iso remained unchanged at SHA-256
7a4a92e953618035097c796a90b54424a0f3ae775552e1e7d102cf8a5130449f.
The owned disk, credential, transfer/media artifacts, linker maps, captures and
endpoints were removed; absence was checked. Only a sanitized ignored receipt
and separate authenticated upstream inputs were retained. No live network,
installed host service, release or artifact redistribution resulted.
