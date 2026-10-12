# Confirmed-handoff implementation and stopped native preparation

2026-10-11 local / 2026-10-12 UTC. Development base:
e3045329f87551e4d9c3464295729082cbd95ee2, merged PR #34. Cam's
"yes,continue" accepted the [bounded handoff scope](installer-handoff-spike.md)
and four exact writable fixtures. Original C implementation is now in
[research/handoff](../../research/handoff/README.md). No prototype binary ran
natively, and no production installer boundary is qualified.

## Implementation and host checks

The front end uses the unchanged storage validator and the previously evidenced
read-only native observation mechanism. Its console reviews capacity/sector size/DUID
and fixed destructive effects, rejects queued multi-line input, requires a fresh
exact erase acknowledgement and compares every reviewed field on reobservation.
A second quiet-queue check rejects input arriving during reobservation. Exact terminal
restoration and native output-flag readback precede the sole fixed execve entry.
The production host stub returns 77; arguments never select a device or command.

Host tests compile original sources with Apple Clang, C99, warnings-as-errors and
pedantic diagnostics. Owned PTYs cover 80x24, 40x12, unknown dimensions, known
narrow/short dimensions, LF/CR/CRLF, valid fresh confirmation, help/back/cancel,
empty/control/high-bit/overflow input, queued multi-line input, changed/unavailable
observations, late queued input, signals/resize, idle timeout and loss of console.
All available terminal attributes/speeds and the changed O_NONBLOCK bit are checked
on the host. Native exact F_GETFL restoration, PENDIN behavior, actual device
observation, resource footprint and fixed exec remain untested in this candidate.

Local `make verify` passed 40 regression groups (14 foundation, 13 UI, three
storage, ten handoff), content/link/license checks, ShellCheck 0.11.0 and Gitleaks
8.30.1 with no leaks found. Host Python was 3.14.7 and Apple Clang was 21.0.0.
The handoff groups include the five-second blocked-output deadline and exact
input-terminal restoration. Pure snapshot mutation comparisons also passed.
These are host results; native behavior is still unqualified.

The native-only build script verifies its frozen source manifest and signed stock
sets before creating output, retains the stock ramdisk reserve, builds a single
static executable, runs pure comparison/storage tests and preserves stock installer
entry identities. It keeps modified boot-only bsd.rd separate from signed stock
bsd.rd and packages only the selected stock sets with SHA256.sig and INSTALL.amd64.
The profile prepares RAM directories and permits one attempt. These scripts passed
host lint; they did not run natively.

## Native stop and independent postconditions

One freshly inspected guest used pinned QEMU 11.1.1, TCG, pc-i440fx-11.1/qemu64-v1,
one CPU/1024 MiB, reviewed BIOS/VGA firmware, one approved writable IDE build disk
and the authenticated stock install79.iso read-only. Paused inspection established
exact attachments, raw format/no backing overlay, no NIC and no IP endpoint.
OpenBSD 7.9 amd64 RAMDISK_CD #396 reached its stock installer through com0.

Supervised answers used a synthetic .example.invalid hostname, no DNS server,
SSH/X disabled, no extra user, the observed sole wd0, no encryption, whole-disk MBR
and automatic FFS/swap layout. Four independently generated synthetic credentials
were held only in private fixtures; only the build credential was used at the
observed stock password prompts. No process argv inspection or tracing occurred
during credential handling. This retains the accepted stock encrypt plaintext-argv
research limitation rather than satisfying production secret-channel requirements.

The stock ISO was wrongly selected as the verification-ready set source. After
selecting only bsd/bsd.rd/base/comp/man, the installer reported that its directory
did not contain SHA256.sig. The ISO itself had authenticated host provenance;
the missing per-set manifest prevented the scoped native verification step.
The agent explicitly answered **no** to verification bypass, interrupted to shell,
and did not extract sets, retry partitioning, resume or boot the partial target.
This avoidable preparation mistake is failed evidence.

Independent disklabel/df checks observed MBR/FFS/swap layout and three mounted
filesystems. Explicit absence checks found no installed kernel, compiler or installed
password database. Before final sync, QMP recorded 90 target write operations /
2504704 bytes and zero optical writes. The guest unmounted its three owned target
filesystems, synced and halted; the owned supervisor then exited 0 after operator
stop. The console capture was 26654 bytes and contained none of the four synthetic
credential byte strings. This verifies that capture only, not every stock secret
consumer or hypothetical channel.

The build target's final whole-file digest was
9363926934822c6e240e42f8340e326423829940d717c60ecb848cc10cd5a118;
its host allocation was 9093120 bytes. The other five targets were never attached,
retained their exact initial whole-file digests and had zero allocation. Therefore
only the build target has performed writes, and it remains a partial installation;
no completion, rollback or application-readiness claim follows.

## Teardown and remaining gate

The owned guest and serial client exited. Host lsof found no fixture handles or
IP endpoints before removal. All six owned disks, credentials, private input queues,
endpoints and raw captures were disposed. A secret-free private outcome receipt
was retained separately; authenticated upstream inputs remain outside the disposed
fixture. No host service, live network, release or merge was performed.

Native static build/linker/ramdisk-footprint checks, native front-end negative and
signal/terminal cases, preconfirmation write observations, rejected host substitution,
serial/VGA confirmed handoffs, verified installs, offline first boots and the separate
interrupted-target case are **not run**. The build-preparation failure is not substituted
for any of those tests. The [proposed corrected run](installer-handoff-retry.md)
requires acceptance of fresh exact targets and the stated media/export changes.
