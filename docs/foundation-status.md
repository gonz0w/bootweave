# Foundation status

Public [Bootweave](https://github.com/gonz0w/bootweave) created 2026-10-11 after
maintainer approval. The signed foundation `8078f10e1634ce558e788c67558d754fef47d2ca` seeded main; repository
protections, disabled Actions and security reporting were applied and read back.
See [observed GitHub settings](engineering/github.md).

The foundation includes generic scope and boundaries, author/security/debugging
requirements, pinned local tools/hooks, contribution/review policy, provenance,
PR/issue templates and versioned protections. It contains no installer,
configuration grammar or runtime. No native test, service installation, disk
modification, boot image or release has occurred.

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

Issue #10 tracks [native preflight](design/native-profile/preflight.md), including
partial tooling/firmware evidence and pending trusted media verification. Installer
[source findings](research/installer-source-findings.md) are provisional until
signature verification; no native test has run. Next execution needs an accepted
verifier/key route, frozen guest configuration and explicit bounded non-install
run authority. Runtime/parser/renderer and privileged contracts remain undecided.
