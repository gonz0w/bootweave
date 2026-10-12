# Confirmed stock-installer handoff research

Original removable C experiment under the [accepted scope](../../docs/research/installer-handoff-spike.md).
The native executable observes only the reviewed OpenBSD 7.9 amd64 sd0 fixture,
reviews fixed effects, requires fresh `erase <displayed DUID>` confirmation,
reobserves all reviewed fields and restores the terminal before attempting one
fixed `/install` exec with a fixed environment. The stock installer remains
interactive and supervised. This is not a production operation adapter.

The existing storage validator is reused without modification. Metadata and input
never become executable paths, device paths, shell commands or environment values.
The separate host driver has a synthetic observation callback and no execution
backend. The production host stub exits 77; arguments are rejected.

`make test-handoff` checks pure comparisons and owned host PTYs. These results
qualify tooling only. `build.sh` runs solely in the separately approved disposable
native build guest, verifies frozen inputs, builds a single static executable,
runs pure tests and repacks the stock RAMDISK_CD reserve without enlargement.
Its profile disables the stock initial menu and automatic entry paths, records
one attempt and prepares only RAM directories before review.

Input is printable ASCII, bounded to 1024 bytes per line and 64 KiB work, with
10-second line/inactivity and 60-second attempt deadlines. Output is bounded below
64 KiB with five-second write deadlines. Quiet queue checks reject queued multi-line
contamination; this does not establish physical presence or distinguish every
single-line paste. Help, back, cancellation, signal, resize, failed observations
and terminal-restoration failures cannot grant handoff.

Known dimensions below 40x12 reject; unknown serial dimensions are explicitly
unqualified. Reviewed text wraps to 40 columns. Capacity/DUID cannot establish
host attachment identity or emptiness. Host identity inspection and writable-target
approval remain separate. Stock writes have no general rollback; an interrupted
install remains partial or uncertain. Synthetic stock password handling retains
the accepted upstream plaintext-argv research limitation.

The [native preparation stop](../../docs/research/installer-handoff-stopped-evidence.md)
is recorded separately. Under the accepted
[corrected-run amendment](../../docs/research/installer-handoff-retry.md), the
[native evidence](../../docs/research/installer-handoff-native-evidence.md)
records 19 native PTY cases, 19 serial/VGA rejection cases, two verified stock
installations and offline OS boots, partial-state observation and full disposal.
VGA shutdown interrupted stock background kernel relinking; a subsequent kernel
boot is unqualified. These results remain bounded research.
`native-pty.c` is an owned research supervisor with a fixed synthetic driver,
base libutil and no installer backend; it is not packaged in the runtime image.

Within a separately approved disposable OpenBSD guest, from this repository, the
fixture-only terminal checks use:

```sh
mkdir -p /root/handoff-check
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -Iresearch/storage \
    research/handoff/host-driver.c research/handoff/console.c \
    research/handoff/snapshot.c research/storage/validate.c \
    -o /root/handoff-check/driver
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic \
    research/handoff/native-pty.c -lutil -o /root/handoff-check/native-pty
/root/handoff-check/native-pty
```

The supervisor checks exact native terminal flags, deadlines, signals, forced
death, terminal loss and fresh/contaminated confirmation using owned PTYs.
It never runs the stock installer or accepts a caller-selected executable.
