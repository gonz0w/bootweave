# Bootweave

A configurable, text-based OpenBSD installer framework for independent projects.
BSD-3-Clause licensed; community contributions welcome. This project is independent
of the OpenBSD project and does not imply upstream endorsement.

**Status: first read-only planner implementation and bounded installer research.**
The planner has an accepted metadata contract and portable tests; OpenBSD planner
qualification is pending. A disposable research image boots the synthetic UI. No
production installer, runtime service, supported hardware profile or release exists yet.
Project: [gonz0w/bootweave](https://github.com/gonz0w/bootweave).
The maintainer approved the name and public repository on 2026-10-11.

The intended result is installer-only boot media: boot directly into a guided text
installer, review choices, install OpenBSD and a project's verified payload, and
report whether the installed system is ready. A general live appliance environment
is outside the initial scope. ANSI menus, shadow boxes and original BBS-inspired
artwork are intended alongside keyboard, serial and plain-text operation.

Project configuration should control branding, bounded flow choices, payloads,
service selection and product bootstrap without forking the common installer.
DNS, firewalls and unrelated OpenBSD applications must be expressible through the
same reviewed extension contract. Product policy and runtime state stay with each
product. A small metadata definition is adopted for the read-only planner. Installer flow
and extension interfaces remain separate designs.

## Start here

- [Accepted scope](docs/adr/0001-generic-public-foundation.md)
- [Product requirements](docs/product/PRD.md) and [architecture](docs/architecture/baseline.md)
- [Installer author guide](docs/configuration/author-guide.md)
- [First read-only project planner](docs/design/project-plan.md)
- [Security boundaries](docs/security/threat-model.md) and [debugging](docs/debugging.md)
- [Research and decisions](docs/research/openbsd-installers.md)
- [First native profile design](docs/design/native-profile/PRD.md) (proposed)
- [Native acceptance gate](docs/testing/native-gate.md)
- [Contribution and local review](CONTRIBUTING.md)

## Develop locally

Use the interpreter pinned in `.python-version`. Python and the downloaded tools
are development-only; no Python dependency is selected for the installed system.

```sh
make tools hooks
make verify
```

Checks run locally in the foreground. GitHub Actions and hosted/self-hosted runners
are not used. Signed Conventional Commits, DCO sign-offs and reviewed PRs are required.
See [development](docs/engineering/development.md) for tool pins and limitations.

The [plain-text research simulation](research/ui/README.md) has host and
[OpenBSD 7.9 native evidence](docs/research/plain-ui-native-evidence.md). It remains
a research simulation; no runtime adoption or installer implementation follows.
The [native ramdisk experiment](research/ramdisk/README.md) packages it in private,
diskless BIOS media and records [build/console evidence](docs/research/ramdisk-ui-native-evidence.md).
The [read-only storage probe](research/storage/README.md) has a native static build
and fresh native diskless, 8 GiB and undersized-disk evidence under the accepted
read-only virtio research amendment.
The [confirmed-handoff prototype](research/handoff/README.md) now has host PTY checks.
Its first native build preparation [stopped before set extraction](docs/research/installer-handoff-stopped-evidence.md).
The accepted corrected run has [native evidence](docs/research/installer-handoff-native-evidence.md):
19 PTY checks, 19 serial/VGA rejections, two verified stock installs and offline OS
boots, a separate partial-install case and full disposal. Stock background kernel
relinking was interrupted during VGA shutdown; a subsequent kernel boot remains
unqualified. This is bounded research, with no production installer adoption.
The [read-only payload inspector](research/payload/README.md) checks a bounded
synthetic ustar subset with twelve host regression groups and
[bounded native evidence](docs/research/payload-native-evidence.md): dynamic/static
builds and 90 checks each on serial and keyboard/VGA. Distinct on-wire directory
alias coverage, authentication, extraction and broader qualification remain separate
gates.
The [host-only authentication controller](docs/research/payload-authentication-host-evidence.md)
adds eight synthetic signature/identity/failure groups and inspects the same retained
bytes after verification. It uses the existing reviewed local verifier; native
authentication, production trust policy and extraction remain unqualified.
The [native authentication experiment](docs/research/payload-authentication-native-plan.md)
has [native evidence](docs/research/payload-authentication-native-evidence.md):
original C/base-signify dynamic/static builds, 128 authentication/alias checks and
90 historical checks on each console, key cleanup and complete fixture disposal.
Production trust policy, extraction and runtime adoption remain unqualified.

The [accepted placement scope](docs/research/payload-placement-spike.md) authorizes
bounded authenticated file creation in fresh owned guest roots. Original C planning,
a restricted unprivileged writer and independent postcondition/failure checks have
[410 serial checks in an earlier stopped run](docs/research/payload-placement-native-evidence.md).
The [accepted fresh rerun](docs/research/payload-placement-rerun-plan.md) now has
[820 serial and keyboard/VGA checks](docs/research/payload-placement-rerun-evidence.md),
observed native key cleanup, clean halts and full fixture disposal. A test observer
correction waits for measured child execution before signaling; resource and
confinement checks remain enforced. No production installer/runtime interface is adopted.

The [authenticated review and placement composition](docs/research/payload-authorization-flow.md)
now connects full file-effect review and fresh confirmation to that confined writer.
[Native evidence](docs/research/payload-authorization-native-evidence.md) records
976 required serial and keyboard/VGA checks, honest partial-state reporting, native
key cleanup, clean halts and full disposal. Earlier unqualified attempts are recorded;
production installer integration and runtime adoption remain separate decisions.

The [complete installed-target payload slice](docs/research/installed-payload-flow.md)
now has [1068 required native checks and observed exact teardown](docs/research/installed-payload-native-progress.md#completed-independent-readback-and-exact-disposal-2026-10-15):
976 build regressions and 46 installed cases each on serial and keyboard/VGA.
Confirmed offline stock installation, installed-root authorization, full actual
review/fresh confirmation, refusal/partial-state cases, retained placement and
independent later readback passed under the accepted OpenBSD 7.9 amd64 BIOS/TCG
research fixture. Native terminal/resource checks, clean halts, complete target/media
hashes and absence of handles/mounts passed. Earlier stopped attempts remain recorded.

The three exact disposable disks, owned media/staging/credentials/raw captures and
helpers were logically disposed; sanitized evidence and reused global tools remain.
Exact-target authority has ended. Production installer integration, runtime adoption,
generic project/service readiness and broader hardware/profile acceptance remain
separate decisions.

The [first project planner](docs/design/project-plan.md) implements a read-only
`bootweave plan` entry point in original base C with strict version-1 JSON metadata.
Its [runtime/contract decision](docs/adr/0003-read-only-project-planner.md) is accepted
for this bounded feature. Run `make build test-plan`; normal host builds provide
help/version and refuse planning outside OpenBSD. See the
[author guide](docs/configuration/author-guide.md) and
[host evidence](docs/testing/project-plan-host.md). Exact native fixture approval was
received; [stock installation and full-kernel prerequisites passed](docs/testing/project-plan-native-progress.md).
Native compilation began, then the emulator exceeded its host memory ceiling during
the final harness build. Partial state is preserved; native qualification remains
pending. The [frozen native run plan](docs/testing/project-plan-native-plan.md)
and remaining-launch proposal stay on one continuation branch, linked from PR #74’s
review; no intermediate phase PR is added.
