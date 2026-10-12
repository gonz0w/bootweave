# Native payload inspection experiment

Proposed, 2026-10-12. Owner/acceptance authority: Cam; acceptance pending.
Base: 5255bee7ccaca8cdca462bb27565868d41a1e6d1, merged PR #39.
Related [accepted host phase](payload-inspection-spike.md),
[host evidence](payload-host-evidence.md), [source binding](payload-source-findings.md)
and [issue #5](https://github.com/gonz0w/bootweave/issues/5).
This proposal does not authorize guest launch or disk writes until explicitly accepted.

## Outcome and exact requested effects

Build the same original inspector with OpenBSD 7.9 amd64 base cc, then test its
bounded input, framing/path policy, output and descriptor cleanup natively.
This remains component research: no extraction, payload authentication, application,
hook, service, production parser or public format is adopted. Do not modify the
inspector's accepted admission policy or increase its budgets silently.

The sole newly owned target is .tools/payload-native/build.raw, exclusively created
0600 under an owned 0700 directory. It is 8589934592 bytes with zero allocated
bytes and SHA-256
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25.
The frozen private receipt binds host device/inode, size, digest and preparation
flags; its SHA-256 is
4726d734b110230dcffdf8efaaa79973a4d401376053c4763bfd5d116e56c274.
No guest has started, credential been created or guest-write authority recorded.
Previous experiment targets were disposed; their authority cannot cover this file.

Acceptance requests writes on **this one exact target only**: manual verified stock
MBR/automatic FFS installation, offline build boots, native compilation and owned
synthetic test/capture files. No separate install/extraction target is requested.
The inspector still reads stdin only and opens no archive path or target disk.
Recheck the receipt and target bytes immediately before first launch; subsequent
boots bind the same host inode/device to newly observed post-write identities.
Substitution, missing identity or uncertain state stops work rather than retrying.

## Frozen platform and installation inputs

The installed QEMU 11.1.1 executable and BIOS/VGA firmware were rehashed and matched
the [reviewed identities](../design/native-profile/preflight.md). Keep their
[local-only provenance/license limits](../design/native-profile/provenance.md);
no update, dependency install or redistribution is requested. Use pc-i440fx-11.1,
qemu64-v1, TCG, one CPU, 1024 MiB, the reviewed BIOS/VGA, private Unix QMP/serial
endpoints and local VGA. No NIC, IP listener, sharing, passthrough, overlay,
hot-plug, reopen or additional candidate disk is allowed.

Target: raw ide-hd, ide.0 unit 0, writable only after acceptance. Bootstrap optical:
unchanged authenticated install79.iso, raw read-only ide-cd, ide.1 unit 0. Data
transport: raw read-only ide-cd, ide.1 unit 1. Pause before CPU execution and inspect
the exact three attachments, raw/backing/access fields, full device tree, effective
arguments, process files/listeners and isolation. The
[saved block checker](../engineering/development.md) assists supplied-observation
comparison but does not establish freshness, provenance, target identity or approval.
Observe guest wd0 and its exact capacity before any destructive answer.

The pinned verifier/key/stock-media identities matched
[ADR 0002](../adr/0002-host-artifact-verifier.md), and signify reverified install79.iso.
Fixed-member read-only ISO inspection copied only bsd, bsd.rd, base79.tgz,
comp79.tgz, man79.tgz and INSTALL.amd64. All six verified against authenticated
SHA256.sig. No host mount or stock source execution was needed.

Start in the stock shell. Mount only the read-only data CD and verify the actual
six files with native signify and the stock release key before entering installation
or partitioning. Missing manifest/files or failed verification stops before writes.
Unmount, enter the fixed stock installer manually and select that same cd1 set
directory. No autoinstall/response file or verification bypass. Use a synthetic
.example.invalid hostname, no physical interface/DNS, SSH/X disabled, UTC, no extra
user, unencrypted whole-disk MBR, automatic layout and no extra disk/game/X/site
set or install.site. Unexpected questions stop input; do not infer defaults.

Generate a fresh disposable credential only after acceptance. Supply it only to
the observed stock password prompt with echo off; no argv capture, tracing, raw
input logging or public credential evidence. This scope explicitly requests the
same stock encrypt plaintext-argv limitation previously accepted for isolated
[handoff research](installer-handoff-spike.md), solely for this guest's synthetic
credential. It does not satisfy the proposed production secret-channel requirement.
Check credential exclusion from capture; restrict and dispose secret/input files.

## Prepared transport and bounded test tooling

Nonbootable verified-source.iso is 675778560 bytes, SHA-256
7326f783900963d64d2be1933080b6a76e3b604b15e3d912869028f001de5467.
The installed host hdiutil assembled this **data transport**, not native installer
media. Strict readback matched all 45 frozen source/set/fixture/manifest files;
no boot record was present. Host preparation does not qualify native mount/read
behavior. Native source/fixture/set hashes must match before compilation or use.

The inspector remains 9237 bytes with SHA-256
8009aed82f67ae60c70538579fa493995a87fa6ff1ea324dc23ee79af0847698.
There are 35 fixed synthetic case files: eight accepted and 27 rejected under the
current host implementation, totalling 2806787 bytes. Their expected outcomes,
exact bytes and digests are frozen; case-manifest SHA-256 is
052f68eeb8c267e6efe4c44ba6c233efc94b9574fed97a6dcaf1b5985b7f2b23.
Host checks matched every expected outcome without extraction. These are planned
native stimuli, not native passes or publisher-authenticated application payloads.

After acceptance, permit original base-libc C test supervision and fixed native
build/check scripts within this scope. They are reviewed executable test code,
not project configuration. No caller-selected executable, operation or path; use
only the fixed owned inspector/fixture locations and fixed environment/argument
arrays. The supervisor may fork/exec the inspector, connect only owned pipes,
send bounded bytes/signals and inspect child resource/descriptor/file postconditions.
It has no installer, archive extractor, network or device backend. Source ceiling
32 KiB, bounded buffers/captures, warning-clean host build and non-OpenBSD refusal
must be reviewed before native use.

The prepared transport may be resealed **only to add those reviewed original test
sources/scripts** after acceptance. Existing stock sets, inspector and 35 fixtures
remain byte-identical. Freeze all new source/script/dataset/ISO identities, retain
the superseding data-ISO size/digest and repeat complete readback before any launch;
the transport must stay nonbootable and below 1 GiB. No attachment replacement
within a running guest. Changed parser policy, set inputs, dependencies, disk
targets, media boot behavior or unreviewed test effects require a new decision.

## Native Test Design, budgets and cleanup

After verified provisioning/halt, inspect a fresh offline boot of only the same
target plus the frozen read-only data CD. Copy only reviewed test source/fixtures
into owned /root/payload; verify hashes before execution. Build a dynamic and
static inspector using fixed cc flags: C99, Os, Wall/Wextra/Werror/pedantic, plus
static only for that build. Record native release/compiler/source/binary/size/map
and base-libc dependencies. Do not package a custom bsd.rd or boot ISO. A linker
or footprint failure stops rather than adding a library or enlarging a reserve.

| Case | Required native observation |
|---|---|
| Build/source | Exact source/fixture identities, warning-clean base-cc builds, binary/hash/map/resource records; no host binary qualifies behavior. |
| 35 supplied archives | Eight exact count summaries; 27 nonzero rejections with no success/name/content output. Input bytes stay unchanged; bounded capture and independent peak RSS observed. |
| Pipe behavior | Valid input in partial writes; EOF/truncation; excessive declared body rejects while writer remains open; ten-second stalled-input stop. |
| Interruption/output | INT/TERM/HUP cannot complete; broken/full output cannot report successful completion; five-second output bound and exact saved native descriptor flags observed. |
| No member effects | Owned existing-file canary and directory inventory unchanged; no archived path opened/extracted or shell-fragment effect. |
| Consoles | Serial and keyboard/VGA show the same fixed trusted summary; TTY attributes unchanged. No interactive archive input, renderer or screen-fit qualification is claimed. |
| Teardown | Children/guest/clients stopped, handles/listeners/mounts absent, secret/output exclusion checked, owned disk/captures/transport removed and sanitized evidence retained. |

Retain the inspector's 1 MiB input, 128 members, 512 KiB aggregate/256 KiB per-file
content, 255-byte path, 64 KiB application buffers, 32 MiB RSS and 4 KiB output
ceilings. Supervisor: 15 seconds/case and 1 MiB private case capture. Guest: 15
minutes to prompt, 60 minutes/run, 10 MiB ordinary capture and 20 GiB total owned
host allocation. No large artifact export is needed. Enforce limits externally
as well as observing the inspector; unavailable metrics remain unqualified.

Stock base startup is permitted only inside this isolated build guest, with SSH/X
disabled and no NIC. Before planned halt, observe stock background kernel relinking
finished or record interruption explicitly; no later-kernel boot claim follows
from an earlier boot. On loss of isolation, verification, resource control or
postconditions, stop dependent work and preserve sanitized failure evidence.
Partial stock writes have no general rollback, automatic resume or retry.

Publish actual pass/fail/skipped/unknown results with clean source/base/head/tree,
commands, artifacts, limits, failures and complete disposal. Full payload trust,
extraction containment/races, installed application readiness, runtime adoption and
broader native-profile support remain outside this experiment. Routine bounded
test corrections need no further permission after acceptance; fresh targets or
wider effects do. Until acceptance, preparation receipts remain unchanged with
guest-start and guest-write flags false.
