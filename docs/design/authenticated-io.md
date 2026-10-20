# Authenticated review: bounded I/O contract

Status: engineering continuation for issue #5, 2026-10-19. The owner explicitly
delegated this contract and the smallest portable increment after PR #79.
Independent design review precedes implementation. This refines the adopted
[authenticated review](authenticated-plan.md); no extraction, privileged operation,
native campaign or production signer is authorized.

## Descriptor and buffer ownership

The future OpenBSD entry has fixed inherited capabilities: definition on fd 0,
payload on fd 3, detached definition signature on fd 4, review on fd 1 and fixed
diagnostics on fd 2. The trusted media policy, including public key bytes and its
identity, is compiled trusted data, never a project-selected fd/path/argument.
There is no policy-file, target, console, network or arbitrary path argument.
An eventual entry rejects root, set-ID, mismatched real/effective IDs and absent
OpenBSD confinement before reading. Close unrelated fds before capture.

The portable increment is an internal capture helper, absent from the production
link and CLI. Its trusted caller supplies valid exclusive snapshot storage and
already exclusively owns fds 0/3/4 and their open-file descriptions. No other
thread, signal handler or consumer may close/rebind them, advance their offsets or
change their flags during capture. Snapshot storage is a fixed typed object with
distinct definition/payload/signature arrays and lengths; it cannot overlap caller
state or another concurrent capture. It reads no pathname, writes no output, allocates nothing,
starts no process and grants no authenticity. It requires ordinary read-only
regular files at offset zero, nonempty and within the following exact ceilings.
All three inputs and both output descriptors must be observable before any read;
reject identical input device/inode pairs and any regular output alias of an
input. Recheck device/inode, type/mode, uid/gid, link count, size and nanosecond
mtime/ctime after all captures. Ignore atime, which reads may change. A completed
snapshot is retained bytes, not proof of publication atomicity or unseen writers.

| Object | Maximum bytes |
| --- | ---: |
| Exact definition | 16384 |
| Complete payload | 1048576 |
| Detached signature representation | 16384 |
| Compiled trusted public-key representation | 1024 |
| Complete review, assembled before output | 65536 |
| Fixed ordinary diagnostic | 256 |

Each capture buffer has one additional byte to detect growth beyond its ceiling.
Read in chunks no larger than 4096; require EOF, exact original size and final
offset agreement. Check a monotonic deadline before each observation/read and
after every interrupted syscall. The helper has a fixed two-second cooperative
deadline, bounded by the eventual entry's hard wall/CPU timers. A regular-file
read can block in the kernel: the helper alone promises no hard syscall deadline.
The future single-threaded entry must enforce hard timers before calling it.
No mutable global state, configurable clock/deadline or fault-injection entry is
present in the production object. Tests replace syscalls in a separate object.

On every controlled return, attempt each owned input close exactly once, even
after another close fails. Never retry an uncertain close: descriptor reuse makes
that unsafe. Return captured/rejected/cleanup-uncertain distinctly. EBADF means
already absent and always invalidates a previously successful capture; it does not
turn a rejected preflight into uncertain closure. Other close errors mean uncertain
closure. Failure clears all
snapshot bytes/lengths; no downstream parse, hash or successful review is allowed.
Success occurs only after all closes. No helper diagnostic can mutate an alias.
Process death may prevent return; the supervising entry reports incomplete state.

## Reservation-bound capture continuity

The source-only continuation after PR #87 adds the closed internal sibling
`bw_auth_capture_reserved(material, snapshot)`. It remains unlinked from the CLI.
Its trusted caller supplies genuine, exclusively owned RESERVED material, valid
exclusive snapshot storage and the original live cancellation flag. No caller
clock, deadline, callback, descriptor selector or accepted-capture credential is
introduced. The legacy `bw_auth_capture(snapshot)` retains its existing fresh
two-second clock, including taking/closing inputs on a null snapshot.

