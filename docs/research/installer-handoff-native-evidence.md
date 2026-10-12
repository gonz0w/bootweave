# Confirmed-handoff native evidence

**Completed bounded experiment, 2026-10-12 local.** Native compilation, 19 final
native PTY cases, 19 media rejection cases, two verified stock installations and
their offline OS boots, the separate partial-install case and owned teardown
passed. The VGA first boot's background kernel rebuild received SIGTERM during
halt; a subsequent newly relinked kernel remains unqualified. No production
installer adoption or supported hardware profile follows. The
[earlier preparation stop](installer-handoff-stopped-evidence.md) remains part
of the evidence.

Cam accepted the [corrected-run amendment](installer-handoff-retry.md). Its private
target receipt, four exact writable files, two read-only bootstrap CDs and bounded
artifact-export exception were bound before launch. These observations use the
same reviewed QEMU/firmware, one CPU, 1024 MiB, private Unix endpoints and no NIC.
Paused inspection found only the exact build disk and authorized read-only media;
subsequent media cases independently checked their exact reviewed attachments
before boot.

## Build provisioning

The stock shell initially lacked cd1 device nodes. Stock MAKEDEV created those
nodes in RAM after attachment readback and observed zero block writes. A bounded
read-only mount retry then succeeded. Ramdisk signify supports verification with
`-V`, rather than the full installed tool's `-C` interface. Signature verification
and independent comparison of all six selected stock-file digests passed before
entering the installer or answering a partition prompt. No verification bypass
was supplied.

The manually supervised stock installation verified and extracted the five
selected stock sets, completed kernel relinking and returned to the shell.
Independent readback observed the reviewed disk capacity and automatic FFS/swap
layout, mounted filesystems, synthetic hostname, SSH disabled and expected
installed files. Three installed file digests matched independently resolved
members of the authenticated base archive, including the compiler's hard-link
member. Password database contents were not read or captured. The system synced
and halted before the same exact disk was hashed and booted offline.

That boot reached OpenBSD 7.9 amd64 GENERIC, the installed root filesystem and its
synthetic login. Offline firmware/patch fetches failed because there was no NIC or
DNS. This is build-guest OS boot evidence, not framework/application readiness.
All synthetic credential byte strings were absent from the private serial
captures checked at these stages; input queues remain restricted and private.

## Native compilation and footprint

Runtime sources are bound to candidate
49b618c80a39dfc62e84a5a37cbc0bc2b6dfa50a, tree
18154c0f6bd3a6084657027515f8919f32081982. Native checks matched all frozen source
files and verified the original ramdisk and selected signed sets. The first build
invoked a stock-entry checksum checklist without its required filenames. It
stopped at usage output, and independent readback confirmed all vnodes unused,
no temporary mount and no output directory. The corrected script supplies the
two fixed filenames. Its native digest was
01dfc6378e5101d019425f0e2e55c3986108bd83240e8cab95262476ed335dca;
it rebuilt against the same immutable source transport.

Native base cc compiled the static executable without warnings under C99,
`-Os -Wall -Wextra -Werror -pedantic`. The reused 44 storage-validator cases and
snapshot field comparisons passed natively. Readelf showed static PIE with no
INTERP or DT_NEEDED. The linker map contained only rcrt0.o, crtbegin.o, crtend.o
and base libc.a. No production library was added.

The stock reserve remained 3768320 bytes/7360 sectors. The mounted FFS had 247 KiB
free before packaging and 80 KiB afterwards. Native comparison checked the copied
binary/profile, fixed raw-device node, unchanged stock install symlink and both
installer-file digests. No automatic-install/upgrade input existed. Independent
post-build readback found all four vnodes unused and only the expected root, usr,
home and read-only source-CD mounts.

| Native artifact | Bytes | SHA-256 |
|---|---:|---|
| handoff | 166512 | cd09eced285362ea70fbee76492a674d3cc5b112162609514956df9c02036181 |
| handoff.map | 136026 | 29712896bb5471d4920c989efb089f3a833d06275a2b146a792a67825268138b |
| modified bsd.rd | 4921230 | 3edb7199f1a1c2812f0d90c9fda6da92ec20ffa4a2f73531d386a96a19d8495e |
| handoff.iso | 677939200 | 46e6462d0dda424f88e7cc2f0f8b94fff228ca09466f6ef3ae657ea09599871f |
| stock-entry.sha256 | 251 | 020f6437fcb8f356445b6bef88e88e546f0b4163760d69c7ee0d4f7d95e35092 |

## Initial native terminal checks

The separately owned native PTY supervisor uses base libutil; it has no installer
backend and is not packaged in the ramdisk. Its fixed driver links the same frozen
console/comparison code and a synthetic observation callback. All 16 initial cases
passed: fresh confirmation, initial PENDIN, 40x12 and unknown dimensions, changed
observation, queued multiline, empty, cancellation, controls, overflow, timeout,
TERM/INT/HUP/resize and forced death. Measured peak RSS was 616 KiB, and the idle
timeout took 10155 ms. Recoverable cases checked exact native termios and F_GETFL
restoration. SIGKILL required the owned supervisor's explicit restoration; the
child cannot restore itself after forced death.

