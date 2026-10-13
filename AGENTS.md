# Agent instructions

1. Read README, CONTRIBUTING and relevant scope/design documents before editing.
2. OpenBSD is the runtime and media-build target. Host checks and cross-builds
   establish tooling only; native tests use disposable OpenBSD guests/networks.
3. Repository setup, documentation and local foundation tooling were authorized
   on 2026-10-11. Installer implementation needs approved scoped PRD/EDD/Test
   Design or an explicitly authorized bounded research spike.
4. This is a public, generic installer framework. Keep product-specific runtime
   policy in its owning project. Do not constrain the engine to DNS or firewalls.
5. Prefer OpenBSD base tools. Runtime language, parser and UI choices need native
   evidence; significant dependencies require review, license/provenance and ADR.
6. Project definitions, payloads, operator input and terminal output are untrusted.
   Never interpolate them into shell commands. Privilege uses typed operations,
   validated arguments and explicit authorization. Debug mode cannot bypass safety.
7. Disk writes need explicit reviewed target confirmation. Separate planned,
   performed and observed state; validate postconditions and report uncertainty.
   Do not claim destructive installation has general rollback.
8. Secrets and private network details never enter Git, default logs or PR evidence.
   Treat build hooks as reviewed executable code, not harmless configuration.
9. Require keyboard, serial, low-color and plain-text flows. Decorative ANSI output
   must not obscure prompts or permit control-sequence injection.
10. Run make verify locally. GitHub Actions, runners and hosted status publishers
    are prohibited. Update compatibility/evidence for behavior changes.
11. Use codex/ branches, signed Conventional Commits and author DCO sign-offs.
    Leave PRs open. Do not merge, release, install services or modify a live network
    without authorization. Public repository creation and the name Bootweave were approved 2026-10-11.
12. Research is evidence until adopted. Do not invent future wire schemas,
    configuration syntax or service names before their scoped design.
13. Do not spawn agents unless the user explicitly requests parallel agent work.
14. Use one PR per complete feature or closely related issue slice. Keep its scope,
    implementation, fixes, tests, native qualification and evidence on the same
    branch and draft PR. Continue requests and phase transitions are not new PR
    boundaries. Build accepted work and document as needed; bring larger decisions
    to review within the existing PR. Keep exact target authorization requirements.
