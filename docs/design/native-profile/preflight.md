# Native baseline execution preflight

Status: Incomplete preparation, 2026-10-11. Owner: Cam.
[Issue #10](https://github.com/gonz0w/bootweave/issues/10).
Related [PRD](PRD.md), [EDD](EDD.md) and [Test Design](TDD.md).
No execution acceptance or native result is recorded here.

## Read-only observations

Host tooling inspection only; no guest was launched and no fixture was created.
QEMU reports version 11.1.1. Installed package inventory agrees. Its machine help
lists pc-i440fx-11.1 and CPU help lists qemu64-v1. These are proposed explicit
candidates for the amd64 software-emulated profile, not observed boot compatibility.

| Installed artifact | SHA-256 observed |
|---|---|
| qemu-system-x86_64 | 25afcb2986e4a0f2c567de17273f55de3c99792b78a07c99e923bb62024c088f |
| bios-256k.bin | ae6f6aa973aaccc143f57aa960fb035fd9de4daee4ad0cd713322f8c259e7650 |
| vgabios-stdvga.bin | e8fc9e55790dbe3cb31f019a3deb57206ba6c54f5e581adb2ab2677a9d391472 |

Commands: qemu-system-x86_64 --version; -machine help; -cpu help;
package version inventory; SHA-256 over the listed installed files. Local package
license/COPYING/COPYING.LIB and installation receipt files exist. Existence is not
file-level license/provenance review or proof these firmware files are selected.
Digests identify local bytes; they do not authenticate package origin. Recompute
before execution; any changed identity requires updated review.

No signify executable was found on the ordinary host PATH. This does not prove no
trusted verifier exists elsewhere. No existing trusted OpenBSD verifier, media digest
or release-key trust bootstrap has been established in this session. Do not boot
unverified media to create the environment used to verify that same media.

## Remaining preflight gates

| Gate | Required evidence | Current state |
|---|---|---|
| Scope acceptance | Maintainer record naming accepted design and permitted non-install run | Pending; merged documents still say proposed. |
| Package provenance/license | Installed package receipt, source origin, executable and bundled firmware license review | [Partial review](provenance.md); [firmware bytes match upstream](firmware-evidence.md), build/license closure pending. |
| Input trust | Accepted verifier/key policy and verified source/media identities | [Host artifact checks passed](release-input-evidence.md); no native claim. |
| Release review | Exact release instructions/source needed for console setup and applicable errata | Not frozen for execution. |
| Backend | Explicit versioned machine/CPU, TCG, firmware/data paths and all enabled devices | Candidates listed; full configuration not reviewed. |
| Attachments | New owned raw disk, read-only optical media, no backing chain/passthrough/sharing/network | No fixture created. |
| Consoles | Reviewed local VGA and private serial endpoints, boot-loader settings and input procedure | Pending. |
| Limits and stop | Approved profile budgets, capacity checks, owned-process termination and bounded capture | Design exists; execution mechanism pending. |
| Observation/cleanup | Effective configuration/open-file review, before/after disk digest, guest inventory and teardown checks | Not run. |

Do not publish a runnable launch command until trust, device selection and authority
are concrete. Disabling defaults alone is not evidence of complete isolation;
review explicit device/controller dependencies and effective attachments. No generic
run command or new evidence wire format is adopted in this record.

## Next bounded work

Review installed package and firmware provenance locally. Arrange the trusted verifier
and key bootstrap without claiming digest-only verification authenticates a publisher.
Then freeze exact media/configuration and prepare two separate console run records
for NPT-01 through NPT-11. Initial permitted goal remains stock boot/menu inspection,
with no install/upgrade/autoinstall submission. Any future OS installation requires
its own inventoried disposable target confirmation and explicit authority.

The read-only inventory does not select QEMU as an installer dependency or qualify
native support. No native process/endpoints/files were created, so native teardown
was unnecessary. Unresolved gates remain explicit rather than reported as skipped
passes. Private package receipts and host paths are omitted from shared evidence.

The [provenance and verification-route review](provenance.md) records installed
license findings and the two media-trust routes requiring review.

[Host-verifier experiment evidence](host-verifier-evidence.md) now records a local
build and adapted synthetic regression pass; release-key/signature trust remains pending.

The proposed [non-install run plan](run-plan.md) freezes backend arguments and
read-only attachment/console/cleanup scope. Native execution acceptance remains pending.
