# Read-only planner native progress

Status: **Stopped during launch 5 native build, 2026-10-18.** Full installed-kernel
prerequisites passed. Bootstrap and fourteen guarded compiler calls completed, then
the emulator exceeded its fixed host RSS ceiling during the final harness build.
No completed native build or planner check is claimed. All **5,868 required native
checks remain unrun**. Native sync/halt was not observed; filesystem consistency is
unverified after supervisor termination. Original partial state is preserved.
Original scope and unchanged limits: [native run plan](project-plan-native-plan.md).
The original exact target, media and private failure evidence are preserved.

## Authority and frozen fixtures

Cam's explicit “continue” replies accepted the concrete target/run questions below.
Each continuation counted prior attempts; none authorized a reset or replacement.
The maintainer merged each draft before its execution. Remaining qualification and
teardown accumulate on one continuation branch; PR #74's review notes link its current
evidence and proposal. A further PR will cover that remaining work together when
qualification is ready.

| Review | Accepted ceiling | Outcome |
|---|---:|---|
| PR #72 | 4 | Launch 1 stopped before installation. |
| PR #73 | 5 | Launch 2 stopped; the keyboard diagnosis was corrected. |
| PR #74 | 6 | Launch 3 stopped on reduced-verifier usage. |
| PR #74 continuation review | 7 | Cam explicitly approved launches 4–7, the RAM manifest and bounded correction. Launch 4 completed stock installation, then stopped at process readback. |
| PR #74 remaining-launch review | 7, unchanged | Cam accepted launches 5–7/full-kernel process readback on 2026-10-18. Launch 5 stopped at the host RSS ceiling during the native build. |
| Current proposal | 8, unapproved | Smaller explicit emulator cache; bounded recovery and partial-build replacement on the original disk, then complete launches 6–8. |

Separate exclusive private acceptance receipts bind each accepted run to the original
host identity/digest, previous failure receipt and reviewed controller/input helper.
Immutable creation receipts were not rewritten as approval or success records.
Earlier research target authority remains ended.

The sole target remains the exclusively created, owned 0600 single-link
`.tools/project-planner-native/build.raw`, exactly 8,589,934,592 bytes under its owned
0700 namespace. Independent complete readbacks before installation and after the
first three
halts matched its original zero digest:

`ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25`.

Through those first three closures, target/parent identities, owner/modes, single
link, size, timestamps and zero allocation remained unchanged. Private device/inode
inventories remain outside Git.
No target recreation, reset, zeroing, clone or overlay was performed. No installer
disk answer was sent in launches 1–3. Launch 4 used only that same target; its
approved installation changed its content/allocation/timestamps. The stopped digest
and independent postconditions are recorded below, separately from creation.

Frozen native source/helper candidate is
`375f6b65ce7a8841a33c0bb2d0f4aaf94c1e6651`, tree
`f10bfa143641f8516afc57f8e2d1f4c08a72cf19`. All sixteen source-bundle files
(2,045,033 bytes) retain SHA256-listing digest
`c58a21ff94aa3ead38d82da1691b5977fb87c23eb608c84209cd536a842e832b`.
The independently extracted and byte-compared nonbootable source/release CD remains
675,287,040 bytes, SHA-256
`b31e06d778c3bb43050dfcdfff66a40537799c0748c623c35de499f73dd5d1cd`.
Its six selected stock files passed explicit-key host verification. All eight original
tool/firmware/verifier/key/release pins matched before and after each launch.
No native source/case/helper or media bytes changed.

## Actual native attempts

Every launch passed complete paused block/device/PCI/ROM/process/handle/listener/mount
audits before CPU continuation. Provisioning used only the original writable raw
file, both expected read-only optical drives, one CPU, 1 GiB RAM and no NIC/sharing.
Generated block-node labels and host CPU thread identifiers were accounted for;
block graph edges and actual attachments were checked independently.

| Launch | Elapsed seconds | Peak emulator RSS, KiB | Captures/logs, bytes | Closure |
|---|---:|---:|---:|---|
| 1 | 504.109 | 619408 | 174184 | Observed clean native halt, reaped, full zero readback. |
| 2 | 634.331 | 659824 | 619312 | Observed clean native halt, reaped, full zero readback. |
| 3 | 1931.342 | 653376 | 1294473 | Observed CD unmount and clean halt, reaped, full zero readback. |
| 4 | 2349.549 | 1580624 | 3000112 | Stock installation/relink completed; process readback failed; target unmount and clean halt observed, reaped, full stopped readback. |
| 5 | 585.181 | 2122576 | 57846 | Process/source gates passed; native build stopped at the 2097152 KiB RSS ceiling. Reaped and full stopped hash; no native sync/halt. |

