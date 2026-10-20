# Portable internal process-guard evidence

On 2026-10-20 the internal [trusted-entry prerequisite](../design/trusted-entry-output.md)
passed both focused groups on the connected Mac ARM64. The 149 independent literal
cases and source/symbol checks used 155 finite launcher invocations: four build/link
steps, 149 drivers and two symbol inspections. Exact hashes and counts are in
[results.json](trusted-start-evidence/results.json), with the complete successful
[focused log](trusted-start-evidence/focused.log). Full clean-candidate verification
and canonical bindings are recorded separately in the PR.

The supported-path fixture intercepts real guard calls, including Darwin's local
signal-set macros, around the exact included production C. It models identity,
all six resource clamps/readbacks, dispositions/masks, unblocking and both timers.
Independent expectations cover lower/infinite/zero limits, every resource and
signal failure, inherited and post-arm timer validity, all four fixed fatal exit
classes, partial refusal and five reuse cases. Fatal callbacks use an intercepted
exit and nonlocal return; a callback that returns normally fails. These tests do
not change host signal dispositions, limits or timers.

A separately linked uninstrumented guard uses the normal unsupported Mac adapter.
It refuses before process mutation; 30 actual read-only before/after queries verify
unchanged limits, dispositions, masks and timers. The production guard object
exports only `bw_process_guard`; symbol inspection rejects file, path, child,
execution, output and confinement dependencies. The planner's four-source CLI
link remains unchanged.

All 155 direct waits were consumed, none lost; all 310 retained streams reached EOF,
closed and matched publication hashes/lengths. All 289 file hashes before and after
matched. The outer observation found no remaining direct PID or process-group row.
The leader remained unreaped until the final owned-group stop attempt; Darwin
refused that signal while only its zombie leader was observed. The actual consuming
wait and later empty group were recorded separately; no post-wait or recovered-PID
signal was used. Focused unittest time was 12.007 seconds; outer time was 12.248 seconds.

The first focused run remains failed and preserved: one reuse-case oracle lacked
its explicit signal-index metadata, causing a checker KeyError. The independent
author added only those two literal fields. Product source, driver, runner,
envelope and limits stayed unchanged; the reviewed fresh run then passed. Before
any candidate execution, static inspection also corrected Darwin macro substitution.
Its earlier unexecuted fixture bytes were recovered by inverse patch and verified
against the previously frozen hashes, with that provenance retained.

The unchanged owned launcher uses compiler CPU 20/fd 64/16 MiB file/20-second bounds,
driver CPU 2/fd 32/16 KiB file/four-second bounds, CORE 0, 16 KiB per live stream and
five seconds cleanup. The outer envelope replaces the environment, supplies private
HOME/tmp, 270 work/300 total seconds, CPU 90 per process, fd 128, CORE 0, 128 MiB per file,
and sampled 512 MiB owned-output/8 MiB log bounds. Address/undefined-behavior
instrumentation is enabled; unsupported Mac leak detection is disabled. This is
a trusted portable fixture, with no hard memory-isolation claim.

Whole source is exactly 81,920/81,920 bytes, leaving no headroom. The guard's own C,
header and complete shared limit helper total 3,239 bytes. That helper is also charged
fully to the signature component, now 16,245/18,432 bytes; its native child caps and
failure/readback behavior are unchanged. The fused component remains 10,332/11,264.
Earlier source/runtime ceilings are preserved.

These are internal source/model results. The CLI is unlinked; coherent original
startup/capture clock and cancellation ownership, exact native confinement,
complete fixed-fd output and actual native timer/resource behavior remain gates.
No authentication, output, extraction, privilege, target or deployment authority
follows from guard success.
