# Text UI and runtime: bounded research plan

Status: Proposed, 2026-10-11. Owner and acceptance authority: Cam.
[Issue #4](https://github.com/gonz0w/bootweave/issues/4).
Acceptance date/source: pending. Documentation preparation is authorized; prototype
implementation, native launches and media modifications require accepted scope or
explicit bounded-spike authorization. No language, renderer or parser is adopted.

## Operator outcome and scope

Evaluate an original BBS-inspired guided text presentation with keyboard, serial,
low-color and plain-text flows. All modes must give the same validation, help,
cancellation and confirmation outcomes. Decorative lightbars, frames and shadows
must never hide the selected target, error or required action.

Use the [first native profile](../design/native-profile/PRD.md) and its isolation,
verification and cleanup rules. Coordinate confirmation/failure semantics with the
[installer research plan](installer-integration-plan.md) and project input trust
with issue #5. This spike has no installer invocation, disk operations, services,
network, real credentials or product policy. Any confirmation is visibly a synthetic
simulation with no privileged backend. A successful simulation is not disk safety
qualification.

## Candidate comparison and evidence

| Candidate | Question to measure | Limits and removal cost |
|---|---|---|
| Base C with curses/terminfo | Keyboard decoding, layouts, colors, memory and library/database availability in ramdisk | Requires native compile/link and file-level license/provenance review; remove renderer/library/data without changing validation semantics. |
| Base C with line-oriented text and bounded terminal handling | Small footprint, input validation, terminal restoration and serial behavior | More terminal handling responsibility; plain flow must remain independently usable. |
| Base shell with plain prompts | Whether base utilities support bounded safe input without shell interpolation | Shell process/error/secret boundaries need native evidence; shell UI is not permission for shell-based privileged operations. |

Compare only these base-tool candidates initially. A significant additional library
or runtime requires dependency review, provenance/license analysis and adoption ADR.
Python remains development-only; this document does not select it for the guest.
Direct arbitrary ANSI output is not an assumed portability solution.

Primary references inspected 2026-10-11:
[7.9 terminfo(5)](https://man.openbsd.org/OpenBSD-7.9/terminfo.5) describes capability
data; [curses(3)](https://man.openbsd.org/curses.3) describes screen and input library
facilities; [termios(4)](https://man.openbsd.org/termios.4) describes terminal control.
Version-specific web fetches for curses/termios were unavailable during preparation;
these two mutable references are orientation only. Inspect authenticated 7.9 source
and installed manuals before execution. Base-system availability does not establish
ramdisk inclusion or a tested serial/UI profile. Source digests remain outstanding.

## Proposed interaction semantics

The synthetic fixture contains a generic project name, a bounded text field, a
choice, validation error, help page, review summary and simulated target confirmation.
No profile format or future public API is specified. Use fixed reviewed fixture
values instead of a configuration parser.

Navigation retains validated values on back/help, identifies focus using text as
well as styling, and provides documented printable-key alternatives to arrows.
Invalid input displays a safe error and does not advance. Cancellation and EOF
leave the simulation unconfirmed. Empty input, timeout, unknown key sequences,
paste bursts or redraw cannot confirm a target. Confirmation requires a fresh
explicit operator action after the complete synthetic identity and effect summary
are visible; changed choices invalidate it. Switching presentation mode cannot
supply or preserve a pending destructive authorization.

Plain-text mode emits readable sequential prompts without cursor-addressing,
colors, alternate-screen or artwork requirements. Required prompts must work at
80x24 and in a 40x12 narrow case by wrapping/paging. Full identity/effect information
cannot be silently truncated. At unusable dimensions, offer plain prompts or stop
without confirmation. Do not rely on CP437, Unicode box characters, mouse input or
color to communicate meaning. Original artwork is optional and independently
reviewed; do not copy third-party art.

## Trust, terminal state and failure

Project/operator strings and TERM/dimensions are untrusted. Treat text as data,
never a format string, command or terminal control program. Permit display controls
only from the trusted renderer; escape or reject embedded ESC, CSI/OSC, C0/C1,
carriage-return spoofing and malformed encoding from data. Review trusted terminal
capability lookup locations and bound dimensions; do not load arbitrary project
supplied capability files. Test invalid/missing TERM and absent database entries.

Use synthetic secret-like fields only. They must not appear in ordinary summaries,
logs, argv or exported evidence. Bound input length, pending escape sequences,
redraw work and capture. No shell xtrace or debugging bypass. Prevent buffered or
pasted input from carrying into confirmation. Freeze decoding/timeout policy in the
prototype run plan, then test delayed serial escape bytes against it.

Record terminal attributes before launch; restore them on normal exit, cancellation
and catchable signals. A forced kill cannot promise application cleanup: verify and
restore the owned console out of band before reuse. Lost console, output failure,
allocation failure or invalid terminal state stops the simulation without confirming.
No automatic restart or unattended installation follows renderer failure.

## Research phases and resources

First inspect pinned release source, build configuration, library/data dependencies
and license headers. Then propose minimal reviewed prototypes and exact compile/run
commands. Prototype code needs explicit spike authorization before implementation.
Compile within a disposable OpenBSD build guest using base tools. Record static and
dynamic linking alternatives and their complete transitive media cost.

An installed guest may establish runtime behavior but not bsd.rd feasibility.
A separate reviewed media-build experiment must prove required binaries/libraries/
capability data exist in the ramdisk and boot with the same behavior. Keep original
stock media as control; modified media gets its own reviewed provenance/digest.
No installation is allowed in this UI spike; build-guest provisioning has its own
confirmed disposable-disk authority and preflight.

Retain native profile limits. Additional proposed spike ceilings: 16 MiB incremental
uncompressed runtime/library/data footprint, 32 MiB prototype resident memory,
1 KiB per input field, 64 KiB pending input/display buffers and 5 seconds from
completed local input to next prompt. Measure startup, input-to-redraw, idle CPU,
RAM, compressed/uncompressed media growth and capture size. These are investigation
ceilings, not measured support guarantees; report unavailable metrics and stop if
exceeded. Distinguish serial transmission/operator delay from renderer latency.

## Test Design

| ID | Fixture/action | Expected evidence and gate |
|---|---|---|
| UI-01 | Identical valid/invalid/help/back/cancel sequence in each candidate/mode | Equivalent values, errors and outcome; native runtime required. |
| UI-02 | VGA keyboard and private serial, printable alternatives and delayed escape bytes | Navigation works; no timeout/partial sequence confirms. |
| UI-03 | Mono/low-color, missing/invalid TERM, missing capability entry | Readable safe fallback or explicit stop without authorization. |
| UI-04 | 80x24, 40x12, live resize and unusable dimensions | Full required summary available; no clipped prompt or redraw confirmation. |
| UI-05 | Synthetic ESC/CSI/OSC, C0/C1, malformed bytes and oversized text | Escaped/rejected data cannot move cursor, spoof prompts or exceed buffers. |
| UI-06 | Paste, queued input, Enter/EOF/timeout at confirmation and changed choices | Only fresh explicit action confirms simulation; all other cases remain unconfirmed. |
| UI-07 | Cancel, catchable signal, forced termination and broken output | Safe stop; terminal attributes checked/restored, uncertainty reported. |
| UI-08 | Synthetic secret field and bounded diagnostics | No disclosure in summary, logs, argv or sanitized export. |
| UI-09 | Native builds and representative screens at limits | Measure resources and dependencies against ceilings; host checks insufficient. |
| UI-10 | Separately authorized ramdisk build/boot | Dependencies present and UI cases repeated; installed-guest results alone insufficient. |

UI-01 through UI-10 are not run. Manual/source review establishes hypotheses only.
Host unit tests can check bounded data handling after code exists but cannot replace
native console behavior. Reviewers compare semantic outcomes separately from screen
appearance. Capture test control bytes privately and publish escaped excerpts only.

## Cleanup, decision and adoption gate

Use fresh isolated fixtures, no network/sharing/passthrough and private endpoints.
Stop on isolation uncertainty, unexpected privileged action or exceeded bounds.
Terminate owned processes, restore/check terminal state, remove owned fixtures and
confirm absent processes/listeners/attachments. Incomplete cleanup is a failed run.

Publish exact repository base/head/tree, source/artifact/backend/console identities,
commands/tool versions, input cases, actual outcomes, resources, skips and teardown.
Compare usability, safety, ramdisk feasibility, maintenance and removal costs.
Recommend a candidate only after evidence; adoption needs scoped PRD/EDD/Test Design
and ADR for durable runtime/dependency choices. Rendering cannot approve privileged
operations or turn stock-installer defaults into a safe engine contract.
