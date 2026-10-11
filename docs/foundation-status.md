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
