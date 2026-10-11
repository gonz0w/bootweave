# Public GitHub bootstrap

Observed 2026-10-11 after maintainer approval of Bootweave and public creation.
Repository: [gonz0w/bootweave](https://github.com/gonz0w/bootweave); default branch main.
The BSD-3-Clause foundation was seeded with signed, author-DCO commit `8078f10e1634ce558e788c67558d754fef47d2ca`
before protections. Every subsequent change uses a reviewed PR.

| Setting | Observed state |
|---|---|
| Visibility | Public; generic OpenBSD installer framework |
| Main ruleset | [24891212](https://github.com/gonz0w/bootweave/rules/24891212), active; PR, signatures, linear history and resolved conversations |
| Deletion/force pushes | Blocked on main; no bypass actors |
| Solo review phase | Zero mandatory independent approvals; no mandatory CODEOWNERS approval |
| Release ruleset | [24891214](https://github.com/gonz0w/bootweave/rules/24891214), active; v* updates/deletion blocked |
| Merge modes | Squash only; auto-merge disabled; delete merged branches enabled |
| Actions/checks | Actions disabled; no required remote status checks; local evidence reviewed by maintainer |
| Security reporting | GitHub private vulnerability reporting enabled and read back |
| Dependency alerts | Enabled; authenticated API read returned 204 |
| Seed signature | GitHub verified; local verification used the existing trusted maintainer signing identity |

Versioned [main](../../.github/main-ruleset.json) and
[release](../../.github/release-tags-ruleset.json) definitions match the applied
rules. Desired files alone do not enforce protections. API readback and local
verification receipts remain in ignored .tools/; shared evidence is sanitized.

No GitHub workflow, runner, hosted scanner, automatic release or test status
publisher is configured. Actions being disabled does not establish test success.
Local foundation checks do not qualify boot media or installer runtime. See the
[review procedure](review.md) and [native gate](../testing/native-gate.md).
