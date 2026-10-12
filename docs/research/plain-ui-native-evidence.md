# Plain-text UI: native build and terminal fix

2026-10-11 local / 2026-10-12 UTC, issues #4 and #10. Cam authorized native
building and documentation as work proceeds, then confirmed the inventoried new
8 GiB disposable target before any guest writes. Scope is the original bounded
simulation and its tests, under [the build experiment](native-ui-build.md).
No installer engine, durable C adoption, configuration interface or ramdisk
qualification follows. Parent UI comparison and issues remain open.

## Execution and source binding

Base: 2f70c8754ee2c4d7483ee5e5302a56e667e40160. Native execution used the source
bytes below from the development worktree; it was not a clean signed candidate
at execution time. The PR's signed candidate retains these exact source bytes;
clean-candidate host/signature/DCO evidence is recorded separately in its body.
Original source and tests are BSD-3-Clause; no upstream implementation was copied.

| Final input | Bytes | SHA-256 |
|---|---:|---|
| research/ui/plain-ui.c | 9116 | 8bdeaed352dabc532c46d12fcb9a7b5fa7e3ab2363058ba24196e99b87094dde |
| research/ui/native-smoke.sh | 1705 | 38b36b51b3acadab2f25b01ccf8cccc49c8b992ef1414ea13087a99db94ee879 |
| research/ui/native-pty.c | 6939 | 38eea7ddfaaa6a73a7f32696e60094724978aedb6a6fca90437b32304536f505 |
| Nonbootable source-final.iso | 921600 | c9bd500574110a8b31dac6127d9eabb56a11487c586614162e9375f6d29d0169 |

The host built the read-only transfer with hdiutil makehybrid -iso -joliet from
only those three files and a SHA256 manifest. Guest sha256 checks bound transferred
sources before copying and native compilation. No host binary was transferred.
Native system: OpenBSD 7.9 GENERIC #399 amd64, base Clang 19.1.7,
target amd64-unknown-openbsd7.9. In the owned guest source directory:

```sh
cc -std=c99 -Wall -Wextra -Werror -pedantic plain-ui.c -o plain-ui
cc -std=c99 -Wall -Wextra -Werror -pedantic native-pty.c -lutil -o native-pty
sh native-smoke.sh /root/ui/plain-ui
./native-pty /root/ui/plain-ui
```

Both native compiles were warning-clean. Native plain-ui was 14824 bytes,
SHA-256 057fcae1fd9de0115e00d2faee10642f34d1d0b071edf14df8d590311d871da5;
ldd reported only base libc.so.103.0 and ld.so. Native native-pty was 14448 bytes,
SHA-256 1bc8e9441ae785ad7ffe78547c9f48421d4ff71310ed4c6eb7f74738e5b44d0c.
Base libutil is a test-only dependency. Dynamic executable size is not complete
ramdisk footprint or a reproducible-build claim.

## Failure that drove implementation

The original Phase A source compiled natively and passed all seven pipe cases.
PTY TERM/INT/HUP/WINCH and supervisor restoration after forced kill failed exact
termios comparison: canonical restoration left PENDIN set. Reapplying saved flags
immediately after flushing did not fix the native failures.

Authenticated 7.9 sys.tar.gz was verified using the accepted ADR 0002 verifier/key
against the previously authenticated release source SHA256.sig. Archive: 68411189
bytes, SHA-256 c9ef294021ef7aafd5f18ffe8ebfed63394e5a86d65d7436d6db78551f9d57f1.
Selected sys/kern/tty.c SHA-256:
dc979a14356742c47b985381bdcdac346b6aebc77d801883285b0dad4fcba39d.
Read-only inspection found canonical-mode transition sets/preserves PENDIN;
FREAD flushing does not clear it, while FIONREAD runs pending processing.
The first attempt sought this file in src.tar.gz; the kernel is in sys.tar.gz.
No kernel build or upstream code execution was used for that inspection.

Processing the emptied queue with FIONREAD fixed the first PTY failures, but a
direct serial completion still exited 3: its saved shell tty already had PENDIN
set. Final cleanup restores saved attributes, flushes owned input, calls FIONREAD,
reapplies the original snapshot, and compares every saved field/speed. It neither
masks PENDIN in the comparison nor performs a blocking read. The permanent native
regression includes initially set PENDIN. The supervisor uses the same checked
out-of-band sequence for its owned PTYs after forced kill.

## Actual final outcomes

