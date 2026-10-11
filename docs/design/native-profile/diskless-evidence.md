# Diskless stock-media observations

Observed 2026-10-11 for [issue #10](https://github.com/gonz0w/bootweave/issues/10).
Cam accepted the [diskless variant](startup-failure.md) by continuing immediately
after the explicit approval question on PR #21. This accepted two bounded stock
observations, private endpoints and reviewed read-only input; no writable targets,
installation, services or network changes. The [run plan](run-plan.md) otherwise
remained in force. No automatic restart was attempted.

## Frozen inputs and effective isolation

Clean source/base: 2f13683014710d1fa15dd19889cc2fc994ca0075.
Source tree: 7f2b95abbb009b64d3aa59816fe8e0fc16d6ec0b.
Rechecked the [reviewed executable and firmware](firmware-evidence.md) hashes and
repeated the accepted [release-image signature/checksum verification](release-input-evidence.md)
before each launch. Used QEMU 11.1.1, pc-i440fx-11.1, qemu64-v1, TCG, one CPU,
1024 MiB, fixed BIOS/VGA firmware and Cocoa display. Omitted only the target drive
and ide-hd tokens. Serial fixture added the reviewed local socket and isa-serial.

Both fixtures started paused. QMP reported prelaunch and one raw, read-only,
798625792-byte optical image, backing depth zero; no target disk. PCI inventory
for VGA and the serial device tree showed no network adapter. Process open-file
inspection showed the media opened read-only, owned private sockets, normal runtime
libraries/window resources and no host disk device. Neither process had a TCP/UDP
listener. Motherboard devices included IDE, ISA bridges, timers, keyboard/mouse,
empty floppy controller, vmport/vmmouse and VGA; these did not provide a shared
folder, clipboard or attached floppy. Serial added com0. The VGA device tree was
captured at continuation and reviewed afterwards; its block/PCI/open-file checks
preceded continuation. This sequencing limits the full pre-continuation inventory
claim. Socket access required scoped sandbox escalation; no safety setting changed.

## VGA observation

SeaBIOS and OpenBSD/amd64 CDBOOT 3.65 loaded the release bsd.rd. The guest reached
the OpenBSD/amd64 7.9 stock Install/Upgrade/Autoinstall/Shell menu. Guest output
reported wsdisplay0 at 80x25, console keyboard, optical cd0, and IDE channel 0
without drives. A keyboard x followed by Enter returned to the stock menu without
choosing an installation path. Prompts were readable; boot messages used VGA color.
No shell was selected and no disk, credential or network input was sent.

Loader reported a missing random.seed; kernel reported CHECK AND RESET THE DATE.
These warnings were observed without changing the guest. They remain baseline
limitations rather than hidden errors. Menu arrival was observed within two minutes
of process startup, including inspection/operator delay, below the 15-minute cap.
A sampled host RSS was 395280 KiB; peak RSS and guest available-memory metrics were
not collected. This establishes stock-media VGA interaction only.

## Serial attempt and input error

The fresh serial fixture reached an observed boot prompt. To interrupt automatic
boot, Enter was queued while paused, then CPU execution continued. Enter instead
started the default VGA boot. The subsequent reviewed set tty com0 key sequence
arrived during loader activity rather than at a command prompt and did not select
serial. This was an operator-input sequencing error, not evidence that com0 fails.
The private serial capture remained empty; boot was never sent on serial. On
observing the continued VGA loader, the exact owned guest was quit through QMP.
No installer choice or shell command was sent, and no restart/fallback was used.

A future separately reviewed attempt should interrupt the observed countdown using
the first character of the intended command, confirm the editable boot prompt,
then complete set tty com0 and observe the serial prompt before boot. This is a
proposed correction, not an executed or accepted third run. Serial-only firmware,
serial prompt/input, baud and 80x24/plain-text behavior remain unobserved.

## Outcomes and teardown

| Test | Actual outcome |
|---|---|
| NPT-01 | Host artifact checks repeated successfully; identities frozen |
| NPT-02/08 | No new corruption or unsafe-configuration experiment; prior host evidence separate |
| NPT-03 | Diskless block/open-file isolation observed; full VGA tree review occurred after continuation; disk-bearing profile not qualified |
| NPT-04 | Stock VGA menu and invalid keyboard choice observed successfully |
| NPT-05 | Serial observation failed due to input sequencing; no com0 qualification |
| NPT-06 | Not run: VGA used color and 80x25; serial terminal behavior unavailable |
| NPT-07 | Skipped: no target disk; no target-byte postcondition claim |
| NPT-09 | Serial stop condition exercised; synthetic capture-cap case not run |
| NPT-10 | Both owned guests absent after QMP quit; disposable fixtures removed |
| NPT-11 | Sanitized evidence recorded; maintainer review pending |

Each run remained under five minutes and the one-GiB guest allocation. Private
fixture file totals before removal were 911142 bytes (VGA) and 897106 bytes (serial),
under the capture/allocation caps. No raw transcript, host inventory or screenshot
is published. Retained only an ignored sanitized receipt. The serial capture client
exited on socket closure. Process inspection confirmed both guest PIDs absent before
fixture deletion, and owned socket/PID/log/image captures were removed. Verified
release media/tool stores were retained separately. Full image SHA-256 after both
runs was unchanged: 7a4a92e953618035097c796a90b54424a0f3ae775552e1e7d102cf8a5130449f.

No Bootweave runtime, renderer, parser, writable disk profile or installation is
qualified by these observations. Issue #10 remains open for serial correction and
review of the remaining evidence gates.
