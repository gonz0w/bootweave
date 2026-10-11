# Native observation attempt: read-only IDE startup failure

Observed 2026-10-11 for [issue #10](https://github.com/gonz0w/bootweave/issues/10).
Scope: [non-install run plan](run-plan.md). Maintainer accepted the bounded two-run
scope by replying “continue with issues” immediately after the explicit run approval
question for PR #20. This did not authorize writable guest targets or installation.

## Planned and performed

Rechecked executable/BIOS/VGA firmware hashes and repeated the accepted-policy image
signature/checksum verification successfully. Host capacity exceeded fixture budgets.
Created a new owned 8 GiB sparse raw file and calculated its complete initial digest.
Started the exact reviewed QEMU argument array with -S, readonly=on on disk/media,
no networking and private local management socket. Guest CPU continuation was never
submitted.

QEMU exited with status 1 during device initialization. Its error identified the
ide-hd target as unsupported with a read-only block node. No fallback, writable flag,
new device or second console attempt was used. No guest boot, prompt, shell, disk
inventory or console equivalence observation exists from this attempt.

## Observed state and cleanup

The startup subprocess was observed exited. A subsequent process identity check
confirmed it absent. Compared the complete disk bytes after exit against the initial
digest: unchanged. Both full-disk SHA-256 values were
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25. Removed the owned disk, stale local QMP socket, PID/log/argument
records and fixture directory after retaining a sanitized failure receipt in ignored
tooling. Fixture removal was checked. No active guest endpoint/process remains.

This is a host backend initialization failure, not a native OpenBSD boot failure.
NPT-01 input checks have host evidence; NPT-03 effective attachment inspection and
NPT-04/05/06 native interaction could not run. Full-disk comparison confirms no change
for this failed attempt; it does not qualify NPT-07's intended native read-only
interaction. Cleanup is observed for this fixture, not a claim of all native cases
passed. Serial run was skipped because it uses the same unsupported device setup.

The first cleanup process inspection was denied by the workspace sandbox; a scoped
approved process read succeeded. No unsafe cleanup or unidentified process termination
was attempted. Resource/console measurements for a running guest are unavailable.
No source/image execution, service, network or installation occurred.

## Proposed next experiment

A diskless variant can test stock optical boot/menu and serial interaction without
any guest target. Propose omitting the target drive and ide-hd tokens, preserving
verified read-only optical media, machine/CPU/firmware, no network, pause/inspection,
private endpoints, limits and stop conditions. Run fresh VGA then serial fixtures;
record no-disk guest inventory and do not choose Install/Upgrade/Autoinstall.
Read-only guest shell commands remain limited to uname, memory query and dmesg;
omit disklabel because there is no target disk.

This narrows exposure but changes the reviewed attachment profile. Record explicit
acceptance before launch; do not silently substitute. Diskless results cannot qualify
disk selection, target confirmation or non-install target-byte postconditions.
Any future disk-bearing profile needs separately reviewed backend support and native
evidence; do not use a writable disk solely to get past this error.
