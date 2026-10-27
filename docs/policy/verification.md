# Verification and execution contracts

These local clauses were extracted from [PROJECT.md](../../PROJECT.md) without changing their
requirements. Source identity is the consumer revision containing this extraction.
Command paths and literal configuration paths remain relative to the repository root.
Selected applicability is recorded only in root `policy.toml`; linked contracts retain
their existing scope. The owner changes required policy through the existing process.

## Bootweave verification and execution

These are local Bootweave requirements, not Agent Temper requirements. The method
asks for appropriate, honest verification; it does not prescribe an OS or a command.

Bootweave's runtime, installer builds, behavior tests and cross-build validation
remain OpenBSD-only under the October 23 decision in #5/#96. Historical Mac/Linux
LAB results remain historical. A product target does not automatically determine
the host for source-only checks. No VM or native authority follows from adoption.

On October 24 the owner directed completion of #95/#102 with verification
appropriate to the changes. For this finite documentation/method-metadata batch,
use the existing supported development host and unchanged `make check lint secrets`,
content/secret and commit-message hooks, signature/DCO checks, exact bundle/source/
license/link validation and separate reader/policy review. This replaces the
previous blanket OpenBSD/full-suite gate only for this adoption. Full `make verify`
and native installer tests are not applicable to its unchanged executable source;
record them as not run, never as passed. No hook/signing bypass is needed or allowed.

Behavior, tool, test, hook, build or dependency changes still require the relevant
OpenBSD checks and full `make verify` on the exact candidate. The held OpenBSD tool
adaptation and genuine authentication prerequisites remain with their owning work;
this adoption neither imports nor qualifies them. Follow
[development](../engineering/development.md). Never substitute fake crypto
positives, skip required assertions or call source review an executed check.

Before behavior changes define allowed/forbidden outcomes and meaningful positive,
negative and failure cases. Prefer test-first changes and reproduce defects before
repair, or record unavailable reproduction and the alternative evidence. Here TDD
in design filenames means **Test Design Document**; it is not evidence that a failing
test ran. Prose uses source, policy, link and reader checks, not artificial red tests.

Record exact source/base/tree, tool/OS identities, commands, exits and limitations.
Keep passed, failed, skipped and unavailable distinct. Preserve original attempts,
pins and failed/UNKNOWN evidence; never rewrite old hashes as fresh results. A
focused check cannot grant native or release acceptance. Exact disposable-target,
input, resource/process, credential, security, cleanup and native-execution gates
remain separate. Routine fixes/recovery on an already confirmed target stay within
its accepted scope and limits; a new target or expanded effects needs new authority.
