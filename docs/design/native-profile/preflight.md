# Native baseline execution preflight

Status: Bounded diskless observations completed; full profile incomplete,
2026-10-11. Owner: Cam.
[Issue #10](https://github.com/gonz0w/bootweave/issues/10).
Related [PRD](PRD.md), [EDD](EDD.md) and [Test Design](TDD.md).

## Current authority and evidence

Cam accepted the bounded [non-install run plan](run-plan.md), then the
[diskless exception](startup-failure.md) after read-only IDE startup failed.
The [VGA observation](diskless-evidence.md) reached the stock menu; the
[corrected serial observation](serial-evidence.md) reached the menu and read-only
shell over com0 after VGA bootstrap. These records contain exact source/base/tree,
input identities, effective attachments, submitted input, resources and cleanup.
They supersede the original preflight's no-run and verifier-pending statements.

This acceptance covers the recorded diskless stock-media research only. The full
disk-bearing profile remains proposed. No writable target, installer implementation,
UI native build, modified media, service or network change is authorized here.
The original readonly IDE configuration failed before guest CPU execution and must
not be retried by silently changing the disk to writable.

## Frozen tooling and inputs

| Installed artifact | SHA-256 observed |
|---|---|
| qemu-system-x86_64 | 25afcb2986e4a0f2c567de17273f55de3c99792b78a07c99e923bb62024c088f |
| bios-256k.bin | ae6f6aa973aaccc143f57aa960fb035fd9de4daee4ad0cd713322f8c259e7650 |
| vgabios-stdvga.bin | e8fc9e55790dbe3cb31f019a3deb57206ba6c54f5e581adb2ab2677a9d391472 |

Recorded runs used QEMU 11.1.1, pc-i440fx-11.1, qemu64-v1, TCG, one CPU and
1024 MiB guest RAM with reviewed firmware/data paths. Rehash before any newly
approved launch; changed identities require review. Installed digests identify
bytes rather than authenticate package origin. [Provenance](provenance.md) and
[firmware evidence](firmware-evidence.md) document upstream correspondence and
remaining build/license limitations; local research use does not authorize redistribution.

The accepted [host-verifier policy](../../adr/0002-host-artifact-verifier.md)
and [release-input evidence](release-input-evidence.md) establish the recorded
source/media signature and checksum checks. Verification was repeated before each
native attempt. Input verification does not qualify native framework behavior.

## Gate reconciliation

| Gate | Current evidence | Remaining boundary |
|---|---|---|
| Scope acceptance | Accepted bounded non-install diskless research; performed inputs recorded | Full disk-bearing profile and any new writable/build scope need separate review |
| Package provenance/license | Executable/firmware identities and upstream byte correspondence reviewed | Full reproducibility/license closure and redistribution review incomplete |
| Input trust | Accepted verifier/key policy; verified release source/media; offline corruption rejection | Repeat checks for new approved runs; derived media needs its own trusted identity |
| Release review | Release-specific boot-console instructions and authenticated selected source reviewed | Installer selected-path prompt/default ledger incomplete |
| Backend | Explicit versioned configuration observed in diskless guests | No disk-bearing backend qualification or hardware timing claim |
| Attachments | Sole read-only optical image; no target/network/sharing/passthrough; process open-file checks | Original readonly IDE target failed startup; no target-byte postcondition |
| Consoles | VGA 80x25 menu/invalid input; com0 menu/shell after VGA bootstrap | Serial-only firmware, measured baud and 80x24 case unqualified |
| Limits and stop | Runs under five minutes; bounded private captures; observed timing/error stops | Peak RSS and synthetic capture-cap failure case not measured/tested |
| Observation/cleanup | Owned guests/clients stopped; fixture sockets/files removed; media digest unchanged | Full profile acceptance review and unrun cases remain open |

The first VGA run's complete device-tree review occurred after continuation; its
block/PCI/open-file checks preceded continuation. Corrected serial attempts reviewed
the full tree before continuation. Do not generalize the stronger sequencing claim
to every earlier run. Detailed failures remain in their original evidence records.

## Test status and next gates

NPT-01 has repeated artifact verification evidence; NPT-02 has separate offline host
corruption evidence. NPT-03 has diskless isolation observations only. NPT-04 has VGA
menu/input evidence, and NPT-05 has serial menu/input evidence after VGA bootstrap.
NPT-06 has plain serial text with zero ESC bytes but no measured 80x24 or independent
baud result. NPT-07 was skipped because no target disk was present. NPT-08's complete
unsafe-configuration review remains outstanding. NPT-09 has actual error/timeout stops
but no synthetic capture-cap test. NPT-10 has observed teardown. NPT-11 has sanitized
evidence for review; these partial outcomes do not satisfy the full proposed profile.

Next issue work can complete source inspection and unlaunched configuration review
within existing documentation authority. A native UI build additionally needs the
[Phase B gate](../../research/plain-ui-spike-plan.md): an approved build environment,
exact source/build/run/transport, attachments, limits and cleanup. The stock ramdisk
observations do not supply a compiler environment. Any proposed OS installation must
identify a fresh disposable target and obtain explicit confirmation of that exact
inventory and allowed writes before the first destructive choice.

Issue #10 remains open. No Bootweave runtime, renderer, parser, installed system,
boot image or supported hardware profile is qualified by these observations.
