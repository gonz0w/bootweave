# First disk-aware build: bounded read-only storage probe

Accepted by Cam's explicit 'approved, continue', 2026-10-11 local. Base:
56feb68696800858651ccbeffff991d722551f13 (merged PR #32). Implementation and native
execution are authorized within this scope and the confirmed exact build target.
This is one removable research slice for issues #3/#5, not a durable runtime,
installer backend, public storage API or configuration-format decision.

## Requirements and engineering scope

Build an original BSD-3-Clause C probe in research/storage. It reads native disk
metadata and displays a bounded plain-text inventory. It performs no disk writes,
partitioning, formatting, mounting, installer invocation or payload/service work.
C remains an experiment; only reviewed OpenBSD base headers/libc are used.

Use bounded HW_DISKNAMES observation, validated device names and an independently
reviewed fixed raw-device path for the amended amd64 virtio fixture. Open the raw device
read-only without following symlinks, verify its character-device type, then use
DIOCGDINFO. Validate label magic, field bounds and checked sector-to-byte arithmetic
before displaying numeric capacity, sector size and fixed-width hexadecimal DUID.
Do not print raw model/label strings or execute discovery output as commands.

Treat optical/ramdisk entries separately from installation candidates. No candidate,
unsupported/ambiguous discovery, missing device, invalid metadata or unexpected
capacity yields a failed/incomplete preflight, never selection of a default disk.
This fixture expects one 8 GiB virtio disk; that is a test constraint, not a generic
framework storage policy. A DUID or guest name does not prove host attachment
identity, freshness, emptiness, installation readiness or write authorization.

Keep the existing synthetic UI intact. The new private image boots the probe in
place of the simulation; one static executable avoids assuming spare ramdisk room.
Use the existing native build approach with checked owned mounts/vnode detachments.
Review required raw-device node creation inside the owned embedded filesystem;
the probe itself creates no nodes/files. No upstream installer hook executes.
Freeze source, binary, linker-map, kernel and ISO identities before attachment.

## Native fixture and exact requested write scope

Reuse authenticated OpenBSD 7.9 amd64 media, the accepted verifier/key route and
reviewed QEMU 11.1.1 / BIOS identities. One CPU, 1024 MiB, TCG pc-i440fx-11.1 /
qemu64-v1, no NIC, sharing, passthrough or IP listener. Inspect paused attachments
and endpoints before each run; no automatic destructive retry or resume.

Prepared build target: .tools/installer-preflight/build.raw, new exclusively owned
0600 raw file, 8589934592 bytes, initially zero allocated bytes, SHA-256
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25.
Its private inode/device/size receipt invalidates approval on substitution. Only
this file may receive guest writes: manual verified stock OS installation using
whole-disk MBR, auto FFS layout/swap and native compilation/media files. No existing
disk or live network is exposed. Confirm observed wd0 mapping before the first
partitioning answer. Private synthetic credentials are excluded from Git/captures.

Prepared negative fixture: .tools/installer-preflight/small.raw, new owned 0600,
2147483648 bytes, initially zero allocated bytes, SHA-256
a7c744c13cc101ed66c29f672f92455547889cc586ce6d44fe76ae824958ea51.
It is read-only in every guest. Probe boots attach at most one read-only candidate
and one read-only ISO; no writable drive. Hash both candidate files before/after
probe runs. At approval, source/media preparation had run; no guest or installer
had launched. Actual execution is recorded separately with the completed evidence.

## Test Design and limits

| Check | Required observation |
|---|---|
| Host validation cases | Reject malformed discovery, duplicate/extra candidates, invalid label fields and arithmetic overflow; host results qualify only pure validation/tooling |
| Native build/checks | Warning-clean static base-cc build; repeat synthetic validator cases natively; record actual linker inputs, footprint and all failed/skipped cases |
| No candidate | Fresh ramdisk boot without disk reports unavailable/incomplete inventory; no installer/default fallback |
| Reviewed 8 GiB candidate | Native metadata agrees with separately observed fixture mapping/capacity; existing partitions do not imply empty/fresh storage |
| Wrong-size candidate | Read-only 2 GiB attachment fails the scoped preflight; unchanged whole-file hash |
| Serial/VGA | Required metadata/rejection text readable; no untrusted control bytes; unknown dimensions reported honestly |
| Failure/teardown | Stop on unexpected device/prompt, verification failure or uncertainty; observe shutdown, process/endpoint absence and owned fixture disposal |

Keep the stock 3768320-byte ramdisk reserve; ENOSPC rejects the candidate. Budgets:
15 minutes to prompt, 60 minutes per run, 20 GiB owned host allocation, 10 MiB private
capture, 32 MiB probe RSS, 4 KiB discovery input and 64 KiB application output.
Use owned supervision for bounded execution; unavailable measurements remain
unqualified. Publish only sanitized source-bound evidence. No artifact release,
full redistribution-license closure, general hardware support, destructive
confirmation implementation or rollback claim follows. A subsequent write-capable
installer adapter is a separate larger decision.

## Preparation evidence

The accepted signify verifier/key identities were rehashed, then source src.tar.gz /
sys.tar.gz and install79.iso signature/checksum checks passed. Read-only inspection
of authenticated headers confirmed the proposed read interfaces; nothing was copied
or executed from source. sys.tar.gz SHA-256 remains
c9ef294021ef7aafd5f18ffe8ebfed63394e5a86d65d7436d6db78551f9d57f1.

| Source member | SHA-256 |
|---|---|
| sys/sys/disklabel.h | 29212a079676a6e34fc8e2a79e4eb8f9a2bc4ae264b04ef3e06fba11fe787ecb |
| sys/sys/dkio.h | 73b4bcc75ddeec561323baa3190dd3962e017cb55bdd01f043d90868e23192a6 |
| sys/sys/sysctl.h | f7b6e6f36cf847837cd415a6b7ac8a241cc53eb4dfd0ec5620dde6fa6783fbe2 |

These preparation observations preceded implementation. Native header definitions
are evidence for a mechanism, not proof of device behavior or storage authorization.
Acceptance of this bounded slice authorizes implementation, routine corrections
within these limits and the sole exact build-target write scope above. Significant
scope changes, additional writable targets or runtime/backend adoption need review.

## Accepted read-only backend amendment

Accepted by Cam's explicit "approved" reply on 2026-10-11 local. The accepted IDE target in probe boots cannot use
QEMU's read-only block node: the same pinned binary already rejected it during
[earlier paused initialization](../design/native-profile/startup-failure.md).
The diskless native probe can proceed, but IDE disk-bearing boots remain unrun.

Accepted replacement for probe boots only: virtio-blk-pci with disable-legacy=off,
disable-modern=on, num-queues=1, romfile empty, discard=off and write-zeroes=off.
The existing candidate raw file and optical image remain readonly=on, with no
overlay or additional writable attachment. Machine/CPU/memory/firmware, targets,
limits and isolation remain as approved. No new ROM or dependency is selected.
Stock IDE stays in the separately authorized writable native build guest.

A host-only check initialized this exact replacement while paused, confirmed both
block nodes read-only and no IP endpoint/NIC, and quit without guest CPU execution.
The 2 GiB fixture remained completely hash-identical and zero allocated. This is
backend preparation evidence, not native support qualification.

Authenticated RAMDISK_CD source configures virtio/vioblk and sd; amd64 conf.c
assigns sd character major 13. The amended probe accepts only sd0/cd0/rd0,
opens the independently fixed /dev/rsd0c and checks major/minor 13/2. Other names,
including wd0, remain rejected. It does not auto-detect or fall back between
storage backends. This fixture change does not adopt a framework storage policy.

Acceptance authorizes rebuilding the original research sources on the sole approved
build disk, refreezing all source/artifact identities, and repeating fresh serial
diskless plus serial/VGA 8 GiB and 2 GiB checks with whole-file comparisons and
full owned teardown.
