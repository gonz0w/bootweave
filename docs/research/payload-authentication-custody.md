# Host authentication wait and cleanup correction

Scope: source-only correction for issue #78, on canonical base
`22761113c5c1ffbc8e42ff3b25872b641bd0db07`. Earlier host and native evidence
remains historical. No vulnerable helper is launched to reproduce the finding.
The owner authorized finite injected regressions, independent review and eligible
source PR work. No guest, installation or renewed native authority follows.

The old controller could reap before resource/input validation, then signal the
reaped numeric group on refusal. Selector closure could also suppress every later
cleanup stage. A surviving original group member reserves its PGID; the reuse
concern requires the original group to empty after reap and be reallocated. This
is a source authority finding, not an observed reuse or leak.

## Engineering design

Keep an immutable born-process binding and explicit wait-consumed/lost state.
Require default SIGCHLD semantics and non-reaping waitid/WNOWAIT support before
spawn. Observe completion without reaping; the original session leader remains
unreaped through the final positive group signal. Record wait consumption as soon
as wait4 returns the child, before status conversion, RSS or input validation.
Never signal or wait again after consumption, or signal after wait ownership is
lost. Sampled process identities cannot renew that authority.

Every group signal occurs before the sole consuming wait. Stop the owned group
before normal terminal collection too, retaining the leader's genuine exit status
and rusage. The claim covers that group, not a separately detached session.

Selector close, stop, nonblocking bounded reap and each stream close are separate
terminal stages. One failure cannot skip later stages. Preserve the original
failure and attach separate cleanup-stage notes; cleanup failure prevents success.
Retain the fifteen-second child and forty-five-second workflow ceilings, reserving
up to one second (one quarter of a shorter allowance) for terminal collection.
No callback is consulted to authorize a signal. Lost/unresolved ownership remains
uncertainty, never cleanup success.

The first corrected portable run exposed Darwin EPERM on a group containing
only its exited, unreaped leader. That failed run remains retained. EPERM is not
treated as a successful stop: while the born leader remains unreaped, a fixed
read-only ps query must completely show no live member in the group. A live,
malformed, failed, empty or timed-out observation preserves refusal. Its timeout
is bounded by the remaining child allowance and 250 ms. This observation cannot
restore consumed/lost wait custody. Two injected groups cover these outcomes.

## Behavioral test design

| ID | Independent oracle |
| --- | --- |
| PAV-LC-01 | A child reaped with excessive RSS refuses; event trace contains no positive signal after the wait event. |
| PAV-LC-02 | A status/usage conversion failure after actual wait cannot trigger a second wait or numeric-group signal. |
| PAV-LC-03 | Injected selector closure failure cannot suppress stop, reap or independent stream closure. |
| PAV-LC-04 | Lost wait ownership refuses signals and preserves the original failure and cleanup uncertainty. |
| PAV-LC-05 | Primary interruption survives multiple terminal-stage failures; every later stage is attempted. |
| PAV-LC-06 | Repeated terminal collection consumes actual wait at most once; no later positive signal is possible. |

Run the injected suite with every process/selector/wait/signal effect replaced
before any candidate helper execution. After independent source review, use the
existing finite host authentication checks and required local make verify, with
owned fixture identities and an independent terminal audit. Native evidence and
historical source hashes are not rewritten by this correction.
