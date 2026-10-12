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
is recorded separately. No native prototype build, handoff or boot was reached.
The [corrected-run amendment](../../docs/research/installer-handoff-retry.md) remains proposed.
