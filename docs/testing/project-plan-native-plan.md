# Read-only planner: exact native qualification plan

Status: **Accepted, 2026-10-16.** Owner: Cam.
Cam replied “continue” after the explicit PR #72 exact-new-8-GiB/four-launch
approval question, including the documented disposable credential limitation.
Source/helper bindings remain unchanged. Launch 1 stopped before installation;
the [failure evidence and accepted five-launch continuation](project-plan-native-progress.md)
record Cam's subsequent explicit recovery approval. Launch 2 then stopped because
`uname` is unavailable in the stock ramdisk; the earlier keyboard diagnosis was
incorrect. The original four-launch plan below and five-launch recovery are historical;
Cam explicitly approved launches 3–6 after PR #74's recovery question. Launch 3 then
stopped because the reduced ramdisk verifier rejects checksum mode. The current
[accepted seven-launch continuation](project-plan-native-progress.md) uses native embedded-signature
verification followed by selected-file checksums; Cam explicitly approved its
RAM-only manifest namespace and narrow prerequisite-correction exception. Launch 4
completed first stock installation/relink, then stopped when full installed ps could
not obtain process information under the installer kernel. Cam explicitly approved
the existing remaining launches 5–7 and full installed-kernel process readback on
2026-10-18 without a reinstall
or extra launch. Launch 5 passed full-kernel prerequisites and began native compilation,
then its supervisor stopped QEMU at the fixed host RSS ceiling. Cam explicitly approved
launches 6–8 add an explicit smaller TCG cache, stock-preen recovery and conditional
partial-build replacement on 2026-10-18. The original failed state is preserved pending
the accepted inventory and conditional cleanup. Launch 6 then completed that recovery
and a fresh build, passed 2,889 serial checks, and stopped at the CPU-accounting gate.
Clean native shutdown and host closure passed. The current
[revised CPU-timer/source/media continuation](project-plan-native-progress.md#prepared-cpu-enforcement-fix-and-proposed-launches-79)
proceeded under Cam's standing instruction to continue routine bounded work without
new approval. All 5,832 revised serial/VGA runtime checks passed in launches 7–8.
The first positive trace failed at the production file-size limit; no partial
trace is accepted. The [proposed trace evidence amendment](project-plan-trace-amendment.md)
requires a major Test Design decision. No production acceptance budget is relaxed.
The original plan below is
historical for source candidate 375f6b6 and launches 1–6; its frozen argument preview
is overridden only by the explicit accepted cache/profile amendments and any later
accepted source-CD replacement, never by recreating its target.
Current result: [all 5,882 required native checks, independent readback and exact
guest cleanup passed](project-plan-native-progress.md#launch-11-independent-readback-and-exact-guest-cleanup).
Automatic approval review requires explicit user approval for exact host disposal;
the original host fixture/source/media/helpers remain intact and delivery is draft.
Implements the native gates of the [accepted planner design](../design/project-plan.md).
The maintainer merged implementation PR #71 before qualification; keep this entire
continuation, fixes, observations and teardown in one draft. The explicit exact-target approval supplies this authority; earlier research
authority remains ended. Source-only preparation preceded approval. Creation,
performed effects and observations are recorded separately in
[native progress](project-plan-native-progress.md).

## Concrete source and independent cases

The production confinement function is shared by the normal CLI and separate
native probes. Its checks/limits are unchanged. Production builds have no injected
calls, environment selectors or host bypass. Only separately linked test objects
interpose IO or confinement failures. Original runtime C/headers total 14,070 bytes,
below the accepted 24 KiB ceiling; the four native helper C files total below
40 KiB, with a separate 48 KiB helper ceiling.

`tests/native_cases.py` generates 1,401 independent synthetic definitions: 688
successes and 713 rejections, containing 359,319 input bytes. Expected validity and
complete text come from Python fixture construction and the independent oracle,
never from the C parser. Printable ASCII escapes, all field/byte bounds, all 576
root/child/project permutations, every truncated prefix, keys/types/duplicates,
controls/Unicode and a fixed 256-case mutation corpus are included.

| Frozen artifact | Bytes | SHA-256 |
|---|---:|---|
| Generated case header | 1573138 | e4c6f6d660ce87249c65fadbbb9218d15ea71c018c15e971d29b859e85d8c581 |
| Case manifest | 413701 | 4bdfa1b1e132da11027749d078b0e424b630d6bd9cb3f1e46eec1ec0804721ae |
| Source SHA256 listing | See linked listing | c58a21ff94aa3ead38d82da1691b5977fb87c23eb608c84209cd536a842e832b |

The [original complete source bindings](https://github.com/gonz0w/bootweave/blob/375f6b65ce7a8841a33c0bb2d0f4aaf94c1e6651/docs/testing/project-plan-native-bindings.txt) name all fifteen
source/data files; the sealed source-only bundle has sixteen files including that
listing, totaling 2,045,033 bytes. Its private owned 0700 staging directory and 0600
files already exist. It contains no ISO, target, credential or native executable.
Preparation tests reproduced the bundle and required exact equality to those bindings.
The current binding file now describes the separately prepared CPU-timer candidate
in the continuation proposal; it does not rewrite the transported launch-1–6 bytes.
The PR supplies clean candidate/base/tree, signed/DCO verification and local results;
documentation-only changes do not change these frozen transported bytes.

The portable rehearsal runs the supervisor against the private host driver under
both binary names; it does not establish native static linkage or confinement.
It covers 2,802 corpus executions, 30 descriptor/output cases, two trace-case
executions and four PTY console cases. Deliberately changed canaries/wrong output
must reject; a stalled child must be killed and reaped. The normal production host
binary still refuses planning with exit 77. See [host evidence](project-plan-host.md).

## Exact fresh target and requested effects

Request authorization to create and use **only**
`.tools/project-planner-native/build.raw` under the repository root. The entire
`.tools/project-planner-native` namespace was observed absent. Do not reuse an
existing directory/file, old research disk, clone, overlay or backing image.
After approval, exclusively create that 0700 directory and a regular 0600,
single-link, owned raw file with O_EXCL/O_NOFOLLOW; truncate to exactly
8,589,934,592 bytes and require zero allocated bytes. Its full initial zero-image
SHA-256 must be:

`ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25`.

Before any launch, independently read back the complete zero image and bind its
actual host device/inode, parent identity, owner/mode, single link, size/allocation,
timestamps and digest in an immutable private creation receipt. Approval applies
only if that exclusive creation and independent observation match; otherwise stop.
Keep this receipt separate from acceptance and later performed/observed state.
Each later boot must use that same file identity and full previous stopped digest.

Requested writes are stock OpenBSD installation, native compilation and trusted
qualification fixtures/logs on this one disposable disk. Engine code itself only
reads metadata and writes caller-selected output descriptors. Guest-owned paths
are `/planner` (sources, sixteen binaries, ten maps, two objects, test work and
receipts, and the fixed private compiler-tmp directory, required empty and removed
after building) and the explicitly owned CD mount point. The work directory and two
canaries are deliberately writable by nobody so invariance is not inferred merely
from filesystem permissions. Root-owned set-ID copies are test artifacts only.
The file-size probe may write exactly 8 KiB to its caller-selected owned output;
the supervisor independently reads it and removes that exact file.

Approve one new sealed, nonbootable, read-only `data.iso` under the same namespace,
assembled after approval from the frozen source bundle and six selected stock
files plus the release SHA256.sig. Extract only fixed reviewed ISO members using
the pinned host tool; independently verify every selected filename with explicit-key
host signify before assembly. Do not copy extra/X/game/site sets or install.site.
Freeze the full derived ISO digest, byte size and inventory, compare its extracted
source and release bytes independently, then make it read-only before attachment.
No host executable or Python runtime is transported into the guest.

Stock installation also needs one fresh disposable root credential, supplied only
at an observed echo-off prompt. This request explicitly includes the stock
installer's encrypt plaintext-argv limitation inside this isolated synthetic guest.
No production secret is involved. Keep it out of host argv/environment, normal
logs and public evidence; guard private input/captures and dispose them. No signing
keys, product credentials, additional user or package installation are needed.

## Frozen tools, attachments and stock installation

Use the previously reviewed local QEMU 11.1.1, pc-i440fx-11.1, qemu64-v1,
single-thread TCG, one CPU and 1024 MiB RAM. Current executable/BIOS/VGA hashes
match the [reviewed platform pins](../design/native-profile/preflight.md).
The [ADR 0002 verifier/key](../adr/0002-host-artifact-verifier.md) and
[release ISO/signature hashes](../design/native-profile/release-input-evidence.md)
also match; explicit-key host signify -C returned success for `install79.iso`.
The initial check passed a directory-prefixed selected name and returned FAIL;
repeating with the exact signed basename in the release directory passed. No input
bytes changed and no native action followed that host invocation error.
Pinned local xorriso 1.5.8.pl02 SHA-256:
`d217ca8e759699dcaf36953822bd508f13587c57a890d2c1a71b19f6d0e8da86`.
These are local research tools; existing provenance/redistribution limits remain.

`tests/native_plan.py` rehashes eight inputs and emits the fixed four argument
arrays. It cannot create media/targets, sockets or processes. Its private preflight
record says target_created=false, guest_started=false, write_authorized=false.
Recheck all pins, source bindings, target identity/hash, free space and exact
arguments before every approved launch. Full independent block/device/PCI/process/
open-handle/listener inspection must precede QMP cont, not just a partial tree.

The sole writable raw file is virtio-blk-pci at PCI 0x3. Pinned standard VGA is
PCI 0x2; ich9-ahci at PCI 0x4 holds only read-only optical media. Provisioning uses
stock install79.iso at AHCI port 0 and data.iso at port 1. Installed boots omit the
stock ISO and retain the same data.iso at port 1. AHCI is selected from the start;
there is no IDE-optical fallback or media swap while running. Start paused with
-S; raw format, no backing chain/snapshots, private Unix QMP/serial endpoints and
headless VGA with QMP keyboard/screendump. No NIC/IP listener, forwarding, sharing,
passthrough, hotplug/reopen or additional disk. Empty machine controllers are not
backed targets and must be identified as such in the full inventory.

The stopped stock-shell `signify -C` method is not executable with the reduced
ramdisk verifier. The corrected method above was explicitly accepted; the original
method below is historical. Independently verify the expected OpenBSD release/key and
all six selected files (`bsd`, `bsd.rd`, `base79.tgz`, `comp79.tgz`, `man79.tgz`,
`INSTALL.amd64`) against the native explicit-key signed manifest on the read-only
data CD. Unmount before the fixed manual installer. Confirm sole guest `sd0`,
exact 8 GiB capacity and host target identity again immediately before destructive
answers. Approve whole-disk unencrypted MBR, automatic FFS/swap layout and only the
stock kernel/base/compiler/man inputs. Use a synthetic `.example.invalid` hostname,
UTC, no interface/DNS, SSH/X disabled, no extra user or site script. Observe each
prompt; no autoinstall, guessed answer, verification bypass, reinstall or reset.
Stock boot activity is permitted only inside this no-NIC disposable guest.

## Four launches and required checks

Maximum **four launches**, each with a single nonrenewing sixty-minute host deadline.
Reach the intended native prompt within fifteen minutes. Stop scheduling new work
at elapsed forty-five minutes and reserve the final fifteen for observation,
unmount, sync/halt and closure. If a stage cannot fit, stop; do not reset the timer.

| Launch | Sole target and allowed stage |
|---|---|
| 1 | Fresh exact zero file: observed stock installation as above. Verify layout, installed account/compiler and completed kernel relinking. Clean native halt, reap, handle/mount closure and complete stopped target/media hashes. |
| 2 | Same stopped image: offline serial build and all suites; two independent syscall traces; four actual serial console invocations. Retain test input/canaries and private metadata/hash observations for later readback. Observe relink/test/compiler absence, unmount, sync/halt, reap and full stopped hashes. |
| 3 | Same stopped image and source CD: all suites on actual keyboard/VGA and four separate full-plan console invocations, each followed by a complete frame observation. Repeat retained-fixture observations. Clean native halt/closure/full hashes. No rebuild or replacement binary. |
| 4 | Same stopped image: independent base-tool readback of retained exact input, both canaries, source/artifact hashes, owner/mode/link/path inventory and recorded times; no planner/writer rerun. Observe exact native fixture removal and absence, unmount, completed relink, sync/halt and host closure. Then dispose this exact host fixture and end authority. |

From verified root-owned `/planner` source copies, fixed commands are:

```sh
sh /planner/native_build.sh
sh /planner/native_run.sh
cd /planner
ktrace -i -t c -f positive.trace ./native-check --tracecase
ktrace -i -t c -f rejection.trace ./native-check --edges
kdump -t c -f positive.trace
kdump -t c -f rejection.trace
./native-check --console80-dynamic
./native-check --console80-static
./native-check --console40-dynamic
./native-check --console40-static
```

Build once in launch 2 with base cc, C99/O2/Wall/Wextra/Werror/pedantic. Record
all sixteen executable digests, ten map digests, size/ldd and readelf static/dynamic
observations. The runner is statically linked with base libutil for test PTYs;
production only uses base libc. Native source/binary verification precedes each
suite and follows it. Compiler ceiling: 512 MiB measured RSS, CPU 120 seconds and
180 wall seconds per invocation. The separate native compiler guard enforces
these limits and records wait4 usage. Its own first compilation uses native shell
DATA/CPU/CORE limits and base time; the host monitor must independently enforce
its 180-second wall bound. A failed compiler or missing measurement stops the stage.

| Suite | Checks per console | Independent oracle |
|---|---:|---|
| Corpus | 2802 | All 1401 cases on each normal dynamic/static production binary; exact expected text or fixed bounded rejection with empty stdout. |
| Edges | 46 | Ten stdin/alias modes, broken/full outputs, blocked error output, termination, five CLI cases and real root/set-UID/set-GID refusals on both variants. No input read for disallowed types/privilege; help/version have exact fixed-text oracles. |
| IO/gate faults | 30 | Short/EINTR, changed stat, read/zero-write/EAGAIN, six limit failures, first/lock unveil, pledge and issetugid failures, separately interposed on both variants. |
| Same-policy probes | 26 | Path open/create, exec/fork, IPv4/IPv6 socket creation and unveil expansion denied; actual limits and closed extra FD, descriptor ceiling, memory/stack/CPU/file-size enforcement on both variants. No bind/connect or live traffic. |
| Pure API units | 2 | Native dynamic/static independent model tampering, failed parse preservation and output-cap checks. |
| Actual console | 4 | Each variant at 80x25 and 40x25 TTY geometry; unchanged attributes and restored geometry, full emitted text, actual serial or keyboard/VGA transport and per-command time/CPU/RSS. |

This is 2,910 checks per console, 5,820 across both. Launch 2 additionally repeats
two positive trace cases and 46 edges under tracing: **5,868 required checks**.
Expected negative statuses/signals are case outcomes; any oracle disagreement is
an unexpected failure. A fatal native resource/pledge signal is recorded as such,
not relabeled as an engine exit code. CPU probes accept the handler's exit 4 or
kernel SIGXCPU/SIGKILL; stack probes require SIGSEGV with no core.

Independently correlate trace child PIDs from receipts, verify successful production
exec and checked confinement before input, no later open/exec/fork/socket attempt,
and no read(0) for disallowed descriptors/root/set-ID. Account separately for the
loader and supervisor; canary invariance alone cannot prove absent attempted syscalls.
[Base tracing](https://man.openbsd.org/OpenBSD-7.9/ktrace.1) and
[kernel tracing permissions](https://man.openbsd.org/OpenBSD-7.9/ktrace.2) are evidence
tools, not runtime dependencies. Record denied-call outcomes against
[pledge](https://man.openbsd.org/OpenBSD-7.9/pledge.2) and
[unveil](https://man.openbsd.org/OpenBSD-7.9/unveil.2); do not claim whole-session
confinement or suppression of libc's documented internal exceptions.

The console helper emits one build's complete plan per command so its canonical
13 lines plus bounded metrics fit a 25-row frame, including wrapping at 40 columns.
Actual QEMU VGA remains its native hardware width: 40-column evidence is the TTY
geometry plus a narrow plain-text viewport of the complete captured native output,
not a claim of physical 40-column VGA hardware. No styling/control sequences,
clipping, paging or synthetic-only console success may replace actual transport.

## Resources, first failure and exact teardown

Keep production ceilings unchanged: input 16 KiB plus probe byte, model 2 KiB,
output 4 KiB, diagnostics 256 bytes, DATA/RSS 16 MiB, STACK 2 MiB, NOFILE 8, CORE 0,
FSIZE 8 KiB, CPU 2 seconds and a single five-second deadline including blocked IO.
Supervisor observations allow one native accounting tick (up to 0.1 CPU second)
without changing the kernel CPU limit. Every child has an independent seven-second
watchdog; suite scheduling stops at 900 seconds and a hard alarm exits at 910.
Supervisor DATA/RSS 64 MiB, STACK 8 MiB, NOFILE 64, CORE 0, FSIZE 8 MiB, CPU 180.
Measure actual parent/child usage. No resource increase or automatic continuation.

Guest test allocation at most 32 MiB, binary trace 4 MiB each, decoded trace/case
capture 1 MiB each, plan/error pipe capture 4 KiB/256 bytes (8 KiB only for the
separate FSIZE probe); source/data CD at most 1 GiB, owned host allocation 20 GiB, emulator measured host RSS 2 GiB,
host raw capture 10 MiB per launch. These require outer capture/allocation checks
in addition to the C harness. Captures, credentials and host device/inode inventories
remain private; publish only sanitized source-bound counts/resources/hashes/outcomes.

An unexpected prompt, identity/hash/inventory mismatch, missing denial, compilation/
oracle/resource/secret/capture failure or unavailable evidence stops dependent work.
Kill/reap only owned active helpers if needed; prioritize bounded native shutdown
inside the original deadline. Preserve failed state and report uncertainty. No
reinstall, reset, scope expansion, swapped source/media, extra launch or retry is
approved. Expected negative tests do not authorize further operations.

After independent launch-4 readback, remove only the observed owned `/planner`
tree and explicitly created mountpoint; prove absence natively. Preserve sanitized
facts outside it first. After an observed clean native halt, reap all owned guests/
clients, independently require no file handles/mounts/listeners, bind full final
target/media hashes to the original identities, then unlink only that exact disk,
derived ISO, owned staging/receipts/credential/input/raw captures/endpoints/helpers.
Prove absence of the exact namespace and prepared source-only directory. Retain
reused global verified tools/release inputs and sanitized evidence. Logical disposal
is not physical erasure or installation rollback. Native failure/cleanup uncertainty
keeps the draft unready; broader installer/hardware/service qualification stays open.
