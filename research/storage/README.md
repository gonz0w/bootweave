# Disposable read-only storage probe

Original BSD-3-Clause research, authorized by the
[bounded spike](../../docs/research/storage-probe-spike.md). It has no installer
backend, target-selection interface or project configuration syntax. C/base libc
remain research choices. The existing synthetic UI is unchanged.
The [native evidence](../../docs/research/storage-probe-native-evidence.md) records
the native build and fresh diskless, 8 GiB and undersized-disk serial/VGA results
under the accepted read-only virtio amendment.

`probe.c` runs only on OpenBSD 7.9 amd64 with no arguments. It observes at most
4096 bytes of HW_DISKNAMES, accepts only the reviewed fixture names sd0/cd0/rd0,
and requires exactly one sd0. Optical and RAM entries cannot become candidates.
It opens the independently fixed /dev/rsd0c with O_RDONLY/O_NOFOLLOW, checks its
character-device major/minor (13/2), and uses only DIOCGDINFO on that descriptor.
It validates magic/version/partition counts, sector bounds, multiplication and
partition extents, requires the scoped 8 GiB capacity, and checks discovery/DUID
agreement again before printing. Device/model/label strings are never printed.

Success reports inventory only. Guest names and DUIDs do not establish host
identity, freshness, emptiness, installation readiness or permission to write.
Missing, extra, malformed, changed or unsupported observations fail incomplete;
there is no default selection or installer fallback. No file creation, disk write,
mount, subprocess or terminal-mode change occurs in the probe. Its output is
fixed ASCII plus bounded numeric/hexadecimal fields, with lines at most 40 columns.
A ten-second alarm bounds ordinary execution/output; the owned host supervisor
also limits the whole guest. Uninterruptible kernel waits are not a guaranteed
deadline. Unknown console dimensions are reported, and known narrow consoles fail.
The native probe reports the kernel's RSS high-water observation before exit and
rejects unavailable, negative or above-budget (32768 KiB) measurements. This is
an observed checkpoint, not a hard memory-allocation limit.

`validate.h` is an internal normalized test model, not the OpenBSD disklabel ABI
or a future wire format. `make test-storage` compiles and checks its synthetic
boundaries on the host; those results cannot qualify native device behavior.
The same `validator-test.c` runs in the approved native build guest.

Mount the independently frozen source transfer ISO read-only at /mnt in the
separately confirmed build guest. Its storage-probe directory contains these
sources, build.sh/profile, SHA256.inputs, signed release bsd.rd/SHA256.sig and
cdbr/cdboot from the authenticated stock ISO. Verify all frozen identities before
executing `sh /mnt/storage-probe/build.sh` without arguments. The manifest does
not authenticate itself; its trust comes from the independent review receipt.

The build uses only native base cc/libc, rdsetroot, vnconfig and mkhybrid. It keeps
the stock embedded filesystem reserve, adds one stripped static probe and the
profile, and checks owned mount/vnode cleanup. Any required raw device node is
created only at the fixed path inside the private embedded FFS. Existing output
paths are refused; failed/partial artifacts cannot become passed candidates.
The profile starts the probe once and then opens a research shell. Its only mount
update is the RAM root; it never invokes the stock installer.

Boot checks attach one read-only ISO and at most one read-only candidate, with no
NIC, sharing or passthrough. Whole candidate hashes must remain unchanged. Images,
credentials, raw captures, inventories and build disks remain private and are
disposed after sanitized evidence is saved. No release or general hardware support
is claimed; redistribution provenance closure remains a separate gate.
