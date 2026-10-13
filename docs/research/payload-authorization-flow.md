# Authenticated payload review and placement flow

Status: Accepted, 2026-10-13. Owner and acceptance authority: Cam.
Related [issue #5](https://github.com/gonz0w/bootweave/issues/5),
[parent PRD](../design/project-trust/PRD.md), [EDD](../design/project-trust/EDD.md)
and [Test Design](../design/project-trust/TDD.md).
Preparation base: 30e4161fa20ed42519e8a1af0fe537e151e5ce61, merged PR #49.
Acceptance source: Cam replied "approved" after the explicit PR #50 bounded-scope,
exact fresh target and synthetic credential exception question. This includes the
new interactive deadline and the implementation/native effects below. The reviewed
head was bab77601ae95a6a6c17a5a7626a8b973ebed6e2a. PR #50 was merged by Cam before
implementation; all subsequent implementation, fixes and qualification stay in one
follow-up draft. The immutable preparation receipt remains historical; a separate
private acceptance receipt binds approval to its digest and unchanged exact target.

## Complete slice and proposed outcome

Connect the evidenced authentication, complete-buffer planning, plain console
review and confined file writer in one removable original C experiment. A synthetic
operator can inspect every intended file effect, cancel without member writes,
confirm one exact current plan, and see independently observed completion or
partial/uncertain state. This is the next build under issue #5, covering narrow
parts of PT-02/03/06/07 in addition to the existing PT-04 placement component.

Keep scope, implementation, routine fixes, host tests, native qualification,
teardown and evidence on this branch in one draft PR. Scope approval permits
building that complete slice without another approval at each phase. Mark the PR
ready only after both native consoles and teardown complete. A merged planning PR
is not required to begin an explicitly accepted scope.

The fixture is inert offline document-viewer data: nested text and empty files.
Do not execute its content or claim application readiness. This composes research
components; it adopts no production C runtime, parser, trust policy, privilege IPC,
project syntax, receipt format or installer interface. No stock installer handoff
is connected to this flow. OS provisioning below establishes a disposable compiler
guest only. Product bootstrap, accounts/services, networking, custom boot media,
release packaging, existing installations, upgrade and automatic retry/resume
remain outside this slice.

## Requirements and engineering boundary

| ID | Required behavior |
|---|---|
| PAF-01 | Authenticate and inspect the retained snapshot before displaying any proposed effects; rejected inputs never launch the writer. |
| PAF-02 | Display the exact held root binding, payload/key identities, all canonical paths/types/content sizes, fixed owners/modes, counts and limits. Planning/review creates no members. |
| PAF-03 | Fresh confirmation grants a single compiled placement operation for that exact plan/root/snapshot; changed context, cancellation or replay cannot reuse it. |
| PAF-04 | Revalidate the complete authorized context at dispatch and retain the existing writer restrictions, no-overwrite traversal and independent oracle. |
| PAF-05 | Show separate planned, submitted and observed counts plus incomplete/uncertain state; process exit alone cannot imply completion. |
| PAF-06 | Keyboard, serial, narrow and plain output carry the same full review, rejection and confirmation meaning with bounded safe text and checked terminal restoration. |

Use the existing original inspector and planner acceptance rules unchanged. A new
fixed compiled composition build may insert the review phase between successful
inspection/planning and the writer. There is no caller-selected callback, command,
tool path, shell, operation flag or serialized privilege request. Only a trusted
compiled supervisor supplies the synthetic signing policy, expected inventory,
held empty case root and console descriptors. Standalone/no-context execution and
arguments refuse. Host execution has only a synthetic dispatch recorder and owned
PTYs; it cannot invoke the native writer or provision/install a guest.

Archive stdin and interactive console are separate capabilities. The trusted
supervisor opens/binds the actual console and supplies its held descriptors; payload
EOF or queued archive bytes cannot become confirmation input. Validate console
type/identity and never accept a payload-selected terminal path. Child verifier,
inspector and writer descriptor closure excludes console capabilities. Compare
binding fields explicitly; structure padding is not an authorization identity.

Keep the same owned payload snapshot from capture through authentication, plan,
review, dispatch and placement. Do not reopen the original input or accept a second
archive stream after confirmation. The review record binds the full length/digest,
explicit key bytes/digest, verifier/inspector identities, canonical plan and the
held directory identity/type/owner/mode. At dispatch compare all bound fields and
recompute the snapshot digest and plan; mismatch consumes the attempt and stops.
The existing placement phase independently rechecks its complete plan, empty root
and separate expected inventory before forking. A successful authentication alone
has no placement authority.

The parent fixes each case root under /root/payload-placement/work inside its
root-only parent, opens it with O_DIRECTORY/O_NOFOLLOW, binds its identity and sets
the already qualified fixed nobody owner. That fixture preparation precedes review
and is distinct from archive member effects. The review clearly says that only this
fresh case root receives members. No archive or operator input selects another
root, account, permission policy or executable. Native setup/probe restrictions,
root traversal checks and writer/oracle implementation remain in force; a missing
or failed prerequisite supplies no fallback.

For this composition only, reject plans with more than 16 realized objects before
review so the complete effect list can fit the bounded console budget. Existing
standalone placement continues to admit up to 128 within its frozen policy. Review
includes implicit directories. Show validated archive names as data with trusted
type/size labels; independently validate the frozen ASCII path alphabet before
rendering, even though the planner also checks it. Never render raw signature
comments, archive header text, child diagnostics or secret-bearing input.

## Confirmation and terminal semantics

Use a private test-only confirmation phrase containing the trusted synthetic case
identifier and current numeric review revision. This is a fixture instruction, not
published configuration or an authorization protocol. The trusted supervisor fixes
the identifier; the controller assigns a checked increasing revision for each
review. Retain the complete binding in memory. No confirmation is persisted,
transferred between processes or reusable after restart; restart supplies no
authority. Refuse revision overflow.

Require an explicit review action followed by the displayed exact placement phrase.
Enter/defaults, help, back, cancel, EOF, timeout, signals and resize cannot place.
Back/help consume the current confirmation and require a complete fresh review with
a new revision. Reject queued input before a review and before granting authority;
contaminated multi-line input cancels rather than being silently drained into a
later confirmation. Check the queue again after terminal restoration immediately
before dispatch. The case/revision binding rejects stale phrases even when the
payload remains identical. This establishes bounded queue/replay handling, not
physical operator presence or detection of every single-line paste.

Use original bounded plain ASCII console code, drawing on the original handoff
console's observed queue/restoration behavior. Do not repurpose its disk/handoff
confirmation as file authorization. No ANSI decoration, curses or new dependency.
Wrap trusted text at known 40-column minimum without clipping values; keep sequential
plain text when serial dimensions are unknown and report screen-fit uncertainty.
Known dimensions below 40x12 refuse. Resizing invalidates review.

Save/check/restore exact terminal attributes and descriptor flags on every controlled
exit, including rejected confirmation. Disable echo before consuming untrusted
bytes. Reject NUL, ESC, other control/high-bit bytes and oversized lines as a whole;
normalize CR/LF/CRLF with tests. Catchable signals only notify controlled cleanup.
Forced death requires the owned supervisor to observe/restore its PTY out of band;
lost-terminal restoration is reported as uncertainty, never authorization. Any
output/restoration/queue failure blocks dispatch. No secret prompt is introduced.

## Exact prepared target and requested native authority

Sole new writable target: .tools/payload-authorization-native/build.raw. Exclusively
created regular raw file in an owned 0700 directory, mode 0600, one link,
8589934592 bytes, zero allocated bytes. Initial complete SHA-256:
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25.
Immutable private receipt SHA-256:
ea6b36d29dec771893163603c4a1a9ae121365f00b2bd50ff17b44c66ac6c839.
The receipt binds host device/inode, size/mode/link count/allocation and full digest.
At preparation: guest started=false, guest write authorized=false, new flow
implemented=false, credential/signing pairs/data ISO created=false. Preserve this
history; later acceptance/effects need separate records. Previous targets were
disposed and grant no authority here.

Preparation rehashed all seven pinned QEMU/BIOS/VGA/verifier/key/media/signature
inputs and repeated explicit-key signify -C for install79.iso under
[ADR 0002](../adr/0002-host-artifact-verifier.md). All matched; the selected media
verified. No source executed, guest started or media mounted. Pins remain those
in the [native preflight](../design/native-profile/preflight.md) and
[release evidence](../design/native-profile/release-input-evidence.md).

Requested acceptance permits the original implementation and fixed host/native
test harnesses, one sealed nonbootable read-only source/data ISO, manual stock OS
provisioning, offline serial and keyboard/VGA boots, exactly two fresh disposable
native signing pairs, one synthetic stock root credential and bounded case-root
effects on this exact target. It permits root-owned test supervision and controlled
filesystem/context/failure substitutions only in the fixture, then the qualified
writer's irreversible drop to existing stock nobody UID/GID 32767. No account is
added/changed by the experiment. No root writer handles archive member paths.

Use pinned QEMU 11.1.1, pc-i440fx-11.1, qemu64-v1, TCG, one CPU and 1024 MiB.
The raw IDE target is ide.0/unit 0; verified stock read-only optical install79.iso
and sealed read-only data ISO occupy ide.1/units 0 and 1. No NIC/IP listener,
host sharing/passthrough, backing chain, hotplug, overlay or running media swap.
Use local VGA and private Unix serial/QMP endpoints. Start paused; inspect effective
argv, block/device/PCI trees, process files and listeners before CPU execution.
Immediately before destructive stock answers, pause/recheck exact target receipt,
attachments and observed wd0 capacity. Any substitution/mismatch stops input.

Inside the stock ramdisk verify the embedded release manifest and six selected
stock inputs with explicit-key base signify before installation. Permit manual
whole-disk unencrypted MBR, automatic FFS/swap, bsd/bsd.rd/base/comp/man, synthetic
.example.invalid hostname, UTC, no interface/DNS/additional user, SSH/X disabled,
no game/X/site sets or install.site. Never bypass stock set verification. Observe
actual layout/configuration/compiler, completed kernel relinking, kernel copy
agreement and clean halt; bind unchanged host identity and full post-write digest
before each later offline boot. Failed postconditions mean uncertain state, no
automatic provisioning retry.

The stock installer's plaintext-argv encrypt limitation is requested only for this
synthetic disposable credential, as in the completed experiment; it is not accepted
as production secret delivery. Keep the credential out of host argv/environment,
Git and ordinary logs; submit only at observed echo-off prompts and privately check
capture exclusion before viewing/publishing. Signing uses fixed private file paths,
never secret bytes in argv/environment. Reuse the same two fresh pairs across
variants/consoles, verify their private lifetime identity and independently observe
all private/public pair files and receipt absent before final clean halt.

## Test Design and resource gates

Independent expected inventories/hashes and dispatch counts come from the trusted
supervisor, never the controller's claimed summary. Native barriers and failure
probes exist only in separately compiled test builds, not payload/CLI/environment
selectors. Host PTYs are owned and closed; host dispatch records no file effects.

| Group | Observable required result |
|---|---|
| PAF-T01 positive | Valid authenticated nested/empty/boundary data displays every planned effect, accepts one fresh action, launches exactly one writer and agrees with the independent final inventory/canaries. |
| PAF-T02 cancellation | Back/help/cancel, blank/default, EOF, timeout, signal, resize and broken output/console never dispatch under stale review; controlled terminal/flags restoration agrees exactly. |
| PAF-T03 input/replay | Controls, oversized lines, CR/LF/CRLF, prequeued review/phrase, multi-line paste and stale/repeated revision cannot grant authority; raw bytes never appear in output. |
| PAF-T04 context changes | Changed snapshot, key, tool identity, plan, root binding/mode/owner or nonempty inventory before/after confirmation rejects before member effects. Held-reference substitution probes preserve outside canaries. |
| PAF-T05 authority | Missing context, unsupported operation and arbitrary argument/command text refuse; no shell or caller-selected privileged dispatch; failed confinement prerequisites prevent writer launch. |
| PAF-T06 honest state | Injected writer short/close/fsync failure, signal/process loss and failed independent observation cannot report completion. Planned/submitted/observed values and uncertainty match the oracle. No automatic retry or rollback claim. |
| PAF-T07 console/resources | 80x24/40x12/unknown/unusable dimensions, complete maximum effect list, source/output/input/deadline/FD/RSS bounds and cleanup failure are observed; missing metrics block passes. |
| PAF-T08 regression/teardown | Repeat the existing 410 authentication/placement/historical checks per console unchanged, record source/binary/maps, remove keys before clean halt and prove full host fixture/client/guest/endpoint disposal. |

Run observable pure/owned-PTY host tests through make verify. Native tests repeat
applicable terminal cases in owned guest PTYs plus actual serial and keyboard/VGA
fresh-confirmation/cancel/substitution flows, for dynamic/static composition builds.
Host tooling cannot qualify native authorization, privilege or terminal behavior.
Before launch freeze the exact implementation, stimuli, expected inventories,
source manifests and commands; inspect the complete signed clean candidate and
independently read back the complete sealed ISO inventory/bytes. Missing gates
stop the run instead of inventing a passing outcome.

Existing authentication/placement builds retain their original 45-second whole
operation and 15-second child deadlines. The new composition build alone requests
a 90-second whole-operation ceiling including interactive review, with at most
60 seconds in review, 10-second line/inactivity bounds and five-second output
responses. Require at least 15 seconds remaining before granting the writer;
insufficient time rejects. Do not reset a deadline after help/back/rejection.
Every subprocess/writer retains its 15-second maximum inside the whole deadline.
This explicitly reviewed new interactive budget is not applied to historical tests.

Keep 1 MiB input, 512 KiB total/256 KiB per-file content, 255-byte paths/depth 16,
64 MiB controller/32 MiB child RSS and native DATA limits, CORE=0, CPU=15,
FSIZE=1 MiB and 160 owned descriptors. New console: 1024-byte line, 64 KiB input
work and 16 KiB output. Child authentication/writer output keeps its 4 KiB shared
bound; a separate bounded console is no route for raw child diagnostics. Additional
original C source at most 32 KiB; total experiment C at most 160 KiB, preserving
existing component source ceilings. Base-libc only, C99/Os/Wall/Wextra/Werror/
pedantic; base libutil remains a test-only PTY dependency. Record actual link maps,
metrics and unavailable observations; no memory/physical-presence/durability promise.

Guest case fixtures at most 20 MiB, 1 MiB private capture/case; host capture 10 MiB,
owned host allocation 20 GiB, source/data ISO 1 GiB. Per boot: 15 minutes to prompt,
45-minute scheduling gate and 60-minute hard stop from initial launch. Do not begin
another suite unless measured remaining time covers it and cleanup; no resetting
clocks, budget increases or weakened restrictions. Missing evidence, unexpected
prompts/attachments, exceeded limits or secret exposure stop dependent work.

## Completion, disposal and acceptance action

Publish source/base/head/tree, exact native compiler/artifact/tool identities,
actual case outcomes including stopped/skipped attempts, metrics, independent
postconditions and limitations. Report controller completion only when terminal
cleanup, reaping, writer/oracle agreement and unchanged outside observations all
pass; a late cleanup/observation failure invalidates success.

Observe native signing-pair cleanup, completed relinking/kernel agreement and clean
halts. Stop/reap owned children, guests, clients and observers; verify no fixture
handles, mounts, Unix listeners or attachments. Dispose only owned case roots,
credential/input/capture/screen files, keys, sockets, staging, data ISO, raw target
and new helpers after retaining sanitized receipts. Logical disposal is not physical
erasure or destructive installation rollback. Native authority ends at disposal.

Acceptance sought: this complete composition build/test slice, its expressly
different interactive deadline and the exact new target/provisioning/key/credential
and bounded file effects above. Ordinary implementation choices and corrections
within those bounds need no repeated approval. Another target, broader privileges,
new dependency/trust policy, production architecture adoption or higher budget
requires review within the same draft PR. Do not mark parent PTT cases or application
readiness qualified from this synthetic composition. No native case has run yet.
