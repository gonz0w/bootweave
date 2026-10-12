# First confirmed stock-installer handoff

Accepted bounded research, 2026-10-11 local. Owner/acceptance authority: Cam.
Base: 7c0a24ce7e3281d6ef06b0e43dd8f35f0a6fe30e, merged PR #33.
Acceptance: Cam replied "yes,continue" to the explicit scope/target question after
PR #34. This authorizes implementation and the exact four new writable targets
below, including the documented synthetic-password research limitation. Previous
storage-spike target approval ended with disposal and does not cover these files.
Implementation base: e3045329f87551e4d9c3464295729082cbd95ee2 (merged PR #34).

## Outcome and selected experiment

Build a removable original C front end in research/handoff. It boots into a
read-only target review, requires fresh explicit confirmation, rechecks observations,
and attempts one fixed interactive /install handoff. After handoff, the experiment
is supervised stock installation; it is not an automated guided installer.
No response-file generator, installer-source patch, project parser, payload,
site hook, product bootstrap, upgrade, network or durable runtime/API is adopted.
C and OpenBSD base libc remain research choices, with no new dependency or ROM.

The experiment tests the first real installation entry boundary while preserving
upstream installation machinery. It does not claim the stock shell, prompt defaults
or later operations enforce Bootweave policy. A production adapter needs a separate
accepted design for operation-by-operation authority and target lifetime.

## Requirements and engineering boundaries

Reuse the native-evidenced read-only metadata mechanism and pure validation from
research/storage: OpenBSD 7.9 amd64, bounded 4096-byte discovery, only sd0/cd0/rd0,
fixed /dev/rsd0c opened O_RDONLY/O_NOFOLLOW and verified as character 13/2,
DIOCGDINFO, checked label/extents/capacity, repeated discovery/DUID agreement.
Only one scoped 8589934592-byte candidate can proceed. No device path, command,
environment name or executable path is constructed from metadata or operator input.
Keep the existing standalone UI/probe research intact.

Present fixed ASCII plus validated numeric/hexadecimal observations: sd0, capacity,
sector size, DUID, and the reviewed effect summary: whole-disk MBR, auto FFS/swap,
verified stock OS sets, bootblocks and an offline first boot. Explain that existing
contents would be destroyed, metadata does not prove emptiness or host identity,
and partial installation has no general rollback. Actual host attachment identity
comes from the separate frozen fixture inspection, never from DUID alone.

Use bounded input handling already evidenced by the UI experiment: fixed 1024-byte
line buffer, 64 KiB work/output limits, monotonic deadlines, no echo of rejected
controls and exact terminal restoration. The final review and effect summary must
remain readable at 80x24, 40x12 and serial/unknown dimensions; known narrower
dimensions reject. Require a TTY for the native handoff. Pipe input cannot grant it.

Use a review step followed by a fresh confirmation step containing the displayed
DUID and a fixed erase acknowledgement. Drain/reject queued input before entering
confirmation and reject multi-line contamination before handoff; no empty answer,
help/back/cancel command, control sequence, oversized line, EOF, timeout or signal
can authorize it. A resize or changed observation invalidates review and confirmation.
Terminal input cannot prove human intent or distinguish every single-line paste;
no general anti-paste or physical-presence guarantee is claimed.

Immediately before handoff, repeat platform/discovery/device/label observation
and compare every reviewed field. Restore the exact saved TTY state, check output
completion, close extra descriptors, change working directory to / and clear
prototype-only timers before exec.
Invoke only /install with no arguments through execve and a fixed environment
(PATH=/sbin:/bin:/usr/bin:/usr/sbin:/, VNAME=7.9, VERSION=79, ARCH=amd64,
TERM=vt220 and LANG=C). No shell command string or operator value reaches argv/env.
Verify the stock entry/symlink and its source identity in the owned native image
before sealing it. Do not add a dry-run or debug flag that bypasses this boundary.

The research profile updates only RAM root before review, prepares fixed RAM-only
/tmp/ai and /tmp/i directories, and records a single-attempt marker. It omits the
stock menu, /upgrade -ax, autoinstall timer and response-file entry paths. No
/auto_install.conf or /auto_upgrade.conf may exist. Prepare required release
variables explicitly rather than inheriting untrusted environment state. Use
stock-compatible umask for handoff, with restricted private capture/secret files.
After failure, cancellation or a returned installer, never rerun automatically.

Freeze source, native binary/map, modified kernel/ISO and unchanged stock installer
identities before execution. Keep the stock 3768320-byte ramdisk reserve. Build a
single static executable, reuse original bounded routines, and optimize within the
reviewed behavior if needed. ENOSPC stops this candidate; reserve enlargement,
kernel rebuild, extra library or alternate backend needs further review.

Planning, handoff requested/attempted, actual stock writes and independent observed
postconditions remain distinct. exec success or stock exit/congratulation text is
not readiness. The prototype does not retain authorization across reboot, restart,
changed attachments or failed observations. Host attachment substitution during
stock operation is excluded by the controlled fixture; no general race guarantee
or stable disk handle passed to stock is claimed.

## Exact prepared targets and allowed effects requested

At preparation, these were exclusively owned 0600 sparse raw files under
.tools/installer-handoff, inside an owned 0700 directory. Each had zero allocated
bytes. A private receipt bound its filename, host inode/device, size and whole-file
SHA-256; substitution invalidates acceptance even when zero-filled files share a
digest. No guest had started, and all preparation write flags were false. The
subsequent explicit acceptance was recorded separately before the build launch.

| File | Bytes | Guest authority requested |
|---|---:|---|
| build.raw | 8589934592 | Manual verified stock installation, native compilation/media files, offline build-guest boot |
| serial-target.raw | 8589934592 | Confirmed serial handoff, supervised MBR/FFS stock installation and one offline first boot |
| vga-target.raw | 8589934592 | Same positive case independently through keyboard/VGA |
| interrupted-target.raw | 8589934592 | Confirmed handoff, partition/format writes, deliberate interruption before OS extraction; no boot or resume |
| cancel.raw | 8589934592 | Read-only in all cancellation/input/identity-negative guests |
| small.raw | 2147483648 | Read-only undersized-capacity rejection |

All five 8 GiB files initially have SHA-256
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25;
small.raw has a7c744c13cc101ed66c29f672f92455547889cc586ce6d44fe76ae824958ea51.
Only the first four exact files may receive guest writes after acceptance. Never
attach two candidate disks in one guest. No existing disk or host device is exposed.

Build guest: existing reviewed writable IDE configuration, observed wd0 before
partitioning. Handoff guests: the reviewed legacy virtio-blk-pci settings from the
[accepted storage amendment](storage-probe-spike.md), observed sd0 and capacity
before confirmation. Candidate is writable only in explicitly authorized positive
or interrupted runs; the front end itself opens it read-only until stock handoff.
No QMP disk replacement/reopen, overlay, hot-plug or shared mount during a run.

Retain pinned QEMU 11.1.1, pc-i440fx-11.1/qemu64-v1, TCG, one CPU, 1024 MiB,
reviewed BIOS/VGA firmware, paused inspection and read-only optical media. No NIC,
IP listener, host sharing or passthrough. Supervisors use private Unix endpoints.
Limits: 15 minutes to prompt, 60 minutes per run, 20 GiB total owned host allocation,
10 MiB capture per run, 32 MiB front-end RSS checkpoint, 64 KiB front-end output.
Front-end limits: 10 seconds of input inactivity, 60 seconds per review attempt,
five seconds from completed input to response, and 64 KiB original source.
The stock installer changes resource limits; the guest supervisor remains the
run/capture/memory boundary after handoff. Measure actual results rather than
claiming the front-end RSS budget constrains stock installation.

## Supervised stock path and secrets

Use authenticated OpenBSD 7.9 media/sets and the accepted signify/key route. Verify
actual set identities natively before extraction. No site set or install.site may
be available. Manual fixed answers select a synthetic .example.invalid hostname,
no physical interface, no DNS server, SSH/X disabled, no extra user, UTC, sd0,
unencrypted whole-disk MBR, auto layout and no additional disk. Select reviewed
bsd/bsd.rd/base/comp/man only; no game/X/site sets or verification-bypass answer.
Optical source must match its frozen attachment, not merely its guest name.

Passwords are independently generated synthetic secrets supplied only after an
observed stock password prompt with echo off. The front end has no secret field
and puts no operator data in argv/env. Upstream install.sub 2906–2919 passes a
plaintext password to encrypt through argv when it is not already a hash;
3426–3433 uses this path for root. Acceptance explicitly permits this stock
behavior only for disposable synthetic credentials inside these isolated guests.
It does not satisfy the proposed production secret-channel requirement. Do not
inspect/capture stock process arguments during password handling, enable tracing,
or put credential bytes in documentation, ordinary logs or public evidence.
Keep private input queues restricted and dispose them. The inspected interactive
ask_pass/ask_password path (888–917) avoids the ordinary ask answer logger;
the source review is not a native secret-exclusion pass. If the expected prompt,
echo state or consumer path is uncertain, stop input.
No automated response matching, omitted-answer/default test or untrusted prompt
input is included. Stop on any unreviewed question or failed verification instead
of inferring an answer. One previously observed read-only optical mount retry is
permitted after attachment readback; destructive steps never retry automatically.

Positive installs exit to shell for independent label/filesystem/configuration and
stock-file checks, then sync/halt. A separately inspected offline first boot of
each same target may run stock base startup within its owned guest: observe GENERIC
7.9 amd64, root mount, synthetic hostname and independently expected installed files.
SSH/X remain disabled and no NIC exists. Stock firmware/syspatch work has no network
attachment; no product service, host service, release or live network is authorized.
Halt after checks. Call this stock OS boot evidence, not generic application readiness.

## Test Design and evidence

| Case | Required independent observation |
|---|---|
| Host model/input tests | Exact confirmation only; cancel/back/help, stale fields, resize, controls, overflow, queued input, EOF/deadlines/signals reject; arguments never select commands/targets |
| Native build | Warning-clean base-cc static build; same pure tests natively, linker/footprint review, stock reserve, owned mount/vnode cleanup |
| Native negative serial/VGA | No candidate/small capacity reject; valid read-only fixture cannot hand off through invalid/empty/pasted-multiple/expired/cancelled input; exact TTY state restored |
| Rejected host substitution | Changed private inode/size/digest/attachments rejected before launch; never attach an unreviewed device to test the boundary |
| Serial positive | Fresh confirmation followed by observed fixed /install entry; supervised verified install, independent target checks and offline boot |
| Keyboard/VGA positive | Same behavior on its separate target; full summary readable before confirmation |
| Interrupted install | On its separate target, stop after independently observed partition/format stage and before extraction; record partial/uncertain state, no completion or automatic retry |
| Teardown | Negative disks hash/allocation identical; authorized writes recorded distinctly; all owned processes/endpoints/mounts gone, secret-free sanitized receipt saved, owned fixture files disposed |

Observe zero block writes before confirmation/handoff, including in writable
positive fixtures. This is native evidence, not an assumption based on code review.
Native secret, signal and terminal postconditions need actual results; host tests
qualify only tooling. Record failures/skips honestly. An unexpected partial state
stops dependent work; retain safe evidence and discard the fixture, not rollback.

Preparation reverified the authenticated src.tar.gz and the three selected installer
members against their [recorded identities](installer-source-findings.md), and
rehash-checked existing QEMU/firmware/stock ISO. Source review reconfirmed /install
as the fixed entry: install.sub selects MODE from that basename, sources install.md
relative to /, and requires release variables normally exported by dot.profile.
This supports the explicit preparation above; none of that upstream code ran here.
The selected root-password consumer review additionally identified the plaintext
encrypt argv limitation above, with no claim about the security of other stock
installation modes or acceptance of this behavior in Bootweave production.
The [disk](installer-disk-question-ledger.md) and
[set/completion](installer-set-completion-ledger.md) ledgers retain stock defaults
and write ordering. Complete automation and broader secret-consumer research remain
outside this manual experiment; no future answer schema or stock safety contract follows.

Accepted work: implement/test this bounded original front end, build it
natively, run the specified supervised cases with only the four exact writable
targets above, and publish sanitized signed-DCO evidence in an open PR. Routine
corrections within these limits need no new approval. New targets, dependencies,
backend/reserve changes, automated stock answers or production adoption do.

The [first build preparation stopped](installer-handoff-stopped-evidence.md)
before set extraction, and all six original fixture files were disposed. This
exact-target authority has ended. Further native work awaits acceptance of the
[new-target/media amendment](installer-handoff-retry.md).
