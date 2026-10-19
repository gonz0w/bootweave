# Project trust contract: staged adoption for issue #5

Status: adopted by independently reviewed [PR #76](https://github.com/gonz0w/bootweave/pull/76),
2026-10-19, canonical35642f92926be6c7c7df1ab57366a115a39370b5. The owner delegated
contract preparation and routine engineering decisions in the continuation after
PR #75. This document records the delegated engineering decision and its review; it
does not authorize native execution.
Related: [requirements](PRD.md), [engineering](EDD.md), [tests](TDD.md),
[bounded next product slice](../authenticated-plan.md).

## Decision and useful next result

Adopt PT-01 through PT-10 as mandatory product safety requirements, with staged
implementation. Adopt **authenticated, read-only payload review** as the next
bounded product contract. Keep `bootweave plan` version 1 unchanged. It continues
to label every payload claim unverified. A distinct future inspection entry can
verify the exact metadata and payload against trusted media policy and describe
its bounded archive contents. It has no target, confirmation, extraction, install,
service, bootstrap or privilege operation. No implementation is included here.

This separates three questions the earlier parent design left together: who
vouches for bytes, what those bytes describe, and who authorizes effects. An
accepted signature answers only the first. An inspectable archive answers only
the second. This stage grants no answer to the third, and no serialized review
can be supplied to a writer. A future write stage must have its own accepted
requirements/design/tests, actual target confirmation and native qualification.

## Normative authority contract

| Boundary | Adopted rule | First-stage disposition |
| --- | --- | --- |
| Untrusted definition | Strict existing v1 grammar, complete validation before output; no hooks or executable values | Reuse qualified parser; no v1 field/meaning change |
| Publisher trust | Trusted media policy independently pins definition identity, project/system tuple and explicit signify public key identity | No trust anchor, key path or algorithm supplied by project data |
| Artifact identity | Retain one bounded payload snapshot; verify its complete digest against authenticated metadata before interpreting its archive | No reopen by payload filename or trust in archive labels |
| Planning | Syntax, authenticated identity, archive policy and compatibility are distinct reported states | Inspection is read-only and has no device information |
| Privilege | A signature, plan, debug flag or prior confirmation cannot authorize effects | No privileged entry, transport, operation or root execution |
| Extraction | Authorize a bounded destination/effect set independently; enforce containment during writes, fixed owners/modes, no shell/general tar fallback | Product extraction is absent; research writer is not promoted |
| Confirmation | Future authorizer binds exact plan/input/artifact/target identity and fresh complete review; executor independently revalidates | No confirmation in this stage; inspection success is not a capability |
| Failure | Separate planned, performed and observed state; incomplete output/observation never means success; no general rollback | Failed check yields no completed inspection and no effects |
| Secrets and extensions | Separate reviewed consumer/lifetime; no ordinary logs/argv; executable extensions require separate authority | No secret input, executable adapter, account or service selection |
| Generic use | Same rules for unrelated application, DNS and firewall; runtime policy stays in each product | Synthetic offline document-viewer remains the primary fixture |

For future writes, stable target binding, mutual exclusion and confinement are
mandatory acceptance properties. Their concrete process/IPC/filesystem mechanism
is intentionally not adopted here. No caller may use that unresolved mechanism
as a reason to substitute arbitrary commands, general extraction or path-only
containment. Until a write contract is reviewed, the only valid operation set is
empty. The earlier placement/authorization/installed-target experiments remain
source-bound evidence with exhausted target authority.

## Engineering choices versus owner decisions

The following choices follow existing requirements and are owned by engineering:
original C/base libc for the new bounded model, OpenBSD base signify for detached
signature verification, explicit trusted key identity, digest binding, strict
archive rejection, no network fallback, fixed resource ceilings, plain/serial
output, separate native gates and deny-by-absence privilege. They require review
and evidence, not a new preference questionnaire. No new dependency is selected.
Use the qualified planner and the researched inspector/authentication ideas;
port and review only the code needed for this product slice, rather than copying
whole research harnesses or adopting their test authority.

Real publisher key bytes and the entity allowed to rotate/revoke them are owner
or project-publisher decisions **when a real distribution is built**. Synthetic
keys can exercise the contract without choosing a production signer. The first
policy pins exact definition bytes, so offline review claims identity relative
to that media policy, not latest-release freshness or global revocation. Changing
pins requires newly reviewed media; no online lookup or automatic key rollover.
Actual disk layout, extraction destinations and product bootstrap effects are
future product decisions, not hidden defaults. None blocks the read-only slice.
No new user direction is needed to review/adopt this narrow contract.

## Adoption and evidence gates

Merging this PR can adopt the safety contract and scope the read-only continuation;
it cannot qualify unimplemented behavior or close parent PTT cases. The scoped
[PRD/EDD/Test Design](../authenticated-plan.md) is the implementation boundary.
Implementation, fixes, portable evidence and a later exact native request belong
in that same feature continuation, without write-capable behavior. Before any
native work, freeze actual code/tool/input identities and obtain fresh exact
fixture authority. Old raw disks, signing keys and removed artifacts stay absent.

Current evidence: [PR #75](https://github.com/gonz0w/bootweave/pull/75), canonical
`df3323a2ae9fb2767ef2ad7e4d801b2bfde560e8`, qualified the metadata planner and
ended its exact target authority. The [authentication](../../research/payload-authentication-native-evidence.md),
[placement](../../research/payload-placement-rerun-evidence.md),
[authorization](../../research/payload-authorization-native-evidence.md) and
[installed-target](../../research/installed-payload-native-progress.md) records
are narrower research evidence, not acceptance of a production privilege API.
Review this decision and the next-slice design as one coherent issue #5 change.
