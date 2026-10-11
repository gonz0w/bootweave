# First native qualification profile: engineering design

Status: Proposed, 2026-10-11. Owner: Cam. Acceptance pending.
Related [PRD](PRD.md) and [Test Design](TDD.md).

## Fixture boundaries and alternatives

Use an unprivileged foreground QEMU process with an explicit reviewed configuration.
The proposed amd64/legacy-BIOS profile permits software emulation on ARM64 hosts;
no host virtualization availability is assumed. UTM may provide a UI around QEMU,
but implicit device/sharing defaults complicate auditing. Same-architecture hardware
virtualization could reduce timing overhead but constitutes a separately recorded
backend. OpenBSD vmm is a future OpenBSD-host option, not this host's fixture.
Apple virtualization/arm64 changes architecture and firmware and is a separate
profile. None of these options is a runtime dependency decision.

Before execution, review the installed emulator's version-specific documentation,
license/provenance, bundled firmware licensing and exact machine/CPU compatibility.
Record version and firmware digest; versioned machine names avoid moving defaults.
No backend is downloaded or installed by this documentation change. A substantial
new dependency requires maintainer review and an adoption ADR where applicable.

## Inputs and trust

Use the release-specific [7.9 amd64 installation instructions](https://cdn.openbsd.org/pub/OpenBSD/7.9/amd64/INSTALL.amd64)
and [release record](https://www.openbsd.org/79.html), inspected 2026-10-11.
Freeze install79.iso, SHA256.sig and applicable source archives by verified digest.
Before boot, verify signed release checksums using OpenBSD base signify in a trusted
OpenBSD environment; do not execute an unverified guest to establish its own trust.
Arrange an independently trusted verifier or stop if none exists. Obtain the release
key through an already trusted OpenBSD installation or a separately reviewed trust
bootstrap; fetching key and image from the same mirror is insufficient by itself.
Record verifier/key provenance and verification outcomes without publishing private
keys or host identities. Review release errata before execution. If source inspection
is needed, pin release archives and exact inspected file digests; do not substitute
-current or mutable FAQ examples for release behavior.

## Attachment and authority review

Create an exclusively owned disposable fixture directory. Record a sanitized
inventory with disk identity, logical capacity, backing-file absence and read-only
media digest. Reject symlinks, backing chains, existing disks and raw/block device
paths. Ensure disk paths resolve within that fixture. Review all emulator arguments
as trusted executable setup; never interpolate project or operator text into shell.
Use explicit device configuration with no automatic networking or host sharing.
Prove attachments by inspecting effective configuration and process open files;
inspect guest-visible devices before interacting with installation choices.
Uncertainty in isolation is a stop condition, not a warning to continue.

Serial must terminate in a local private endpoint, never a public TCP listener.
Freeze serial port, baud and boot-loader console settings from release instructions.
Run VGA keyboard and serial separately against freshly created fixtures. Do not
mistake stock installer observations for Bootweave plain-text equivalence.

The initial run may boot and inspect read-only information but must not submit
installation, partitioning, formatting or set extraction choices. A future write
run requires maintainer authorization referencing the exact disk inventory and
scope plus operator confirmation immediately before the destructive step. Guest
disk names alone do not prove attachment identity. Recreate rather than resume a
fixture whose identity or state changed unexpectedly.

## State, failure and cleanup

Record planned actions separately from actions submitted and observations obtained.
An exit code is not proof of boot or install success. On unexpected prompts,
network/device discovery, exceeded limits, verification failure or lost console,
stop input and terminate the guest. Record disk state as uncertain if write activity
cannot be excluded. Do not restart into an unattended installer.

Keep raw console output private, bounded and access-restricted. Never publish raw
terminal control sequences; use escaped, manually reviewed excerpts with synthetic
values. Stop at capture limit rather than silently truncating critical evidence.
Cleanup stops the exact owned process, closes local console endpoints, removes only
owned fixture files after retaining sanitized evidence, and confirms absence of
processes, listeners and attachments. Failed cleanup leaves the run incomplete.
No service installation or live network modification is permitted.

## Evidence and remaining gate

Record base/head/tree and clean source, host OS/architecture, emulator/machine/CPU,
firmware, media/source digests, verifier version, console configuration, commands,
actual outcomes, limits, skips and cleanup. Store raw receipts privately; publish
only sanitized facts. This is a human-reviewed record, not a new wire schema or
harness API. Exact execution configuration and acceptance remain outstanding.

Primary option references, inspected 2026-10-11:
[QEMU x86 system emulation](https://www.qemu.org/docs/master/system/target-i386.html)
and [UTM system settings](https://docs.getutm.app/settings-qemu/system/).
These describe available options; neither establishes OpenBSD qualification.
