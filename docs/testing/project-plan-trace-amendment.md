# Proposed planner trace evidence amendment

Status: **Proposed major Test Design decision, 2026-10-18; not accepted or executed.**
This changes the accepted evidence method for PLAN-T07, not the production policy.
[Actual launches 7–8](project-plan-native-progress.md#launch-7-revised-cpu-timer-passes-tracing-interferes)
passed all 5,832 runtime checks. The first traced production invocation failed
because kernel trace writes exceeded its 8 KiB file limit; the partial trace ended
at the limit change. Neither that failure nor the 48 missing trace checks is a pass.

## Decision requested

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
If this evidence tradeoff is declined, keep the trace gate and feature unqualified.

## Evidence-tool basis

[OpenBSD 7.9 ktrace(2)](https://man.openbsd.org/OpenBSD-7.9/ktrace.2) documents
per-process enable/clear, syscall returns, inheritance and append-only records;
a FIFO/device is rejected. Upstream
[kern_ktrace.c revision 1.116](https://github.com/openbsd/src/blob/master/sys/kern/kern_ktrace.c)
writes using the traced current process and stops tracing on a write error.
These explain the proposed observation method; they do not replace native evidence
or identify the installed kernel solely from current upstream source. No upstream
implementation is copied.
