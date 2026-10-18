# Proposed planner trace evidence amendment

Status: **Accepted continuation, 2026-10-18; all 50 additional native checks and
independent guest readback/cleanup passed. Host disposal awaits explicit approval
required by automatic approval review.**
After the concrete proposal and explicit design question, Cam directed: “keep
batching work and knocking out the backlog until we have a PR ready”. This is
recorded as direction to proceed with this test-only method, not as a selected
answer to the prior question. Production policy and resource limits stay unchanged.
This changes the accepted evidence method for PLAN-T07, not the production policy.
[Actual launches 7–8](project-plan-native-progress.md#launch-7-revised-cpu-timer-passes-tracing-interferes)
passed all 5,832 runtime checks. The first traced production invocation failed
because kernel trace writes exceeded its 8 KiB file limit; the partial trace ended
at the limit change. Neither that failure nor the 48 missing trace checks is a pass.

## Accepted evidence method

Accept separately labelled dynamic/static **test-only trace companions** built from
the same frozen CLI/parser/IO/confinement sources. Interpose only the confinement
object's `setrlimit` call: when FSIZE is requested, require the exact expected
8,192-byte soft/hard bounds and forward a fixed 4 MiB soft/hard limit to the real
kernel call. Reject an unexpected request. Forward all other resources unchanged,
including errors; preserve checked closefrom, privilege refusal, unveil, pledge,
two-second CPU timer/limit and five-second deadline. No production source switch,
environment selector or shipped test binary. Normal production builds retain
FSIZE=8 KiB and their real native enforcement probes.

This deliberately accepts **source-correlated companion traces**, rather than
claiming that a modified binary is the exact production executable. Those traces
support syscall ordering/absence observations; they do not establish production
8 KiB enforcement or binary identity. The existing normal production evidence
supplies resource/oracle results. This tradeoff requires review because the
original plan explicitly required traced production executions.

## Bounded implementation and acceptance

- Add one original small test wrapper and a separate native confinement object
  compiled with only `setrlimit` renamed. Link companions from unchanged production
  source bytes; record source/object/build-command/binary/map bindings. Verify the
  wrapper accepts exactly the one fixed FSIZE substitution, forwards other limits
  and failures, and is absent from normal build commands and binaries.
- Add dedicated trace modes to the trusted test supervisor. Use root-owned 0600,
  exclusive, single-link regular trace files. Enable syscall/return/signal inheritance
  immediately before each case fork; clear only the supervisor's own tracing after
  fork, without descending into children. Check every tracing operation. This
  keeps parent polling from flooding the trace while retaining child exec/startup,
  privilege checks, limits, confinement, input and exit/signal records.
- Run the same two positive cases and 46 edge cases on companion variants, including
  separately owned set-ID copies. Keep exact independent text/status/descriptor/
  canary oracles. Correlate all 48 child PIDs. Check successful confinement before
  input, no later path open/exec/fork/socket attempt and no read(0) for disallowed
  descriptor/root/set-ID cases, accounting separately for loader and supervisor.
  Every PID needs complete expected terminal evidence; absence from a truncated
  trace proves nothing. Check native trace-write error logs before/after the stage.
- Retain the existing 4 MiB limit **per raw trace**, 1 MiB per decoded trace/receipt,
  32 MiB aggregate guest test allocation and all compiler/supervisor/emulator/
  capture/deadline limits. Fail before exhaustion with at least 4 KiB raw headroom;
  reject missing/malformed/truncated records or write errors. No rolling truncation,
  signal suppression or accepting partial evidence. The original failed trace
  remains untouched. Base `ktrace`/`kdump` remain evidence tools only.
- Preserve the completed original `/planner` tree and binaries. Prepare sealed
  trace-only source/media and compile only qualification artifacts in an exclusive
  owned `/planner-trace` tree on the same original target; no reinstall/reset or
  additional disk. Both trees count toward allocation. Recheck exact original
  retained metadata/digests and all host/media/tool identities before continuation.
  Freeze and independently verify changed source/helpers/media before attachment.
- Plan one bounded build/trace launch, then a separate independent readback/cleanup
  launch (nine and ten, counting the eight consumed). Existing original deadlines,
  paused full device/process/digest audits, stop-on-failure, exact conditional
  disposal and native halt/host closure requirements remain. Capture disk-sync
  completion explicitly; launch 8 did not capture that frame. Routine preparation,
  fixes and bounded recovery follow the standing continuation instruction.

This proposal adds no dependency or production capability, drops no runtime case,
and raises no production acceptance limit. New wrapper/regression checks supplement
the 48 trace cases. Qualification remains incomplete until accepted implementation,
complete source-bound trace results, independent retained readback and exact teardown.
Any incomplete or failed trace leaves the trace gate and feature unqualified.

## Evidence-tool basis

[OpenBSD 7.9 ktrace(2)](https://man.openbsd.org/OpenBSD-7.9/ktrace.2) documents
per-process enable/clear, syscall returns, inheritance and append-only records;
a FIFO/device is rejected. Upstream
[kern_ktrace.c revision 1.116](https://github.com/openbsd/src/blob/master/sys/kern/kern_ktrace.c)
writes using the traced current process and stops tracing on a write error.
These explain the proposed observation method; they do not replace native evidence
or identify the installed kernel solely from current upstream source. No upstream
implementation is copied.

## Prepared implementation and exact continuation

The original runtime source bytes are unchanged from native candidate `fab0816`.
`BW_TRACE_POLICY` compiles only the fixed limit wrapper; `BW_NATIVE_TRACE` compiles
the separate trusted supervisor modes. Neither macro is in production build
commands. The original `/planner` binaries, source and failed trace remain intact.
The supervisor brackets each fork with checked tracing and checks the held/named
regular-file identity, root ownership, 0600 mode, single link and size headroom.
Five host regression groups cover real limit forwarding/refusal, sealed unchanged
runtime sources, all 48 PID/oracle layouts, missing returns/terminal records,
forbidden attempts, disallowed input reads, malformed/oversized and uncorrelated
records. Native behavior is not inferred from host checks.

The trace bundle has **twelve files, 2,037,106 bytes**, including its SHA256 listing:

`d04ee612b8b67eab57c47c1ae3f1735faa27290095b1491da6bbed451bb923dc`.

[Exact trace bindings](project-plan-trace-bindings.txt) are separate from the
normal preparation listing. The latter now includes the guarded trace hooks in
the test helper; it does not rewrite the actual launch-7/8 source listing, preserved
at [native candidate fab0816](https://github.com/gonz0w/bootweave/blob/fab0816bad2de10e7c45bff3e50f9deb3798eea1/docs/testing/project-plan-native-bindings.txt).
The independent case header and manifest are unchanged. Runtime source remains
14,337 bytes; all six native helper C sources remain below the 48 KiB ceiling.

The separate nonbootable read-only CD has **675,276,800 bytes**, SHA-256:

`f88d606b7722f14fd14f38a548351243742885d033f56b3d56c3989ef3ffd104`.

It contains `/planner-trace` and the unchanged six selected stock release files
and signed manifest. Explicit-key signature/checksum verification and independent
full extraction/byte inventory passed. Preparation preserved the original stopped
disk digest and both older media; it did not start a guest. Attachment is bound by
a separate private continuation receipt after source/helper/tool/target checks.

Launch 9 preserves and independently rechecks the exact 52-entry original tree
before creating `/planner-trace` exclusively. It reuses only the verified original
compiler guard, makes seven guarded compiler invocations, records nine executable,
two object and two map hashes/linkage, then runs two real-limit helper checks and
the 48 traced cases. Original normal runtime checks remain 5,832; the complete
required count becomes **5,882** with these two additional helper checks. Native
traces and `kdump -t cs` observations remain bounded, private and independently
checked. A failure stops dependent work and preserves both trees.

Launch 10, dependent on actual complete trace success and normal native shutdown,
independently compares all retained source/artifact/input/canary/trace/receipt
hashes and exact metadata without rerunning a planner/writer. Only the checked
owned `/planner` and `/planner-trace` trees and created mountpoint may be removed.
Prove native absence, no workers, unmount, explicitly observed disk-sync completion
and native halt, then complete original-target/media/tool hashes and host closure.
Exact original fixture, all three derived media/source candidates and owned private
helpers/evidence are logically disposed only after those guards. Target authority
then ends. Nothing broadens installation, services or live-network authority.

Local `make verify` passed **121 groups** (39 tooling, 13 UI, 3 storage, 10 handoff,
33 payload, 23 planner/preparation/trace), repository checks, shell lint and secret
scan. This includes the unchanged portable native-supervisor rehearsal and five
new trace-policy/evidence groups. Host-only results do not qualify native tracing.

## Routine build-binding correction after launch 9

Pre-execution review found that the retained compiler guard rejects cwd outside
`/planner` and fixes TMPDIR there. Launch 9 ran no compiler or native case; original
52-entry metadata/50-file hashes matched exactly and native disk sync/poweroff/full
host closure passed. The corrected script calls the same pinned guard from its
required cwd with fixed absolute `/planner-trace` input/output paths. It creates
`/planner/compiler-tmp` exclusively as root-owned 0700, then removes it with `rmdir`
after the seven successful guarded calls. No original file is modified. Only the
original root directory's bookkeeping/timestamps may change from that transient
child; retain exact identities/modes/owners/links and all regular-file metadata.
Failure preserves the exact partial directory. Total guest allocation still counts
both roots and temporary files, with the original 32 MiB ceiling.

The new twelve-file source candidate is 2,037,561 bytes, manifest SHA-256
`b39814630f4692f25589139bada8df32d0b020bdc6cfe3f60d24a2ced7095817`.
Only the build script changed; all C sources and independent cases remain unchanged.
Current [trace bindings](project-plan-trace-bindings.txt) describe this correction;
the earlier sealed media/source are preserved. The bounded stages become launches
10 (build/50 checks) and 11 (independent readback/teardown), counting all nine used.
Host sealing/refusal tests and shell lint passed after the correction. Native
execution is still required; no failed or unattempted case is counted as passed.

Corrected nonbootable CD: 675,276,800 bytes, SHA-256
`4987679ca56d91ee45bce3f559ad4b9194835e95db05bcb1f9b4a95dd8be809a`.
Selected stock files/signatures and independent extraction/byte inventory passed;
the stopped original disk and all older candidates were unchanged.


## Completed native evidence

[Launch 10](project-plan-native-progress.md#launch-10-complete-trace-qualification-and-retained-state)
passed all seven guarded compiler calls, two native real-limit helpers, two positive
traces and 46 edge traces. All child PIDs, checked confinement/input ordering,
forbidden-call absence and expected terminal records were independently validated;
no new kernel trace-write error occurred. The host observer was corrected to account
for a proven loader descriptor-zero lifetime before planner entry, retaining the
required later EBADF and absence of engine reads. Regressions reject missing
open/close/EBADF evidence. Native source/artifacts/oracles/traces were unchanged;
no native retry supplied this observer correction.

[Launch 11](project-plan-native-progress.md#launch-11-independent-readback-and-exact-guest-cleanup)
matched all 85 entries/81 hashes independently, removed only checked guest fixtures
and observed disk sync/normal poweroff. All original host fixtures/source/media and
private helpers remain because automatic approval review requires explicit host
cleanup approval. Native test results do not make that unperformed disposal a pass.
