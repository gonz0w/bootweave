# Authenticated composition: source-fit decision

Status: **Historical failed-fit assessment; candidate preserved**,
2026-10-20. The owner subsequently approved 80 KiB whole / 11 KiB fully charged fused
source budgets on 2026-10-20, without changing runtime/workspace/I/O limits.
The [current source contract](authenticated-composition.md) records that resume.
The immutable original candidate/contract and this failed-fit accounting remain
historical evidence; the counts below apply to the original 72/8 KiB decision.

This is the fit result for the independently accepted
[minimal fused contract](authenticated-composition.md), not adoption of source.

The canonical base is `72592b7f608100e1a4b60476d5d078cbbed504dc`.
The accepted contract SHA-256 is
`db06733d031d25aca720baa1b276cc935b0c58cf716d326d9c55a8279b2c34a3`.
One complete proposed source tree was prepared outside the product worktree,
including ownership, all 15 alias pairs, original deadlines, cancellation,
once-only cleanup, early final confinement and full authenticated reporting.
It was neither compiled nor run. Product source remains byte-identical to base.

| Account | Complete proposed bytes | Unchanged/proposed cap | Excess |
| --- | ---: | ---: | ---: |
| Whole source | 79,308 | 73,728, unchanged | 5,580 |
| New fused component, including both shared headers | 10,332 | 8,192, scoped proposal | 2,140 |
| Existing policy component, including shared span header | 7,980 | 8,192, unchanged | 0 |
| Existing content component, including both shared headers | 6,204 | 8,192, unchanged | 0 |

The base source is 70,850 bytes, leaving 2,878 bytes. Exact proposed changes are:

| File | Byte change |
| --- | ---: |
| authenticated_review.c | +6,829 |
| authenticated_review.h | +1,471 |
| content_review.c | -1,332 |
| policy_binding.c | -542 |
| private_content_format.h | +1,353 |
| private_span.h | +679 |
| **Net** | **+8,458** |

All other 18 existing C/header files are identical. Whole-source accounting counts
each shared header once; component accounting charges its complete bytes to every
using component. The two helper extractions together add 158 bytes to the existing
whole tree after removing the old copies. They avoid duplicating the report in
the new module but do not create source headroom. Static literal comparison
confirms the existing unauthenticated format is byte-identical and still has
18 LF-terminated lines. This is source comparison, not behavioral verification.

For this complete proposal to fit, at least 5,580 whole-source bytes must be
saved, including at least 2,140 bytes charged to the new component. Those figures
are overlapping constraints, not additive costs. No ceiling was raised and no
check, report line, ownership rule or runtime limit was removed. Repacking file
boundaries cannot change the whole-source total.

This result does **not** prove that every alternative architecture must exceed
72 KiB. It establishes that the reviewed minimal closed composition with these
two private helper extractions does not fit. Continuing requires a separately
reviewed broader factoring design that demonstrates those savings, or an explicit
source-budget decision. Native evidence cannot resolve this source-fit gate.
The smallest current action is to retain the contract and exact fit evidence,
leave the existing pure content review unchanged, and stop speculative C work.

The immutable local evidence set is `bootweave-authenticated-fit-candidate/`
(full source, complete patch, frozen contract and per-file hash/size manifest),
`bootweave-authenticated-composition-base-accounting.json`,
`bootweave-authenticated-composition-independent-design.json` and
`bootweave-authenticated-fit-static-comparison.json`. The fit manifest SHA-256 is
`b82e82d382193785a0a3dda6f3402b953b7eb8b9238ddbf69cdd4904d33cc089`.
No fixture dispatch, candidate compile/run, native test, key generation, CLI,
extraction, privilege, deployment or PR publication occurred for this fit slice.
Independent complete-source/accounting review is recorded separately before
handoff; this document alone does not claim implementation acceptance.
