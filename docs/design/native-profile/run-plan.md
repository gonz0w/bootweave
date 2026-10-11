# First non-install native observation: run plan

Status: Accepted for bounded attempt, 2026-10-11. Owner/acceptance authority: Cam.
[Issue #10](https://github.com/gonz0w/bootweave/issues/10); related
[PRD](PRD.md), [EDD](EDD.md), [Test Design](TDD.md) and
[verified release inputs](release-input-evidence.md).
Acceptance source: maintainer continued after the explicit bounded run approval
question for PR #20. The [attempt failed at paused startup](startup-failure.md)
before guest CPU execution; no native boot evidence exists.

## Exact intended boundary

Two fresh OpenBSD 7.9 amd64 stock-media observations: VGA keyboard, then serial.
No installation, upgrade, automation response files, modified media, network adapter,
accounts, services, package operations or writable guest disks. Provision only a new
owned 8 GiB sparse raw host file in a disposable fixture directory; attach it to the
guest read-only. This is a stricter initial boundary than the original proposed
profile. A later writable disk experiment requires a new reviewed target/scope.

Retain original profile limits: one CPU, 1 GiB guest RAM, 2 GiB media cap, 20 GiB
host fixture allocation cap, 15 minutes to prompt, 60 minutes per run, 10 MiB private
capture. Unmet limits or isolation checks stop rather than broaden the profile.

## Frozen backend and argument review

Use the existing unprivileged QEMU 11.1.1 executable, BIOS and standard VGA firmware
identities recorded in [preflight](preflight.md) and [firmware evidence](firmware-evidence.md).
Rehash before launch. Upstream byte correspondence exists; complete reproducibility
and redistribution review remains limited. No binary/source is redistributed.
Native guest observations will not establish hardware timing.

Arguments are trusted fixed tokens plus validated absolute fixture/artifact paths
passed directly as an argument array. No shell interpolation or project input.
Construct the following configuration after scope acceptance; resolve each path from
owned fixed storage rather than operator-supplied comma-containing options.

| Argument group | Exact intended tokens/value |
|---|---|
| Machine and CPU | -machine pc-i440fx-11.1; -accel tcg; -cpu qemu64-v1; -smp 1; -m 1024 |
| Defaults | -no-user-config; -nodefaults; -nic none; -monitor none; -parallel none |
| Firmware/data | -L reviewed installed QEMU data directory; -bios exact reviewed bios-256k.bin |
| VGA | -device VGA,romfile=absolute reviewed vgabios-stdvga.bin; -display cocoa |
| Raw disk | -drive file=owned new disk,format=raw,if=none,id=target,readonly=on; -device ide-hd,drive=target,bus=ide.0,unit=0 |
| Optical | -drive file=verified install79.iso,format=raw,if=none,id=media,readonly=on; -device ide-cd,drive=media,bus=ide.1,unit=0 |
| Boot/control | -boot order=d,menu=on; -S; -no-reboot; -pidfile owned private fixture PID record |
| Management | -qmp unix:owned private fixture socket,server=on,wait=off |
| VGA run serial | -serial none |
| Serial run | -chardev socket,id=console,path=owned private fixture socket,server=on,wait=off; -device isa-serial,chardev=console,index=0 |

The private management socket permits pre-execution attachment inspection and controlled
quit, not a public listener or product API. Protect fixture/socket permissions and
connect only local owned clients. Do not enable VNC, TCP QMP/GDB, USB passthrough,
shared directories, clipboard, user networking or forwarding. Cocoa is a local window;
verify no unexpected sharing/listener. Implicit motherboard/IDE/keyboard components
must be identified in effective configuration rather than assumed absent.

These exact combinations are not yet executed. If version-specific validation fails,
stop and revise the review record before substituting devices or flags. -S halts guest
CPU but initializes the emulator; preflight must precede even paused startup.

## Preflight, observation and allowed input

Before startup: confirm scope, clean source/head/tree/base, available storage/RAM,
exact executable/firmware/input hashes and signature checks, fixture ownership/no
symlinks/backing chains, full-disk initial digest and read-only attachments. Use
absolute regular-file paths inside the owned fixture or verified input store;
reject raw host devices. Record the intended effective inventory before opening QEMU.

Start paused. Inspect the owned process open files and QMP block/device inventory;
confirm only approved disk/media/firmware plus normal runtime library/window resources,
read-only guest blocks, no network device and only private sockets. Unknown storage,
listener or device stops the process before CPU continuation. Record the inspected
inventory; absent evidence is failure, not permission to proceed.

Continue via the private management connection only after these observations.
For VGA, observe BIOS/boot-loader and the stock installer menu, type one invalid
menu choice to demonstrate rejection, and optionally choose Shell for the explicitly
reviewed read-only commands below. Never choose Install, Upgrade or Autoinstall.
No credentials or network configuration are entered.

For serial, use a new fixture with the same verified media and read-only disk. A
local VGA window may be used to interrupt stock boot and select com0; record this
as a bootstrap dependency, not serial-only firmware qualification. The official
[console FAQ](https://www.openbsd.org/faq/faq7.html) identifies set tty com0 and default
9600 baud. At the observed boot prompt, use only set tty com0, then boot; if those
commands are unavailable or behavior differs, stop. Do not edit boot.conf/media or
blindly send input. After switching, installer input/output must be through the private
serial socket; record actual baud/terminal dimensions and no graphics dependence
for that phase. The [7.9 amd64 boot manual](https://man.openbsd.org/OpenBSD-7.9/man8/amd64/boot.8),
inspected 2026-10-11, confirms the tty variable, set tty com0 example and default
9600 baud. The mutable FAQ remains orientation only. Observe actual boot behavior
before sending subsequent input.

If Shell is selected, allow only uname -a, sysctl hw.machine hw.physmem, dmesg and
read-only disklabel inspection of the independently mapped guest disk. No mount,
network, fdisk editor, disklabel write, newfs, installer invocation or script execution.
Stop at unknown devices/changed mapping. Shell output stays private until escaped
and manually sanitized; full dmesg or raw transcripts never enter Git.

## Tests, termination and cleanup

Map observations to NPT-01/03/04/05/06/07/09/10/11; negative input verification already
has host evidence, but no native case is predeclared passed. Compare full target
bytes before/after; readonly flags alone do not establish target identity or all
planning semantics. Record stock prompt readability and serial limitations without
claiming Bootweave renderer equivalence. VGA 80x25 and serial 80x24 observations
should retain their actual dimensions rather than claim identical screens.

Use supervised bounded capture; terminate on capture/time limit, unknown prompt,
lost console or unexpected authority. Quit the exact owned emulator through QMP;
if unresponsive, terminate only that recorded child process after confirming identity.
No automatic restart. Verify process exit before hashing or deleting files.

Record actual memory/time/image/disk/capture sizes, outcomes, skips and uncertainty.
Retain only sanitized evidence, remove owned fixture disk/PID/socket/capture files,
and confirm no process/listener/attachment remains. Keep verified input/tool storage
separate from disposable fixture cleanup. An incomplete teardown is an incomplete run.

## Approval and remaining gate

Approval must explicitly authorize two bounded stock non-install guest observations,
new read-only disposable file fixtures, private console/management endpoints, the
listed read-only guest commands and the stop/cleanup policy. It may accept the local
firmware-use provenance limitations; it grants no redistribution or writable install.
Release-specific boot-console instructions are reviewed above. No guest has run;
this plan is the concrete review target for native execution authorization.

The original configuration is now known to fail with readonly IDE disk. Do not
retry it or broaden disk authority; review the proposed diskless variant in the
[startup failure record](startup-failure.md).

The diskless variant was subsequently accepted and attempted; see
[actual VGA success, serial failure and teardown](diskless-evidence.md). Historical
no-run statements above describe the original review state.
