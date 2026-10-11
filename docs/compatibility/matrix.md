# Compatibility and evidence

| Capability | Evidence | Status |
|---|---|---|
| Local repository foundation | macOS ARM64, 14 regressions, lint/content/secret checks | Host checks passed; native unqualified |
| Native installer boot/runtime | None | Unqualified |
| First proposed guest profile | [7.9 amd64 BIOS design](../design/native-profile/PRD.md); no runs | Proposed; unqualified |
| Project configuration/extension contract | Requirements only | Undesigned |
| ANSI/serial/plain-text renderer | [Research plan](../research/text-ui-runtime-plan.md); no native runs | Proposed; unqualified |
| Stock installer integration | [Bounded research plan](../research/installer-integration-plan.md); manuals only | Proposed; native tests not run |
| Disk operations, service bootstrap, recovery | Requirements only | Unimplemented |

Update this record with actual source-bound results; do not turn inherited project
qualification or an OpenBSD cross-build into framework support claims.
