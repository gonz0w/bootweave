# Internal verifier material preparation

Status: delegated portable continuation of issue #5 after PR #80, 2026-10-19.
Independent review of this refinement precedes implementation. The existing
[bounded I/O contract](authenticated-io.md) remains the authority boundary.
No CLI, extraction, process launch, cryptographic success or native authority is
added. The exact OpenBSD verifier/loader/confinement composition is a later gate.

## Useful supported increment

Implement strict signify public-key/signature representation checks and prepare
the three anonymous read-only regular handles needed by the fixed future verifier.
This is preparation, not authentication. A structurally valid signature may be
cryptographically invalid; it cannot authorize parsing with privilege or effects.
The future trusted caller supplies immutable retained definition/signature bytes
and public key bytes from compiled policy. Project data cannot supply policy,
paths, executable identity or a trust anchor. No secret key or credential is used
by this increment or its new fixtures. Existing full regressions remain separate.

The internal C API takes a valid exclusive request and a zero-initialized material
object. These and the referenced immutable input buffers must be disjoint and
live throughout the call. A signal-safe cancellation flag is caller-owned; no
callback, environment override or configurable deadline is accepted. Refuse a
nonempty/already-used material object without changing it or losing its handles.
The object records EMPTY, RESERVED, WORKING, READY, CLOSED, REJECTED or UNCERTAIN
state. A reserve function creates only the empty exact owned directory during
trusted startup, before any input capture or unveil lock. The future entry then
preserves only this one typed reserved directory capability when closing unrelated
fds, unveils that exact generated path plus fixed executable/loader/fd paths, locks
unveil and establishes the adopted pledge before capture. Prepare consumes only
that RESERVED capability; it never creates a new directory or widens /tmp after
locking. Release can consume a RESERVED empty directory when capture fails, as
well as READY anonymous handles. Native tests must prove removal of that exact
unveiled directory; failure cannot widen the whitelist or regain unveil promises.
READY means only that three owned handles are available. A separate release
function consumes them exactly once; a second release cannot close reused fd
numbers. The caller must not concurrently close/rebind/advance those descriptors.

