# Plain-text UI research simulation

Original BSD-3-Clause research code for the accepted Phase A of
[the bounded spike](../../docs/research/plain-ui-spike-plan.md).
No installer, disk, payload, configuration or privilege backend. Host-only evidence;
C is not adopted as the Bootweave runtime and OpenBSD behavior remains unqualified.

Run the owned host checks with `make test-ui` or `make verify`. The Python suite
compiles a fresh temporary binary using the installed cc, creates owned PTYs,
checks postconditions, and removes its directory/binary and closes its endpoints.
No external terminal is changed by the tests. No third-party source is vendored.

The simulation reads an ASCII label and alpha/beta choice, offers help/back/cancel,
then prints a review and accepts only the literal complete command. h, b and q
are reserved commands. After back, keep retains the displayed saved field value.
Completion is synthetic and has no authorization meaning; it may consume queued
input and is not a model for destructive confirmation freshness. Secrets and
confirmation semantics remain outside this experiment.

On a tty, input echo and canonical processing are disabled temporarily. Exit
restores and verifies saved termios fields/speeds and discards this simulation's
queued input. Signals/resize cancel. Forced kill requires the owned supervisor to
restore attributes; lost-console restoration may be unavailable and returns failure.
The code reports unknown/unusable dimensions rather than treating 80x24 as measured.
Exit 0 means simulation completed, 2 means incomplete/cancelled, and 3 means setup
or restoration failure. There is no installation success or readiness result.