The initial native supervisor source digest was
499774c93421916a4fc9c989127ba89c2d17a5ae0109330792e64b3ef9c65d47.
The fixed synthetic driver's digest was
a2c1cd9e731e5cc8452da945114bde63755506d026aebe4ad273b4eebaefe98e.
These fixture results do not replace actual read-only media or confirmed-install
cases.

The final supervisor source digest was
9cc94844689e5727ba23cba16d649c709e7d306f34612a5072de73bcd15ab419.
All 19 final cases passed after adding real terminal EOF/hangup, input during
reobservation and unavailable observation. Final peak RSS was 612 KiB and idle
timeout took 10268 ms. Seventeen cases restored their own exact termios/F_GETFL;
forced death required supervisor restoration, and a lost terminal could not
provide restoration readback and was disposed. The three added failures never
granted handoff. Total reviewed original C/header/build/profile/fixture source
was 40923 bytes, below the 64 KiB budget. These checks qualify this bounded research fixture only.

## Artifact transport and build-guest halt

The serial helper initially failed to handle socket backpressure during a test
source transfer. The capture was preserved, queued commands abandoned, and an
incomplete heredoc interrupted before use. A corrected partial-write helper and
paced source transfer produced exact native source hashes before compilation.

The first full ISO export was too slow. Its incomplete decoded file was identified
and removed without being used as media. Disabling terminal output translation
improved throughput on the same serial channel; its baud rate was temporarily
115200 and the exact original terminal format was restored afterwards. The
decoder's shorter observation window expired during the continued frame; a
bounded streaming observer completed that existing capture without requesting
another export. Neither incomplete file could pass the media launch guard.

Final streaming audit found exactly one complete frame per frozen artifact and
one abandoned incomplete frame. Each decoded size and digest matched the native
record, and rehashing the read-only host artifacts passed. All four credential
byte strings were absent from the complete 951353131-byte capture, within the
1073741824-byte exception. The build guest unmounted its source CD, reported all
vnodes unused, synced and powered down. Its emulator exited zero after 3140.27
seconds, within the 3600-second bound; the owned client exited and endpoint/file
handle readback was empty.

The diskless native media case rejected with exit 2 and zero block writes.
Independent RAM-root binary/profile hashes matched, and sourcing the profile
again reported the single-attempt marker instead of rerunning the front end.
The fresh read-only 2 GiB case likewise rejected with zero writes and matching
embedded hashes. Both guests halted and their owned processes exited.

## Media rejection cases

Ten fresh serial cases and nine fresh VGA cases rejected with exit 2: no disk,
undersized disk, cancellation, invalid acknowledgement, empty input, control
input, overflow, queued multiline input and expiration on both surfaces, plus
serial EOT. Every case observed zero QMP block writes, matched embedded native
binary/profile hashes, halted and closed its owned guest/client. The cancelled
and undersized files remained read-only attachments throughout; final whole-file
hashes, identities and zero allocations matched after all guests were stopped.

Four host identity substitutions (device, inode, size and digest) were rejected
without launching a guest. Four mutations of copied paused-attachment observations
(file, read-only state, driver and backing depth) were rejected for each supervised
serial rejection case. Actual attachments were not substituted. These are host
launch-guard checks, separate from the native disk metadata checks.

Two VGA observers stopped without qualifying their first runs. The diskless
observer encountered the pre-boot 640x480 screen before the expected 720x400 text
mode; it stopped before the kernel and passed on a fresh corrected observation.
The queued-input case actually rejected with exit 2, but late keyboard events
reached the research shell and prefixed the exit-status line. The observer was
corrected to recognize that exact exit value and the fresh case passed. This
prototype does not prevent all future in-flight input from reaching the stock
research shell after rejection, and does not establish production operation
authority, physical presence or a general anti-paste guarantee.

## Completed serial installation and boot

The fresh serial guest displayed the three fixed review pages on the actual native
console. Dimensions were unknown and explicitly unqualified. Paused attachment
readback and each review boundary observed zero block writes before the exact
fresh acknowledgement. The stock installer then started; each subsequent answer
was supplied only after observing its actual prompt. No response file or
noninteractive installer was used.

Native stock verification accepted the signature and all five selected files
before extracting them. The reviewed whole-disk MBR and automatic FFS/swap layout
was applied, configuration completed, the kernel relinked, and stock installation
returned exit 0. Independent disklabel/mount/df readback observed the full reviewed
capacity, root/usr/home filesystems and swap. Synthetic hostname and disabled SSH
configuration matched, as did authenticated ksh, release-key and compiler hashes.
Only the existence and metadata of the password databases were checked. The
guest synced, unmounted all target filesystems and halted; owned processes closed
before the exact target identity and whole-file digest were sealed.

