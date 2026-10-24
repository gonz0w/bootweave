## Problem and behavior

Describe the operator problem and resulting behavior. Link the scoped design.

## Validation

Link the owning issues and map each acceptance ID to evidence. Record candidate
source/tree, check-environment/tool identities, commands and actual results. Distinguish
source checks, native evidence, unavailable/skipped capabilities and remaining gates.
Include the resolved base, actual PR title, signature/DCO verification and clean
candidate result for the required change-specific gate (see PROJECT/development).
For #95/#102, report full `make verify` as not run/not applicable to the documentation
batch; behavior/tooling changes still require it. Actions are disabled; Cam reviews
local evidence.

## Security and recovery

Describe changed authority, compatibility, migration/rollback and partial failure
where relevant. Confirm disposable-fixture teardown. Omit private data and secrets.

## Review

- [ ] Scope matches the active design or authorized research spike.
- [ ] Evidence matches the current head/base/title; native limits are explicit.
- [ ] Signed commits and author DCO trailers were verified.
- [ ] No secrets, generated fixture credentials or operator data are included.

Leave this PR open for maintainer review and squash merge.