| Fixture | Observed outcome |
|---|---|
| Seven native pipes | Valid, CRLF, cancel, incomplete review, ESC, NUL and help/back all passed |
| Twelve native PTYs | Complete, initially set PENDIN, 40x12, unknown dimensions, overflow/control cancellation, idle timeout, TERM/INT/HUP, resize/WINCH and forced-kill supervisor restoration all passed |
| Direct com0 socket console | Completion at declared 80x24 and 40x12: exit 0, exact stty -g before/after match, zero application ESC bytes |
| VGA pc0 keyboard | Declared tty 80x24, fixed example/alpha/complete input; complete review and SIMULATION COMPLETE visible, subsequent echo of exit status was 0 |

PTY child peak RSS was 908–976 KiB, under the 32 MiB ceiling. Idle timeout was
10068 ms; other cases took 22–61 ms. Per-case deadlines were 15 seconds and output
was bounded below 64 KiB. TTY echo was checked disabled during input; exact saved
attributes/speeds and noncompletion were checked on cancellation. Forced kill
restoration belongs to the supervisor, not the killed application. PTY output is
captured through a pipe; direct serial/VGA observations supply console evidence.
Serial declared speed was 9600; the private socket is not physical baud evidence.
The VGA framebuffer was 80x25 while tty dimensions were explicitly declared 80x24.

Startup, idle CPU, independent input-to-redraw latency, media growth, static linking,
physical serial, ANSI/curses comparison, secret-field semantics and destructive
confirmation freshness were not measured or qualified. This simulation consumes
queued input and supplies no installation authority. No ramdisk UI was built.

## Provisioning, observations and cleanup

The [reviewed QEMU/firmware/verifier identities](../design/native-profile/preflight.md)
were rechecked at each launch. One CPU, 1024 MiB, TCG pc-i440fx-11.1/qemu64-v1;
no NIC, shares, passthrough or additional disk. Paused QMP block/PCI/device and host
open-file/listener inspection preceded execution. The sole guest target was wd0,
8192 MiB / 16777216 sectors, matching the owned host file inode/device/size receipt.
Only that confirmed target was partitioned by the stock installer: whole-disk MBR,
auto layout, FFS root/usr/home and swap. Private synthetic credentials and captures
were excluded from Git and public evidence.

Stock ISO sets lacked SHA256.sig. The verification-bypass prompt was answered no.
A fixed-member reader extracted only bsd, bsd.rd, base79.tgz, comp79.tgz, man79.tgz
and INSTALL.amd64 from the authenticated ISO; accepted host signify verified all
six against the signed release manifest. A derived read-only nonbootable sets ISO
included those files and SHA256.sig: 672909312 bytes, SHA-256
d32647df00333236a495c32922ac3bdaa226875de90412c7d41c441feba76ba3.
Guest signature verification and all selected set verification/extraction passed.
The first optical discovery failed, then read-only disklabel inspection and a
subsequent discovery succeeded. Some later transfer swaps initially reported no
medium; the next read-only mount succeeded. Their cause is not asserted.

Stock install postconditions were independently observed: disklabel/filesystems,
installed kernel/compiler and boot.conf, then disk sync and OS halt. Installation
run took 879.68 seconds. Post-install target SHA-256 was
3397884c26cecc43ef0b1c260b36e00b890aae347b775755245a9e2303852c73;
2410201088 allocated bytes. That observation occurred with the next launch paused,
after the installer emulator was absent. The installed guest booted successfully.
SSH/X startup was disabled; ordinary stock daemons started. Offline firmware/update
DNS attempts failed because the fixture had no NIC/DNS; no live network changed.

The serial build run took 754.90 seconds. Halt was requested, but QMP quit happened
before completed sync/halt was observed. The subsequent VGA boot reported an
unclean root filesystem; automatic fsck checked root/home/usr and marked them clean.
This is an observed abrupt stop, not a clean-shutdown claim. The final VGA run
completed in 411.82 seconds; disk sync and OS halt were visibly observed before
QMP quit. All runs stayed within 60 minutes; fixture allocation remained below
20 GiB and each private capture below 10 MiB. Final disposition is recorded below after
process/endpoint absence checks; destructive provisioning has no rollback claim.

Final teardown observed all three owned emulator PIDs absent, no open fixture files
and no remaining socket endpoints; console clients had exited. Captures were
940257 / 50642 / 894161 bytes for install/build/VGA. The disposable credential was
absent from all three console captures. Final target allocated 2426945536 bytes,
SHA-256 2c85a82dd334cdd9d72f9bccda68dfb3d74428d9da2acacd250468eb69f86f26.
The stock install79.iso remained unchanged at SHA-256
7a4a92e953618035097c796a90b54424a0f3ae775552e1e7d102cf8a5130449f.
The owned fixture directory, disk, credential, transfer/sets images, captures and
endpoints were removed; absence was checked. Only a sanitized ignored receipt and
separate authenticated upstream inputs were retained. No binaries/images are
redistributed, host services installed, or rollback promised.