Counts describe retained launch captures at closure; later diagnostic enlargements
retain the original frames. These include prerequisite and partial-compiler
observations, not successful native planner tests. Each closure independently found
no owned guest/
controller, fixture handles, mounts or IP listeners. Only checked stale owned Unix
endpoints and redundant PPMs with retained lossless PNGs were removed. Failed disks,
media, source staging, original frames and receipts remain preserved; final fixture
disposal has not occurred. Logical disposal would not establish physical erasure.

### Launches 1 and 2: unavailable ramdisk command

`uname -srvm` returned `sh: uname: not found`. **Correction:** the earlier report
misread the VGA font as `umame`; enlarged inspection of the original frame reads
`uname`. The frames do not establish keyboard corruption. Launch 1 also queued later
verification text before earlier results were observed; that orchestration error
produced a failed CD mount and failed directory change, with no verification success.
Original private failure receipts remain intact; a separate correction records the
changed interpretation. PR #73's published description was corrected explicitly.

Launch 2 used 800 ms spacing and observed the full pending line before Return, then
stopped at the same unavailable command. Bounded read-only diagnosis listed the
ramdisk tool inventory: no `uname`, with `sysctl`, `sha256`, `signify` and stock
mount tools present. It confirmed absent `/dev/cd1a` and `/mnt/cd`, existing `/mnt`
and stock `/dev/MAKEDEV`, and the native release-key filename. Inspection of the actual
ramdisk script showed its cd-unit branch; no node or mount was created in launch 2.
The previously reviewed base-set MAKEDEV also includes only cd0 in its ramdisk profile.

### Launch 3: reduced verifier lacks checksum mode

