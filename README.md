# Bootweave

A configurable, text-based OpenBSD installer framework for independent projects.
BSD-3-Clause licensed; community contributions welcome. This project is independent
of the OpenBSD project and does not imply upstream endorsement.

**Status: repository foundation and bounded UI/media/storage/handoff research.** A disposable
research image boots the synthetic UI. No production installer, configuration parser, runtime
service, supported hardware profile or release exists yet.
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
product. The configuration format and extension interface are not designed yet.

## Start here

- [Accepted scope](docs/adr/0001-generic-public-foundation.md)
- [Product requirements](docs/product/PRD.md) and [architecture](docs/architecture/baseline.md)
- [Installer author guide](docs/configuration/author-guide.md)
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
