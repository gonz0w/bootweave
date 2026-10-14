# Confirmed installation to installed payload: complete research slice

Status: Accepted, 2026-10-13. Owner and acceptance authority: Cam.
Acceptance: Cam replied "approved" after draft PR #52 and its explicit complete-slice,
three exact targets, two signing pairs and synthetic credential exception question.
Reviewed scope head: e999eec0e453f1e3823fb3b16534bb6382b497ce. The immutable
preparation receipt remains historical; a separate private acceptance record binds
that approval to its digest and rechecked unchanged full target identities/bytes.
Base: bc989d022f97d25fda9702c97b531aa51da12cf2, maintainer-merged PR #51.
PR #52 was maintainer-merged at the scope-only head before its implementation
commits were included. The implementation and stopped evidence now remain together
in [PR #53](https://github.com/gonz0w/bootweave/pull/53), based on
676814196afa8bdd7d61d7f42faad8d246cec199. Preserve historical source/media receipts;
its cherry-picked source tree was independently equal to the already verified tree.
Related [issue #5](https://github.com/gonz0w/bootweave/issues/5),
[handoff evidence](installer-handoff-native-evidence.md) and
[payload authorization evidence](payload-authorization-native-evidence.md).

## Deliverable and review boundary

Build a removable staged original C experiment that installs stock OpenBSD on an
exact fresh target through the evidenced confirmed handoff, boots that installed
system offline, reviews an inert authenticated document payload, confirms its
placement on that same installed filesystem, and independently checks it after
another clean boot. This connects the two evidenced boundaries without selecting a
production runtime, profile grammar, adapter API or persistent receipt format.

PR #53 was also maintainer-merged before native qualification. Keep all remaining
fixes, tests, native qualification, cleanup and evidence on
codex/installed-payload-native in one follow-up draft PR. Acceptance
covers that complete bounded feature, rather than requiring a planning merge or
another PR for each phase. Mark it ready only after both console flows and disposal.
Larger scope changes return to review in that draft.

The installation remains manually supervised stock /install; no answer generator,
site set, install.site, upstream installer patch or autoinstall is introduced.
The installed payload stage is manually launched from fixed read-only research
media after offline OS boot. It does not run automatically at boot or install a
service. This measures a staged path, not a continuous production installer UI.

## Requirements and engineering boundary

| ID | Required result |
|---|---|
| IPF-01 | Fresh disk confirmation and unchanged host attachments precede stock writes; a previous file-effect confirmation never grants disk authority. |
| IPF-02 | Stock completion is checked independently; successful child exit/text alone cannot authorize the installed payload stage. |
| IPF-03 | A read-only installed-root gate proves the sole selected disk, root filesystem, mount/device binding and held destination context before review and again before dispatch. |
| IPF-04 | Complete authenticated member review and fresh one-use confirmation bind that installed target, retained bytes, signing/tool identities and full plan. |
| IPF-05 | The existing nobody writer and independent oracle retain confinement, no-overwrite and resource rules; rejected/changed contexts launch no writer. |
| IPF-06 | Report OS observation, payload planned/submitted/observed state and later readback separately; counts/exit zero cannot conceal partial or uncertain content. |
| IPF-07 | Serial and actual keyboard/VGA supply equivalent confirmation, cancellation and partial-state meaning with exact terminal restoration. |

Reuse the original handoff, storage validator and payload components. Add a fixed
compiled installed-root observer/supervisor; no executable, mount path, destination,
account, shell text or privilege operation comes from payload/operator input.
Host tests have synthetic observations and no disk/placement backend. Standalone
unsupported context/arguments refuse. Inputs and kernel mount/device text are
untrusted and validated before display or comparison.

The installed-root gate correlates the held root filesystem and mount identity
with the sole fixed sd0 raw-device/disklabel observation and reviewed root partition.
Capacity or a displayed DUID alone is insufficient. Freeze authenticated native API
source, device/partition rules and exact postconditions before implementation;
if those observations cannot prove the binding, stop dependent work and review a
replacement mechanism. Reobserve complete context before dispatch. No mount,
remount, chrooted OS bootstrap or arbitrary target selection belongs to this gate.

After independently observed OS installation/boot, trusted setup may create only
/bootweave-research (root-owned 0700), its outside canary and one empty case directory
owned by existing nobody UID/GID 32767 with mode 0700. This exact preparation is a
separately preauthorized fixture effect, shown distinctly from payload member effects;
it is not evidence that a production planner has no writes. Review binds that held
empty case root and its installed-filesystem association. Only fresh member
confirmation authorizes archive effects. Root setup never interprets member paths.

The payload remains frozen inert nested text/empty files, with fixed expected hashes,
0700 directories and 0600 files. No executable bit, executable content, account,
product configuration, networking or service is added. Each failure probe has a
fresh empty owned case; only named owned fixtures may be cleaned up between probes.
A final positive case remains for independent readback after halt and reboot.
Cancellation cannot undo the preceding OS installation or fixture preparation.

Use the current exact binding/queue/terminal rules and ninety-second composition
budget. Earlier disk confirmation does not carry into payload confirmation; a
reboot supplies no persisted authorization. Changing root filesystem/device/mount,
label binding, held case, snapshot, key/tool identity or plan invalidates the attempt.
Unchanged member counts with wrong content must remain incomplete. Failed postconditions
stop dependent work; no automatic retry, resume, repair or general rollback.

## Frozen installed-root mechanism, 2026-10-13

Accepted-policy explicit-key signify verified sys.tar.gz against the previously
verified release manifest. Full archive SHA-256:
c9ef294021ef7aafd5f18ffe8ebfed63394e5a86d65d7436d6db78551f9d57f1.
Only the selected bounded regular members below were read; no upstream code ran.

| Release source | SHA-256 |
|---|---|
| sys/sys/mount.h | 1b7b54af74f683ebde14ae63db88dfd2718c81b853baab76bd0f0aa484a63714 |
| sys/kern/vfs_syscalls.c | 26e1b069c1e790f516279c59f523405f53a3c357946546f9fe44b126a28f9b62 |
| sys/kern/vfs_subr.c | 5dc75eebf7484897cd0577d1b9eb80247af677714e766acb92322c89cb0cbfa9 |
| sys/ufs/ffs/ffs_vfsops.c | 100fee4f552dbc21428a1da211ad2e237bc27b6d4dacfab7032c1b90ed2d52eb |
| sys/sys/disklabel.h | 29212a079676a6e34fc8e2a79e4eb8f9a2bc4ae264b04ef3e06fba11fe787ecb |
| sys/arch/amd64/amd64/conf.c | cf0ca3b13b8dc36f8c183b5a9e75b278c460d175877f60b6deab9d3722c8dc68 |
| sys/kern/subr_disk.c | 3696f57abafb8b7138893f91926d9babb205e6b5293ce16a35788e9baa3f767e |
| sys/sys/types.h | 4c8207f20baf3e965be7dde3ba7e4334fb649667617790fb45181c7de6d901c7 |
| sys/sys/param.h | 80017c9395be25d7dcf8a3103ddb8c032c47cde46ea5575971297cb3452812f4 |

fstatfs returns the filesystem attached to the held vnode, with full visible mount
flags, owner, last mount-update time and fsid. FFS fsid[0] is the actual block-device
number; amd64 maps raw sd major 13 to block major 4. The fixed sd0 root partition
is block minor zero, raw whole-disk minor two. Root st_dev, held directory st_dev,
block-device st_rdev and FFS fsid[0] must agree, with local/rootfs flags, FFS type,
mount point / and mounted-from /dev/sd0a. Mounted-from-spec must be that same fixed
name or the independently observed DUID followed by .a. Native ioctl on the held
raw and block references must agree on the complete validated label geometry,
partition types and DUID; root partition a must be nonempty BSD FFS.

Compare fields explicitly, excluding changing free-space/I/O counters and structure
padding. Keep all visible flags and mount-update time in the binding; readonly,
async, permission-bypass or exported roots refuse. Reobserve device references,
label/discovery, filesystem and held/named parent/case identity before dispatch
and after writer observation. No mount operation is added. These source facts
are not native qualification; native measurement must establish every claimed gate.

Reuse authentication's transient public-key/signature verification records and a
fixed synthetic source file only inside the owned research parent; remove/observe
them after each attempt. They are fixture bookkeeping, separately identified from
archive members, contain no private key, and cannot select paths or executables.

## Exact unstarted fixture and requested authority

Three new sole writable files, exclusively created in owned 0700 storage:

| Path | Bytes | Mode/links | Initial allocation |
|---|---:|---|---:|
| .tools/installed-payload-flow/build.raw | 8589934592 | 0600 / one | 0 |
| .tools/installed-payload-flow/serial-target.raw | 8589934592 | 0600 / one | 0 |
| .tools/installed-payload-flow/vga-target.raw | 8589934592 | 0600 / one | 0 |

Each complete initial SHA-256 is
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25.
Private immutable target receipt SHA-256:
56f416bbdf7f25ac96bb610d1c2b81f1f7a4db4aee9abdb68bdcacf90e2dac5f.
It binds each host device/inode, size, mode, links, allocation and full digest.
Preparation rehashed all seven pinned emulator/firmware/verifier/key/media/signature
inputs and repeated explicit-key host signify -C for install79.iso: all passed.
No guest started, guest write authority was granted, integration code implemented,
credential/signing pair generated or derived media built during preparation.
Previous exact-target authority ended at disposal and does not cover these files.

Acceptance sought includes original implementation and fixed test/build/supervision
code, native compiler guest provisioning, modified handoff-only boot media within
the existing RAMDISK_CD reserve, sealed read-only source/artifact media, stock OS
installation on serial-target and vga-target, their offline payload/readback boots,
exact fixture/member effects above, two fresh native signing pairs and three fresh
synthetic stock root credentials (one per target, reused only across its boots).
The stock encrypt plaintext-argv limitation is requested solely for those synthetic
credentials, as previously evidenced; it is not production secret delivery.

Use unchanged pinned QEMU 11.1.1, pc-i440fx-11.1, qemu64-v1, TCG, one CPU/1024 MiB
and firmware from the [preflight](../design/native-profile/preflight.md).
Build target: existing IDE wd0 profile. Installation targets: the evidenced sole
virtio sd0 handoff profile, never attached together. Read-only optical inputs at the
fixed reviewed IDE slots; no NIC, IP listener, sharing, passthrough, overlays,
backing chain, hotplug or running media swap. Private Unix endpoints only.
Inspect full paused effective argv/block/PCI/device trees, process files/listeners,
then exact target identity/digest/capacity immediately before stock destructive input.
Any mismatch stops without substituting a target or increasing authority.

Verify release inputs with explicit-key native base signify before stock installation
and retain stock set verification. Whole-disk unencrypted MBR/automatic FFS+swap,
synthetic .example.invalid hostname, UTC, no interfaces/DNS, SSH/X/additional user,
site/game/X sets or hooks. Build guest selects bsd/bsd.rd/base/comp/man; installation
targets select bsd/bsd.rd/base/man. Keep generated first-boot work offline and observe
completed kernel relinking/copy agreement before each clean halt. A partial OS
installation or uncertain kernel state blocks the payload stage.

Generate both disposable signing pairs only in the build guest. Reuse them for all
fixed fixture variants; export only authenticated inert payloads, public keys,
signatures, binaries/maps and sanitized identities through bounded checked frames.
Never put private keys or credentials on source/boot media. Independently observe
both key pairs and lifetime receipt absent before the build guest's final halt.
Credentials remain in private host files, outside argv/environment/Git/public evidence;
submit only at observed echo-off stock prompts and audit captures before viewing.

## Test Design, frozen commands and budgets

Before each native phase freeze its exact clean signed source/base/tree, source
manifest, commands, tool/artifact/media hashes, stimuli and independent expectations.
Read back complete sealed media bytes/inventory before attachment. Commands use
only reviewed fixed paths/arguments. Missing dependencies, confinement, metric,
terminal or observation evidence stops the phase; no host result qualifies native
execution. Base tools only; new dependencies or production adoption need review.

| Group | Required observations |
|---|---|
| IPF-T01 host/pure | Each changed installed-root binding field rejects; no-context/arguments refuse; owned PTYs retain full review, fresh confirmation and safe terminal behavior without a write backend. |
| IPF-T02 regression | Repeat all 976 required #51 component/composition checks in the build guest across serial/keyboard-VGA and dynamic/static builds. Record earlier failures separately. |
| IPF-T03 root binding | Missing/ambiguous device or root observation, unsupported partition/filesystem, changed label/mount/root identity or flags and mismatched held destination refuse before member dispatch. Fixed synthetic substitutions test each comparison; native observations independently establish the positive binding. |
| IPF-T04 installed flow | Each fresh serial/VGA target passes confirmed stock installation, observed offline OS boot, complete member review and one fresh placement; independent inventory/content/owners/modes and outside canaries agree. |
| IPF-T05 installed refusals | Actual serial/VGA cancellation and postconfirmation snapshot/root-binding substitution launch no writer; failed terminal/queue/restoration never dispatch. |
| IPF-T06 partial state | Separately compiled short/close/fsync failure probes leave independently observed incomplete content with honest counts/uncertainty, no retry. Disposed probes cannot become a readiness pass. |
| IPF-T07 later readback | Preserve a final positive inventory, clean halt, bind full target digest, boot the same exact disk offline again and independently check bytes/modes/owners/root association without running payload code or rewriting members. |
| IPF-T08 teardown | All owned processes/relinkers reaped, key lifetime ended, clean halts/full digests, no handles/mounts/listeners/attachments; dispose only the exact new fixture/helpers after sanitized evidence. |

Frozen implementation case list: seventeen automatic cases per dynamic/static
build (positive, cancel, nine changed-observation bindings, postconfirmation snapshot
change, short/close/fsync failure, wrong signer and truncated signature); five actual
console cases per build (positive, cancel, snapshot change, changed observed flags,
short writer); one static retained-positive case and one static later-readback case
per installed console. This requires 92 installed checks plus 976 build-guest
regressions, 1068 total, with no substituted or silently omitted case. Pure field
comparisons/public packing and host refusal remain three host groups, not native
filesystem qualification. Dynamic and
static installed-root supervisors both need native cases and link-map/ldd/readelf
identities. Positive RSS/CPU/time/allocation measurements remain required.

Retain component ceilings: 1 MiB input, 512 KiB total/256 KiB member, depth sixteen,
255-byte paths, sixteen reviewed effects, 1024-byte line, 64 KiB input work and
16 KiB review output; 64/32 MiB controller/child RSS/DATA, CORE=0, CPU=15,
FSIZE=1 MiB, 160 descriptors. Old operations stay 45 seconds whole/fifteen child;
review builds alone use ninety whole/sixty review/ten line/five output seconds and
at least fifteen remaining before dispatch. No deadline resets on help/rejection.
Disk console stays within its original handoff bounds; stock provisioning is covered
by the launch clock rather than a fictional payload-operation deadline.

Additional original C/headers at most 16 KiB. Combined reused payload/handoff/storage
and new C/headers at most 200 KiB (existing union 181,940 bytes); retain all existing
component source caps. Modified ramdisk fits the original 3,768,320-byte reserve
without expansion and packages only the handoff, not the payload controller. Payload
supervision runs in the installed OS from reviewed read-only media; ramdisk payload
confinement/availability is not claimed.

At most 20 MiB guest test-fixture allocation and 3 GiB separately measured native
media-build scratch/output in owned build directories; check actual free space before
starting a build. Private capture at most 1 MiB/case, aggregate host observation
capture 10 MiB, allocated owned host storage 20 GiB and each sealed source/artifact/
boot ISO 1 GiB. Sparse logical target sizes are distinct from allocation. Artifact
export at most 1 GiB total including encoded frames, separate from observation
capture, with explicit frame/length/hash checks. Each
launch: fifteen minutes to prompt, 45-minute scheduling gate, sixty-minute hard stop
from initial start; no clock resets or extra run outside the accepted sequence.
Maximum nine launches: build provisioning/serial qualification/VGA qualification,
serial install/payload/readback and VGA install/payload/readback. Do not begin
another suite unless measured remaining budget includes cleanup. Unexpected prompts/inputs, secret exposure or
exceeded/missing metrics stop dependent work; retries/changed budgets need review.

## Completion and acceptance action

Publish actual source/artifact identities, native compiler/link/resource observations,
case results including failed/stopped attempts, separate OS/payload/readback state,
terminal limitations and postconditions. Booted inert data readback is not product
function, service readiness or full parent PTT/installer acceptance.

Observe clean halts and unchanged exact host identities/full post-write digests.
Restore consoles, stop/reap owned guests/clients/observers, verify no handles/mounts/
listeners/attachments, then remove only new disks/media/staging, key/credential/input/
capture/screen/receipt files and helpers. Retain sanitized evidence; logical disposal
is not physical erasure or rollback. Exact-target authority ends at disposal.

Review request: accept this whole bounded implementation/native slice, its three exact
fresh targets, synthetic credential exception, two signing pairs, staged installed-root
fixture effects and limits. This is a larger target/authority boundary under
[AGENTS.md rules 3 and 7](../../AGENTS.md). Ordinary bounded fixes then proceed within
the same draft without repeated approval. Broader privileges, another target,
production adoption, new dependency or increased budget require review there.

## Stopped pre-install execution and proposed launch-count amendment

Historical stopped report, 2026-10-13, before amendment acceptance below.
At this point the amendment was proposed and the original nine-launch ceiling
remained in force. Implementation source
1f256a815107cde4fa66ecebf5eee40b3acd2727 passed clean local make verify: 81 groups
(22/13/3/10/33), no failures/skips; two trusted signatures and author DCO passed.
All four installed entry variants passed host syntax checks with authenticated
native headers; no native implementation build or installed-root case has run.
Additional original C including reused-source changes: 16,376 bytes within 16 KiB.

The sealed nonbootable source ISO is 687,392,768 bytes, SHA-256
89227f5240375a4dcb665d6bb1f659076657d715e84f19b26a6621a9e6f3fcf3.
Complete ISO9660 and Joliet readback independently matched all 276 source, fixture
and stock-input files. Duplicate staging set paths were removed before sealing;
input filenames/bytes and native build source manifests were frozen.

Two launches have been consumed and count against the nine-launch ceiling:

- The first stopped while paused, before any guest CPU continuation. A host capture
  classifier counted the public sys.tar.gz source archive as observation capture.
  The target retained its full initial digest and zero allocation. The emulator was
  reaped; only documented observations now count as capture, while source archives
  still count toward allocated storage. No limit was increased.
- The next reached the stock RAMdisk and verified release inputs, but host input and
  observation setup consumed the provisioning window. Legacy timed key events did
  not deliver the observation training reliably; explicit down/up events independently
  produced the expected harmless shell output before any credential or write answer.
  Stock SMALL ksh had no printf; authenticated print -r produced/read back all 95
  printable glyphs. The missing cd1 node was first given the wrong character type,
  then the wrong unit stride. Both mounts refused. Authenticated 7.9 disklabel.h
  defines MAXPARTITIONSUNIT=64: the reviewed cd1 block partition c is major 6/minor
  66, not 18. Only that newly owned RAMdisk node was corrected; attachments stayed
  unchanged. This does not affect the installed gate's fixed unit-zero sd0 nodes.

A conservative private observer text guard also stopped at its 1 MiB bound.
The history was retained losslessly in a compressed checkpoint; cursor deduplication
was corrected, and actual screen readback independently established verification.
The approved aggregate capture bound was not exceeded. Submitted input was never
counted as an observed outcome. These are host supervision/preparation failures,
not passing native implementation tests.

Native explicit-key base signify observed Signature Verified for the release
manifest, and native SHA-256 for all six stock inputs independently matched it.
No installer was invoked, password submitted, native signing pair generated or
payload member created. Remaining time was insufficient for a complete installation
and cleanup using the previously measured installation durations, so dependent
writes were not started. The RAMdisk visibly completed sync and halted; its requested
poweroff event was unavailable at the press-any-key reboot prompt. QMP exit followed
that independently observed native halt, with no reboot or forced running-OS stop.
Normal emulator exit occurred after 2220.832 seconds, within the scheduling/hard bounds.

All three exact targets retained their full initial zero-image digest, identity,
0600 mode, single link and zero allocation. Guests, clients and observers were
reaped; fixture handle, mount and Unix-listener observations were empty. Capture
credential exclusion passed. The same three unused synthetic credentials and exact
unwritten targets remain privately held for review; no fixture/authority disposal
or native completion is claimed. All 1068 required implementation cases remain not run.

Requested amendment: allow **one replacement stock provisioning attempt** on the
same exact untouched build.raw and increase the aggregate maximum from nine to
**eleven launches**, accounting for both stopped attempts and preserving the
original nine-phase sequence, including separate serial/VGA build qualification.
Retain every original per-launch, operation, capture, storage, source, memory,
privilege and artifact bound. No new target, credential, signing pair, dependency,
network, service or production adoption is requested. Recheck the immutable receipt
and all full target digests before recording any acceptance or continuing.

The fixed observer, keyboard and read-only source-node corrections will be reviewed
with the existing draft before that attempt; do not repeat known failing preparation
or renew clocks within a running launch. This is a concrete retry/launch-count
review under this plan's existing stop rule, not permission to retry a partial
installation. No destructive installation has occurred. All implementation, fixes,
qualification, stopped evidence and eventual disposal continue in PR #53.

## Accepted replacement provisioning amendment, 2026-10-13

Cam replied "approved" to the request for one replacement provisioning attempt and
eleven total launches. Review head: 7acc72385f7e445f477088263353f6e4e941327d.
A separate private amendment receipt preserves original acceptance history and binds
the unchanged immutable target receipt. All three full target digests, exact identities,
0600 modes, single links and zero allocations were rechecked before recording acceptance.
The replacement is build-install-retry; both prior stops count toward eleven. Every
original per-launch and other bound remains unchanged. PR #53 includes implementation
and stopped evidence; it does not contain native qualification. All remaining work
continues together in one draft after that maintainer merge.

## Installed disk namespace adjustment — accepted, 2026-10-14

Native compiler-guest provisioning completed on the same confirmed build.raw,
followed by an offline serial boot and all 488 required serial regression/composition
cases. Native hw.disknames also reports fd0 with an empty DUID after the wd0 and
cd0 entries. Paused effective blocks contain only the exact target and read-only
source optical; the legacy floppy controller has no backing image. Native fd0 raw
and block nodes are major/minor 9/2 and 2/2; disklabel refused with Device not
configured. That refusal alone is not evidence that arbitrary floppy media is empty.
Authenticated OpenBSD 7.9 fd.c shows probe-based attachment and disk_attach even
when density is unknown. The RAMdisk discovery list did not expose this full-kernel
entry. The current strict storage_discovery refuses it; no installed-root case ran,
and the two payload targets remain untouched.

Accepted bounded replacement: keep the existing generic storage validator unchanged.
Add only a private installed-research discovery adapter that permits at most one
exact fd0: record with no DUID, removes that record for the existing validator and
retains the complete original kernel namespace in before/after equality checks.
The remaining records must still prove exactly the fixed sd0/DUID with only the
already permitted empty cd0/rd0 entries. Duplicate, malformed, nonempty-DUID fd0,
fd1 or any other disk entry refuses. Root/block/raw label agreement, root FFS fsid,
device/inode/mount/held-context checks and fresh confirmation remain required.
Neither fd0 device is opened or written by the payload gate or writer.

This exception requires the already mandatory exact no-hotplug virtual fixture
preflight, including a complete paused block/device inventory proving no floppy
backing media. It is not a production physical-media classification rule. The stock
installer and emulator/firmware profile remain unchanged. No new target, launch,
credential, signing pair, dependency or budget is requested. Retain both original
source ceilings; update only frozen installed source/manifests and sealed inputs
as necessary, with native source/artifact identities recorded separately.

Add pure-host acceptance/refusal cases for every namespace boundary above and native
read-only namespace/empty-backend observations on both build consoles. Existing 976
regressions remain required. The later installed target must demonstrate positive
root binding in every existing installed case; a refusal cannot count as placement.
Cam approved the concrete replacement after reviewing head
2ea6560fcf823d33cf447576e031384b019e015b; a separate private acceptance receipt
binds the preserved target receipt and rechecked full current target digests.
PR #54 was maintainer-merged before the correction/native completion. Keep all
remaining fixes, qualification and disposal on codex/installed-payload-completion
in one follow-up draft. This is the frozen mechanism's
existing stop rule: "if those observations cannot prove the binding, stop dependent
work and review a replacement mechanism."

## Recovery and range-export amendment — accepted

2026-10-14. Cam replied "continue" to the concrete twelfth-boot review question,
accepting the fixed recovery/range export and twelve-launch ceiling. The separate
private acceptance receipt binds reviewed head 2f2aeb3e844edc195bb1c1962e307b7c11698831,
the original target receipt and freshly rehashed stopped/untouched target bytes.
PR #55 was maintainer-merged at a8c217874a915476709bd39816534c37e103640f;
remaining recovery, target qualification and disposal stay in one follow-up draft
on codex/installed-payload-recovery. Historical receipts remain unchanged.

In [PR #55](https://github.com/gonz0w/bootweave/pull/55),
the accepted private discovery adapter and its routine inherited-declaration fix
are implemented. All 976 build regressions passed across serial and actual VGA;
all six corrected installed entry variants compiled. The planned VGA launch then
exported eighteen complete public artifacts but not the large native handoff ISO.
Its bounded export ended incomplete. Foreground cancellation, exact accessory TTY
restoration, matching kernel copies, absent test/relink workers and key absence
were observed. The clean-halt dispatch was too late: the launcher enforced its
sixty-minute hard limit at 3600.198 seconds. QEMU exit zero is not a native halt.
The build filesystem and persisted key absence require recovery observation.

The emulator/client/observer were reaped; exact target identities, 0600 modes and
single links matched, with no fixture handles/mounts. Current complete build.raw
SHA-256: 8f63565048599f8c212d8deb7b1dde8ea2be23e121685be4b1ca242e925374c7.
The two payload targets retain their original full zero-image digest and zero
allocation. Historical clean-halt/acceptance receipts are preserved; this stopped
image's digest is recorded separately, without upgrading it to a clean state.

Accepted amendment: one additional build-export-recovery cold boot on
that same exact build.raw, before the six unchanged target phases, increasing
the aggregate launch ceiling from eleven to **twelve**. Preserve every other
launch, source, capture, storage, memory, operation, privilege and export bound.
Use the unchanged pinned offline IDE build profile and a sealed read-only source
optical carrying only the fixed reviewed reader and its SHA256 manifest; reuse the
existing build credential. Complete ISO9660/Joliet inventory readback is required. No additional target, credential,
signing pair, service, network, dependency or runtime adoption is authorized.

Observe stock automatic filesystem recovery (failed preen or an unexpected
interactive repair stops, with no blind fsck -y), independently recheck the frozen native source
ISO's file size/mode/full SHA-256 and already exported artifact identities, and
reobserve both key pairs/lifetime receipt absent. If old owned key files reappear
after recovery, remove only their previously authorized fixed paths and independently
observe absence; never generate or reuse another pair. Missing source/artifact or
filesystem proof stops dependent work; this is not recovery of a payload install.

Replace the full-ISO replay with one fixed read-only range export of the same
natively built handoff.iso. Retained decoded prefix: 508315170 bytes, SHA-256
3783a0f7c9277c39f33801cbf79b1ef1e6d0f068470700b645bae68095ecd4bf.
Freeze a validated 2048-byte-aligned resume offset of **508313600**; discard only
the final 1570 prefix bytes for assembly. Remaining native suffix: **169625600**
bytes. Before reading it, the complete native ISO must still be 677939200 bytes
and match 607f1cc73a90402e5a02889b1bdc79c29dc8f67176086be38beffe218422f624.

Use base dd with the fixed public ISO path and typed bounded block offset/count,
base sha256 and one canonical base64 frame with exact native suffix size/hash.
No path or shell operation comes from native output or payload input. The concrete native reader is frozen for review: base dd reads the fixed
/root/handoff-probe/handoff.iso with bs=2048, skip=248200 and count=82825 into
one absent fixed owned /root/handoff-probe/handoff-tail. After sufficient free
space and media-budget observations, set -eu and umask 077 require successful dd,
regular nonsymlink source and exact root ownership/mode/size/full hash. Require
root-owned 0600 suffix size 169625600; publish its native sha256 before the complete
uuencode -m handoff-tail frame. Rehash the original ISO afterward, remove the fixed
scratch suffix and observe absence. Retain dd diagnostics. Scratch stays within
the original 3 GiB media bound. ShellCheck passed for the accepted fixed reader;
it has not executed. Freeze exact source/helper identities before the launch.
Assemble only in the
owned host fixture, require the original complete native ISO hash and complete
boot/catalog/member inventory readback before attachment. The current partial
frame and unsuccessful host reconstruction candidates are unusable. No host
rebuild may substitute for the independently identified native image.

Current aggregate encoded export is 728731015 bytes. Conservative current-plus-
suffix estimate, including overlap/framing reserve: 961749041, below the unchanged
1 GiB ceiling. Current owned allocation is 8581140480 bytes below 20 GiB; private
observation capture is 473800 below 10 MiB. Source remains 16344 additional original
C/header bytes; all component/combined caps remain enforced. Do not replay the
whole ISO or renew clocks. Freeze enough measured time for transfer, independent
postconditions, exact terminal restoration, unmount/sync and **native clean halt**.

All 92 installed cases, final artifact sealing and fixture disposal remain required
in the same feature draft. Native test success before the stop does not qualify
the failed shutdown. Exact-target authority has not ended; no general rollback
or physical erasure is claimed. This changes a frozen launch/export mechanism
under the plan's rule: "retries/changed budgets need review." Acceptance covers this fixed additional guest; dependent target installation still
requires complete media and recovery postconditions.


## Diagnostic build and replacement serial payload amendment — proposed

2026-10-14. The first installed dynamic entry refused its combined initial
preflight before review or writer dispatch, with zero installed passes. Native
OS/runtime/namespace/fixture/relink observations and clean halt are recorded in
[native progress](installed-payload-native-progress.md#installed-preflight-refusal-and-proposed-diagnostic-continuation).
The source now separates the same five initial requirements to identify the failing
call, without weakening any binding or confinement check. It passes host verification
and source caps; native compilation and qualification remain pending.

Concrete review request in draft PR #56: one additional build-diagnostic-refresh
cold boot of the same exact build.raw, then one replacement serial-payload cold
boot on the same exact serial-target.raw, preserving the four remaining original
serial-readback/VGA install/payload/readback phases. Increase the aggregate ceiling
from twelve to **fourteen**; eight launches are already consumed. All other
launch clocks, operation, capture, source, storage, export, CPU/memory, target,
privilege and confirmation rules remain unchanged. No new target, credential,
signing pair, dependency, network, service or production adoption is authorized.

Use the same pinned offline IDE compiler profile, existing build credential and
sealed read-only corrected source disc. Reobserve native key absence, exact source
and public fixture closure, compile only the six fixed installed variants, and
freeze full native binary/map/ABI identities before bounded public export. Preserve
the prior failed source/artifact records. The verified native handoff ISO is reused;
no full-ISO replay or rebuild. Current encoded export is 957874717 bytes; all new
frames still count within the original 1 GiB, with 115867107 bytes remaining. Every
new sealed source/runtime image requires full inventory/hash readback before its
cold attachment. No running media swap or deadline reset.

At review, unchanged exact file identities/modes/single links are held. Complete
latest clean build.raw SHA-256:
409f405f8458211d6d3fb2d85dee70fcda117fca828d3cdb2553e7c74634bdf7.
Complete latest clean serial-target.raw SHA-256:
5d86c4bfc030aae075cdd11df5ba6df24fd33cc43f7a95267e9a9c3ab73f0c5c.
VGA target retains the original full zero-image digest/zero allocation. Bind any
acceptance separately to the unchanged original target-receipt digest and fresh
complete target rechecks; preserve historical receipts.

The replacement serial boot must repeat independent OS/runtime/root/namespace and
completed-relink proofs before its fresh preflight. Its existing fixed owned parent/
canary are separately rechecked; each case is fresh and empty. Seventeen automatic and five manual cases per variant, plus one retained positive
and one later readback per console, remain required, without substituting
the failed attempt for a pass. If diagnostics show the frozen observation mechanism
cannot establish its binding, stop and review a concrete replacement before any
member dispatch. A diagnostic label is not proof and debug cannot bypass safety.

Clean native halts, complete post-halt hashes, reaping/handle closure and final exact
fixture disposal remain required. These are retries and an increased frozen launch
budget under the existing rule: "retries/changed budgets need review." Do not start
the additional compiler or replacement target guest before acceptance.
