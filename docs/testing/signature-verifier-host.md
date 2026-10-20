# Fixed signature-stage portable evidence

The [scoped design and Test Design](../design/signature-verifier.md) implement
only the internal fixed detached-signature stage of issue #5. The common source
consumes exclusive prepared material and never selects a project path, policy or
executable. Production main/Makefile linkage is unchanged; no CLI invokes it.
Non-OpenBSD production objects refuse before fork. A future trusted OpenBSD entry
must establish the adopted exact locked unveil table and hard overall limits.
That entry, actual signify acceptance, policy/digest authentication, extraction
and privilege authorization remain separate unqualified work.

## Observed focused result

On 2026-10-20 focused r8 passed all 14 unittest groups, zero skips,
with 82 inert driver invocations: 50 actual fixed test children, 29 no-child cases
and 3 nonspawning syscall models. The 14 groups include pure observer/assertion
controls. It ran 01:18:13.666–01:18:21.206 UTC, 7.539 seconds total.
[Source-bound results](signature-verifier-evidence/results.json) and the
[passing log](signature-verifier-evidence/focused-r8.log) retain the observations
and original/sanitized log hashes. Base:
2f32fb16fee2c6f3f3e29d6a57938f1784ee640f.

The real common orchestration/material implementation was linked with a fixed
in-child execve double and static native adapter. Inert format-valid records
contain no private key; no actual verifier, key generator, shell or project code
runs in these tests. The double checks actual descriptors 0..5, exact retained
bytes, fixed argv/environment, removed unrelated child descriptors and unchanged
parent sentinel. It emits only controlled private diagnostic bytes and exits.
The production function never writes raw verifier output to caller stdout/stderr.

Observed controls cover unsupported/identity/signal/descriptor preflight,
child mapping/limits/boundary/exec failures, group/gate failure and real EPIPE,
exact 4096/overflow diagnostics, cancellation, work/cleanup/final-boundary timing,
once-only close ownership, unknown close quarantine, non-reaping observation
before stop and actual consuming wait, and refusal after lost wait. Lost-wait
faults are nonspawning models and dispatch no actual numeric signal/wait.
Actual child alarm 5 fires independently while the modeled parent clock is frozen.
The test-only two-fd rendezvous makes each real child wait after common alarm
setup until the parent's actual setpgid succeeds. Successful and failed syscalls
are separate records; actual versus injected failure and immediate errno remain
explicit. Failed requested groups never become observation/signal authority.
This controlled ordering does not qualify native concurrent group behavior.

Native DATA/STACK, pledge and closefrom outcomes are explicitly modeled on this
Mac. Host child CPU/fd/core/file/alarm controls and fork/pipe/wait behavior were
observed. Darwin can refuse a group signal with EPERM while completion is pending:
that remains production uncertainty. Only a positive unreaped completion witness
lets this fixed no-descendant test double model the native stop result, with the
actual refusal separately recorded. Positive stage controls require acceptance;
the early-rejection exception permits uncertainty only for the recorded host stop
refusal, consumed wait, no lost wait, zero unknown-close masks and no cleanup-budget
failure. Pure negative controls enforce that exact exception.

All 82 case ledgers preserve independent identity rows, known setpgid operation
results, actual/unavailable group observations, state and original input/canary
device/inode/mode/hash parity. Later unavailable samples never erase positive
history or authorize a signal. The outer supervisor uses only newly created
ledgers; historical builds are references. Actual born IDs and positive observed
or syscall-established groups were absent at terminal observation. Original
leaders remain unreaped through the final stop attempt; actual wait consumption
is latched before interpretation. No post-wait or recovered-ID signal occurs.

## Bounds and preserved failures

One owned foreground worker, replaced minimal environment and no inherited
credentials/network request. Work 270 / total 300 seconds, CPU 90/process, fd 128,
CORE 0, 128 MiB/file and sampled 512 MiB HOME/tmp/current-tools plus 8 MiB log limits
bound the outer run. Driver processes use CPU 2 / fd 32 / CORE 0 / 16 KiB file and 15-second
timeouts; common child alarm 5 is independent. RSS is observed, not hard memory
isolation. No sandbox, hostile-worker qualification or global cleanup proof is
claimed. The [retained envelope source](signature-verifier-evidence/portable-envelope.py)
and the [corrected full-check envelope](signature-verifier-evidence/portable-envelope-r2.py)
are source snapshots for review, not relocation-safe launchers or permission to
reuse a completed job/approval. Full clean-candidate make verify and exact
signature/DCO/canonical binding are recorded in delivery review, separately from
this focused result.

The six earlier focused attempts remain failures, with original case directories, caches,
objects, logs, receipts and ledgers retained locally. Published logs redact the workspace prefix and strip trailing whitespace to meet
repository checks; original and sanitized hashes are distinct in results.json.

| Attempt | Actual stopped result |
| --- | --- |
| r1 | Fixture setup exit 94 before verifier call; insufficient stage diagnostic. |
| r2 | Sentinel allocation exit 94/EMFILE with READY material; open versus dup not distinguished. Lowering the test sentinel minimum from 28 to 20 within the same fd 32 cap allowed continuation; no stronger OS root-cause claim. |
| r3 | First 11 controls passed, then observer rejected an unavailable zombie PGID. Positive birth/group history and consumed wait were retained; original completeness failure remains. |
| r4 | First 33 controls passed; material-close-0 correctly returned uncertain with readonly-role bit 3. Test expected bit 0 and failed. |
| r5 | First 36 controls passed; overflow correctly preserved actual stop uncertainty. Test incorrectly required ordinary rejection. |
| r6 | First 57 controls passed; early-death child raced parent group establishment and correctly caused GROUP rejection. A bounded test-only rendezvous now establishes the group before child exit, proving actual EPIPE. |

Production common source remained unchanged throughout these fixture corrections.
The first [focused green r7](signature-verifier-evidence/focused-r7.log) remains
preserved. Its subsequent [full check](signature-verifier-evidence/full-r1.log)
failed one of 164 tests: the overflow fixture was refused at actual GROUP setup
before launch. The group's syscall errno was not captured, so no exact OS
root-cause is claimed. The test rendezvous now orders parent establishment first,
with actual failure metadata retained rather than broadening expected causes.
That full attempt also skipped the eight legacy authentication tests because
this worktree lacked the existing pinned verifier-tool link; it stopped before
the final secret scan. Its one failure and one skipped class remain recorded.
The same existing tool's source/binary pins were checked and its owned link
restored, with no acquisition or security-setting change. The new inert tests
never use it. A corrected full check must execute 172 tests with zero skips,
including the eight unchanged host authentication regressions, and reach secrets.

No native run, live credential, installed target, service, deployment or renewed
native authority was involved. Historical evidence and primary worktrees remain
preserved. Issue #5 remains open for the remaining trust/extraction/privilege work.
