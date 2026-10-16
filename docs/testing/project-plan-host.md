# Read-only planner host evidence

Status: Portable implementation checked, 2026-10-15. OpenBSD qualification pending.
Scope: [accepted PRD/EDD/Test Design](../design/project-plan.md) and
[ADR 0003](../adr/0003-read-only-project-planner.md). PR #70 was merged by the
maintainer before implementation; its exact merged tree is the implementation base.
Continue the feature in one implementation draft through native qualification.

## Implementation and commands

Original BSD-3-Clause C99 in `src/` separates schema parsing, typed model validation,
complete-buffer rendering, inherited descriptor handling and production dispatch.
No parser/library/runtime dependency was added. `make build` uses base `cc` with
`-O2 -std=c99 -Wall -Wextra -Werror -pedantic`. `bin/bootweave --help` and
`--version` run on the host; `plan` refuses with exit 77. The production source has
no test bypass. Test-only entry points and IO fault interposition live in `tests/`.

Observed host: macOS ARM64, Apple clang 21.0.0 (clang-2100.3.20.102), Python 3.14.7.
`make build test-plan` builds the normal executable and portable harness. Seventeen
unittest groups passed: fourteen planner groups and three native-preparation
tooling groups. These groups contain multiple independently asserted cases:

- 576 successful combinations: all 24 root orders and eight child-order
  combinations for each of three unrelated viewer/DNS/firewall definitions.
- Independent complete-text oracle, allowed whitespace/escapes, literal shell and
  format metacharacters, all field length/alphabet/system bounds.
- Missing, duplicate, escaped and unknown keys at every object level; wrong types,
  unsupported numbers/versions, every truncated prefix of the canonical definition,
  malformed/trailing/deep/oversized JSON, raw and escaped controls/Unicode.
- Exact 16383/16384-byte successful inputs and 16385-byte refusal; deterministic
  256-case insertion corpus (seed 20261015, forbidden-byte oracle).
- Pipe, FIFO, PTY, socket, device, read-write and nonzero-offset refusals; independent
  input/canary bytes, mode, ownership, mtime/ctime and directory inventory comparison.
  Input/output inode aliases refuse without modifying the input or diagnostics.
- Separately linked fault harness: short reads/writes with EINTR, observed changed
  stat, read error, zero-progress write and EAGAIN. Successful output checks that
  stdin was closed first. Broken stdout/stderr failures return 5. A full output pipe
  with inherited blocked SIGALRM exits 4 at the original five-second deadline.
- Direct model-tampering/nonterminated-field and undersized-output checks. Rejected
  parsing leaves the prior model unchanged; rejected rendering emits no buffer.
- ASan/UBSan builds of the separate driver and model unit harness; 68 bounded
  input cases (two successes, two structural/size failures, 64 deterministic NUL
  insertions) and direct model cases passed without sanitizer findings. Flags add
  `-fsanitize=address,undefined -fno-omit-frame-pointer -g`. Options are
  `ASAN_OPTIONS=detect_leaks=0:halt_on_error=1` and
  `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=0`. Leak checking is not claimed.

The [native preparation](project-plan-native-plan.md) adds a deterministic 1,401-case
manifest and a trusted C supervisor. On the host, that supervisor passed 2,802 corpus
executions against the portable driver under both binary names, 30 descriptor/output
cases, two trace-case executions and four PTY console commands. Exact terminal
attributes/geometry were restored. Changed canary and wrong-output controls rejected;
a stalled child was killed/reaped by the independent seven-second guard. The host
copies do not qualify native dynamic/static linkage. Native build/run/compiler/probe
tools retain host refusal. Fixed argument previews and sealed source closure also
passed; no target, derived media or guest was created. The production confinement
function was moved unchanged to `src/native.c` for shared native probes; it has no
test selectors or bypass. Runtime C/headers now total 14,070 bytes.

The first rehearsal found Darwin's kernel-owned first-write flag changing a pipe's
F_GETFL result. The supervisor now writes/drains one owned staging byte before
recording the exact flag baseline; it masks no flags. A negative-control assertion
was corrected to expect the actual content/identity diagnostic. These were host
harness corrections before any native authority, not native successes or failures.

The resource timer restores its required signal dispositions and unblocks those
signals before arming a single nonrenewing timer. It does not change terminal
attributes or read environment variables. Only the private fault harness reads a
test-selection environment value. Production output uses trusted format strings.

## Limits and delivery gate

Host tests do not execute the OpenBSD privilege check, limits, closefrom, unveil or
pledge. They do not prove native build compatibility, denial of forbidden syscalls,
native resource/console behavior, actual serial/VGA rendering or native teardown.
Read/stat faults are controlled interposition, not proof of an atomic file snapshot.
Canary comparisons do not alone prove absence of attempted syscalls. These limits
remain explicit requirements of PLAN-T04 through T08.

The feature draft must remain open and unready until a concrete frozen native
manifest/helpers/source and exact fresh fixture run plan receive approval, all
required native cases pass, and independent readback/disposal completes. No native
fixture was provisioned or launched for these host checks. Previous research target
authority has ended. This feature grants no installer/payload-write authority.

The PR records the full base/head/tree, signed commit and DCO verification, exact
runtime source bytes and clean-candidate `make verify` results. This avoids a
self-referential source SHA in a file included by that commit. Local temporary test
fixtures/binaries are removed by their owning temporary-directory contexts; the
ignored development executable can be removed with the owned `bin/bootweave` file.
