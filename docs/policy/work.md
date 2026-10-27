# Work and delivery contracts

These local clauses were extracted from [PROJECT.md](../../PROJECT.md) without changing their
requirements. Source identity is the consumer revision containing this extraction.
Command paths and literal configuration paths remain relative to the repository root.
Selected applicability is recorded only in root `policy.toml`; linked contracts retain
their existing scope. The owner changes required policy through the existing process.

## Work selection and completion

GitHub Issues alone own live scope, priority, dependencies, ownership, progress,
approach, results and continuation. The backlog document links issues; it is not
another queue. Use the existing Backlog/Ready/In Progress/Blocked/Review/Done
states in each issue's status field or existing board. The optional GitHub profile
does not impose a new label scheme. Keep current issue bodies and historical
decisions; state a supersession explicitly instead of deleting inconvenient facts.

Select a finite ready outcome before implementation. Name owner, scope/non-goals,
observable acceptance, dependencies, proportional checks, authority and recovery.
A broad milestone or implementation proposal is not an executable assignment.
Use INVEST and Ready/Done as existing refinement aids, without adding ceremonies.
Small specifications stay in the issue; consequential contracts use scoped design
documents or ADRs. Record unrelated discoveries before implementing them. Changing
accepted outcomes or guarantees requires an owner decision.

Normal WIP is two active batches per repository and one per implementer; Blocked
and Review count. Related issues in one coherent batch count once. The existing
#95 documentation exception and inherited batches are tracked in that issue;
#102 continues that same method/delivery batch. Review its owner, reason, cap,
expiry and actual count at handoff. A branch is not a way to hide work or obtain
extra capacity. Preserve other owners, dirty files, held fixtures and failed evidence.

Keep one PR for a complete feature or closely related slice, including its fixes,
checks, native evidence and teardown when applicable. A phase transition does not
need another PR. Map each issue to actual acceptance and evidence; use closing
references only when every criterion is satisfied. Source-only work does not close
installer/use/restart or other operational acceptance. After accepted integration,
verify canonical source and reconcile issues, dependencies and parent outcomes.
