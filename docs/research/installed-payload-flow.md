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


## Diagnostic build and replacement serial payload amendment — accepted

2026-10-14. Cam replied "continue" to the concrete two-boot/fourteen-launch
question, accepting this amendment. A separate private acceptance receipt binds
reviewed head b45127866cc2dbca1e4826a321208b18ec78030b, the immutable original
target receipt and fresh full target rechecks. PR #56 was maintainer-merged at
1720584704b511e95cb76cfff357458d2c0f6657; all remaining diagnostics, qualification
and disposal stay together on codex/installed-payload-diagnostics in one follow-up
draft. Historical proposal/source/artifact/target receipts remain unchanged.

The first installed dynamic entry refused its combined initial
preflight before review or writer dispatch, with zero installed passes. Native
OS/runtime/namespace/fixture/relink observations and clean halt are recorded in
[native progress](installed-payload-native-progress.md#installed-preflight-refusal-and-accepted-diagnostic-continuation).
The source now separates the same five initial requirements to identify the failing
call, without weakening any binding or confinement check. It passes host verification
and source caps; native compilation and qualification remain pending.

Accepted continuation: one additional build-diagnostic-refresh
cold boot of the same exact build.raw, then one replacement serial-payload cold
boot on the same exact serial-target.raw, preserving the four remaining original
serial-readback/VGA install/payload/readback phases. The aggregate ceiling increases
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
budget under the existing rule: "retries/changed budgets need review." Acceptance
covers these two fixed additional guests; dependent cases remain gated by complete
native preflight/postcondition proof.


Routine capture diagnostics further label the existing ordered requirement groups:
device/label, release/namespace, directory identity, filesystem binding, canonical
comparison/namespace stability and final label recheck. The same conditions and
call order still fail closed. Only fixed stage numbers and numeric errno/flags/
fsid are emitted on refusal; zero initialization prevents undefined diagnostic
fields. Errno may be retained after a failed comparison and is advisory. No path,
operation selector or untrusted string is introduced. Compact indentation in only
the new installed sources funds these diagnostics; original C/header is 16340
bytes, below 16 KiB. These diagnostic builds and the stopped serial execution are
now observed in the [native progress](installed-payload-native-progress.md#diagnostic-refresh-and-mounted-block-refusal-2026-10-14).

## Raw root-partition binding amendment — accepted, 2026-10-14

Status: **Accepted.** Cam replied "continue" to the concrete raw-partition binding
and two-added-boot/fourteen-to-sixteen question after reviewed head
c02a56073146e5fb9d9c6e8e500b3aa873bb8dd4. Separate private acceptance preserves
the immutable proposal receipt and rechecks all three complete targets/identities.
The two accepted diagnostic boots completed. Ten launches
are consumed, including previous stops. Signed native source was
ab71ee966019d4af6f8e09be7f2352b75d5457b4. The serial entry refused `open`, before
capture, member review or writer dispatch; zero installed cases passed.

Independent base `dd` with read-only input and count zero returned Device busy for
fixed /dev/sd0a, but succeeded for /dev/rsd0a without reading any bytes. Base
disklabel on that raw partition returned the same selected whole-disk identity,
geometry and partition types. This is diagnostic evidence, not a replacement gate
pass. Authenticated release member sys/kern/spec_vnops.c, 18202 bytes, SHA-256
6368d8c80e8643ee356568b2e152e268207df4a63cfc6187d8024b55f0d315d0,
was read from the already verified sys.tar.gz recorded above. Its spec_open VBLK
branch checks vfs_mountedon unconditionally, before device open; the corresponding
VCHR mounted-device restriction applies to writing. No upstream code executed.
Opening the mounted block root read-only cannot satisfy the frozen mechanism.

Accepted replacement is restricted to the same OpenBSD 7.9 amd64, sole-sd0,
8 GiB, no-hotplug virtual research profile:

1. Hold /, /dev/rsd0c and /dev/rsd0a with the existing read-only/no-follow flags;
   both device opens retain nonblocking mode. Require character major 13, minors
   two and zero respectively, root ownership and no group/other write permission.
   The authenticated amd64 character-to-block mapping makes the root raw partition
   correspond to block major 4/minor zero. Rename the private block descriptor
   field to root-partition descriptor; close all three held references on refusal.
2. Independently lstat fixed /dev/sd0a with no symlink traversal, before and after
   every capture. Require a block node, major 4/minor zero, root ownership, no
   group/other write permission, root-filesystem st_dev and a nonzero inode.
   Capture and explicitly compare its inode, complete mode, group, link count and
   flags across those observations and each authorization boundary. These are
   named-node metadata observations; no held block descriptor is claimed.
3. Keep held root/parent/case ownership, modes, identities and named/held matching.
   Root st_dev and FFS fsid[0] must equal block device 1024. Preserve every mount
   owner/flag/time/fsid/type/name/spec comparison, including root mount / and
   /dev/sd0a, and the independently observed DUID.a alternative for mount spec.
   Both held raw descriptors must independently return the same complete validated
   disklabel geometry, partition types, flags/checksum and DUID; partition a remains
   nonempty BSD FFS. Repeat both labels and the entire raw namespace at capture end.
4. Bind the added named-node fields and root-partition descriptor in explicit
   canonical equality and the fixed numeric installed-root prelude. Preserve the
   1024-byte prelude and 16 KiB total review output. Reobserve the complete binding
   at all existing review/dispatch/post-observation boundaries. Missing, changed or
   unsupported observations refuse before writer dispatch; diagnostics waive none.

The held root filesystem and raw partition provide the continuing references;
named block metadata is an additional correlation check. This does not establish
an atomically held block vnode, arbitrary physical-media stability or support for
hotplug/concurrent privileged device replacement. Host attachment/no-hotplug proof
remains mandatory. No unmount/remount, privilege increase, device write, bootstrap,
new dependency, production interface or arbitrary path selection is proposed.

Test Design retains **17 automatic and five actual console cases per dynamic/static
variant, plus static KEEP and later READBACK per console: all 92 installed cases**.
The nine native binding substitutions retain their meaning; the former block-FD
substitution now changes the held raw root-partition descriptor. Extend pure
IPF-T01 field mutations to every added named-node field, including zero inode and
unsafe node attributes. IPF-T03 independently observes both raw node types/numbers,
named block metadata, full matching labels, mount/held filesystem association and
the before/after canonical binding on each console. Neither the zero-count probe
nor synthetic host substitutions qualify native positive binding. Initial capture
must pass before any automatic case; subsequent failures stop dependent work.
The 976 historical build regressions remain recorded. Freeze source/media/helper
identities, compile all six revised variants and verify full native artifact/maps/
ABI readback before installed execution. The 16 KiB original C/header and existing
component/generated-header ceilings still apply; inability to fit requires review.

Accepted execution amendment: one **build-raw-root-refresh** cold boot of the same
build.raw and one **serial-raw-root-retry** cold boot of the same serial target;
increase the aggregate launch ceiling **fourteen to sixteen**. Ten consumed plus
these two added boots plus the four original remaining serial-readback/VGA-install/
VGA-payload/VGA-readback phases fit sixteen. Reuse existing credentials, inert
payload/public signatures and fully verified native handoff ISO; no stock bootstrap
reinstallation, full-ISO replay, signing pair or credential regeneration. Use new
sealed read-only source/runtime discs with full inventory/hash readback before
cold attachment. Every other original clock, operation, source, capture, storage,
export, CPU/memory, confinement and exact disk/member confirmation limit remains.
Current encoded export is 964990122 bytes; 108751702 remain within 1 GiB.

The same three regular 8 GiB, 0600/single-link targets retain original exact host
identities bound by immutable receipt SHA-256
56f416bbdf7f25ac96bb610d1c2b81f1f7a4db4aee9abdb68bdcacf90e2dac5f.
Latest complete clean post-halt build.raw SHA-256:
ea2eb3b28d6967d42855747901fc9a84b6781dcdfebefe64ac5dcb77d18123ec.
Latest complete clean post-halt serial-target.raw SHA-256:
b283bd4c9960e250f6c6c51cad53af46f6da5c2056531b07b138c17e6dd7a408.
VGA remains the original untouched full-zero digest and zero allocation. Before
acceptance/execution, independently rehash all three and compare original identity,
mode/links; preserve prior receipts and bind acceptance separately to this proposal.
That full three-target recheck passed, with no fixture handles or mounts. Immutable
private proposal receipt SHA-256:
2b337ea148062e1e0da34d423fc48f385bae9ff23bd98265e262d81b9db3d2c6.
Owned allocation was 11902095360 bytes below 20 GiB, observation capture 510335
bytes below 10 MiB; export totals above were independently recounted.
Clean native halt/reaping/full hash/handle closure and eventual exact fixture
disposal remain required. All fixes, qualification and disposal stay in
[PR #57](https://github.com/gonz0w/bootweave/pull/57) was maintainer-merged before
implementation/qualification. Its merged main tree matches the reviewed tree;
remaining implementation, qualification and disposal stay in one follow-up draft
on codex/installed-root-binding. Acceptance covers this concrete replacement and
two added guests; every other original review/stop boundary remains.

Implementation holds the raw root partition read-only, captures/rechecks all five
named-block metadata fields and compares them explicitly at existing boundaries.
Host pure tests mutate each new field and reject zero inode/link count, wrong node
type and group/other writable mode. Shorter private C identifiers fund these fields
without removing a guard: additional original C/header is 16369 bytes, within 16 KiB.
All installed modes pass host syntax checks against authenticated native headers.
Native compilation and all 46 serial cases, including positive binding and later
readback, now passed; all 46 VGA cases and final disposal remain required.

## VGA confirmation observer retry — accepted, 2026-10-14

Status: **Accepted.** Cam replied "continue" to the concrete replacement-boot and
seventeen-launch proposal, reviewed at 78c58f826455d94025f258d1b4a7dc4f0548f11a.
Fourteen launches are consumed; the accepted aggregate ceiling is seventeen.
The accepted raw-partition implementation built natively and all 46 installed
serial cases passed. The original VGA install launch stopped before any frontend
input, disk confirmation or stock install: its observer attempted ASCII encoding
of an undecoded boot glyph. Native handoff subsequently timed out at initial review.
Complete zero write counters, native RAMDISK sync/halt, reaping/handle closure,
original full zero hash and zero allocation independently prove that target unwritten.

The concrete host observer fix preserves decoder unknown glyphs as UTF-8 text;
it does not guess their meaning or repair a garbled safety field. The public
tools/check-native-prompt.py guard bounds a screen to 25 rows/80 cells, permits
only printable ASCII/decoder unknown markers and rejects native handoff refusal
before matching any prompt. Each input page requires its complete literal marker
to be the current active line, after only trailing cursor/space normalization;
stale review text above a shell prompt cannot authorize input. Confirmation-result
observation may follow the transition to a stock question, with native refusal
still rejected. Capacity remains exactly 8 GiB/512-byte sectors, and the displayed
DUID must still match sixteen lowercase hexadecimal characters. It is validated
typed confirmation data, never shell text. Host tests cover unknown boot text,
expired/stale and wrong prompts, garbled prompt/control text, bounds and result
transition. Native positive confirmation remains unqualified on VGA.

Accepted retry: one **vga-install-retry** cold boot on the same untouched exact
VGA target; increase aggregate launch ceiling **sixteen to seventeen**. Fourteen
consumed plus this replacement install and the two original VGA payload/readback
boots fit seventeen. Do not repeat within the stopped guest or renew any clock.
Reuse the original fully verified handoff ISO, revised verified runtime disc,
existing target credentials and inert signed fixture. No compiler boot, rebuild,
full-ISO replay, key pair, credential, dependency, network or service is added.
Every original per-launch, input/operation/confirmation, source, capture, storage,
export, CPU/memory and privilege bound remains. Stop/reap the general observer
before dedicated halt observation, avoiding the recorded closed-socket race.

Repeat full original target identity/mode/single-link/zero-allocation/hash checks,
paused complete attachments and zero write counters. Wait for supported actual
text geometry, freeze corrected observer/helper identities, and supply both effect
review pages and fresh exact disk confirmation from the observed current prompts.
Host identity/full zero hash/attachments must again match immediately before stock
whole-disk input. Preserve independent native release/set verification, stock
completion, clean halt/cold OS boot, all 46 VGA payload/refusal/readback cases and
final exact fixture disposal. Any unexpected observation stops dependent work.

Immutable original target receipt SHA-256 remains
56f416bbdf7f25ac96bb610d1c2b81f1f7a4db4aee9abdb68bdcacf90e2dac5f.
Fresh complete build image SHA-256:
6c5fc4f5a2d07d4067c35f1c77de2ef58c44691d33fe9b422e2ec85bc95dc592.
Fresh complete final serial image SHA-256:
2964e99af1a1eedbd1bda1dca5853a5c6744b427a219d1d10ea3b9de4a7c0021.
VGA remains the original full-zero digest, exact identity and zero allocation.
All three full rechecks and no fixture handles/mounts passed. Separate immutable
private retry proposal receipt SHA-256:
345aa6e51c4c617e655b44a6cebc4a00ed1cba18f0eb54e6bfcc1811ca7db8ff.
Owned allocation is 11928199168 bytes below 20 GiB; captures 548805 below 10 MiB;
encoded exports 972121651 below 1 GiB, with 101620173 bytes remaining. A separate
acceptance receipt SHA-256
d3260c65f2f855c65a8381ae55b9292081611af843e6d4b7f856ebe39ac16042
binds the reviewed candidate, original receipt and fresh complete target rechecks;
all prior evidence is preserved. PR #58 was maintainer-merged
before this continuation; its main tree equals the reviewed tree. Keep remaining
implementation/fixes/qualification/disposal together on codex/installed-vga-qualification
in [draft PR #59](https://github.com/gonz0w/bootweave/pull/59). This accepts only the
concrete retry and changed launch budget under the existing explicit review rule.

## Diskless diagnosis after VGA stock panic — accepted, 2026-10-14

Status: **Accepted for two diskless read-only diagnostic boots only.** Cam replied
"continue" to the concrete proposal at reviewed head
ca282f747858b91351ad011bd3dbe70cffd9105e. No repair or reinstall is authorized. The accepted
retry passed VGA confirmation and independent native signature/input verification.
Stock extraction subsequently panicked in the IDE path; full stopped evidence is in
[native progress](installed-payload-native-progress.md#vga-confirmation-passed-stock-extraction-panic-2026-10-14).
The original stop rule applies: "Failed postconditions stop dependent work; no
automatic retry, resume, repair or general rollback." The incomplete VGA target is
quarantined, with no later OS/payload boot. Fifteen launches are consumed; all 46
VGA cases remain pending. Root cause is unproven.

Acceptance repurposes the **two remaining launches within seventeen** for a bounded
read-only comparison: one diskless boot from the original authenticated stock
install79.iso, and one from the unchanged fully verified handoff.iso. Both use the
same pinned QEMU/firmware, pc-i440fx-11.1/TCG/qemu64-v1, one CPU, 1 GiB RAM,
keyboard/VGA and private serial observation. Each attaches only its single read-only
CD: none of the three target images, no other block backing, network, sharing,
hotplug or floppy backing. Complete paused effective inventory must prove that
absence before execution. The reviewed purpose and attachment change applies only to these two boots.
Remaining launch count is not permission to perform another experiment.

Observe the stock shell through its actual prompt; on handoff media, absence of an
eligible disk must cause the unchanged frontend to refuse before returning to its
shell. Do not bypass a refused confirmation or run /install. Mount only fixed cd0c
read-only. Independently verify the explicit native release key, embedded signature,
manifest and six frozen full input hashes. List the same authenticated base79.tgz
through native tar to /dev/null, without extraction, at most twice per boot. Only
bounded verification scratch in ephemeral RAMdisk /tmp is allowed; remove it before
unmount/sync/halt. No target filesystem, destination directory or payload operation
exists in this experiment. Record actual output/exit and resource outcomes. A panic,
unsupported prompt, hash mismatch or other unexpected observation stops dependent
work; no repetition beyond the two listed passes or automatic replacement boot.

Keep the original fifteen-minute prompt, forty-five-minute scheduling and sixty-minute
hard clocks from launch, all existing capture/storage/export and CPU/memory/input
bounds, credential exclusion and no hosted checks. No compilation, new dependency,
ISO rebuild/export, signing pair or credential is authorized. Reuse existing media:
stock SHA-256 7a4a92e953618035097c796a90b54424a0f3ae775552e1e7d102cf8a5130449f;
handoff SHA-256 607f1cc73a90402e5a02889b1bdc79c29dc8f67176086be38beffe218422f624.
Their complete fresh hashes match. Freeze exact helpers/arguments before any accepted
execution. Observe native halt when possible, honestly record any panic/unclean exit,
reap all owned processes and prove closure. Preserve all three original target
identities and stopped hashes; no guest target attachment/write/reset/disposal in
this diagnosis. Host integrity hashing remains read-only.

The comparison can show whether a read-only optical workload reproduces this failure.
Media boot/RAMdisk/layout differences and lack of concurrent FFS writes limit what a
non-reproduction proves. Do not claim isolation, general stability or qualification
from two successful reads. Report evidence before proposing any installation retry,
profile replacement or fix. This acceptance authorizes none of those later actions and
does not complete the 92-case deliverable or end exact-target/disposal authority.
Keep diagnosis, subsequent decisions and eventual qualification/cleanup together in
[draft PR #59](https://github.com/gonz0w/bootweave/pull/59).

Immutable private proposal receipt SHA-256:
a2d8dbfde9aeb7dc3684778be7983e29aff0db38609e1a740a81a8932805457f.
It binds the completed stop audit and original target receipt, quarantined VGA hash,
exact existing media, bounded operations, unchanged launch ceiling and no-target rule.
Separate acceptance receipt SHA-256:
7b5cb8328a764df9631b5fc01c1151cdffd606a6917a3163a293d5e324687007.
It binds the reviewed candidate, fresh full hashes and original identities of all
three stopped targets, both media hashes and absent handles/mounts. The proposal
remains unchanged. Exact helper/argument/host-pin freeze SHA-256:
0b4572b43ed1a4bed29738eabbcf8b670d44aade8f736396c060019076bcbd17.
PR #59 was maintainer-merged; its tree equals the reviewed tree. Keep remaining
diagnosis, decisions, qualification and disposal together in one follow-up draft on
codex/installed-optical-diagnosis.


## AHCI optical complete VGA qualification — accepted, 2026-10-15

Status: **Accepted, 2026-10-15.** Cam replied "continue" to the concrete four-boot
AHCI qualification, ceiling twenty-one and same-original-VGA reset proposal at
reviewed head 2fadb6320bab81a278997c4394805e8595bf46d5. Reset authority remains
conditional on the positive diskless preflight, clean halt and closure below.
The accepted two-boot diagnosis is complete as an attempted comparison: stock input
verification stopped on a missing CD signature file, while fully verified handoff
media reproduced an ATAPI not-ready/guest-exit failure during read-only listing, with
no target attached. See [diagnostic evidence](installed-payload-native-progress.md#diskless-optical-diagnosis-2026-10-14).
Root cause remains unproven, and the accepted seventeen launches are exhausted.

Acceptance covers four fixed boots, increasing the ceiling **seventeen to twenty-one**, as a
complete remaining VGA qualification attempt. Change only the optical controller
path to the pinned emulator's existing ich9-ahci device, id optical, with the same
ide-cd backend attached to optical.0. Do not attach CD media to chipset IDE. Preserve
pc-i440fx-11.1/TCG/qemu64-v1, one CPU, 1 GiB RAM, BIOS/VGA firmware, no network,
sharing, hotplug or overlays. Where a target is present, retain its exact sole
legacy virtio sd0 path and the same fixed raw-root observer. No C, kernel, runtime
interface, dependency, compiler, ISO, export, signing pair or credential changes.
This tests another virtual optical path; it does not fix or identify the old failure.

The pinned emulator's help lists ich9-ahci. The authenticated RAMDISK_CD configuration
includes ahci at pci; bounded config-member SHA-256 is
7b9c3b64e95725518a92992b9ecd57f21f8adb710dc0e4e20c768096bde4a1aa.
Frozen handoff-kernel symbols contain ahci_attach and ahci_pci_attach. These are
availability evidence only; BIOS boot, native attachment and operation remain
unqualified until observed. No silent controller fallback is allowed.

| Order | Fixed stage | Required gate/result |
|---|---|---|
| 1 | diag-ahci-optical | Diskless, sole read-only handoff CD. Prove paused AHCI attachment/no IDE CD/no target; actual native AHCI/cd0 association and diskless frontend refusal. Native explicit-key signature, manifest/six-input verification and two listing-only tar passes must pass, followed by clean native halt, closure and complete target/media integrity checks. |
| 2 | vga-ahci-install | Only after step 1 passes, reset the same exact quarantined VGA file as below; fresh full-zero proof and paused sole virtio target/read-only AHCI CD proof. Supply actual effect pages and fresh exact disk confirmation; manually supervise unchanged stock /install and independently prove all stock completion/relink/configuration postconditions before clean halt. |
| 3 | vga-ahci-payload | Cold boot that fully hashed installed image with the same read-only revised runtime CD on AHCI. Prove stock boot/relink, actual optical/root/label/namespace/held context and complete media hashes. Pass 17 automatic and five actual manual cases per dynamic/static variant, plus KEEP: 45 VGA cases, exact TTY and independent retained-content/canary checks. Clean halt/closure/full image hash. |
| 4 | vga-ahci-readback | Another cold boot of that same hashed image/runtime CD/profile. Independently prove unchanged retained inventory/content/metadata/root association; READBACK adds case 46 without a writer or replacement authorization. Clean halt/closure/full hashes, then original exact fixture disposal and authority closure. |

Accepted reset authority is only for the **same original vga-target.raw identity**
bound by immutable original target receipt SHA-256
56f416bbdf7f25ac96bb610d1c2b81f1f7a4db4aee9abdb68bdcacf90e2dac5f,
currently complete SHA-256
fd79de7a50ad70adfcc7f6fd5eeaa694fc9c5dc961396535085e54090d85a438.
Keep it quarantined until step 1 passes and all owned processes/handles/mounts close.
Recheck full stopped hash, original device/inode, regular type, 8 GiB size, 0600 mode,
single link and named/held agreement using a no-follow held host descriptor. On
that descriptor only, truncate to zero then exactly 8 GiB, fsync, and independently
verify original identity/mode/link, zero allocation and complete zero-image SHA-256
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25.
No unlink, replacement image, changed path, physical disk or other target write.
Preserve pre-reset receipts/captures; reset discards partial bytes and is not rollback.
Host reset does not supply native install confirmation. Any reset failure stops,
without retry, and OS/payload stages cannot run before their independent gates.

Reuse existing vga credential only at observed echo-off prompts; no extra account,
SSH, X, DNS or networking. The same verified handoff/runtime discs and inert signed
fixture are frozen. Both media full hashes, all three stopped target full hashes and
original identities, and absent handles/mounts are freshly rechecked in preparation.
Build and serial images remain unchanged until original exact disposal. Existing 46
serial passes belong to the earlier IDE optical profile; do not claim AHCI serial
qualification or general hardware equivalence from them. Report VGA AHCI evidence
separately, with no production profile/runtime adoption.

All existing per-launch clocks, source/input/operation/review/output/resource,
capture/storage/export, CPU/memory, privilege and exact confirmation limits remain.
Freeze exact helpers/arguments before acceptance/execution. Unsupported boot/driver,
input mismatch, native failure, incomplete postcondition or deadline stops dependent
work; no retry, BIOS/controller fallback, boot substitution or ceiling extension.
Keep all implementation, tests, fixes, native qualification and all-three-fixture
disposal on codex/installed-ahci-qualification in one continuation draft after
maintainer merges of PR #59/#60; no phase PRs. PR #60 was merged before this
continuation; its tree matches the reviewed candidate. Remaining qualification and
disposal continue together on codex/installed-ahci-qualification. Separate immutable
acceptance binds the reviewed candidate/proposal to fresh full target/media hashes,
original identities, closure and frozen helpers. No other guest or reset is granted.

Immutable private proposal receipt SHA-256: f5b0c4d85d24e6bde4e1c91cb35f3b1a4e00a57a9d8d74bfecb0d914bfb2bd47.
It includes the four exact argument arrays and both completed diagnostic receipt
hashes, unchanged full target/media hashes, the exact single-file reset and all gates.

Initial exact helper/argument/host-pin freeze SHA-256:
0f80bfeab360ebb3f0059cdeee8b1cd0e0150ad6dee912e8f0c7dbd019eb0bb0.

Acceptance receipt SHA-256:
b4dbfd47df866412e577b6d2c342513968724ee58af4acb3c0b37e0e38cd85d3.
All three complete stopped hashes/original identities, both media and closure passed
again before this receipt; no image reset preceded the preflight.


## Corrected stock AHCI read-only control — accepted, 2026-10-15

Status: **Accepted by Cam’s continue on 2026-10-15 for one diskless control only;
no target reset authorized.** The accepted
AHCI preflight established one successful listing, then its second pass remained
incomplete and was aborted for scheduling cleanup. It cleanly halted before forty-five
minutes. All three target hashes/identities remain unchanged. The reset gate is
false and the three conditional VGA stages were not started. Eighteen launches are
consumed under twenty-one. See [actual evidence](installed-payload-native-progress.md#ahci-preflight-incomplete-targets-unchanged-2026-10-15).
Do not guess another controller fix or treat an archive-sized read count as completion.

Accept **one diag-stock-ahci-control** diskless boot, repurposing one remaining
conditional VGA slot within the **unchanged twenty-one ceiling**. Use the original
unmodified authenticated install79.iso, same pinned QEMU/firmware/machine, TCG CPU,
one CPU/1 GiB RAM, AHCI optical.0, VGA/keyboard/private serial observation and no
target, network, sharing, hotplug, overlay or floppy backing. It does not install,
reset, run payloads, add media/input transport, compile, regenerate keys/credentials
or authorize any further boot. Freeze exact helpers and the prepared argument array
before accepted execution. Complete paused effective inventory and native AHCI/cd0
association are required, then choose only the actual stock shell prompt.

Correct the unavailable-input assumption explicitly for this read-only control:

1. Before launch, the existing reviewed host signify and fixed explicit release key
   must authenticate the complete unchanged stock ISO and release manifest. Full ISO
   SHA-256 remains 7a4a92e953618035097c796a90b54424a0f3ae775552e1e7d102cf8a5130449f;
   public-key SHA-256 is b7ee8e7981549a1f707a4ed05115a99959a5f9e34b9bed9f9a098be17dd7e4b1.
   Offline authenticated inventory confirms bsd, bsd.rd, base79.tgz, comp79.tgz,
   man79.tgz and INSTALL.amd64 present at the fixed sets path, with SHA256.sig absent.
   Authenticated manifest body SHA-256 remains
   50bec66f28426a22b9c9436f6a87cf3e7029e636bb915ba1d0db80d638881b87.
2. In the guest, mount only cd0c read-only. Hash the existing native public key and
   all six fixed input files; compare every full digest independently to the frozen
   authenticated host expectations before either tar pass. Mismatch stops the test.
3. Do not invoke or claim native embedded-signature verification when its input is
   absent. Do not ignore a signature warning, replace a key, inject a manifest or
   create verification scratch. Record host ISO/release authentication separately
   from native key/input hash agreement. This location change is accepted explicitly
   for this non-writing control, not a fallback or production trust-policy adoption.

Then run at most two fixed listing-only tar passes to /dev/null, without extraction.
Each requires its actual native zero return and fresh shell before the next step.
Stop dependent work on unexpected output or incomplete observations; no retry or
substituted input. An independent hash gate and one submission per pass remain.
The result can compare stock behavior with the handoff attempt, but media layout,
RAMdisk contents and host observation conditions limit isolation. Do not infer a
root cause, stable profile or installer qualification from a successful control.

Use one owned screen reader, with rapid bounded boot capture for native attachment
and at most one screen read per five seconds during unattended work; take a fresh
actual prompt observation before input. Avoid overlapping dumps/readers. Preserve
observation gaps honestly; a recoverable read-only host reobservation sends no
additional workload or confirmation. Put the already documented temporary host-awake
and default scheduling requests in place before CPU execution, scoped only to the
owned PID/lifetime, with no persistent setting or virtual resource increase. Their
prior state/cause remains unproven and changed observation conditions are reported.

Keep original launch-clock prompt fifteen minutes, scheduling forty-five minutes
including two-minute cleanup reserve, hard sixty minutes and every original source,
input/output/capture/storage/export, CPU/memory and privilege bound. No clock renewal.
If a pass remains incomplete, abort only that read-only process for cleanup; count
no pass from the abort. Observe CD unmount/sync/native halt when possible before
emulator exit; otherwise record the unclean stop. Reap all owned processes, prove
closure, exclude credentials from captures and recheck full media/all-three-target
hashes and original target identities/modes/links. No target is opened by the guest.

A successful stock control **does not unlock the failed handoff preflight**, approve
VGA reset, replace its required two passes or complete the 92-case feature. Two
remaining slots would still not authorize the blocked three-stage VGA path or a
ceiling extension. Return evidence to review before a specific further test/fix;
keep this control, diagnosis, qualification and eventual original exact disposal in
one continuing draft on codex/installed-stock-ahci-control after maintainer merge of
PR #61 from the identical reviewed tree. No phase PRs.

Fresh all-three-target hashes/original identities, absent handles/mounts, authenticated
actual ISO inventory, native expected digests and one exact proposed no-target
argument array are bound by private proposal SHA-256:
d6b7078e8bb4609268b63d3d2d0e1a74c7b80686240d5c0b46e729c905110787.
Acceptance must separately bind the reviewed candidate/proposal and fresh integrity/
closure checks. The earlier immutable receipts and failed/incomplete results remain.

Cam accepted the concrete control at reviewed head
30f314ca6b4086e3e261ef7a6909546046b6837c. Maintainer merge
96eeed0fd7d64878ed964d44f3efc1294dbcfc7c has the identical tree
d356473682e4f6bdc5a70b5b308f8c6fe2ac789d. Separate acceptance receipt
befcf847a405c22abaecbd8fb3784e436fb2e2379a453b8d76f7909853ff73cb
binds fresh original target identities/full hashes, whole stock ISO/release
authentication and closure. Exact helper/argument/host-pin freeze:
700398a6326e9c1f570e7ed6e2b3938597ed077dff252ea5b23d9d8f2cc232a8.
The failed handoff preflight gate remains false regardless of this control outcome.

## Stock control with corrected QMP cleanup — proposed, 2026-10-15

Status: **Proposed; no further boot or target reset authorized.** The separately
accepted stock control passed native key/six-input hash agreement and one listing,
then lost observation during its second listing and required forced host closure.
Its [actual evidence and host cleanup regression](installed-payload-native-progress.md#stock-ahci-control-stopped-host-cleanup-fix-2026-10-15)
remain failed/incomplete. Nineteen of twenty-one launches are consumed. The corrected
host helper has two synthetic lifetime cases, without native recovery evidence.

Propose exactly one diag-stock-ahci-recovery-control diskless boot, consuming launch
twenty under the unchanged twenty-one ceiling. Keep the original stock ISO, explicit
host ISO/release authentication, native key/six-input agreement, AHCI/Cocoa/QEMU/
firmware, CPU/RAM, fixed keyboard/serial observation and all no-target/network/
sharing/hotplug rules unchanged. No fallback controller/display, native source,
compiler, dependency, key/credential, media or production policy change. List the
same base archive at most twice, one submission per pass, no extraction. Require
actual zero return and fresh shell before a next pass. Native signature remains
unavailable and is neither invoked nor claimed.

Use the separate exception-safe QMP helper, preserving original frozen helpers and
receipts. Freeze exact driver/helpers, arguments and pins before accepted execution.
Retain one screen reader, five-second unattended cadence and fresh prompt guard.
At the first timeout stop dependent native work. Allow at most three read-only
recovery observations, separated by at least five seconds, without resubmitting a
native job, confirmation or renewing clocks. A single bounded one-second owned-PID
host thread sample, capped at 1 MiB and retained privately, may precede cleanup to
preserve evidence otherwise lost at process exit. It cannot write guest state or
establish a root cause by itself. Do not take an unbounded sample or attach a debugger.

If read-only observation recovers, allow only the existing cleanup: one Ctrl-C for
a submitted read-only process if necessary, then fresh actual shell before fixed CD
unmount/sync/halt. Observe native halt before QMP quit; otherwise record a bounded
unclean stop. Keep original prompt 900-second, scheduling 2700-second including
120-second cleanup reserve and hard 3600-second launch clocks, every other bound
and pre-CPU owned-lifetime host-awake/default scheduling requests. No retries or
clock resets. Reap all owned processes and recheck complete stock ISO/all-three-target
hashes, original identities/modes/links, closure and credential-excluded captures.

Private proposal SHA-256:
1f057a3d8c4e5509d5c9efd957e7757395d363e51202af58567354aca89ed68d.
It binds the failed control audit, two-case host regression, corrected helper,
original target receipt/latest audited hashes and one exact no-target argument array.
Acceptance must separately bind the reviewed candidate and fresh full integrity/
closure checks. Even success does not unlock the failed handoff preflight, VGA reset
or any installed case. One remaining slot cannot authorize the three VGA stages;
any further qualification/budget decision returns to this same feature draft.
