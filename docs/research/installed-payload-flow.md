# Confirmed installation to installed payload: complete research slice

Status: Proposed, 2026-10-13. Owner and acceptance authority: Cam.
Implementation and native effects await explicit acceptance below.
Base: bc989d022f97d25fda9702c97b531aa51da12cf2, maintainer-merged PR #51.
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

Keep scope, implementation, routine corrections, host tests, native qualification,
cleanup and evidence on codex/installed-payload-flow in one draft PR. Acceptance
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

Freeze the complete new per-build case list/counts with implementation before native
execution; no missing or stopped case is silently counted as passing. Dynamic and
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
