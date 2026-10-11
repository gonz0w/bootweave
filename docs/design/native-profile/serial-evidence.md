# Diskless serial stock-media observation

Observed 2026-10-11 for [issue #10](https://github.com/gonz0w/bootweave/issues/10).
Cam continued after PR #22's documented correction and merged evidence. This was
accepted as continued bounded diskless serial research under the [run plan](run-plan.md),
including the reviewed stock-menu and read-only shell commands. No writable target,
installation, network, service, media change or new dependency was authorized or used.
The [earlier serial failure](diskless-evidence.md) remains an accurate historical result.

## Identities and inspection

Clean source/base: bd084413c7c281e86a74236b2976f62ba8a24b15.
Tree: 516011b1360699ae944e0e269dbf207382612a18.
Used the same frozen QEMU 11.1.1, pc-i440fx-11.1, qemu64-v1, TCG, one CPU,
1024 MiB and reviewed BIOS/VGA firmware as the earlier diskless configuration.
Rehashed executable/firmware and repeated the accepted signify image verification
before each launch. The [release input identities](release-input-evidence.md) and
[firmware limitations](firmware-evidence.md) remain applicable.

Four manually supervised fresh attempts used identical argument arrays and 0700
owned fixture directories; no automatic restart or changed backend/device authority.
Before each CPU continuation, inspected QMP prelaunch state, complete device tree,
block inventory and process open files. The sole guest block was the read-only raw
798625792-byte optical image with no backing chain, opened read-only by the process.
No target disk, attached floppy, network adapter, shared directory, clipboard or
passthrough existed. Implicit motherboard devices matched the previously recorded
inventory; com0 was the only added serial device. Only private Unix QMP/console
sockets existed; process TCP/UDP listener inspection returned none.

## Timing correction and performed input

Attempt 1 paused at boot> but resumed after the countdown deadline; queued s did
not interrupt default boot. Attempt 2 observed the running prompt, but the next
model/tool round trip exceeded the countdown. Both stopped without further commands.
Attempt 3 used an ephemeral local screenshot observer. Its initial 640x480
pre-initialization screen violated the initial 720x400 assertion; no input was sent.
The observer was corrected to ignore other dimensions. By then the prompt had
expired; its bounded 15-second observation timed out and quit the guest. This helper
error is recorded rather than counted as serial evidence.

Attempt 4 used the corrected observer from the start. It compared the previously
visually inspected boot> pixels (x=0..39, y=160..175 in a 720x400 PPM) and required
all pixels below that line to be black. Only a match sent one fixed s key; timeout
quit the owned guest. It sent s 2.67 seconds after continuation. Manual screenshot
review confirmed boot> s and an editable cursor before completing set tty com0.
The private serial capture then displayed CDBOOT 3.65 and boot>. Only then was boot
submitted through serial. This local control helper is research tooling, not an
adopted runtime or public interface. Reference-pixel SHA-256:
9531dafbb5ebe07fc393bd1b487f021d5cff6229c0cfbd05704e101d1f96ae8a.
Corrected helper SHA-256:
a263a288306c004e0dcedd8ca8ec119795797074966706688f0fb21200b501e4.
Helper sources and sanitized receipts remain in ignored local tooling.

The OpenBSD/amd64 7.9 stock menu arrived over serial. x plus carriage return
returned to the menu; s opened the shell. Submitted only uname -a,
sysctl hw.machine hw.physmem and dmesg, separately after observing each prompt.
uname returned not found. sysctl returned hw.machine=amd64 but no hw.physmem value;
no reason is inferred. dmesg reported OpenBSD 7.9 (RAMDISK_CD) #396,
com0: console, real memory 1056833536 bytes (1007 MB), available memory
1018683392 bytes (971 MB), optical cd0 and no IDE channel-0 drives.
No substitute-binary, mount, disklabel, partitioning, installation or script
commands were submitted. The stock ramdisk boot/installer itself executed normally.

## Console, resources and uncertainty

After VGA boot-loader selection, all boot/menu/shell input and output used the
private serial socket. Guest output reported wsdisplay at vga1 not configured;
no further graphical input was needed. Initial firmware/boot-loader selection
still depended on VGA, so this is not serial-only firmware qualification.
The release boot manual specifies default 9600 baud; UART rate was not independently
measured, and Unix-socket throughput does not establish physical serial behavior.
No guest terminal dimensions were measured or changed. The 80x24 case remains unrun.

The 24151-byte serial capture contained zero ESC bytes; required stock prompts were
readable as sequential text without color/cursor control. This does not qualify a
Bootweave renderer, terminal resize, low-color navigation or untrusted-text handling.
Missing random.seed and CHECK AND RESET THE DATE warnings remain observed; no
attempt was made to alter the guest to suppress them.

The successful fixture reached the menu within two minutes of process startup,
including inspection/operator delay. Every attempt lasted less than five minutes;
no resource cap was reached. Successful fixture file total was 918761 bytes before
removal; earlier totals were 896587, 897254 and 909837 bytes. A successful-run RSS
sample was 786256 KiB; peak RSS was not measured. One CPU/1024 MiB configuration
and observed memory values are evidence for this fixture, not resource minima or
hardware timing guarantees.

## Test mapping and cleanup

| Test | Outcome for this follow-up |
|---|---|
| NPT-01 | Repeated frozen artifact verification passed before every attempt |
| NPT-03 | Full pre-continuation diskless isolation and guest optical/no-target inventory observed |
| NPT-05 | Serial prompt, input and shell observed successfully after VGA bootstrap |
| NPT-06 | Plain serial text observed; zero ESC bytes; 80x24 and independent baud checks not run |
| NPT-07 | Skipped: no target disk |
| NPT-09 | Timing/error stop conditions exercised; synthetic capture-cap test not run |
| NPT-10 | All four owned guests absent; clients exited; owned files/sockets removed |
| NPT-11 | Sanitized source-bound evidence recorded; maintainer review pending |

Each failed attempt was stopped and its owned fixture removed before manually
starting the next. Final process inspection confirmed all four owned PIDs absent.
The successful capture client exited on socket closure, then owned PID/socket/log/
transcript/screenshot files and directory were removed. No raw transcript, host
inventory or screenshot enters Git. Verified inputs and diagnostic helper sources
are retained separately in ignored tooling. Full media digest after all attempts
was unchanged: 7a4a92e953618035097c796a90b54424a0f3ae775552e1e7d102cf8a5130449f.

Remaining gates include evidence review, full dependency/firmware licensing review,
disk-bearing backend design and confirmation, target-byte postconditions, synthetic
failure/capture cases, measured terminal dimensions and later scoped installer/UI
research. No Bootweave runtime, installation or supported hardware profile is qualified.