The closed [startup/reservation sibling](trusted-entry-output.md#closed-startup-and-reservation-lifetime)
supplies its own pre-REAL-arm origin and permanent private cancellation flag to
the same reserve core through a private implementation bridge. It accepts only
EMPTY material, with no caller seed/flag/path selector. Guard failure is terminal
and leaves material untouched; reservation failure owns its existing scratch
cleanup. The core copies the exact normalized seed without a new origin stamp;
guard/arm time therefore consumes the same original two-second allowance. Legacy
reserve still stamps its own origin. Charge private_start_material.h fully to
material and entry; it is absent from the public material API.

The later [reservation-bound capture](authenticated-io.md#reservation-bound-capture-continuity)
borrows the original clock/flag and latches its input attempt in material. Failed
capture preserves first cause and separate input-close masks; unknown input
closure latches uncertainty. Preparation consumes poisoned/uncertain RESERVED
material before member creation, so replacement bytes cannot bypass capture
failure. Scratch remains caller-owned for one release or genuine successful
transfer; the latch is never reset. Legacy valid material preparation is unchanged.

Reserve checks cancellation and non-elevated identity before namespace creation.
Prepare validates nonnull inputs, lengths (definition 1..16384, signature 1..16384,
trusted public key 1..1024) and the same cancellation flag before creating any member file. A malformed
request consumes/releases its reserved directory; the pure record predicate
requires no scratch or system call.
The key and signature must each have exactly two LF-terminated lines: literal
`untrusted comment: `, a nonempty printable ASCII comment body of 1..255 bytes,
then canonical base64 with correct zero padding bits, decoding to exactly 42/74
bytes respectively and beginning `Ed`. No CR, NUL, third line or trailing bytes.
This narrows the representation consistently with the existing contract; it
does not implement cryptography or choose a real publisher policy.

## Scratch lifecycle and ownership

The only production template is `/tmp/bootweave-verify.XXXXXXXXXX`; names inside
are exactly `key.pub`, `signature.sig` and `definition`. Do not accept a pathname
or basename in the API. Tests substitute filesystem syscalls in a separate object
to put these fixtures beneath their owned test directory; that mechanism is not
present in the normal object or CLI. No payload/member bytes are written.

Require non-elevated real/effective identity before effects. Use mkdtemp, then a
held O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC directory handle. Keep the reserved handle
outside fixed roles 0..4: if open occupies a missing fixed role, move it using
F_DUPFD_CLOEXEC with minimum 5 and consume the original owned descriptor once.
Failure/uncertain close refuses reservation; no fixed-role hole may be silently
filled on return. Then check the lstat/fstat
device/inode/type, same euid, exact 0700 mode and positive link count. Directory
link counts may change as members are added; a fixed count is not portable.
Only successful rmdir establishes namespace emptiness. Create each file with
openat O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC, 0600. Establish its regular-file,
device/inode/owner/mode/link identity immediately; after complete bounded writes
require exact length and link count 1. Open read-only counterparts relative to the
held directory, verify identity/metadata and byte-for-byte contents against the
retained buffer, require EOF and restore offset 0. At most 4096 bytes per transfer.

Close every writer before anonymous handoff. Before each unlink, require the held
directory and its pathname still match and the fixed member's no-follow identity
matches the created regular file. Remove only that exact name, never recurse.
After all three names are absent, remove only the verified empty directory.
Require each retained read handle to remain O_RDONLY, CLOEXEC, at offset 0, with
original inode/owner/mode/length and link count 0. Close the directory handle.
READY requires complete known namespace cleanup, all writer/directory closes,
no cancellation and a live deadline after cleanup. The caller later maps the
handles to child 3/4/5; this increment does not perform fd remapping or execution.

Directory/name checks detect observed substitution; POSIX check-then-unlink does
not prove atomicity against an adversarial same-uid/root peer or filesystem.
Such a peer can already alter the process/trusted media; no new resistance claim
is made. No project code is executed. Native confinement and hostile filesystem
qualification remain separate. Unexpected ownership, extra names, link state or
identity is quarantined; do not widen permissions or delete an unknown object.

## Cancellation, bounded failure and uncertainty

Use one fixed two-second monotonic cooperative deadline beginning at legacy
reserve, or at the closed startup sibling's original pre-REAL-arm seed,
including the intervening capture/handoff; prepare cannot reset or extend it. This
conservatively refuses earlier than separate maximum stage budgets and does not
widen the adopted resource ceilings. Native qualification must establish useful
performance within this bound. Check the original deadline before every
observation/transfer and after EINTR; invalid/backward clock refuses. Kernel calls
can block: future entry hard timers and native qualification remain mandatory.
Check cancellation at those same boundaries, including after controlled cleanup.
This component adds no signal handler, fork, exec, socket, native run or key tool.

On failure, stop dependent work and preserve the first cause. Independently attempt
each known owned writer/read/directory close once and each safely identified
member removal. Cleanup does not stop merely because cancellation/deadline was
the primary failure. Bound it to these fixed objects, with no close retry or
open-ended enumeration. The future entry's four-second cleanup reserve and hard
overall timer still apply. Do not delete a path after unknown identity.

Latch each fd consumed before close and never retry its number. Unexpected EBADF
invalidates success; other close errors preserve uncertain closure. Failed or
identity-uncertain cleanup returns UNCERTAIN, retaining a bounded private quarantine
path, known directory identity and unresolved-close mask for observation; none is
an instruction to delete/retry. No successful material handles survive failure.
Logical unlink never claims physical erasure. No raw input or private path goes
to ordinary diagnostics. Release also preserves uncertainty and consumes all
three returned handles independently, even when an earlier close fails.

## Test Design and budgets

Add only internal src/auth_material.c/.h and separately linked fixtures. Main,
Makefile production sources, planner/parser and extraction remain unchanged.
New C/header source is capped at 18 KiB and whole src at 48 KiB; previous planner 24 KiB
and capture 6 KiB ceilings stay separately enforced. Scratch content is at most
33792 bytes across three files; private transient C workspace is at most 8 KiB.
Runtime budgets in the parent I/O contract are unchanged. No RSS claim follows.

| Test | Independent observation |
| --- | --- |
| VM-T01 | Valid inert public records and exact-bound binary definition: exact readback from three distinct readonly/CLOEXEC/link-zero handles; namespace empty; release closes each once. No authentication result. |
| VM-T02 | Pure format errors have no system-call witness; prepare rejects all prefix/comment/base64/marker/padding/line/trailer/bound/null errors before any member open/write and releases only its pre-existing reservation. |
| VM-T03 | Short/EINTR transfers preserve bytes; errors, truncation, size/mode/owner/link/identity/content changes refuse. Outside sentinel bytes/inodes remain unchanged. |
| VM-T04 | Cancellation before creation and at creation/write/read/unlink/final boundaries; clock errors/deadlines/repeated EINTR: finite refusal, independent exact cleanup, no READY. |
| VM-T05 | Open/stat/write/read/close/unlink/rmdir failures and wrong/symlink/extra directory members: no wrong-object deletion, UNCERTAIN when custody/cleanup cannot be proved, first cause preserved. |
| VM-T06 | Reserve/capture/prepare ordering with each missing fixed role 0..4 (reservation remains >=5 and survives rejected capture), original deadline continuity, duplicate reserve/prepare/release and caller-owned state and fault per owned role: no double close, no reused-number signal/close, all independent closes attempted. Normal object has no test environment, launch or crypto dependency. |

New fixtures use inert format-valid bytes, not key generation or private keys.
Run them under finite supervised owned checks with explicit replaced environment,
no inherited credentials, per-child deadlines and resource/output bounds. Retain
failed-run and quarantine observations. Do not inspect/remove historical targets.
The fixture child caps core output at zero, CPU at two seconds, descriptors at 32,
and regular files at 16 KiB, respecting lower inherited limits; the parent allows
four seconds per child. The owned outer run samples aggregate owned filesystem
output against 512 MiB and records child peak RSS. Neither is hard memory
isolation. These are cooperative portable checks: this Mac refused finite
DATA/STACK limits with EINVAL before
material execution, including a separate C probe. No DATA/STACK or native RSS
qualification follows from this run. The product workspace and parent I/O
ceilings above remain unchanged.
Full clean-candidate make verify, independent source/terminal review, signatures,
DCO and canonical binding precede normal PR merge. Existing research authentication
tests are inherited regression evidence only; new tests launch no verifier.
