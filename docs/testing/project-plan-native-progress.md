# Read-only planner native progress

Status: **Stopped after stock installation in launch 4, 2026-10-16.** Stock
installation and kernel relinking completed, but the post-install process query
failed under the installer kernel. No native planner compilation or check has run.
All **5,868 required native checks remain unrun**.
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
| Current proposal | 7, unchanged | Review the post-install failure before remaining launches 5–7; no extra launch or reinstall. |

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

Counts describe retained launch captures at closure; later diagnostic enlargements
retain the original frames. These are supervision/prerequisite observations, not
successful native planner tests. Each closure independently found no owned guest/
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

## Proposed continuation: remaining launches 5–7, review required

Keep the **existing seven-launch ceiling** and the same original installed target,
bound to the full stopped digest above. Four launches have been consumed. Request
review of the post-provisioning evidence failure, allowing launch 5 to start from
this exact stopped state despite launch 4’s false stage flag. No reinstall, reset,
replacement/clone, source/media change, extra launch or resource increase.

Use the normal base process query only after an observed boot into the **full
installed OpenBSD kernel**, together with its native release/architecture, clean
filesystem, account/compiler, layout and source/media prerequisites. Do not retry
installed ps under the installer kernel or count the earlier failure as success.
The prepared private controller requires a new explicit acceptance receipt bound to
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
