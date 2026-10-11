# Plain-text simulation: Phase A host evidence

Issue #4, observed 2026-10-11. Cam accepted Phase A by continuing after the explicit
implementation approval question on PR #26. [Accepted spike scope](plain-ui-spike-plan.md)
permits original C simulation code and host checks only. No OpenBSD process,
installed build guest, media modification or installer implementation occurred.

Base: 0d1143b180b6b9bdc3356b3c7c1ecd8d26b099df.
Base tree: 4466bdc8cca9da55e2bca933f38f7070e572ecad.
The final candidate identity is recorded in its PR; exact source identity below.
Host: macOS ARM64; Apple clang 21.0.0 (clang-2100.3.20.102), arm64-apple-darwin27.0.0;
Python 3.14.7. No compiler, library or dependency was installed.

## Performed and observed

Implemented original [research source](../../research/ui/plain-ui.c) and
[host tests](../../research/ui/test_plain_ui.py), BSD-3-Clause under repository terms.
No upstream code was copied. Local make test-ui compiles a new owned temporary binary
with cc -std=c99 -Wall -Wextra -Werror -pedantic and runs deterministic pipe/PTY
cases. make verify now includes these checks alongside existing foundation checks.
All compilation and execution are on the host, not OpenBSD qualification.

The simulation offers fixed Example project label/alpha/beta/review flows. Help,
back and keep preserve validated values; q cancels. Only literal complete at review
can finish the simulation. Empty input has no completion default. Queued input may
complete this synthetic flow; it has no destructive confirmation/authorization
meaning. Secret fields and installer actions are absent.

Fixed 1024-byte input storage rejects whole lines containing controls, high-bit bytes
or overflow; no truncated prefix advances. Labels have a separate 64-byte ASCII bound.
Total input work is capped at 65536 bytes, output at 60000 bytes, and application
buffers/output remain below 64 KiB. No arbitrary data is a format string or command.
Input echo/canonical processing are disabled only on the owned tty, with exact saved
flag/control-character/speed verification at cleanup. Queued simulation input is
flushed on exit. Unknown/unusable dimensions are reported as unqualified; valid
widths wrap text rather than clip. Resize cancels instead of preserving pending review.

13 host regression methods cover PUI-01 through PUI-08 with valid/help/back/keep,
CR/LF/CRLF, empty/invalid/64/65-byte labels, invalid choices, control/high-bit bytes,
1024/1025-byte lines, large/unterminated paste, EOF/quit/no-Enter-default, input timeout,
blocked/broken output, 80x24/40x12/unknown PTYs, control echo suppression, resize,
INT/TERM/HUP, SIGKILL supervisor restoration and actual console loss.
No native UI-01 through UI-10 test is marked passed.

A preliminary full host run passed all 14 foundation and 13 UI regressions in
25.527 seconds for the UI suite, plus content/link/lint and secret checks. Subsequent
final-source and committed-candidate verification results are retained in the PR.
The final source remains warning-clean; all published pass claims apply to that
verified candidate rather than an earlier build.

## Failures corrected and measurement limits

Initial host compilation hid SIGWINCH under a strict POSIX feature macro; the
prototype now uses the installed platform's default declarations. Native header/
compiler compatibility has not been established. Initial PTY checks caught macOS
PENDIN after restoring canonical mode. The source now flushes the simulation's
queued input and compares the actual restored attributes without masking a flag
mismatch. Forced kill restores through the owned supervisor; a lost PTY may reject
restoration and yields failure/uncertainty before fixture disposal.

An initial aggregate child-RSS measurement included the compiler/Python pre-exec
footprint and exceeded the proposed ceiling. Replaced it with posix_spawn and wait4
measurements of the exact simulation child, avoiding inherited pre-exec peak metrics.
Four EOF/valid/overflow/help representative cases had maximum observed peak RSS
1916928 bytes in the final-source host run, below 32 MiB. This is not native RAM usage or an exhaustive
peak measurement for every case. Tests enforce source size below 32 KiB, output/work
caps and deadlines. Input inactivity and per-line processing stop within 10 seconds;
blocked output stops within five seconds; total application time is capped at 60.
Required prompt observations use a three-second host-test deadline. Scheduling and
physical serial latency remain unqualified.

No TERM/TERMINFO/TERMCAP database is read. Static review of the complete original
source found external effects limited to inherited-stream read/write/poll, clock,
termios/ioctl, signal and fcntl operations, with no file-open/create, shell execution, network, installer or
privilege calls. Libc/host loader overhead is measured as host RSS, not vendored
runtime adoption or complete native transitive dependency qualification.

## Cleanup and remaining gates

Every subprocess is reaped. Each exclusively owned PTY is checked/restored where
available, then closed, including failed/forced cases. Temporary compiled binaries
and owned test directories are removed; the initial manually compiled binary was
also removed. Tests do not alter external/live terminals, disks or networks.
Only original source/tests and sanitized facts enter Git; no raw captures/inventory.

Phase B still needs exact native source/build/runtime identities, separately approved
native build provisioning and console commands. A host binary cannot be uploaded to
establish OpenBSD behavior. Ramdisk inclusion and UI-10 need separately reviewed
native media builds. C adoption, curses comparison, secret handling, confirmation
freshness, configuration and privilege contracts remain undecided/unimplemented.

Final original C source: 8918 bytes; SHA-256
1adf0d4ecade984c360ab7cd0db34f9686f54c36db9d563885656a287b8a6bcb.
