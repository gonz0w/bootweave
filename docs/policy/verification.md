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

## Portable checks and target verification

On 2026-10-31, Cam approved the lasting host-policy replacement in
[issue 111](https://github.com/gonz0w/bootweave/issues/111), coordinated by
[Temper 38](https://github.com/gonz0w/agent-temper/issues/38). This replaces the
previous blanket BSD-only restriction for portable documentation, metadata and
source checks. It does not renew any expired adoption exception.

These portable checks may run on a permitted development host after inspection
of their exact commands, dependencies and effects. Existing trust, isolation,
resource and action limits still apply. OS-dependent behavior and integration
checks run on the affected component's declared target OS. Release qualification
follows the declared support matrix. Documentation-only changes do not by
themselves require native product qualification.

Mixed entry points retain every restriction of the checks they invoke. A command
name, file type or implementation language does not make a check portable.
Preserve required current CI, hooks and executable selectors; this decision does
not implement a narrower selector or turn an unavailable gate into a pass.
Record applicability, exact source, host/tool versions, results, skips and limits.
Portable evidence cannot substitute for target-native evidence. Product support,
worker admission, VM/service operations, credentials, signing, security settings,
release and deployment retain their existing boundaries.

For documentation/method-metadata-only changes, unchanged `make check lint
secrets`, content/secret and commit-message hooks, source/license/inventory/link
checks, signature/DCO checks and separate reader/policy review are portable.
Full `make verify` includes executable behavior suites and is not automatically
required for this scope. It retains its OpenBSD and change-impact obligations
for behavior, tooling, tests, hooks, builds or dependencies. No check is bypassed.

Historical completed #95/#102 decision only (expired):

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