Before reading material members, the new sibling rejects null/overflowing or
overlapping material and snapshot spans. It then checks the original flag's
nonempty numeric span against both objects before reading state or taking inputs.
Numeric checks do not establish mapped-memory validity: valid typed storage is
a trusted caller obligation. Malformed, poisoned or used reservation shape returns
NOT_TAKEN with all objects/descriptors unchanged and no syscall. Required shape:
RESERVED, no capture attempt or recorded failure, a known directory outside roles
0..4, no members/writers/read handles, a nonempty reserved path, normalized
nonnegative original time and recorded 0700 directory identity with positive links.
Do not mutate live material to manufacture that shape. Correcting an argument
after NOT_TAKEN is safe because ownership did not transfer.

After preflight, `capture_taken` is permanently latched before snapshot clearing
or any clock/descriptor observation. The call now owns all three fixed input
closes on every return. It checks the retained directory descriptor's full
identity/metadata against the unchanged empty reservation before input queries.
This proves only the held descriptor observation; current pathname binding
remains preparation's responsibility through its existing held/named checks.

The common core keeps every existing input/output alias, access, offset, size, EOF
and final metadata check. Reserved capture uses material's unchanged `started` and
cancellation pointer at every live boundary, including after EINTR and otherwise
successful closes. Reservation, capture and preparation share the same conservative
original two-second window; neither capture nor preparation renews it. Existing
cooperative/hard-timer limitations remain.

Success leaves material RESERVED with the same origin/flag and exact snapshot.
It grants no authentication or operation authority. Failure clears all snapshot
bytes/lengths and poisons material's first cause: cancellation is CANCEL, invalid,
backward, unavailable or expired clock is DEADLINE, held-reservation mismatch is
OWNERSHIP and other input/alias/read/metadata refusal is IO. Close failure is CLOSE
only if no earlier cause exists. Separate `capture_close_errors` and
`capture_unknown_closes` use bits 0/1/2 for definition/payload/signature. EBADF
invalidates success but is known absence; every other close error also latches
material uncertainty. Never retry a close or reset the capture latch.

Capture never releases scratch. The caller owns one later release or genuine
successful transfer to the fused pipeline. Preparation consumes an already
poisoned/uncertain RESERVED object before any member creation or write, even if a
caller supplies replacement bytes or clears the external clock/flag fault. Its
existing exact cleanup runs independently; input uncertainty cannot become clean
success merely because scratch cleanup is known. Rebound input descriptors must
survive every refused repeat and later material cleanup.

The capture C/header and complete local fixed-object disjointness helper fit the
approved 8 KiB cap. The helper handles exactly three nonempty object pairs; no
implementation moves outside accounting. The generic private span header remains
unchanged and fully charged to its other consumers. Both declarations share one
concise reference to this normative contract. Other component/runtime ceilings
remain unchanged.

