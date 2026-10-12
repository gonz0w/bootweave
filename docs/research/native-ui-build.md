# Native UI build experiment

2026-10-11, issue #4. Cam authorized moving to native UI development and documenting
as work proceeds. This extends the bounded original simulation to an installed
OpenBSD guest; it does not select C durably or authorize installer integration.
Cam then explicitly confirmed the inventoried disposable target and continued.
Writable native execution is authorized for this bounded build experiment under
AGENTS.md rule 7; actual outcomes are recorded separately.

The reviewed target is the newly owned `.tools/native-ui-build/target.raw`, 8 GiB,
initial SHA-256 ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25.
Its private preparation receipt records inode/device/size and must match before launch.
No backing file, existing disk, raw host device or other guest target is permitted.
Confirmed allowed writes: stock 7.9 amd64 partition/label/filesystem/set installation
on this one disposable disk, then native compile/test temporary files. Recheck
host identity and observed guest mapping before destructive choices. A changed
attachment invalidates confirmation.

Use the verified stock install79.iso and reviewed QEMU/BIOS/VGA identities from
[preflight](../design/native-profile/preflight.md). Preserve one CPU/1024 MiB,
TCG, pc-i440fx-11.1/qemu64-v1, no network/sharing/passthrough, private QMP/serial
sockets and local VGA. Target uses raw ide-hd on ide.0 unit 0 with writes enabled;
optical stays raw readonly ide-cd on ide.1 unit 0. Pause and inspect block/device/open
files/listeners before continuation. Confirm guest disk/size before partitioning.

Install manually from stock optical sets after observing each prompt; no autoinstall
or response file. Use a synthetic hostname, no network, SSH and X startup disabled,
no additional account, no encryption/keydisk/editor/template/site set/hook.
Guest root credential is disposable, generated privately and excluded from output,
Git and evidence. Select whole-disk MBR and auto layout only for the confirmed target.
Include base, compiler, manual and kernel sets; review actual set selection and refuse
verification bypass. Unexpected prompts halt input for review, not default acceptance.

Keep endpoint capture private and bounded at 10 MiB. Budget: 15 minutes to prompt,
60 minutes per installation/build run, 20 GiB owned host allocation. Stop the owned
process on limit/isolation loss. Partitioned/partially installed state is uncertain
until independent observation; there is no rollback claim.

After authorized installation, reboot only the same owned no-network guest from
its target disk. Replace the installation optical image with the prepared read-only
source-final.iso data image (same ide-cd attachment, no additional disk). Host hdiutil
makehybrid -iso -joliet built it from only plain-ui.c, native-smoke.sh, native-pty.c and their
SHA256 manifest. Check its digest before attachment and all three source hashes in
the guest before copying/compiling. Never execute host binaries or interpolate
project/operator input into commands. No packages or host dependencies
are added. Native fixed build command:

    cc -std=c99 -Wall -Wextra -Werror -pedantic plain-ui.c -o plain-ui

Record uname, compiler identity, source/script/binary hashes and byte sizes. Run
native-smoke.sh against the absolute native binary path; its seven fixed pipe cases
cover completion, CRLF, cancellation, empty review, ESC, NUL and help/back. Its
owned /tmp output is removed on exit. Test VGA and serial tty manually with stty
snapshots, known dimensions, valid/cancel/control/timeout cases and terminal
restoration. Native pipe cases do not qualify tty restoration or ramdisk inclusion.
Record actual outcomes and unavailable resource metrics without substituting host
results. Any native failure should drive a bounded code fix and regression check.

Stop guest/clients before fixture disposal, inspect absence of owned processes and
sockets, retain sanitized evidence and delete only owned disk/capture/transfer
fixtures. Preserve verified upstream inputs separately. No guest image/binary is
redistributed, services installed on the host or live network modified. Results
and cleanup will be recorded as they occur, rather than predeclared passed.

Final source/transfer and binary identities, observed native failures/passes and
cleanup are recorded in [native evidence](plain-ui-native-evidence.md). Earlier
transfer revisions remain experiment history rather than final tested identities.

Provisioning adjustment: stock optical sets lacked SHA256.sig, and the bypass
question was answered no. A bounded fixed-member ISO reader copied only bsd,
bsd.rd, base79.tgz, comp79.tgz, man79.tgz and INSTALL.amd64 from the verified ISO.
All six passed accepted host signify checks against the signed release manifest.
hdiutil made a nonbootable read-only data ISO containing those files and SHA256.sig:
672909312 bytes, SHA-256 d32647df00333236a495c32922ac3bdaa226875de90412c7d41c441feba76ba3.
Only the optical medium changed, with target inode/size/device and attachment
mapping unchanged. QMP reconfirmed raw read-only optical and the same single target.
The first post-change discovery reported no filesystems; read-only disklabel showed
ISO9660 a/c partitions, and a subsequent stock discovery succeeded. No cause is
asserted. Guest signature verification then succeeded; no bypass was used.

The original native-pty.c supervisor uses OpenBSD base libutil and twelve owned-PTY
cases, including initially set PENDIN. Compile with the same warning flags plus
-lutil and run against the absolute native UI binary. Each case is capped at
15 seconds, captured output below 64 KiB and child peak RSS at 32 MiB. It refuses
to claim native results on other systems. Native failures drove the bounded
terminal restoration fix described in the evidence; no runtime adoption follows.
