# QEMU firmware source correspondence

Observed 2026-10-11 for [issue #10](https://github.com/gonz0w/bootweave/issues/10).
Read-only research; no build, guest launch or dependency adoption.

## Byte comparison and upstream pointers

Downloaded QEMU v11.1.1 reference firmware to private temporary research storage,
then compared SHA-256 against the installed bytes recorded in [preflight](preflight.md).
Both match:

| Artifact | Installed and upstream SHA-256 |
|---|---|
| bios-256k.bin | ae6f6aa973aaccc143f57aa960fb035fd9de4daee4ad0cd713322f8c259e7650 |
| vgabios-stdvga.bin | e8fc9e55790dbe3cb31f019a3deb57206ba6c54f5e581adb2ab2677a9d391472 |

Reference files: [BIOS](https://github.com/qemu/qemu/blob/v11.1.1/pc-bios/bios-256k.bin)
and [standard VGA BIOS](https://github.com/qemu/qemu/blob/v11.1.1/pc-bios/vgabios-stdvga.bin).
Commands used HTTPS retrieval, local SHA-256 and read-only GitHub contents API.
These comparisons establish byte correspondence to the observed tag contents, not
independent authentication of the tag or the original installed bottle.

The tag's [roms/Makefile](https://github.com/qemu/qemu/blob/v11.1.1/roms/Makefile)
(lines 80–107 inspected) builds bios-256k.bin from SeaBIOS's 256k configuration and
vgabios-stdvga.bin from its standard VGA configuration. Its SHA-256 is
674d68f910ad630234da2b0f3b7acf50984715898ce5d0fb2e6e27333809f31d.
The contents API reports roms/seabios gitlink
b52ca86e094d19b58e2304417787e96b940e39c6; the tag's
[.gitmodules](https://github.com/qemu/qemu/blob/v11.1.1/.gitmodules)
identifies the QEMU-hosted SeaBIOS source repository.

The pinned gitlink and build rules are source references. They do not demonstrate
that the checked-in binaries were rebuilt from precisely that gitlink with known
compiler/configuration inputs. No reproducibility build was performed. The generic
pc-bios README also mentions an older VGA BIOS project; use artifact-specific build
rules rather than assuming every VGA image has the same source or license.

## License evidence and limitations

At the matching commit in the SeaBIOS mirror, inspected
[COPYING](https://github.com/coreboot/seabios/blob/b52ca86e094d19b58e2304417787e96b940e39c6/COPYING),
[README](https://github.com/coreboot/seabios/blob/b52ca86e094d19b58e2304417787e96b940e39c6/README)
and [src/post.c](https://github.com/coreboot/seabios/blob/b52ca86e094d19b58e2304417787e96b940e39c6/src/post.c).
COPYING contains GPLv3 text (SHA-256
8ceb4b9ee5adedde47b31e975c1d90c73ad27b6b165a1dcd80c7c545eb65b903);
the representative post.c file has an LGPLv3 distribution notice. A COPYING file
alone therefore cannot assign one license to every compiled firmware component.
Complete relevant file-level notices and build composition review remains outstanding
before redistribution or any claim of complete license clearance.

This review is for existing local disposable-research tooling; no firmware or source
is committed to Bootweave or included in its media. Future redistribution needs its
own artifact license/provenance review. Removing the research fixture would remove
its firmware references; no installed-system dependency is introduced.

## Remaining gate

Firmware byte correspondence is now observed. Tag authentication, complete build
provenance/notices, exact runtime firmware selection and effective device configuration
remain review items. Media trust and explicit non-install run acceptance are still
pending. No native qualification or guest teardown occurred.

Separately attempted release-specific OpenBSD installer source lookup returned no
usable source through the queried endpoints. Do not infer a completed prompt ledger
or substitute mutable current source for authenticated 7.9 release source. Source
inspection in issue #3 remains outstanding.