At capture adoption, measured source was 8,093/8,192 capture bytes, 15,466/18,432 material
bytes and 87,218/98,304 whole-runtime bytes. The complete local predicate is in
capture C; public material interface declarations stay with their owning component,
as in existing consumers. No implementation helper or runtime source is omitted.
Current startup/material accounting is recorded with the closed
[startup lifetime](trusted-entry-output.md#closed-startup-and-reservation-lifetime).

| Test | Independent expected behavior |
| --- | --- |
| RC-T01 | Real reservation, exact binary inputs, short reads/EINTR and spent original time preserve exact snapshot, original origin/flag and caller scratch ownership; preparation uses those same bytes and window. |
| RC-T02 | Exact two-second boundary, cancellation and invalid/backward/failed clock at preflight/read/EINTR/final close refuse finitely, clear storage, poison first cause and attempt all input closes once. |
| RC-T03 | Each role's EBADF versus unknown close preserves separate masks and uncertainty; an earlier input error remains primary. Scratch release cannot erase unknown input closure. |
| RC-T04 | Input/output aliases, invalid access/offset/type/size, EOF/read/metadata failures retain old refusal semantics under the original reservation window. |
| RC-T05 | Null/overflow/each pairwise overlap, invalid reservation shape and reuse are NOT_TAKEN with unchanged objects and zero effects. Rebound canary descriptors survive repeats. |
| RC-T06 | Failed capture followed by valid direct preparation cannot create/write members; successful capture is not a substitute for signature/content review. Legacy capture, material, fused and CLI regressions remain. |

Use synthetic portable fixtures with real owned reservation/input/prepare/release
operations and separate syscall/clock models. No signer, native verifier, listener
or host security mutation is introduced. Independent source/fixture/envelope review
precedes execution, then focused/full clean-candidate checks, exact squash/DCO
review and canonical verification. The later closed startup sibling supplies the
pre-first-arm origin and permanent private cancellation flag; calling legacy guard
and reserve separately does not. Complete entry ownership and exact native
confinement/qualification remain separate gates.

## Verifier handoff and private scratch

The future verifier is exactly OpenBSD base `/usr/bin/signify`, with reviewed
release/source/binary identity bound by media construction. No PATH lookup,
environment override, interpreter, project-selected tool or alternative algorithm.
The exact argv is `signify -V -q -p /dev/fd/3 -x /dev/fd/4 -m /dev/fd/5`.
Only detached verification is allowed; never use `-e`, `-z`, `-C` or implicit key
discovery. The [OpenBSD 7.9 manual](https://man.openbsd.org/OpenBSD-7.9/signify.1)
defines explicit-key detached verification and its two-line record representation.

Before launch, strictly validate the bounded public key/signature record: exactly
two LF-terminated lines, printable ASCII comment beginning `untrusted comment: `,
one nonempty comment of at most 255 bytes, canonical base64 with zero padding bits,
decoded `Ed` marker, exactly 42 key bytes or 74 signature bytes, and no third line,
CR, NUL or trailing data. These are format checks, never cryptographic verification.
The key's exact bytes/identity must equal compiled policy; the definition's exact
digest and project/system tuple must match policy independently of verifier exit.
The retained payload digest must match that authenticated definition before the
existing pure structural inspector consumes the same retained buffer.

Create one private 0700, same-euid scratch directory using a trusted fixed `/tmp`
template, hold its directory fd and validate its identity. Create only three fixed
basenames with openat O_CREAT|O_EXCL|O_NOFOLLOW, 0600: public key, signature and exact
definition. No archive member or payload bytes are written. Write complete buffers,
check counts, open O_RDONLY|O_NOFOLLOW counterparts relative to the held directory,
compare device/inode/size/mode/owner/link metadata and full retained contents, close
all writers, then unlink the three names and remove the exact empty directory
before launching. Held read-only handles remain; require link count zero and offset
zero. Failure stops verification and independently attempts exact owned cleanup;
unknown scratch identity/closure is quarantined and reported uncertain, never
recursively deleted or followed through an altered name. Logical unlink is not
physical erasure. No secret key exists in this flow.

Map these three read-only handles to child 3/4/5, null stdin and bounded private
stdout/stderr pipes 1/2; close all other descriptors. Fixed environment contains only
LC_ALL=C, LANG=C, TZ=UTC and empty PATH. The parent retains its original buffers;
the verifier never receives payload, terminal or external output capabilities.
Native qualification must prove `/dev/fd` handoff, dynamic-loader access and actual
signify read/exit behavior under the specified unveil/pledge transitions. No named
file or unconstrained execution fallback may replace a failed qualification.

The future entry unveils only its owned scratch and the exact base executable,
required loader/libraries and `/dev/fd` entries; it locks unveil before verification.
Its initial pledge is restricted to stdio/rpath/wpath/cpath/proc/exec for this
bounded preparation. The child inherits that locked whitelist and calls
`pledge("stdio rpath exec", "stdio rpath")` before exec, dropping wpath/cpath and
providing explicit post-exec promises as required by the
[OpenBSD 7.9 pledge manual](https://man.openbsd.org/OpenBSD-7.9/pledge.2);
it does not attempt to alter a locked unveil table. The scratch names are already
unlinked and the directory removed, so its remaining named read access is limited
to the fixed fd aliases and reviewed executable/loader whitelist. After launch,
the parent drops exec and all path promises, retaining stdio/proc until custody
ends, then stdio only. Failure of any transition refuses. Native observations must
establish actual syscall availability and the smallest necessary loader whitelist;
these transitions are acceptance requirements, not host-qualified confinement.

## Deadlines, custody and failure

The eventual entry has 15 seconds hard wall, 4 seconds CPU, 64 MiB DATA, 2 MiB STACK,
32 fds,CORE=0 and FSIZE=65536 (including caller-owned regular output).
Each scratch write is separately capped at its smaller input limit. Respect lower
inherited limits and fail if they cannot support the operation. Input capture 2 seconds, scratch preparation 2,
verifier 5, rendering/output 2, with 4 seconds reserved for cleanup; each uses the
remaining overall allowance. Stop dependent work no later than overall deadline
minus four seconds, preserving the cleanup reserve. Verifier child has its own
five-second hard wall timer established before exec (default termination action),
plus parent supervision; never rely only on the parent timer. Verifier child:
CPU 2 seconds,DATA 32 MiB,STACK 2 MiB,
16 fds,CORE=0,FSIZE=0; combined private diagnostics 4096 bytes. Full buffers and the
34 KiB structural workspace fit a 2 MiB explicit application-storage ceiling; these
are design bounds, not measured RSS. Later native tests must measure parent and
child resources without inheriting a larger portable-test allowance.

Own one original child identity and its actual group from birth. Keep the leader
unreaped with [waitid WNOWAIT](https://man.openbsd.org/OpenBSD-7.9/waitid.2)
until the final group stop; latch consumed immediately
on actual wait4 before any fallible status/RSS processing. Never signal a numeric
group after loss/reap of its identity. Stop, drain, actual wait, fd closes and
scratch cleanup are independent bounded stages preserving the primary error.
Unsupported WNOWAIT/SIGCHLD custody refuses before launch. Missing or uncertain
cleanup cannot become successful authentication; verifier errors remain private
and ordinary output uses fixed categories. No historical approval is inherited.

## Portable increment and Test Design

Implement only `bw_auth_capture` in src/auth_input.c/.h and a separately linked
test driver. Existing planner/CLI and pure archive parser remain unchanged. New
C/header source is capped at 6 KiB and the whole src tree at 32 KiB; the existing
planner implementation remains within its earlier budget. No signature parser,
SHA implementation, scratch writer, verifier launcher or native entry is included.

| Test | Independent expected behavior |
| --- | --- |
| IO-T01 | Distinct regular files, binary contents and exact caps: byte-for-byte snapshots, all inputs closed, no output/file changes. |
| IO-T02 | Missing/empty/over-cap/nonregular/read-write/nonzero-offset inputs: no snapshot; reject before any read when preflight fails. |
| IO-T03 | Input hardlink/dup aliases and regular stdout/stderr aliases: reject without writes or source changes. |
| IO-T04 | Short reads/EINTR, I/O error, growth/shrink, mode/owner/link/identity/time mutation and wrong final offset: reject or preserve exact bytes as applicable; zero failed storage. |
| IO-T05 | Monotonic clock failure, elapsed deadline or repeated EINTR: finite refusal; all close attempts. Close failure: cleanup-uncertain, no retry, independent remaining closes. |
| IO-T06 | Repeated invocations have no shared state; compiled production CLI remains unchanged and new helper has no pathname/write/exec dependency. |

Run complete `make verify` on the identified clean candidate, independent source
and evidence review and signature/DCO checks. Host tests establish only this
capture component. Verifier/scratch/confinement composition and native serial/plain
evidence require their later implementation and fresh exact native authority.

The next [portable verifier-material increment](verifier-material.md) refines the
startup order: reserve only an exact empty scratch capability outside fd roles
0..4 before locking confinement/capture, then populate it from retained bytes.
It adds format validation and anonymous read-only handles without a verifier
launch or CLI. The same native composition gates remain.

The later owner-approved [complete entry source package](trusted-entry-output.md)
supersedes the historical source-only limits above with 8 KiB capture and 96 KiB whole
source. Capture's input, timing, ownership, cleanup and storage limits do not change.
