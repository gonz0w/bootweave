# First project definition and read-only planner

Status: **Accepted for implementation and host tests, 2026-10-15.** Owner and acceptance authority: Cam.
Scoped PRD, EDD and Test Design for issue [#5](https://github.com/gonz0w/bootweave/issues/5).
Acceptance source: Cam's “continue” after the explicit scope/ADR approval request
in PR #70; confirmed by the following continuation. Native target authority remains
separate. Related runtime decision: [ADR 0003](../adr/0003-read-only-project-planner.md).
This is the next complete feature after the
[installed-payload qualification](../research/installed-payload-native-progress.md#completed-independent-readback-and-exact-disposal-2026-10-15).
It adopts the bounded metadata/runtime contract below for this planner only.
Implementation and portable host checks are complete; native qualification is pending.

## PRD: outcome and accepted work

An independent author can supply a small project definition and obtain a complete,
plain-text candidate plan from `bootweave plan`. The result clearly distinguishes
validated definition syntax from unverified claims, unselected targets and absent
effects. This establishes an engine entry point usable by unrelated projects before
connecting the existing installer/payload mechanisms.

Accepted scope: original base-C implementation, the exact version-1 definition
below, portable parser/model/output tests, a production CLI that refuses unsupported
hosts/elevated execution, build integration and author documentation. Keep design,
implementation, fixes, native qualification and evidence in this one feature draft.
Routine implementation choices within these boundaries do not need another review.

Native execution and fixture provisioning are not granted by this proposal. After
the implementation and host checks are concrete, prepare one exact native run plan
for review in the same draft. Earlier disks/helpers were disposed and their target
authority ended; do not recreate or reuse that authority. Do not mark the feature
ready or claim native support before its native tests and teardown pass.

### Requirements

| ID | Required behavior |
|---|---|
| PLAN-01 | Accept exactly the bounded version-1 definition; reject malformed, ambiguous, duplicate, unknown, oversized or unsupported input before a plan is emitted. |
| PLAN-02 | Produce a complete plain-text candidate plan, with fixed unverified/unselected/no-effects state and identical semantics for generic, DNS and firewall metadata. |
| PLAN-03 | Definition values cannot cause execution, artifact access, device operations, filesystem mutations, network access or authorization. |
| PLAN-04 | Unprivileged OpenBSD CLI uses checked confinement, closed extra descriptors and bounded resources; unavailable confinement/elevated execution refuses without a plan. |
| PLAN-05 | Diagnostics contain fixed categories and optional numeric offsets, never raw rejected input, paths, values, secrets or terminal controls. |
| PLAN-06 | Host checks and native observations are separate; publish exact source/artifact identities, actual results/resources and observed teardown. |

This slice does not add installation, target enumeration/selection, payload hashing
or signature verification, extraction, questions, secrets, services, hooks, product
bootstrap, persistent receipts, resume or a public privilege protocol. Its plan is
not an executable request and cannot be submitted to a write backend. Existing
research code is evidence and remains separate; no bulk promotion into the engine.

## EDD: accepted public contract

### Command and input

The invocation is `bootweave plan < project.json`. The caller opens the
input; the planner accepts standard input only when fstat identifies a regular file
and F_GETFL identifies a read-only descriptor at initial offset zero. Reject
terminal, pipe, FIFO, socket,
device and read/write input before reading. There is no input pathname or project
text in command arguments. The engine does not invoke a shell. The shell redirection
in this operator example is not configuration-to-command interpolation.

Also expose fixed `--help` and `--version` output. Reject extra arguments/unknown
subcommands. A normal non-OpenBSD build must refuse `plan` with exit 77; portable
tests link the parser/model through a separate test driver, not a production bypass.
There is no debug flag that weakens this behavior.

Accept at most 16384 definition bytes in a private bounded buffer, allowing one
additional overflow/EOF probe byte. Any extra byte rejects before parsing. Validate
the entire copied definition, including trailing bytes, before rendering.
Compare held-input size/mtime/ctime before/after reading and refuse observed changes.
This is a validated byte snapshot, not a claim of atomic file publication or future
freshness. Close the input descriptor before parsing. Regular-file output aliases
of the input inode are refused silently before reading or diagnostics, so an error
cannot append to the held definition. No file is reopened, and no authorization
can depend on the input path or inode.

### Version-1 definition

Use an explicitly restricted [JSON](https://www.rfc-editor.org/rfc/rfc8259) object.
The following fields are all required; no other fields exist in version 1.

| Field | Exact value/type and decoded bounds |
|---|---|
| schema_version | JSON integer token `1`, with no sign, fraction or exponent. |
| project | Object containing only id and name. |
| project.id | 1–64 ASCII bytes; first byte a–z, remaining bytes a–z, 0–9 or hyphen. |
| project.name | 1–80 printable ASCII bytes, U+0020 through U+007E, with at least one non-space byte. |
| system | Object containing only release and architecture. |
| system.release | String `7.9`; a requested system, not an installation-support claim. |
| system.architecture | String `amd64`. |
| payload | Object containing only file and sha256. |
| payload.file | 1–128 ASCII bytes; first byte alphanumeric, remaining bytes alphanumeric, dot, underscore or hyphen. A basename only, never opened by this slice. |
| payload.sha256 | Exactly 64 lowercase hexadecimal bytes; an unverified claim, not publisher trust or a measured digest. |

Member order and JSON whitespace may vary. Duplicate members at any level reject;
keys must use their literal ASCII spelling, so escaped key aliases reject. String
values recognize JSON escapes; decoded controls, NUL, DEL, non-ASCII values and
surrogates reject. Printable ASCII `\uXXXX` values may be accepted after decoding
and field validation. No locale-dependent classification or case folding. Reject
BOMs, comments, trailing commas/data, arrays, booleans, null, other number forms,
missing fields, wrong types and unknown schema versions. Depth is at most two
objects, with at most ten members total including the three container fields.
The implementation is a reader for this schema, not a general-purpose JSON library.

These are new project metadata fields, not payload/archive, receipt or privileged
operation schemas. Once accepted, do not silently reinterpret version 1 or ignore
unknown fields. A changed field set/meaning needs a reviewed new schema version;
unsupported versions refuse. No automatic migration or compatibility coercion.

### Model and output

Keep separate input parsing, typed model validation and rendering functions in
`src/`; keep any numeric field/enum internal. Fixed state in every successful plan:

- Definition syntax: validated.
- Project and requested system: supplied data.
- Payload filename and claimed SHA-256: unverified; payload not opened.
- Target: unselected; no device identity collected.
- Operations authorized, performed and independently observed: none.

Begin with `UNVERIFIED PROJECT PLAN - NO OPERATIONS AUTHORIZED`. Render the full
project/system/payload fields in deterministic field order. Label all data and
continuation lines; use trusted format strings and printable ASCII only. No color,
cursor movement, TERM lookup, terminal mode changes, prompt, confirmation or input
loop. The renderer emits the same plan text for serial/keyboard/plain displays. Narrow terminals may
wrap lines, but the program must emit all required text without clipping or paging.
The human summary is not a machine-consumable serialization contract.

Build the complete plan in a bounded output buffer before writing any plan bytes.
Validation/read/confinement failures leave stdout empty. Complete output is required
for success; short/broken output or timeout returns failure and may leave a partial
display, never a completed or authorized action. No persistent state or logs.
Diagnostics use fixed categories/offsets, at most 256 bytes per invocation.

| Exit | Meaning |
|---:|---|
| 0 | Full unverified plan, help or version output completed. |
| 2 | Invocation error. |
| 3 | Definition/input rejection. |
| 4 | Privilege, confinement, resource or other runtime refusal/failure. |
| 5 | Output failure. |
| 77 | Planning unsupported on this host. |

### Runtime authority and limits

Original C99, OpenBSD base libc and compiler only. No third-party parser, interpreter,
package manager or cryptographic implementation. Keep original runtime C/headers at
most 24 KiB; this is a reviewed source ceiling, not a native memory measurement.
Use bounded cursor/index parsing without recursive descent into arbitrary values,
dynamic growth, callbacks or evaluation.

For `plan`, reject root and changed real/effective UID/GID or issetugid before input
access. Close descriptors above 2. Establish limits/signal handling before reading;
call checked `unveil("/", "")`, lock unveil, and `pledge("stdio", NULL)` before
untrusted bytes. No filesystem paths are granted to the application. Failed calls
refuse; do not fall back on another platform or silently omit confinement.
The [7.9 unveil](https://man.openbsd.org/OpenBSD-7.9/unveil.2) and
[pledge](https://man.openbsd.org/OpenBSD-7.9/pledge.2) manuals define these mechanisms;
native tests must establish the selected combination. Pledge's libc-internal
exceptions and caller-owned output descriptors prevent a claim that the whole user
session or every possible descriptor operation is sandboxed.

Accepted per-invocation ceilings: 16 KiB input, 2 KiB typed model, 4 KiB plan output,
256-byte diagnostics, depth two/ten members; 16 MiB DATA and measured RSS, 2 MiB
STACK, eight-descriptor limit, CORE=0 and FSIZE=8 KiB. CPU limit two seconds and
whole wall-clock deadline five seconds from CLI work entry, including read/output;
do not reset on EINTR or partial progress. A hard timer must bound blocked I/O.
Measure actual native CPU/time/RSS/output. Limits changing or evidence unavailable
stops qualification and needs review.

Read-only here means no engine-requested state mutations: reads can affect filesystem
access times and output goes to caller-selected descriptors. Native oracles compare
input/canary bytes, ownership/modes and unexpected path changes, excluding access
times and explicitly owned output captures. The application opens no payload,
device, configuration, log or other path and creates no child process or socket.

## Test Design and delivery gates

Each row below is a required group, with enumerated fixtures/cases frozen before
qualification. These are required tests; the
[host evidence](../testing/project-plan-host.md) records the implemented subset
separately from pending native cases. Use an independent expected model/text oracle; do not derive expected validity/output from the C implementation.

| ID | Required stimuli and independent evidence |
|---|---|
| PLAN-T01 | Generic offline-viewer, DNS and firewall metadata; all object-order permutations and permitted whitespace/escapes. Same model/output policy, no project-specific branch. |
| PLAN-T02 | Every missing/duplicate/unknown field and wrong type at every level; escaped keys, unsupported versions/system values, malformed/truncated JSON and trailing bytes. No plan stdout. |
| PLAN-T03 | Every bound at below/exact/above; deep/large objects, long numbers/strings, BOM/UTF-8/surrogates, raw/escaped controls and format-string/metacharacter text. Bounded rejection or labeled full data, no terminal controls. |
| PLAN-T04 | Root/set-ID/unsupported host; stdin TTY/pipe/FIFO/socket/device/read-write, oversized or observably changed regular input and read errors. No input read for disallowed descriptor types; no success plan. |
| PLAN-T05 | Independent input/outside-canary metadata/content/path inventory; absent artifact and hostile claimed filenames/digests never trigger an artifact/device open, subprocess or socket. |
| PLAN-T06 | Broken/short/blocked stdout, stderr failure, interruption, CPU/wall/resource failure. No success or authority; bounded process exit and no residual owned files/processes. |
| PLAN-T07 | Native production confinement, including unavailable-call failures; separate native probes show new-path open, exec/process and network requests denied. No unsupported-host macro in the production binary. |
| PLAN-T08 | Native dynamic/static builds, actual serial/keyboard-VGA invocation, 80-column and narrow plain displays, full output capture and measured time/CPU/RSS/footprint; unchanged terminal attributes. |
| PLAN-T09 | Existing `make verify` regressions, public host refusal, BSD license headers, source bounds and sanitized source/artifact/candidate/base/tree evidence. |

Host tests run pure parser/model/render code and descriptor/output harnesses without
claiming OpenBSD confinement. Use a bounded deterministic mutation corpus and
independent assertions over rejection/no-effects, with host memory/UB diagnostics
when available; freeze tool/options and report limitations. No unbounded fuzz job
or dependency installation. The normal host executable must retain refusal.

After scope acceptance, implement the complete host-tested slice and author guide
in this same draft. Then prepare a bounded native plan with exact source/artifacts,
release/backend/firmware, fresh target identity/allowed effects, case manifest,
commands, resource/capture limits, stop/reap and independent disposal. Obtain exact
target approval before provisioning/launch/write; no inherited authority from #69.
Freeze cases, oracles and helper hashes before native execution. Unexpected failures
stop dependent work; no automatic retry, scope expansion or deadline reset.

Only mark ready after all required native groups and local verification pass and
owned native fixtures are disposed. Contract acceptance does not establish native
qualification. This slice advances PT-01/02/06/07/09/10 and INST-02/03/07/09/10 but does not close the broader
project-trust, UI, native-profile or full installer acceptance issues.
