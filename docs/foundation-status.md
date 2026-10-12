# Foundation status

Public [Bootweave](https://github.com/gonz0w/bootweave) created 2026-10-11 after
maintainer approval. The signed foundation `8078f10e1634ce558e788c67558d754fef47d2ca` seeded main; repository
protections, disabled Actions and security reporting were applied and read back.
See [observed GitHub settings](engineering/github.md).

The foundation includes generic scope and boundaries, author/security/debugging
requirements, pinned local tools/hooks, contribution/review policy, provenance,
PR/issue templates and versioned protections. It contains no installer,
configuration grammar or installer runtime. Stock-media [diskless native observations](design/native-profile/diskless-evidence.md)
have occurred. The separately confirmed [UI build guest](research/plain-ui-native-evidence.md)
received a manual stock installation on an owned disposable disk. A
[private research ramdisk](research/ramdisk-ui-native-evidence.md) now packages the
synthetic UI; no installer integration or release exists.

Clean seed verification passed on macOS ARM64: signature and author DCO, pinned
development-tool bootstrap, repository/configuration/link checks, ShellCheck,
all 14 foundation regressions and Gitleaks with no findings. Head and tree identities
and actual outputs are retained in ignored .tools/foundation-verification.json.
The bootstrap-record PR has its own clean candidate verification evidence.
These are host foundation results; native runtime remains unqualified.

## Development readiness, 2026-10-11

Initial design deliverables have merged: [native profile](design/native-profile/PRD.md)
(issue #2), [installer research scope](research/installer-integration-plan.md)
(issue #3), [text UI/runtime scope](research/text-ui-runtime-plan.md) (issue #4),
and [project trust design](design/project-trust/PRD.md) (issue #5).
They remain proposed; merging documentation has not recorded implementation or
native-run acceptance. Those issues remain open for scope review.

Issue #10 tracks [native preflight](design/native-profile/preflight.md). The accepted
[verifier/key route](adr/0002-host-artifact-verifier.md) verified release inputs.
The accepted bounded diskless run reached the stock VGA menu; serial selection
initially failed, then [corrected serial research](design/native-profile/serial-evidence.md)
observed com0 menu and read-only shell interaction. A later bounded build guest
established native UI evidence; general disk-bearing profile and framework
qualification remain pending.
Runtime/parser/renderer and privileged contracts remain undecided.

[Accepted Phase A UI research](research/plain-ui-spike-plan.md) now has an original
[plain-text simulation](../research/ui/README.md) and local host/PTY checks.
[Host results](research/plain-ui-host-evidence.md) and separately authorized
[native results](research/plain-ui-native-evidence.md) now exist. Native tests drove
a terminal restoration fix. The separately authorized ramdisk experiment now
provides native media and console evidence; broader qualification and runtime
adoption remain deferred.

## Next build decision, 2026-10-12

The [bounded handoff](research/installer-handoff-native-evidence.md) completed its
approved supervised stock installs and offline boot checks; its targets are
disposed and exact-target authority has ended. The
[saved block-observation checker](engineering/development.md) is now local tooling.
The next proposed component is a
[read-only payload archive inspector](research/payload-inspection-spike.md) under
issue #5. Its bounded host implementation scope awaits acceptance. Payload trust,
extraction, public project syntax and production installer adoption remain separate
decisions; no new native run or writable target is prepared by this proposal.
