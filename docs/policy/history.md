# Historical finite decisions

These local clauses were extracted from [PROJECT.md](../../PROJECT.md) without changing their
requirements. Source identity is the consumer revision containing this extraction.
Command paths and literal configuration paths remain relative to the repository root.
Selected applicability is recorded only in root `policy.toml`; linked contracts retain
their existing scope. The owner changes required policy through the existing process.

Historical completed decisions only. Their finite authority is exhausted and is
not renewed by this selection. Preserve the original scope, failures and expiry
conditions below; they do not authorize this adoption or a future bypass.

## Finite verification decision for routing adoption #103

On 2026-10-24 the owner explicitly approved scoped Bootweave verification for
[#103](https://github.com/gonz0w/bootweave/issues/103). This replaces the
OpenBSD/full-suite acceptance requirement only for the thirteen-file adoption
based on `2142175dc0ff86f09593fbbcb8296053e3aa0fe3`, tree
`acfec5d0a49da7c9ed2eab81cdbaed27ea514711`, with sorted path+NUL+bytes+NUL
fingerprint `438e57075da16c370beeb8487701fc1d50d80f0c00320660028e2a500fef18ea`,
plus only this separately approved PROJECT/owning-issue decision delta. Any other
source change requires fresh applicability, checks and review.

For this exact adoption, unchanged pre-commit/commit-message hooks and
`make check lint secrets` must pass in the admitted isolated OpenBSD guest.
Exact upstream bundle/license/inventory/hashes, affected links, TOML/role mapping,
signed actual-author-DCO source and separate exact-candidate review must also pass.
Verify the public signed commit again on BSD and the canonical source after
integration. A failure of check, lint, secrets, either hook, provenance, signature,
DCO or required review remains blocking.

Only for this exact adoption, the unchanged `test`, `test-ui`, `test-storage`,
`test-handoff`, `test-payload` and `test-plan` targets are not applicable to
adoption acceptance. This is an explicit six-target applicability decision for
unchanged product/test/hook/tool/dependency sources, not a blanket exemption
inferred from one Darwin-only guard. Retain the unchanged full `make verify`
attempt, actual exit, every failure and existing skip. A failed aggregate stays
FAILED and is never relabeled as passing. Until execution occurs, the predicted
Darwin-only-harness failure is a source observation, not execution history. Its
unresolved full-suite/native/authentication acceptance stays open under its
original ownership.

This finite replacement changes only #103's source/configuration verification
applicability. It does not qualify product behavior, native authentication,
runtime project-role discovery or measured model benefit. Actual role/model
metadata remain unverified until observed. All behavior, build, tool, hook, test,
dependency and broader policy changes retain their existing OpenBSD/full-suite gates.

Preserve held work, resource/isolation/teardown, credential/access, signing and
publication boundaries. Gated merge permission applies only after all required
scoped gates above and separate review; no general future merge or operational
authority follows. The exact owner-approved proposal is retained in #103; this
decision delta requires separate review before final payload or signing.

## One-time Terra routing integration decision

On 2026-10-27, Cam approved the reviewed one-time Terra routing integration
proposal for [this owning issue](https://github.com/gonz0w/bootweave/issues/105). Separate exact-source review supplies
acceptance for the declarative workflow/model/provenance patch. Required native
BSD checks and commit hooks are waived/not run for this patch only; SSH signatures,
genuine author DCO, PRs and separate review remain. One additional bounded routing
batch is permitted for this repository, ending at merge or abandonment. This
decision expires at that PR's merge or abandonment and authorizes no CI
infrastructure, product, release, deployment, other permission/protection change,
future bypass or runtime-routing claim.

No VM, runner, publisher, tool installation, extra retry or infrastructure startup
is included. Existing failed evidence remains failed; earlier exceptions stay
expired outside this exact Terra patch and policy recording.

## One-time Temper guidance integration — 2026-10-27

On 2026-10-27 the owner approved a one-time integration decision for this owning
Temper guidance-refresh issue. For this exact reviewed pin/provenance patch only,
the upstream read-only auditor and trusted passive source/inventory/hash/link/
role-preservation checks may run on inert coordinator copies. Separate source
review and those checks replace unavailable required BSD gates and commit hooks,
which are waived/not run, never passed. Existing SSH signatures, actual-author
DCO, PRs, separate review and canonical verification remain. No consumer check,
module, hook or product/native workload may execute on that host. The decision
ends at this PR's merge or abandonment and grants no future bypass, CI workflow/
infrastructure change, executor startup, model change, release or deployment.

Owning work: [issue #107](https://github.com/gonz0w/bootweave/issues/107).
