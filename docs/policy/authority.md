# Review and action authority

These local clauses were extracted from [PROJECT.md](../../PROJECT.md) without changing their
requirements. Source identity is the consumer revision containing this extraction.
Command paths and literal configuration paths remain relative to the repository root.
Selected applicability is recorded only in root `policy.toml`; linked contracts retain
their existing scope. The owner changes required policy through the existing process.

## Review, integration and external actions

Use `codex/` branches, signed Conventional Commits and genuine author DCO sign-offs.
Preserve configured identity/signing and existing hooks. Verify the actual commit
range, trusted signatures and author trailers; do not invent sign-offs, transfer
private signing keys into guests, disable checks or silently bypass hooks.

The current maintainer policy remains: agents prepare scoped PRs against main;
Cam deliberately reviews and authorizes merging. General agent merge authority
has not been granted for Bootweave. Leave a PR open until that authority and its
actual gates are satisfied. The owner authorized bounded read-only agent reviews
of accepted work on 2026-10-24 in #102, replacing the blanket no-spawn restriction
for that scope. The [October 24 routing decision](routing.md#codex-routing-selection-and-policy-change--2026-10-24) additionally permits
bounded implementation delegation; native execution and merging retain their
existing authority gates. An implementer's second pass is not independent review, and peer review
cannot replace a required specialist or native witness.

Review current head/base/tree, acceptance, effects, source, evidence and exact
proposed squash author/message/DCO before integration. Reconcile changed inputs
with affected checks/review and verify the canonical commit afterward. Preserve
PR/signature/linear-history/conversation protections and deliberate maintainer
review. No GitHub Actions, runners, hosted success publishers or bypass actors.

Release, deployment, service/live-network operations, new credentials, repository
security changes, fixture disposal and expanded native effects need their existing
explicit authority. Public source work never implies those actions. Publish only
reviewed, sanitized evidence; use SECURITY.md for sensitive reports. Keep private
Suite documents, operator inventories, keys and raw private transcripts outside Git.