The stock prompt was observed at 107.924 seconds. The positive shell probe emitted
exact `BOOTWEAVE-CONSOLE`. `sysctl` exited 0 and emitted release 7.9 and architecture
amd64; it omitted `kern.ostype`. OpenBSD family was independently observed in the
stock boot banner. The [reduced sysctl source](https://github.com/openbsd/src/blob/master/distrib/special/sysctl/sysctl.c)
lists those two supported keys and silently ignores unknown names; that source
explains the observation and does not replace native evidence.

Native SHA-256 of `/etc/signify/openbsd-79-base.pub` matched pinned key digest
`b7ee8e7981549a1f707a4ed05115a99959a5f9e34b9bed9f9a098be17dd7e4b1`.
After exact expected-absence checks, the approved stock `MAKEDEV cd1` returned 0.
Independent inspection observed root/operator block node (6,64) and raw node (15,64);
the actual script's UNITMULT=64 binds those minors to unit 1. Kernel maxpartitions=16
is a separate property. The owned root/wheel 0700 mountpoint was created and checked.
The read-only mount returned 0; native mount output showed only the ramdisk root and
cd1a as cd9660 read-only, with no target-disk mount. The fixed release-directory
change returned 0. A fresh complete host readback still matched the zero target.

After complete prefix/middle/suffix pending-line inspection, the exact six-file
`signify -C` invocation returned usage and status 1. Dependent work stopped.
The ramdisk [verifier build](https://github.com/openbsd/src/blob/master/distrib/special/signify/Makefile)
uses VERIFYONLY, which excludes checksum mode. This was a planning error: the reduced
ramdisk and full installed-system tools are different. No signature or selected-set
checksum success is claimed. No installer answer, credential or planner operation
followed the failure.

Bounded read-only diagnosis inspected `/install.sub`, which uses `signify -Vep` to
authenticate/extract the embedded checksum manifest. An expected usage diagnostic
confirmed that the actual ramdisk `sha256` supports `-C checklist`. Reading `ulimit -f`
confirmed the shell builtin; no limit was changed. The already verified stock ksh
manual specifies 512-byte blocks. After leaving the CD directory, unmount returned 0
and an independent mount listing showed only the ramdisk root. Completed sync/halt
was observed before emulator closure. The native RAM nodes/mountpoint do not survive
closure; their observations are preserved. No new manifest output was created.

## Accepted continuation: launches 4–7

Cam accepted a **seven-launch total ceiling**, counting all three stopped attempts, on
that same original zero target and sealed media. No reinstall, reset, source/media
replacement or extra authority follows from this acceptance. Repeat all original pins,
source/identity/full-digest/free-space checks and complete paused audits.

| Launch | Accepted stage |
|---|---|
| 4 | Observed stock prerequisites, corrected native release verification, first installation, clean halt and complete stopped readback. |
| 5 | Original serial build, all suites, traces and actual console cases. |
| 6 | Original keyboard/VGA suites and console cases using the same native binaries. |
| 7 | Original independent retained-fixture readback, exact native cleanup, clean halt and exact host disposal. |

Use only observed stock-tool capabilities: positive `echo`, the two supported
`sysctl` values and OpenBSD boot identity, pinned native key hash, and the same
conditional cd1-node/mountpoint creation. Do not call ramdisk `uname` or `signify -C`.

### Exact corrected authentication and checksums

Acceptance includes one additional **RAM-only** owned namespace, `/tmp/bootweave-release`, which
must be absent before exclusive 0700 root-owned directory creation. Its sole allowed
file is `SHA256`, root-owned 0600, one link, at most 4096 bytes. It is not a disk,
media input, project definition or native executable. With the CD still read-only
and its fixed release directory selected, use the observed embedded-signature mode:

```sh
(umask 077 && ulimit -f 8 && ulimit -f && signify -Vep /etc/signify/openbsd-79-base.pub -x SHA256.sig -m /tmp/bootweave-release/SHA256)
```

The subshell must report limit 8, a 4096-byte file ceiling; it leaves parent limits
and umask unchanged. Require signature success and status 0. The
[verifier implementation](https://github.com/openbsd/src/blob/master/usr.bin/signify/signify.c)
verifies the embedded message before opening its output with O_NOFOLLOW. Independently
require the exact output size **1992 bytes**, expected ownership/mode/link/type and
complete body SHA-256:

`50bec66f28426a22b9c9436f6a87cf3e7029e636bb915ba1d0db80d638881b87`.

The accepted proposal incorrectly inferred selected-file support from usage text:

```sh
sha256 -C /tmp/bootweave-release/SHA256 bsd bsd.rd base79.tgz comp79.tgz man79.tgz INSTALL.amd64
```

The selected-file `-C` command returned usage only and status 1 in launch 4. Under
the accepted one-correction rule, the original target was independently rehashed
and still matched every zero-image identity/time/allocation binding. The correction
uses the already proven plain `sha256` mode for the same six fixed names, followed
by independent complete-digest comparison against the exact authenticated whole
manifest. The reduced [SHA2_ONLY implementation](https://github.com/openbsd/src/blob/master/bin/md5/md5.c)
excludes the `-C`/`-c` handlers even
though its usage lists them; usage text did not establish capability. No altered
manifest, extra file, auth retry, source/media change or deadline reset is involved.
This used the one read-only invocation correction for launch 4. The corrected
command was:

```sh
sha256 bsd bsd.rd base79.tgz comp79.tgz man79.tgz INSTALL.amd64
```

All six complete digests matched independently and status was 0. The whole manifest
was authenticated, not a filtered/rewritten list. Independently recheck its complete digest, metadata
and directory inventory afterward. Remove only that checked owned file and empty
RAM directory, observe absence, leave and unmount the CD, and reconfirm the sole exact
8 GiB target before original destructive installer answers. No verification bypass
or change to accepted stock installation/credential choices is permitted.

A bounded host-only rehearsal with the already pinned verifier extracted exactly
those 1992 bytes under a 4096-byte file limit and 0600 mode. A deliberately tampered
message rejected without creating output. The owned rehearsal was disposed. This
checked the procedure before launch 4. The actual native results are recorded below.

### Routine prerequisite correction and unchanged hard stops

Accepted 200 ms keyboard spacing with full pending-text observation before Return and
full result/status observation before dependent work. The earlier 110 ms input was
correctly delivered; its failure was misdiagnosed. Retain at most one correction of
an unsubmitted line and preserve full original frames, including horizontal views.

The accepted bounded exception permits **at most one correction
per launch** of command-not-found or unsupported-option/usage-only errors during
read-only prerequisite discovery, before the installer begins and while the original
target remains independently proven zero. Record the exact error, observed stock
capability and correction first. This does not permit retrying a failed signature,
checksum or test, changing inputs/media/devices or resetting any deadline. This
narrow exception was explicitly included in Cam’s approval; every other
first-failure stop remains in force.

Authentication/checksum/identity/inventory, confinement/oracle/compiler/resource,
secret/capture failures, unexpected prompts and all errors after provisioning starts
still stop dependent work for review and bounded shutdown. Preserve the original
60-minute nonrenewing launch deadline, 15-minute prompt limit, 45-minute scheduling
cutoff, all guest/host resource and allocation limits, 5,868 native checks and exact
independent readback/disposal. Native support and feature completion remain unclaimed.

## Launch 4: stock provisioning and post-install evidence stop

Cam explicitly accepted the seven-launch continuation. Paused full inventory and
fresh full original-zero-target checks passed. The corrected native embedded-signature
command reported limit 8, Signature Verified and status 0. Independent readback
observed the exact sole 1992-byte root/wheel 0600 single-link manifest and expected
complete digest. One permitted usage-only checksum invocation correction was recorded;
all six complete native plain digests then matched that authenticated whole manifest,
with status 0. Manifest metadata/digest remained unchanged afterward. Exact file and
nonrecursive empty-directory cleanup, expected absence, CD unmount and ramdisk-only
mount state were observed.

Immediately before disk answers, a fresh full target readback still matched its
original identity/owner/mode/link/size/timestamps/zero allocation and zero digest.
An overly strict host comparison initially asserted on normal firmware/guest PCI
BAR, IRQ and SMB address allocation after boot. Diagnosis preserved the full snapshot;
complete immutable device/media/firmware fields matched the paused audit. Runtime
assignments were independently retained and stable; the corrected structural check
passed without an actual attachment change, native auth/test retry or target reset.

The native sole sd0 reported 8192 MB, 512-byte sectors and 16777216 sectors. Only sd0
was available at the installer disk prompt. The approved first installation applied
whole-disk MBR and automatic FFS/swap to that exact target. Stock set selection contains
only bsd, bsd.rd, base79, comp79 and man79; its own signature check passed. Synthetic
planner.example.invalid, no NIC/DNS, SSH/X disabled and no extra user were observed.
Stock com0 at 9600 was selected for the next serial boot. Credential entry used the
actual echo-off prompts and remained hidden. All selected inputs verified and
extracted, UTC was accepted, and stock finalization
reported no firmware additions/updates. Kernel relinking explicitly reported done
and the installer reported successful completion. Independent native disklabel and
mount readback confirmed sole sd0, 16777216 sectors, automatic FFS root/usr/home and
swap. Installed clang 19.1.7 and nobody UID/GID 32767 queries returned 0. Non-secret
configuration readback confirmed sshd_flags=NO, com0 at 9600 and the UTC link.

The next process-name-only query, `chroot /mnt /bin/ps -ax -o comm`, returned status 1:
`ps: maxslp: failed to get vm.maxslp` and `ps: kvm_getprocs: Not a directory`. A
chroot changes the filesystem view, not the running installer kernel. Full installed
ps is therefore not evidence of available process interfaces in that kernel. This
query was a procedure error; process evidence is unavailable, not a passing absence
check. The accepted rule “all errors after provisioning starts still stop dependent
work” applies. No corrective post-provisioning query, compiler or planner invocation
followed. The launch stage remains false; installation success does not qualify the
feature or establish a later boot.

Bounded shutdown unmounted only /mnt/usr, /mnt/home and /mnt, returning 0. Independent
mount output showed only the RAM root. Native sync completed and OS halt was observed
before QMP exit. The owned emulator/controller were reaped. Independent full stopped
readback retained the original target identity/owner/mode/single link/size and was
stable throughout hashing; its installed digest is:

`ec45716ce479accd87fc3a5224e512c47b11e89220e5b21d23b28a89df9ef908`.

Its observed allocated bytes were 2,346,500,096. The sealed CD and all eight pins still
matched. No owned process, handles, host mounts or listeners remained. Only the
checked stale control socket was removed; other endpoints were already absent.
Credential text was absent from launch captures. Original disk, media, source,
credential and failure evidence remain private and preserved; no final disposal.

## Accepted continuation: remaining launches 5–7

Keep the **existing seven-launch ceiling** and the same original installed target,
bound to the full stopped digest above. At acceptance four launches had been
consumed. Cam accepted the post-provisioning failure review, allowing launch 5 to start from
this exact stopped state despite launch 4’s false stage flag. No reinstall, reset,
replacement/clone, source/media change, extra launch or resource increase.

Use the normal base process query only after an observed boot into the **full
installed OpenBSD kernel**, together with its native release/architecture, clean
filesystem, account/compiler, layout and source/media prerequisites. Do not retry
installed ps under the installer kernel or count the earlier failure as success.
The private controller requires a new explicit acceptance receipt bound to
the failed closure, original target/media and reviewed helper hashes; prior approval
cannot start it. Later phases still require an actually passed previous stage.

Launch 5 then performs the original one-time serial build, suites, 48 trace cases
and four actual console cases. Launch 6 performs all original keyboard/VGA cases
on those same binaries. Launch 7 independently reads back retained exact fixtures,
performs observed native cleanup/halt, and disposes only the exact owned host
fixtures. All 5,868 checks, deadlines, resource/capture limits, first-failure stops
and independent observations remain unchanged. The prerequisite-only usage exception
cannot correct post-provisioning errors. Keep all remaining work on this branch and
PR #74’s continuation review; no intermediate phase PR.

## Launch 5: full-kernel prerequisites and native build memory stop

Cam replied “approved, continue” to the exact remaining-launch question on
2026-10-18. A separate exclusive acceptance receipt bound the original installed
image, failed closure 4 and reviewed controller/input helpers. Complete source/media/
tool pins, original identity/full stopped disk digest, free-space and no-handles
checks passed. The complete paused profile matched the prior inventory minus only
the approved omitted installer CD; raw graph edges, firmware, one CPU/1 GiB, exact
process arguments/handles, no host IP listener and original full disk digest passed.

The full OpenBSD 7.9 GENERIC amd64 kernel booted from that original installed image.
Root/usr/home filesystems reported clean. Actual serial login arrived at 115.508
seconds. Disposable credential input stayed hidden. Stock first-boot firmware/patch
fetches failed without NIC/DNS; they are expected offline boot observations, not
planner test results. Native uname/machine/root identity returned 0. Normal full-kernel
ps returned 0 and showed no relinker, compiler or test workers. Installed clang 19.1.7,
nobody UID/GID 32767, original disklabel/layout and no physical NIC matched. The CD
mounted read-only as cd0; only loopback/enc/pflog interfaces were present.

Exact absence guards preceded creation of root-owned 0700 /mnt/cd and /planner.
The read-only CD source-manifest digest and copied manifest matched the frozen pin.
All fifteen source checksums passed. Independent sixteen-file inventory confirmed
root/wheel 0600, single links, expected byte sizes and no extra file. The unchanged
accepted build script then started once. No new native source, media or dependency.

Bootstrap base cc returned successfully: 5.06 seconds real, 1.79 user, 1.39 system,
35120 KiB maximum RSS. Its independent host watchdog observed the guard-ready marker
at 6.030 seconds, within 180. Fourteen subsequent native guarded compiler calls
returned wait status 0. Maximum recorded compiler RSS was 37408 KiB, combined CPU
5.450 seconds and wall 5.702 seconds, within the original 512 MiB/120 CPU/180 wall
bounds. This includes normal dynamic/static engine and test objects; it is not a
complete accepted build, artifact inventory or runtime qualification.

The final static native-check harness compilation did not produce its completion
record. The supervisor observed emulator RSS **2122576 KiB**, exceeding the fixed
**2097152 KiB** ceiling, and terminated/reaped only its owned emulator. A subsequent
pause request found the already closed controller; no guest pause or native shutdown
is claimed. Overall launch elapsed was 585.181 seconds. There was no build-complete
marker, final artifact/map/linkage readback, planner invocation or native suite.
The native per-compiler measurements passed so far; the outer host resource gate
failed and remains failed. No automatic retry, resource increase or new boot followed.

Independent host closure confirmed absent owned processes, handles, mounts and
listeners. Complete stopped readback preserved the original target/parent identity,
owner/mode/single link/size and was stable while hashing. Its new partial-state digest:

`b456d19e70d597f3532d1d6146d0da6ea727fe7ca98822a26f232ab9c02b9c91`.

Media, all eight pins and the frozen source bundle matched. Credential text was absent
from captures. Owned allocation was 4,402,786,304 bytes, below 20 GiB. Only checked
stale Unix endpoints were removed. Raw disk, partial source/build/test fixtures,
credential, media and original evidence are preserved privately. No native unmount,
sync/halt or filesystem-consistency observation exists after the termination. QEMU
return code 0 establishes process closure, not a clean guest filesystem or stage pass.

## Proposed bounded recovery: launches 6–8, approval required

Request an **eight-launch total ceiling**, counting all five consumed attempts, on
only the original same-identity disk and stopped partial-state digest above. No
reinstall, raw-target recreation/zeroing/clone/overlay, added disk, changed guest
memory/CPU, source/media/dependency change or higher resource ceiling is proposed.
The partial build fixture replacement and stock preen recovery below are new
requested effects; previous approval does not authorize them.

Set the existing single-thread TCG accelerator explicitly to:

```text
-accel tcg,thread=single,tb-size=128
```

Pinned local QEMU help lists tb-size; [QEMU's primary documentation](https://www.qemu.org/docs/master/system/qemu-manpage.html)
defines its translation cache size in MiB. This bounds the requested cache at 128 MiB
while preserving the **2 GiB host RSS** and every native/host deadline, CPU, memory,
file/capture and allocation ceiling. The cache is a candidate contributor, not a
measured attribution of the failed RSS. Its effect and the changed profile require
fresh native evidence; no successful memory bound is claimed in advance. All other
attachments/firmware/arguments stay fixed. A normal host help query confirmed option
syntax without booting a guest. An earlier accelerator-help form returned a property
error before any VM/endpoint/CPU initialization; it is not a capability success or
an additional native launch.

Launch 6 may boot that exact preserved disk under the changed cache profile, with
complete paused identity/full-digest/device/process audits first. Request only the
unmodified stock boot filesystem **preen** path, including ordinary repairs it accepts
after the unclean stop. Observe full filesystem results and native login before any
build/test work. A manual-repair prompt, requested reboot, uncorrected error, missing
observation or resource/identity failure stops the run. No manual fsck-y, alternate
repair tool, reset or reinstall is included. Preen is recovery, not installation
rollback or proof that partial files were durably written.

After successful boot/prerequisites and no active compiler/relink/test workers,
independently inventory and hash the retained partial /planner tree using base tools,
preserving its actual metadata/content observations privately. Do not execute any
partial binary or claim the earlier fourteen compiles as qualification. Request
conditional disposal of **only that observed partial build tree**, after confirming
its exact /planner root, expected source/build/temp/work namespace, approved root/
nobody ownership, only single-link regular files and expected directories, no
symlink/special file or nested mount, no unexpected/out-of-scope path, and at most
the original 32 MiB
guest allocation. Allowed root entries are the sixteen frozen source files; the
sixteen executable names and ten map names listed by native_build.sh; fault-io.o,
fault-native.o and ARTIFACTS.SHA256; compiler-tmp; work; and outside-canary. Work may
contain only input and canary. Compiler-tmp may contain only bounded compiler-owned
regular intermediates. Missing/partial outputs are diagnostic state, not accepted
artifacts. Preserve each actual file digest before cleanup. Unknown inventory
stops cleanup.
No disposal of the original disk, media or unrelated guest paths is included here.

Then exclusively recreate /planner on that same installation, copy the unchanged
sealed source bundle, and independently reverify its manifest, every source checksum
and expected ownership/modes before one complete fresh native build. This explicitly
replaces the partial guest build/test fixtures; it does not reset the raw disk or
alter the source. Retain new qualified input/canaries/artifacts through later readback.

| Launch | Requested complete stage |
|---|---|
| 6 | Stock-preen recovery, full native prerequisites, independent partial-state readback/conditional exact build-tree disposal, unchanged source recopy, complete native serial build, suites, 48 trace cases and four actual serial consoles. |
| 7 | All original keyboard/VGA suites and four actual console cases on the same completed native binaries. |
| 8 | Independent retained-fixture readback, observed exact native cleanup/halt and exact owned host disposal. |

All 5,868 native checks and first-failure stops remain required. A new exclusive
acceptance receipt must bind this failed closure, original target/media, exact new
accelerator argument and reviewed controller/helper hashes. The prepared controller
cannot run under earlier receipts; only its first recovery entry may accept the
explicitly recorded unclean prior stop. Later phases require an actually passed stage
and clean native halt. Keep this work on the same continuation branch and PR #74
review, with one further consolidated PR when qualification and teardown are complete.
