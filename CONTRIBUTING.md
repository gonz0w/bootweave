# Contributing

Read [PROJECT](PROJECT.md) for the pinned Agent Temper method, local extensions
and lasting decisions. Select a finite ready owning issue through the
[delivery index](docs/engineering/backlog.md). Issues alone own live work and
handoff. Related stories may share one coherent PR; map acceptance to evidence
and keep unmet native/product criteria open. Resolve missing or conflicting
required policy with the owner; record the answer and apply it in later work.

Read [agent instructions](AGENTS.md), [scope](docs/product/PRD.md) and the design for
our active work. The framework is open to any OpenBSD project; use a generic
synthetic fixture when a change does not depend on a real product.

Start a `codex/` topic branch from current `origin/main` for a complete feature or
closely related issue slice. Keep related scope/design, implementation, fixes,
tests, native qualification, cleanup and evidence in one draft PR, using incremental
commits. Continue on that branch and update the existing PR as the work develops;
a phase transition or request to continue does not require another PR. Build
accepted work and document as needed or at a major decision point. Review larger
decisions in the same draft; scoped implementation and exact disk target approvals
still apply. Mark the PR ready when its agreed deliverable and required verification,
native qualification and fixture teardown are complete.

Use Conventional Commit subjects and PR titles. Sign canonical commits and annotated
release tags; include the author's Developer Certificate of Origin sign-off:

```sh
git commit -S -s
```

Signing and DCO are separate requirements. Never silently disable signing.
The sign-off certifies [DCO 1.1](https://developercertificate.org/), not assignment
of copyright. Contributions remain under BSD-3-Clause; retain upstream notices.

Use the change-appropriate checks and environment in
[development](docs/engineering/development.md). The owner-approved #95/#102
adoption uses existing supported-host documentation/lint/secrets, provenance/link,
hook/signature/DCO checks and separate review. Behavior/tooling changes retain
OpenBSD and full `make verify` gates; the old bootstrap is not an OpenBSD setup
recipe. Missing prerequisites never authorize a hook bypass or a claimed pass.

Run checks locally in the foreground. No GitHub Actions, runners or remote success
publisher is used.
PRs must contain clean candidate/base/tree identities, signature and DCO results,
commands, actual outcomes and applicable native evidence. Host results cannot
qualify an OpenBSD installer. Define meaningful positive, negative and failure
checks before behavior changes; prefer test-first work and reproduce bugs or
record unavailable reproduction and alternative evidence. Documentation-only
work uses source, policy, links and reader review, without invented red runs.
Test Design documents are specifications, not execution evidence. See
[review](docs/engineering/review.md) and [document lifecycle](docs/process/document-lifecycle.md).

Nontrivial implementation requires accepted scoped requirements, engineering design
and Test Design, or explicit research authorization. Setup does not approve the
proposed runtime architecture. Document dependency licenses, provenance, trust and
removal paths. Do not introduce unbounded arbitrary-shell project configuration.

Use synthetic data and disposable disks/networks. Do not commit keys, credentials,
operator inventories or raw installation transcripts. Leave PRs open for maintainer
review; agent merging, releases and live installations need explicit authority.
Bounded read-only agent reviews of accepted work are allowed; this does not grant
delegated implementation or execution authority. Respect the
[code of conduct](CODE_OF_CONDUCT.md) and [security reporting policy](SECURITY.md).
