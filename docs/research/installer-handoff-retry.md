# Corrected confirmed-handoff run amendment

**Accepted, 2026-10-12 local.** Owner/acceptance authority: Cam.
Acceptance: Cam replied "yes,continue" to the four fresh writable targets,
two-read-only-optical bootstrap and 1 GiB artifact-export question after PR #35. This is the next
major decision following the [stopped build preparation](installer-handoff-stopped-evidence.md).
The [accepted original scope](installer-handoff-spike.md) governs everything
except the concrete changes below. Before acceptance no new guest had launched or received write
authorization. Exact receipt binding was reverified before recording acceptance. The previous six fixture files and their approval lifetime ended
with disposal; their names/digests cannot authorize replacement inodes.

## Correct the source before any destructive step

The authenticated stock install79.iso omits SHA256.sig from its set directory.
For build provisioning only, permit **two read-only optical attachments**:
unchanged authenticated stock boot CD as cd0 and the prepared nonbootable verified
set/source transport as cd1. The sole candidate remains the exact new build.raw
on the reviewed IDE configuration. Paused inspection must verify all three
attachments and absence of NICs, IP endpoints, overlays and other candidates.
No media replacement, reopen or hot-plug is permitted during a run.

Start in the stock shell, mount only the read-only data CD and verify the actual
selected set files with native signify and the stock 7.9 release key **before
entering the installer or answering a partitioning prompt**. Require visible
SHA256.sig and INSTALL.amd64 and all six signed stock files. Unmount before
starting stock installation. A missing file, failed verification or unexpected
question stops before any destructive answer. During installation manually select
cd1 and its fixed 7.9/amd64 directory; never accept verification bypass. Other
reviewed fixed stock answers and synthetic-secret limitations remain unchanged.

After successful provisioning/halt, a freshly inspected offline build boot uses
the same exact build target and only this read-only data ISO. Freeze/check its
sources against the private receipt before invoking the reviewed build script.
The script verifies input identities and signed sets before creating outputs;
its owned temporary workspace is on /home and final output on /root, separating
large copies across the existing automatic filesystems without changing target
size/layout, ramdisk reserve or backend.

Native mkhybrid packages the modified boot-only kernel under /bootweave/bsd.rd
and the **unchanged signed stock** bsd/bsd.rd/base/comp/man, INSTALL.amd64 and
SHA256.sig under 7.9/amd64. Only this one resulting read-only optical image attaches
to each native handoff guest. There is no game/X/site set or install.site.
The native media build and final stock installer reverify set identities; host
transport preparation is tooling evidence only.

## Bounded artifact export exception

Packaging signed sets makes the native output too large for the original 10 MiB
console capture budget. Permit **one build-artifact export capture up to 1 GiB**,
using the existing private serial channel and fixed uuencode/base64 exports of
only the frozen handoff ISO/kernel/binary/map/stock-entry identity record. This
exception applies solely to the completed native build guest, after credential
handling has ended, with no process-argv snapshots or tracing. Ordinary prompt,
installation and first-boot captures retain the 10 MiB/run limit.

Check native artifact size/hash before export, require unique complete bounded
frames and exact decoded size/hash, then freeze them before any guest consumes
an artifact. Incomplete or oversized exports cannot boot. The owned supervisor
still enforces 60 minutes/run, one CPU/1024 MiB, private endpoints, no NIC and
20 GiB total allocated host storage including transport, captures, disks and
artifacts. No larger ramdisk, extra library, ROM, backend or network is requested.

## Exact newly prepared targets

The new private fixture is .tools/installer-handoff-next, owned 0700. Every raw
file below was exclusively created 0600, has zero allocated bytes, and is bound
in a frozen private receipt to its host device/inode, size and whole-file digest.
The receipt SHA-256 is
7c5b8e6f584f29dca334e2980df7344144a2ae2c5350f77b441fa113763c06aa.
Preparation flags remained false; the subsequent explicit acceptance is recorded
in a separate binding before launch, leaving the preparation receipt unchanged.

| File | Bytes | Requested guest effects |
|---|---:|---|
| build.raw | 8589934592 | Verified manual stock provisioning, native compilation/media files, offline build boots and bounded artifact export |
| serial-target.raw | 8589934592 | Fresh confirmed serial handoff, verified MBR/FFS stock install and one offline first boot |
| vga-target.raw | 8589934592 | Independent keyboard/VGA handoff, verified install and one offline first boot |
| interrupted-target.raw | 8589934592 | Confirmed handoff, partition/format writes, deliberate interruption before extraction; no resume or boot |
| cancel.raw | 8589934592 | Read-only in every negative guest |
| small.raw | 2147483648 | Read-only undersized-capacity rejection |

The five 8 GiB initial digests are
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25.
The 2 GiB initial digest is
a7c744c13cc101ed66c29f672f92455547889cc586ce6d44fe76ae824958ea51.
Acceptance must cover the four exact writable files, the two-optical build
bootstrap exception and the bounded artifact-export exception together. Recheck
all identities/approval bindings immediately before each launch. No approval is
inferred from preparation or a matching zero-filled digest alone.

## Prepared transport and remaining evidence

The read-only verified-source.iso is 1350772736 bytes, SHA-256
e98a256b1a53f0ab5fdffedeb296c3dcb5901ef04bb1f7f613ce43b9b1d88492.
Host extraction used only the authenticated stock ISO's selected fixed members.
The accepted signify/key route verified bsd, bsd.rd, base79.tgz, comp79.tgz,
man79.tgz and INSTALL.amd64 against authenticated SHA256.sig. Frozen original
research/build sources and authenticated bootloaders are included separately.
Strict host ISO readback matched every frozen source/set payload, including
both set directories and their signature manifests. The nonbootable transport
was assembled with existing host hdiutil; it does not qualify native media.
Native mount/path/manifest verification passed before target writes; the
[current native evidence](installer-handoff-native-evidence.md) records build
provisioning, compilation, corrections, completed supervised handoffs/OS boots,
shutdown limitation and full disposal.

The original native Test Design remains required: warning-clean static build,
pure cases, linker/footprint/reserve and stock-entry checks; native serial/VGA
negative and signal/terminal cases; rejected host substitution; observed zero
writes before confirmation; serial/VGA verified installations with independent
postconditions and offline boots; the separate partial-install case; credential
exclusion and full owned teardown. Failed verification or unexpected partial
state stops dependent work. No installer/runtime adoption, complete automation,
project policy or general rollback is proposed.