A fresh independently inspected guest booted that same sealed disk once, without
a NIC. It reached OpenBSD 7.9 amd64 GENERIC #399, the installed root, clean
filesystems and successful root login. Independent readback again matched the
three authenticated hashes and synthetic hostname/SSH/console configuration.
Firmware and patch fetches failed in the offline guest as expected. A native
halt command produced successful sync output and the emulator exited zero; its
owned console client closed. This demonstrates stock OS boot, not application
readiness.

## Deliberate partial installation

A third independent fresh target received the same observed reviews and exact
confirmation with zero writes beforehand. Manually supervised stock prompts
performed the approved MBR and automatic filesystem formatting. At the initial
set-source prompt, before extraction, the documented stock shell escape allowed
independent disklabel/mount/df readback. It observed formatted root/usr/home
filesystems and the absence of kernel, base shell and saved hostname configuration.
QMP observed nonzero target writes.

That state is partial: planned effects, performed formatting and observed missing
OS/configuration remain separate. No installer completion or rollback is claimed.
The fixture synced, unmounted, halted and closed its owned processes without
returning to installation, extracting a set, resuming or booting the target.

## VGA installation, boot and shutdown limitation

The separate keyboard/VGA target displayed all three complete review pages using
ASCII wrapped to 40 columns, before fresh exact confirmation. Independent
attachment checks and QMP observations showed zero writes at each review boundary.
The observed stock keyboard-layout prompt accepted its default; all later
installer answers were supervised individually. Both no-echo password prompts
remained blank. Native signature verification and all five set checks reached
100 percent, extraction completed and kernel relinking finished. Stock installation
returned exit 0.

Independent VGA disklabel/mount/df/configuration/file readback matched the reviewed
capacity, FFS/swap layout, synthetic hostname, disabled SSH and console settings.
All three authenticated file hashes matched. The target synced, unmounted and
halted; owned processes closed before its exact identity and whole-file digest
were sealed. A fresh inspected offline guest then booted that same disk once,
using its configured serial console. It reached GENERIC #399, clean installed
filesystems, successful root login and matching configuration/file hashes.

During its subsequent halt, stock reorder_kernel reported failure. A bounded
read-only diagnostic inspected only the halted approved image, with no new guest
execution or attachment. Its initial FFS1-only guard refused the image; reviewed
FFS2 superblock/inode layout then allowed the fixed lookup. Matching the installed
reorder_kernel file to its independently authenticated archive member checked
the reader before it consumed the fixed relink log. That script's SHA-256 was
06f8e6fdcabe9a2a967d783862e43595f76bb28c1cbc2df15376c395a84ab751;
the log's was
77250e5a77159bbefac62d37428af62cbd45ec1850b722d0eec3238dc59da011.

The log recorded a successful /bsd checksum check and a SIGTERM during the
background newbsd build. Shutdown sync completed and the emulator exited zero.
The stock OS boot checks passed, but this shutdown interrupted background kernel
relinking; neither its completion nor a subsequent kernel boot is qualified. No
retry, resume, next boot or application-readiness claim was made.

## Resource, secret and disposal checks

Every guest retained one CPU/1024 MiB, no NIC or IP endpoint, fixed reviewed
firmware/media and the exact approved target attachment. Owned supervisors enforced
3600 seconds per run, the original 10 MiB ordinary capture limit and 20 GiB total
allocated storage; only the accepted completed-build export used its 1 GiB
exception. The serial install took 1005.33 seconds, VGA install 1017.99 seconds,
partial case 274.38 seconds, and their offline boots 177.38 and 138.91 seconds.
Native PTY peak RSS was 612 KiB, source stayed below 64 KiB, and the packaged
ramdisk reserve remained unchanged.

Final readback found unchanged device/inode/size for all six targets. The two
read-only negatives retained their exact original whole-file hashes and zero
allocated bytes; authorized build/install/partial targets each had observed
writes and distinct final digests. All four synthetic credential byte strings
were excluded from checked output captures and decoded VGA evidence. The accepted
stock plaintext-password argv limitation remains research-only; no guest
process-argv snapshot, password-database content or tracing was captured.

All owned guest/client processes exited, endpoint/file handle readback was empty,
and native mount/vnode cleanup was observed in the corresponding runs. A sanitized
receipt was saved outside the fixture, then all six owned disk files, credentials,
input queues, captures, screens, source transport and native artifacts were
disposed. Their exact-target authorization lifetime ended with disposal.

The tested original runtime candidate was squash-merged as
55f60d7a733a407f7cbb9714c390f606ac1cdd9b with the identical tree
18154c0f6bd3a6084657027515f8919f32081982. The follow-up changes the explicit stock
checksum invocation, adds the measured native PTY supervisor and records evidence.
Final runtime C/header/profile hashes matched the frozen native inputs; the
corrected build script and final supervisor hashes are recorded above. The open
follow-up PR binds its exact candidate/base/tree and local signature/DCO/check
results. These results do not adopt a parser, UI library, privileged operation
contract, arbitrary project execution or a production installer.
