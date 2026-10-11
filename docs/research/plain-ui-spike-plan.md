# Bounded plain-text UI prototype: implementation review target

Status: Proposed, 2026-10-11. Owner/acceptance authority: Cam.
[Issue #4](https://github.com/gonz0w/bootweave/issues/4), parent
[UI research scope](text-ui-runtime-plan.md), [ramdisk inventory](ramdisk-ui-source-findings.md)
and [terminal lookup review](terminal-lookup-findings.md).
Documentation preparation only is authorized. This concrete scope requests explicit
bounded-spike implementation authority under AGENTS.md rule 3; no prototype exists.
It does not adopt a runtime language or approve installer implementation.

## Selected experiment and source constraint

Implement one removable original C plain-text simulation in research/ui/plain-ui.c,
with host-only test/PTY tooling alongside it. No curses, terminal database, parser,
project definition, branding artwork, payload, privilege or installer integration.
Use the fixed name Example project and two synthetic choices, alpha and beta.
No disk identity, install action, secret field or confirmation screen is included.
Completion prints SIMULATION COMPLETE and grants no authority. Confirmation cases
remain deferred rather than represented by a misleading safe-install claim.

The authenticated 7.9 bin/ksh/c_sh.c read implementation accepts p/r/s/u options;
it supplies neither a byte-count nor timeout option, and expands XString storage
while consuming a line before application validation. A later shell length check
cannot establish the proposed input-allocation ceiling. This makes direct builtin
read unsuitable for this bounded-input experiment without another reviewed mechanism.
It does not prove every possible shell approach impossible or select C durably.
The exact inspected SHA-256 is
07d03fa66b035573dfac8a5c15fd89af03cc2bb860cb2909ab7c0ca367bf26a4,
from the [authenticated release archive](../design/native-profile/release-input-evidence.md).
Source inspection executed no shell implementation or build script.

## Phase A: code and host-only checks requested now

Scope acceptance permits creating only the research source/test files and extending
local make verify with their deterministic host checks. No background task, Actions,
network, package, runtime service, source-vendor copy or guest provisioning.
Use the already installed host C compiler and Python test/PTY facilities; record
compiler/interpreter identities. Host compilation is tooling evidence only.
No host binary is uploaded to OpenBSD or used to qualify runtime behavior.

Compile the original source with fixed arguments:
cc -std=c99 -Wall -Wextra -Werror -pedantic research/ui/plain-ui.c
-o .tools/ui-spike/plain-ui-host. The implementation must be warning-clean with
the recorded installed compiler; this is a proposal, not an executed command.
Create only the owned ignored output directory and remove its binary/test fixtures
after retaining sanitized receipts. No compiler substitution/dependency installation.
Use subprocess argument arrays in the host test tooling; input fixtures are bytes,
never command fragments, environment variable names or executable scripts.

The source uses bounded read/poll input, monotonic deadlines, fixed-format output
and termios for owned tty input. No system, popen, exec, dynamic plugin, shell command,
file creation, project-controlled format string, terminal database or privilege API.
Do not load TERM/TERMINFO/TERMCAP/HOME capability data. Measure tty dimensions using
ioctl where available, validate them, and report unknown dimensions honestly.
Pipe-mode tests establish data semantics only; PTYs supply terminal restoration tests.

## Required behavior and engineering bounds

States: label input, choice, review, complete or cancelled. Printable h shows fixed
help; b returns one state retaining validated values; q cancels. Empty label is
invalid; labels permit only ASCII letters, digits, space, underscore and hyphen,
maximum 64 bytes. Choice accepts only fixed alpha/beta tokens. Review supports
back, help, cancel and a printable completion command. No Enter default completes.
Output includes the simulation banner and full validated label/choice values.

Input is bounded while reading, before parsing or allocation. Use a fixed 1024-byte
line buffer; overflow rejects the entire line, never a truncated valid prefix.
Drain only within the same deadline and byte-work budget, otherwise cancel.
Reject NUL, ESC, other C0/C1, high-bit/malformed bytes and unsupported key sequences
without echoing them. Delimiter CR/LF normalization must have explicit tests; one
CRLF pair must not become two actions. Do not reinterpret data as terminal commands.
All output is trusted fixed ASCII or validated ASCII data, with trusted line breaks;
no color, cursor motion, alternate screen or decorative control sequences.

On tty input save attributes, disable echo so rejected controls never reach the
terminal through driver echo, and use explicitly reviewed input flags for byte
handling. Restore exact saved attributes on normal exit, EOF, timeout, cancellation
and catchable termination. Check every terminal/output operation; failure cancels.
Forced kill cannot promise restoration: the owned host PTY supervisor checks and
restores it out of band, records uncertainty, then closes the fixture. No external
terminal is altered by tests. Signal handlers perform only signal-safe notification;
cleanup occurs in controlled main-loop flow. Native behavior is still unqualified.

Ceilings: 64 KiB aggregate application buffers/output, 1 KiB pending line input,
32 MiB process resident memory, 10 seconds of input inactivity, 60 seconds per case,
5 seconds from completed input to response, 10 MiB private capture and 20 MiB total
owned host test allocation. Monotonic time applies to every read/drain loop. Source
size ceiling is 32 KiB; no imported library/runtime is added. Record actual metrics
and unavailable measurements. Exceeded limits stop the case without completion.
The 16 MiB media-growth ceiling remains untested because no media is built here.

At known 80x24 and 40x12 dimensions wrap/paginate fixed messages and display every
required value; do not clip. At unknown/unusable dimensions retain sequential plain
text and mark screen-fit unqualified. Resize invalidates pending review and returns
to an uncompleted state or cancels; no resize/EOF/timeout/control input can complete.
Do not claim terminal-size fallbacks are measurements.

## Phase A Test Design and acceptance

| Case | Action | Required host result |
|---|---|---|
| PUI-01 | Valid label/choices, help/back, completion command | Fixed synthetic state/value outcomes; banner always visible |
| PUI-02 | Empty/invalid/64-byte/65-byte labels and invalid choice | Safe rejection with no unintended transition |
| PUI-03 | ESC/OSC-like bytes, NUL, C0/C1 and high-bit data | Rejected; no injected controls or unvalidated text in output |
| PUI-04 | 1024/1025-byte lines, large paste and unterminated input | Bounded memory/work; reject or cancel; no prefix accepted |
| PUI-05 | CR, LF, CRLF, EOF, idle timeout and output failure | Explicit delimiter semantics and incomplete/cancelled outcome |
| PUI-06 | PTY 80x24/40x12, zero/unknown dimensions and resize | Required text available; uncertainty explicit; review invalidated |
| PUI-07 | Quit, INT/TERM/HUP, forced kill and broken console | Owned PTY attributes restored or out-of-band uncertainty recorded |
| PUI-08 | Static call/dependency review and measured ceilings | No installer/privilege/file/network authority; limits respected |

Tests must exercise observable state, output, limits and terminal postconditions,
not merely mirror functions. Include failure cleanup; no test touches live devices
or real secrets. Local make verify must run and all existing foundation checks pass.
Publish clean base/head/tree, exact code hash, build arguments, actual outcomes,
compiler/host identity and native-unqualified limits. A host pass is Phase A only.

## Phase B: native evidence explicitly deferred

No native launch, guest command expansion, script upload, compilation, writable disk,
installed guest, customized ramdisk or media build is authorized by Phase A approval.
Before Phase B, review the completed exact source/hash and native compile/run/test
commands, provision a separately approved disposable native build environment and
freeze its verified artifacts, attachments, console transport and cleanup.
Stock bsd.rd lacks a compiler; a host binary or source copy cannot bypass that gate.
Do not infer writable install authority from the existing diskless run approvals.

Native cases repeat applicable parent UI tests on fresh VGA and serial fixtures.
Installed-guest results cannot qualify ramdisk inclusion; UI-10 needs a separately
reviewed native media-build and boot experiment. Secrets, confirmation freshness,
curses comparison and destructive operations remain outside this minimal prototype.
No candidate adoption follows without complete native evidence and scoped design/ADR.

## Concrete acceptance and removal

Approval sought: implement Phase A's original bounded C simulation and host tests,
run local compiler/PTY checks and make verify, publish signed DCO evidence in an
open PR, retaining all Phase B gates above. Maintainer acceptance must be recorded
before code is written; merging this plan alone is not implementation authority.
The prototype is removed by deleting research/ui files and its local check entry;
there is no runtime/configuration/privilege interface to preserve.
