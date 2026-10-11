# Bootweave

A configurable, text-based OpenBSD installer framework for independent projects.
BSD-3-Clause licensed; community contributions welcome. This project is independent
of the OpenBSD project and does not imply upstream endorsement.

**Status: repository foundation and bounded UI research.** No installer, configuration parser, bootable
image, runtime service, supported hardware profile or release exists yet.
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

The [plain-text research simulation](research/ui/README.md) has host-only tests;
it does not qualify an OpenBSD runtime or implement an installer.
