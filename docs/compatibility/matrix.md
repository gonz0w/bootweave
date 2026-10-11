# Compatibility and evidence

| Capability | Evidence | Status |
|---|---|---|
| Local repository foundation | macOS ARM64, 14 regressions, lint/content/secret checks | Host checks passed; native unqualified |
| Native installer boot/runtime | None | Unqualified |
| First proposed guest profile | [7.9 amd64 BIOS design](../design/native-profile/PRD.md); no runs | Proposed; unqualified |
| Project configuration/extension contract | [Trust design](../design/project-trust/PRD.md); no runtime tests | Proposed boundaries; syntax undecided |
| ANSI/serial/plain-text renderer | [Research plan](../research/text-ui-runtime-plan.md); no native runs | Proposed; unqualified |
| Stock installer integration | [Bounded research plan](../research/installer-integration-plan.md); manuals only | Proposed; native tests not run |
| Disk operations, service bootstrap, recovery | Requirements only | Unimplemented |

Update this record with actual source-bound results; do not turn inherited project
qualification or an OpenBSD cross-build into framework support claims.

OpenBSD 7.9 release [source/media signature and checksum evidence](../design/native-profile/release-input-evidence.md)
is host artifact verification under accepted ADR 0002; boot/runtime remains unqualified.

The [first guest attempt](../design/native-profile/startup-failure.md) failed during
QEMU read-only IDE device initialization, before guest CPU execution. OpenBSD boot
remains unqualified; the owned fixture was checked unchanged and removed.
