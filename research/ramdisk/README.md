# Disposable UI ramdisk experiment

Original BSD-3-Clause build tooling for the separately authorized next research
milestone. It has no installer engine, project format, payload or service backend.
[Native evidence](../../docs/research/ramdisk-ui-native-evidence.md) records the
OpenBSD build and diskless console checks; host lint cannot qualify this build.

Inside the reviewed OpenBSD 7.9 amd64 build guest, mount the frozen input data ISO
read-only at /mnt. Its ramdisk-ui directory contains the original UI and native
checks, this build script/profile/smoke test, signed release bsd.rd and SHA256.sig, and cdbr /
cdboot bytes from the authenticated stock ISO. Verify the frozen transfer identities
before executing `sh /mnt/ramdisk-ui/build.sh` with no arguments.

The script compiles/tests the static UI natively, extracts the signed kernel's
embedded filesystem using base rdsetroot, attaches only its new owned root.fs with
vnconfig, adds the stripped static UI, original login profile and ramdisk smoke test, and re-embeds it. Base mkhybrid
builds a BIOS-only optical image. It uses no package or network. Owned mount/vnode
cleanup is checked; failures retain the fixture for inspection. An existing output
path is refused. Failed or partial output must never boot as a passed build.

The profile invokes the synthetic UI once per boot, reports its exit status, then
opens a research shell. It invokes no installation path and never automatically
retries a failed UI. Completion authorizes nothing. Boot tests attach only read-only
optical media, with no writable disk, NIC, sharing or passthrough.

The [experiment scope](../../docs/research/ramdisk-ui-build.md) records exact-target
confirmation, native source/artifact gates, limits and required console evidence.
No artifact is published or runtime adopted by this research tooling.
