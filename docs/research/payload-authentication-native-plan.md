# Native payload authentication experiment

Proposed, 2026-10-12. Owner/acceptance authority: Cam; acceptance pending.
Base: 89f21b0a6225b329f6a1605ad536d159b28c8c3a, merged PR #44.
Related [issue #5](https://github.com/gonz0w/bootweave/issues/5),
[accepted host experiment](payload-authentication-spike.md),
[host evidence](payload-authentication-host-evidence.md) and
[parent Test Design](../design/project-trust/TDD.md).

## Intended result and authority

Build original bounded C research that authenticates a retained synthetic payload
with OpenBSD 7.9 base signify, then feeds those same bytes to the unchanged native
inspector. Use base-libc SHA-256 for independently expected identity, fixed trusted
context and fixed subprocess argument arrays. Python remains host tooling; neither
this mechanism nor a runtime language, public API, signer policy or profile syntax
is adopted for the installer. No extraction, application, hook or product service
execution is included. This is the next native component experiment, not acceptance
of the proposed parent trust design.

Acceptance would authorize original controller/test implementation, a nonbootable
read-only data CD, two disposable guest-generated test key pairs and synthetic
signing, and writes on the **one exact target below** for stock provisioning, offline
compilation and test fixtures. No other disk, live network, dependency installation,
custom crypto, host-port redistribution, custom boot media or release signing.
Previous native targets were disposed and supply no authority here.

## Prepared target and inputs

The sole target is .tools/payload-auth-native/build.raw: an exclusively created
regular file, mode 0600, under an owned 0700 directory, 8589934592 bytes with zero
allocated bytes. Initial SHA-256:
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25.
The immutable private preparation receipt binds host device/inode, mode, size,
allocation and initial digest. Receipt SHA-256:
77db7bfff06f3615071ca9639243ac7e85a544b77066593e4e8dd8fa353416db.
Its preparation flags are guest started=false, credential created=false and guest
write authorized=false. Keep them historical; record later acceptance and performed
effects separately. Recheck receipt and complete target bytes before first launch;
subsequent boots bind the same device/inode to observed post-write identities.
Missing identity, substitution or uncertain state stops dependent work.

The exact installed QEMU/firmware pins matched the
[reviewed platform](../design/native-profile/preflight.md). The host verifier,
release key and unchanged install79.iso matched
[ADR 0002](../adr/0002-host-artifact-verifier.md), then signify -C with explicit
key and selected filename again returned Signature Verified and install79.iso: OK.
Fixed-member read-only ISO inspection copied only bsd, bsd.rd, base79.tgz,
comp79.tgz, man79.tgz and INSTALL.amd64; all six separately verified against the
authenticated release manifest. No mount or stock source execution occurred.

Private staging contains 76 files, 675765274 bytes: those six release inputs and
signature/derived hash manifest, unchanged inspector and existing native harness,
35 frozen historical archives, and 26 additional alias archives with manifests.
The complete staged-file manifest SHA-256 is
3ef21e8be8cbe27cedd88faed11d3c2c30ea6c3a20d651bd09a993654dfaa863.
Every staged file was read back and matched. Historical cases.json remains
052f68eeb8c267e6efe4c44ba6c233efc94b9574fed97a6dcaf1b5985b7f2b23;
the inspector remains 9237 bytes, SHA-256
8009aed82f67ae60c70538579fa493995a87fa6ff1ea324dc23ee79af0847698.
Alias-manifest SHA-256:
e7fb400fb68a1d9ebf3bb1d123e287f86fd543b9e4ba2c1465994a62df44207d.
Six distinct raw headers each admitted alone with an unrelated regular file; all
20 ordered alias pairs rejected with the expected tree-conflict result in host
checks. This preparation does not close their native gap.

No data ISO exists. Automatic approval review rejected its creation because this
new native preparation scope and exact target lacked explicit reviewed confirmation.
No workaround was attempted. The proposed acceptance explicitly includes assembling
that ISO after approval, adding only reviewed original controller/test sources,
scripts and further bounded synthetic authentication cases. Preserve the existing
stock inputs, inspector and 35 historical archives byte-for-byte. Seal a complete
superseding manifest, record ISO size/digest, and independently read back every file
and the complete inventory; require no boot record and size below 1 GiB. Stop before
launch on mismatch. This is host data transport, not native installer media.

## Release-specific source facts

The existing release src.tar.gz retained SHA-256
fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b;
its signed manifest retained
8c23e45e4c09939151f119eddc3619a7fa2f09c8c745e00bfa4dfe46ddb09900.
The exact reviewed host verifier again verified the signature and selected source
archive under accepted ADR 0002. Six bounded regular-file members were read into
fixed private review filenames, without general extraction or execution:

| Release member | Revision | Bytes | SHA-256 |
|---|---|---:|---|
| usr.bin/signify/signify.c | 1.137, 2025-05-20 | 21899 | 5c4297e804888d468a6bf28311081afc8ec71238e3590df4a4aa28dc21becfc9 |
| usr.bin/signify/Makefile | 1.12, 2017-05-28 | 261 | 36e14baf58f39472c0d2944de7ba1cb234b58c32dce65dd08fbf7814f5801870 |
| include/sha2.h | 1.10, 2016-09-03 | 7406 | 533cc52fd2f35af94a46e80cbae6bc887c7e88e3eef5d11396d382f3da29306e |
| lib/libc/hash/sha2.c | 1.28, 2019-07-23 | 27640 | 1df139d758a1d36308ed7f944890e73ad406612e5fa3ca6681a8148e9c344df1 |
| lib/libc/hash/Makefile.inc | 1.25, 2019-08-30 | 1353 | 75416157c06bf11b2e784333ba612cda9625e8bf66c459afc866ed896040ac80 |
| lib/libc/hash/SHA256Init.3 | 1.4, 2025-06-13 | 9686 | 3fb7bddbe3200085fa3d89a172554dc0969b27b5b42897a903f7177e1510ddeb |

signify's xopen duplicates stdin for message `-`; readmsg reads to EOF with a
1 GiB ceiling, so the controller must impose its own smaller cap first.
verifysimple reads the message and detached signature and calls readpubkey with
the explicit selected key. The comment-based hint is consulted only when that
explicit key is absent. Detached verification reduces pledge to stdio/rpath;
this is signify's own source behavior, not controller confinement evidence.
Its Makefile links base libutil. The SHA-256 declarations and libc hash Makefile
provide base hashing without another crypto dependency. These are authenticated
source observations, not native transport or linkage passes.

The [7.9 signify manual](https://man.openbsd.org/OpenBSD-7.9/signify.1) distinguishes
detached verification from embedded output, explicit key selection and checksum-list
mode; it does not establish the generic stdin transport used here. The release
source supplies that prerequisite; real positive/negative native transport tests
must still pass before controller cases. Stop on unsupported behavior instead of
materializing a new payload-file fallback or modifying cryptography.

signify.c retains Ted Unangst's ISC notice, sha2.c Aaron Gifford's BSD three-clause
notice, and the SHA-256 manual Todd Miller's ISC notice. Reviewed upstream files
stay private and are not copied into Git or custom media. Use the verified stock
base tools/libraries in the disposable guest; record actual binary identities,
link maps and dependency notices. This limited source review is not a crypto audit,
exhaustive base-system redistribution review or framework dependency adoption.

## Platform, permitted stock writes and secret limitation

Use the previously reviewed local-only QEMU 11.1.1, pc-i440fx-11.1, qemu64-v1,
TCG, one CPU and 1024 MiB. Rehash executable and selected BIOS/VGA before launch;
keep [provenance/license limitations](../design/native-profile/provenance.md).
Only raw writable ide-hd at ide.0 unit 0 is the confirmed target. Bootstrap optical
is unchanged authenticated install79.iso, raw read-only ide-cd at ide.1 unit 0;
the frozen data CD is raw read-only ide-cd at ide.1 unit 1. Use private Unix QMP/
serial endpoints and local VGA. No NIC, IP listener, sharing, passthrough, overlay,
hot-plug, reopen or extra disk. Pause with -S before CPU execution and inspect full
block/device trees, effective arguments and process files/listeners against this
inventory. Observe guest wd0 and exact capacity before any destructive answer.

Start in the stock shell; mount only the read-only data CD and verify all six
release files with native signify and the stock release key before installation.
Unmount, manually enter the fixed stock installer and select cd1's verified sets.
Permit whole-disk unencrypted MBR and automatic FFS layout on that one target,
synthetic .example.invalid hostname, UTC, no interface/DNS, SSH/X disabled, no
additional user and no game/X/site set or install.site. No autoinstall, verification
bypass or inferred answer to an unexpected prompt. Stop on any changed inventory,
failed verification or unexpected question. Partial disk writes have no general
rollback, automatic retry or resume.

After acceptance only, generate a fresh disposable stock root credential and
submit only at the observed echo-off password prompt. The stock installer encrypt
plaintext-argv limitation is explicitly requested for this isolated synthetic guest,
as in earlier accepted experiments; it does not satisfy the proposed production
secret channel. Keep it out of host argv/environment, ordinary logs, tracing and
PR evidence, check private captures for exclusion and dispose private input files.
The guest test keys are also disposable secrets, separate from this credential:
base signify -G -n creates two owned pairs; signing takes private key-file paths,
never key bytes in argv/environment. No test secret grants production authority.

After provisioning and clean halt, offline build boots use only the same target
and sealed read-only data CD. Observe background kernel relinking finished before
planned halt; record interruption and omit subsequent-kernel claims if it did not.
Stock base startup is permitted only inside this no-NIC guest with SSH/X disabled.

## Controller and test engineering

Implement original BSD-3-Clause C with base libc, plus fixed build/test scripts.
New controller and test C combined ceiling: 64 KiB; inspector policy/source stays
unchanged. Build with base cc, C99, Os, Wall/Wextra/Werror/pedantic, recording
dynamic/static results and actual maps. No copied hash/signature implementation,
new package, host binary or Python runtime in the guest. Non-OpenBSD execution
refuses before effects; host checks establish tooling only.

Trusted test context supplies expected length/SHA-256 and selected public-key
bytes independently of payload/signature input. It is an internal research call,
not a serialized contract. Reject executable/path/operation arguments and
payload-controlled environment options. Bound key/signature to 2 KiB each and
validate the exact bounded two-line records before dependent execution. Comments
cannot select a key, command, option or output text.

Capture stdin once into one retained buffer: 1 MiB cap, EOF required, ten-second
deadline. Compare length and base SHA-256 against trusted expectations before
authentication. Do not reopen a payload filename or recapture stdin. Copy the
selected public key and supplied signature into exclusive 0600 files in a fresh
owned 0700 directory under /root/payload-auth. Keep directory identity/mode,
fixed-file identities/modes/contents and exact inventory as checked postconditions.
Feed retained bytes to fixed /usr/bin/signify with -V -q, explicit -p/-x and -m -.
Only zero exit with no stdout/stderr permits inspection. Then feed the same buffer
to the fixed source-bound native inspector; exact bounded numeric summary and
successful exit/cleanup are required for completion. Never print verifier output,
archive text, comments, key material or untrusted names. Use fixed failure categories.

Recheck owned executables and stock-tool identities before use, use fixed argument
arrays and a minimal fixed environment with no shell or PATH lookup, close unrelated
descriptors, supervise nonblocking pipes and reap every child. Fifteen-second child
and 45-second workflow deadlines, 4 KiB aggregate stdout/stderr, CORE=0, CPU=15 and
FSIZE=1 MiB for owned children; native DATA limits and independently measured RSS:
64 MiB controller, 32 MiB child. RSS measurement is an observed completion ceiling,
not a claim that RLIMIT_RSS is a hard allocator limit. Missing metrics block the
corresponding pass. Preserve saved descriptor flags and terminal state. Signals,
stalled/full/broken pipes, resource or cleanup failures cannot yield completion.

Trusted guest code, base tools and private paths are the boundary. This does not
claim resistance to another root process, immutable executable references,
privileged key-file races, general pledge/unveil confinement, debugger/memory
protection, physical secret erasure or safe extraction. Explicitly report those
gaps; do not widen this experiment into production privilege separation.

## Native Test Design and completion

Run real detached stdin verification with correct and changed message bytes first,
using native-generated synthetic keys. Failure stops dependent tests. Controller
and supervisor are separate reviewed code; test-only fixed barriers/doubles cannot
be selected by archive input, command flags or runtime debug mode. Mock success
never counts as cryptographic evidence. Observe actual child launches, received
byte hashes, wait status/resource data and filesystem postconditions independently
of the controller's claimed result.

| Case | Required observation on serial and keyboard/VGA |
|---|---|
| PAN-01: identity and selected signature | Real base verification precedes inspection; exact summary and independent retained-byte observations agree. |
| PAN-02: altered valid archive with matching new identity but old signature; wrong identity, signer or corrupt/missing/truncated signature | No inspector launch/completion or unsigned fallback; bounded fixed failure. |
| PAN-03: controls, shell fragments and alternate key hints; oversized/malformed records/context | No execution, trust change or untrusted output; no dependent launch where preliminary validation fails. |
| PAN-04: authentic malformed archive | Authentication may pass; inspector rejects; no completion or member effects. |
| PAN-05: original payload mutated/replaced after capture/verification | Real verifier and inspector receive original retained bytes; changed source cannot substitute authenticated content. |
| PAN-06: empty/exact/excessive/unfinished input; argument/environment or tool-identity mismatch | Bounds/authority/deadlines hold; source and tool mismatch stops. |
| PAN-07: actual child exit, signals, stall, broken/full pipes, excessive output and controlled secret diagnostics | No false completion, secret/control output or leaked children/descriptors; limits and cleanup independently observed. |
| PAN-08: private directory/file identities, permissions, canary, inventory and lifetime | Altered postconditions reject; canary unchanged; no archived path opened/extracted; all owned keys/temporary files disposed. |
| Historical regression | Exact 35 unchanged archives plus existing native pipe/signal/output cases retain dynamic/static outcomes. |
| Directory-alias gap | Six independently admissible raw encodings and all 20 ordered conflicting pairs run natively, including slash/no-slash and prefix/name aliases. |
| Build/consoles/disposal | Release/compiler/source/binary/maps, independent resource observations, unchanged TTY state, fixed trusted serial/VGA summaries and complete owned-fixture disposal. |

Inspector ceilings remain 128 members, 512 KiB aggregate/256 KiB per-file content,
255-byte paths, 64 KiB inspector buffers and 1 MiB input. Authentication retains
that 1 MiB snapshot separately. Owned private authentication allocation: 20 MiB;
supervisor capture: 1 MiB/case. Guest limits: 15 minutes to prompt, 60 minutes/run,
10 MiB ordinary host capture and 20 GiB total owned host allocation. Enforce
external run limits as well as component deadlines; stop on loss of isolation,
resource control or observability. Do not enlarge a failed budget or add a library.

Run make verify on a clean signed/DCO candidate before native use. Freeze all new
source/script/fixture/media identities; verify native transferred hashes before
compilation/execution. Record planned, performed and observed state separately,
actual failures/skips/unknowns, tool/source/base/head/tree and sanitized results.
Stop owned children/guests/clients; observe absent handles/listeners/mounts; remove
task-owned keys, credential/input files, captures, staging, data CD and this target,
retaining only sanitized evidence. No disposal/pass claim without observed cleanup.

Approval covers this bounded experiment and exact target only. Routine test fixes
within its bounds need no repeated confirmation; fresh targets, extraction,
changed trust/effects/dependencies or increased budgets return to review. Parent
PTT-04/05, compatibility/freshness/rotation policy and full native support remain
open even if this component passes.
