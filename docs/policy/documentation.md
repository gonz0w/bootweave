# Documentation and method maintenance

These local clauses were extracted from [PROJECT.md](../../PROJECT.md) without changing their
requirements. Source identity is the consumer revision containing this extraction.
Command paths and literal configuration paths remain relative to the repository root.
Selected applicability is recorded only in root `policy.toml`; linked contracts retain
their existing scope. The owner changes required policy through the existing process.

## Documentation and maintenance

Write reader-oriented Markdown and STE-informed plain English without claiming
formal ASD-STE100 compliance. README explains purpose, maturity, setup and routes;
AGENTS directs agents; PROJECT owns lasting choices; product docs specify behavior;
ADRs record decisions; issues own work; evidence describes dated observations.
Procedures need prerequisites, working directory, commands, expected results and
failure/recovery instructions. Preserve useful canonical manuals and examples.

Keep the imported method unchanged and local rules outside its directory. Updates
need a finite issue, immutable source/license/inventory/hash/link checks in the
applicable check environment, compatibility and separate review. Walk a representative item from selection to
completion. Classify findings under the [extension rules](../../.agent-temper/docs/reference/extensions.md);
use sanitized examples for public upstream proposals. No automatic pin upgrades,
new policy engine or source cleanup follows merely from adopting this method.
