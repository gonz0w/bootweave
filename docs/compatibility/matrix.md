# Compatibility and evidence

| Capability | Evidence | Status |
|---|---|---|
| Local repository foundation | macOS ARM64, 14 regressions, lint/content/secret checks | Host checks passed; native unqualified |
| Saved QMP block observations | [Development checker](../engineering/development.md), eight host regression groups | Supplied snapshot path/access/raw/backing comparisons passed; no guest/file access, live inspection or native qualification |
| Stock OpenBSD 7.9 optical boot/menu | [Diskless VGA observation](../design/native-profile/diskless-evidence.md) | VGA and [serial](../design/native-profile/serial-evidence.md) observed; framework unqualified |
| First proposed guest profile | [7.9 amd64 BIOS design](../design/native-profile/PRD.md); diskless VGA observation only | Disk-bearing profile unqualified |
| Project configuration/extension contract | [Trust design](../design/project-trust/PRD.md); no runtime tests | Proposed boundaries; syntax undecided |
| Payload archive inspection | [Accepted host spike](../research/payload-inspection-spike.md), [host evidence](../research/payload-host-evidence.md), nine groups | Host framing/path/type/bounds and failure checks passed; no native, extraction or payload trust qualification |
| Plain-text synthetic UI research | [Host/PTY evidence](../research/plain-ui-host-evidence.md) and [native results](../research/plain-ui-native-evidence.md) | 7.9 amd64 installed guest: 7 pipe/12 PTY cases, serial/VGA completion; runtime adoption unqualified; [ramdisk results](../research/ramdisk-ui-native-evidence.md) recorded separately |
| Disposable synthetic UI ramdisk | [Native build/console evidence](../research/ramdisk-ui-native-evidence.md) | Native static build, ramdisk pipes, serial/VGA startup/completion observed; broader qualification pending |
| Read-only storage probe research | [Native evidence](../research/storage-probe-native-evidence.md) and [scoped amendment](../research/storage-probe-spike.md) | Native static build/44 validator cases; serial diskless rejection; serial/VGA read-only 8 GiB acceptance and 2 GiB rejection under accepted virtio scope; no installer qualification |
| ANSI/serial/plain-text renderer | [Research plan](../research/text-ui-runtime-plan.md) and [ramdisk source findings](../research/ramdisk-ui-source-findings.md); no custom renderer runs | Proposed; unqualified |
| Confirmed stock handoff research | [Accepted scope](../research/installer-handoff-spike.md), [earlier stop](../research/installer-handoff-stopped-evidence.md), [corrected native evidence](../research/installer-handoff-native-evidence.md) | Host checks; native static build/pure tests, 19 PTY cases, 19 serial/VGA rejections, two verified stock installs/offline OS boots, partial-state checks and disposal passed; VGA shutdown interrupted background kernel relink, subsequent kernel unqualified; bounded research only |
| Stock installer integration | [Bounded research plan](../research/installer-integration-plan.md) | Production integration proposed; unqualified |
| Disk operations, service bootstrap, recovery | Requirements only | Unimplemented |

Update this record with actual source-bound results; do not turn inherited project
qualification or an OpenBSD cross-build into framework support claims.

OpenBSD 7.9 release [source/media signature and checksum evidence](../design/native-profile/release-input-evidence.md)
is host artifact verification under accepted ADR 0002; boot/runtime remains unqualified.

The [first guest attempt](../design/native-profile/startup-failure.md) failed during
QEMU read-only IDE device initialization, before guest CPU execution. OpenBSD boot
remains unqualified; the owned fixture was checked unchanged and removed.

The [diskless follow-up](../design/native-profile/diskless-evidence.md) reached the stock
VGA menu. The first serial selection missed the boot prompt; the
[corrected observation](../design/native-profile/serial-evidence.md) reached its menu
and read-only shell through com0 after VGA bootstrap.
Those diskless observations tested no writable target or Bootweave runtime.
The later [native UI experiment](../research/plain-ui-native-evidence.md) manually
provisioned one separately confirmed disposable target; no installer engine was tested.
