# Contributing

Read [agent instructions](AGENTS.md), [scope](docs/product/PRD.md) and the design for
our active work. The framework is open to any OpenBSD project; use a generic
synthetic fixture when a change does not depend on a real product.

Start a focused `codex/` topic branch from current `origin/main`. Use Conventional
Commit subjects and PR titles. Sign canonical commits and annotated release tags;
include the author's Developer Certificate of Origin sign-off:

```sh
git commit -S -s
```

Signing and DCO are separate requirements. Never silently disable signing.
The sign-off certifies [DCO 1.1](https://developercertificate.org/), not assignment
of copyright. Contributions remain under BSD-3-Clause; retain upstream notices.

```sh
make tools hooks
make verify
```

Run checks locally. No GitHub Actions, runners or remote success publisher is used.
PRs must contain clean candidate/base/tree identities, signature and DCO results,
commands, actual outcomes and applicable native evidence. Host results cannot
qualify an OpenBSD installer. See [review](docs/engineering/review.md).

Nontrivial implementation requires accepted scoped requirements, engineering design
and Test Design, or explicit research authorization. Setup does not approve the
proposed runtime architecture. Document dependency licenses, provenance, trust and
removal paths. Do not introduce unbounded arbitrary-shell project configuration.

Use synthetic data and disposable disks/networks. Do not commit keys, credentials,
operator inventories or raw installation transcripts. Leave PRs open for maintainer
review; no agent merges, releases or live installations. Respect the
[code of conduct](CODE_OF_CONDUCT.md) and [security reporting policy](SECURITY.md).
