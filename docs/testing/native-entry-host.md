# Native boundary and CLI: portable source evidence

This records [PR #91](https://github.com/gonz0w/bootweave/pull/91) and the
[source-only contract](../design/native-entry-source.md). The candidate native
boundary and review dispatcher remain outside production PLAN_SOURCES. Normal
entry always refuses and normal readiness always returns zero on every platform.
The original legacy CLI body is byte-identical behind a thin wrapper.

## Focused acceptance

Clean signed candidate `5166b44350c9f90f3b2feeebb86cef6094d660cc`, tree
`517d4ffdedbbc9ce680ebd6dabeaef4043fd3c29`, base `35d439d6f09883edeaaa299544508b206e3d2fce` passed
3 unittest groups, 148 boundary cases and 25 CLI cases: 173 literal cases and
190 launches (9 compile/link, 173 drivers, 8 symbol queries), with zero skips.
The run began 2026-10-20T23:57:54.868977+00:00 and took 14.911888 seconds.
[Focused log](native-entry-evidence/focused.log) and
[normalized actual results](native-entry-evidence/results.json) are committed.
All 324 tracked files were unchanged. Three source commits have trusted Cam SSH
signatures and genuine author DCO. Independent actual acceptance has no blockers:
`f1c22186972f8a919f3bf718a7862a279f7ccc4ed77c4594e91e490fc829c213`.

The audit checked 190 actual consuming waits, 380 retained raw streams, all173
literal outcomes, 525 detailed FD probes and 50 successful single-close results.
A fresh bounded observation found all191 recorded PIDs and the owned group absent.
Full clean-candidate verification and normal merge acceptance are recorded later
in the PR and retained coordinator evidence; this record does not claim them.

## What was exercised

The actual native orchestration body uses separately linked models for genuine
reservation representation, original private-clock liveness, identity, held/named
stats and fixed security calls. Exact prefix traces cover all15 live boundaries,
14 work steps, malformed/unknown/replaced material, stat/identity failures and every
fixed unveil/lock/pledge failure. Storage stays borrowed and unchanged. No real
namespace lookup, reservation, clock refresh, security syscall, verifier or native
fixture is supplied by these new boundary models. Existing reservation/lifetime
and ownership tests remain separate evidence.

The actual future dispatcher calls modeled readiness and closed entry; status
mapping and no-fallback behavior are literal. Nonreview invocations delegate once
to the actual renamed legacy body; thin production main, help/version/invalid/plan
and failure cases preserve exact bytes, argv and call counts. Actual normal entry,
readiness and non-OpenBSD primitive bodies refuse. Candidate objects are compiled
for symbols but never substituted into the production planner link.

CLI probes observe inherited0..4 without mutation. A newly owned private pipe is
promoted above4, receives7 known bytes and is read/closed once. All21 before/ready/
after probes require fully valid observations, or inherited-only consistent EBADF
absence. All inherited setup/call and private call masks are zero; all25 canaries
pass. Both private close dispositions are independently observed. Full per-FD
witnesses remain retained locally and bound by each public stdout hash; this
normalized summary omits host FD identities, paths and process identities.

## Failed attempt and fixture correction

The first focused attempt failed all25 CLI aggregate canary assertions while all
148 boundary cases, symbols, budgets and other CLI behavior fields passed. Its
receipt remains `5fc32c2ec75b0efeb673960269dc708d9415ddaea96165d97bd09f1fa3c21c14`;
independent failed-gate handling is `34fefc721d53d0271651f93526d214ffb308ffc9c76e4370561361738659817b`.
Its 190 waits/380 streams and terminal absence were accepted as failure handling,
not a passing test. Its exact failing probe cannot be reconstructed from that
aggregate-only output. Original bytes, raw evidence and every oracle revision
remain preserved; no retry reused its namespace or cleanup authority.

The fixture now establishes private preservation after its own completed setup,
retains old construction-time witnesses, records actual errors, rejects uncertain
probes and attempts both owned closes independently. All173 frozen behavior
expectations and strict canary1 requirements are unchanged. The new witnesses show
all25 private write-pipe status flags change from1 to65537 during own pre-call
setup, with mask2, and remain unchanged across the candidate. That is current
observed setup evidence, not retrospective proof of the missing first-run probe
or a native explanation. Runtime source did not change between focused attempts.

## Budgets and remaining gates

Whole runtime source is100,932/102,400 bytes and fully charged entry16,342/16,384:
1,468 and42 bytes remain. Material16,635/18,432 and planner14,708/24,576 include
complete shared helpers; signature17,162/18,432, capture8,093/8,192 and review/output
12,782/16,384 retain their ceilings. Unexecuted over-cap drafts are preserved.
All runtime/resource/timer limits remain unchanged. Host ASAN/UBSAN and cooperative
270/300-second supervision do not qualify native confinement or application RSS.

Native activation still needs exact OpenBSD profile/tool/media/publisher-policy
bindings, actual locked loader/signify accesses, fd3/4/5 remapping, scratch removal,
ordinary/set-ID refusal, child/descriptor custody, timers/resources and serial/plain/
output acceptance. The provisional8-path profile is not a qualified whitelist.
Only a later reviewed source change after qualification may connect native entry
and CLI readiness. No native guest, security-setting change, live credential,
deployment, retained-image action or consumed native approval occurred.
Historical failed sequential-run custody gaps remain preserved and unrepaired.
