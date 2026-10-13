# Authenticated payload placement: serial evidence and stopped qualification

Recorded 2026-10-12 local time; final observations 2026-10-13 UTC. Owner and
acceptance authority: Cam. Related [issue #5](https://github.com/gonz0w/bootweave/issues/5),
[accepted exact-target scope](payload-placement-spike.md) and
[parent Test Design](../design/project-trust/TDD.md).

## Outcome and source identity

Original C/base-libc research now plans the complete authenticated retained
snapshot and creates bounded regular data files/directories through a restricted
unprivileged writer. The serial console completed 192 placement checks, 128
native authentication checks and 90 frozen historical checks: 410 total, with no
failure or skip in those completed suites. Two earlier stopped placement attempts
are recorded separately below. Keyboard/VGA tests were not run. The 60-minute
watchdog stopped the serial emulator before planned shutdown postconditions
completed. This is partial native evidence, not complete placement qualification.

Base: 9ff2ad75d88a6bbbaf2162c941c517471f7a98d6, merged PR #47.
Final native source candidate: 40d6585e91a0325f817bcb8e42bb9e950c343263,
tree f768ed36539f12bcf55f46ec866619dd2ad89972. All five contributor commits had
verified deliberately trusted SSH signatures and author DCO sign-offs. Clean
make verify passed 71 local groups (22/13/3/10/23), zero skips, before execution.
Authentication/historical sources and fixtures were unchanged between their
serial builds/tests and this candidate. Evidence-only updates follow; final PR
head/tree, full-range signatures/DCO and clean verification belong in its receipt.

Native release/compiler: OpenBSD 7.9 GENERIC#399 amd64, base clang 19.1.7.
Platform: pinned QEMU 11.1.1, pc-i440fx-11.1, qemu64-v1, TCG, one CPU and
1024 MiB. No package or dependency was installed. Runtime adoption, a public
installer API, production signer policy and complete parent PTT-04/05 remain
unqualified. Host planner/cross-syntax checks establish tooling only.

## Confirmed stock provisioning and transport

The accepted sole 8 GiB target was verified by complete initial digest and
private host identity/size/mode, with paused full block/device/PCI, process argv,
handles and listener observations. Guest wd0 reported 16777216 512-byte sectors.
Immediately before destructive answers, another paused check confirmed the
unchanged zero-allocation target and zero observed writes. No NIC/IP listener,
backing chain, host sharing, passthrough, overlay or hot-plug was present.

Native explicit-key base signify verified the embedded stock release manifest;
its payload digest was 50bec66f28426a22b9c9436f6a87cf3e7029e636bb915ba1d0db80d638881b87.
All six fixed stock inputs matched before installation, and the stock installer
independently verified its selected sets. Approved manual answers wrote whole-disk
unencrypted MBR/automatic FFS and swap, bsd/bsd.rd/base/comp/man, UTC, a synthetic
.example.invalid hostname and no interface/DNS, SSH, X or additional user.
Observed root/swap/usr/home layout, credential-file metadata only, compiler and
stock nobody UID/GID 32767 agreed. The installer completed unique-kernel relinking,
matching installed/relink-copy hashes, then observed sync/halt before emulator exit.
A complete same-identity post-install digest bound the later serial boot.

Nonbootable read-only data media held exactly 212 files with independent complete
ISO9660/Joliet inventory and byte/hash readback. Initial provisioning ISO:
682027008 bytes, SHA-256 171b8206eec0765f0403b8a80ebd1fe2620eae7d1b08db1d06a5426beccfe076.
Serial ISO: 682029056 bytes, SHA-256 f1c8edd2c8ba5beb6807c4786c5d1f3e9b1cdc454b403f615a5abd8102ebcaea.
Separately sealed classifier and final fixture-correction ISOs were the same size:
b844851653181dffbb661c4397d104e886057dc55b142846aa2b1eb12f4fdbcd and
78de9ada389051d6c0bfe8439f7b1f68e16110f91c850d8bbbd3581e402df286.
Neither correction ISO was attached. Fixed bounded stdin frames transferred only
signed original C and its manifest, with base uudecode/gzip for the final frame;
native file hashes and complete source checks preceded compilation. The running
CD was never swapped. All media stayed below 1 GiB, with no boot record.

Stock offline firmware/syspatch name resolution failed as expected without a NIC;
no network fallback/readiness claim followed. Stock startup host keys were separate
from exactly two newly generated synthetic signing pairs. The same two pairs were
verified by private file identity receipts and reused across all serial binaries
and corrected attempts. No additional signing pair was created. The explicitly
accepted stock encrypt plaintext-argv limitation was confined to this synthetic
guest; the credential never entered host argv/environment or public logs/evidence.

## Original mechanism and independent observations

The default standalone authentication entry still refuses without its trusted
compiled context. Placement is a private compiled phase, with no shell, public
schema, payload-selected tool or configurable callback. It keeps the same bounded
owned bytes through expected identity, real explicit-key signify, unchanged
inspector, complete planner and writer. Original source path changes cannot
replace placed contents. No general extractor or archive executable runs.

The planner preserves frozen ustar rejection policy and additionally limits depth
to 16 and realized objects, including implicit parents, to 128. Input/content/file
ceilings remain 1 MiB/512 KiB/256 KiB. All planning and independent expected-content
checks precede member effects. Archive owner/mode/time metadata is never applied.

The child binds an empty held no-follow root, closes unrelated descriptors, clears
supplementary groups and drops real/effective/saved UID/GID to existing stock
32767. It observes privilege-regain denial, locks unveil for its held directory
and uses exactly stdio/rpath/wpath/cpath/fattr. Separate native prerequisite
children passed outside absolute/openat denial, closed-FD denial, locked unveil,
fixed stock true exec denial, AF_INET socket-creation denial, device-creation
and privilege-reacquisition denial before member effects. Socket probes never
bind/connect/send. No chroot or weakened restriction supplied a fallback.

Component-relative no-follow traversal, held identity/same-device checks and
exclusive final creation enforce fixed directories 0700/files 0600. File contents,
fsync/close results and final held directories/root are checked. A separate
read-only oracle compares trusted independently generated paths/hashes, exact
inventory, fixed owner/modes/link counts and root binding. Outside canary
identity/type/owner/mode/link count/size and all 14 bytes remain unchanged.
These are controlled disposable cases, not general resistance to another root
process moving hierarchies concurrently or proof of crash atomicity.

| Case | Serial observation; both dynamic/static placement variants |
|---|---|
| PPL-01/07 | Real stdin positive/negative crypto and eight confinement probes passed before members; dropped IDs/groups and locked restrictions observed. |
| PPL-02 | Admitted frozen ordinary/boundary, nested/empty/depth-16/128-realized cases matched independent inventory, bytes, owner/modes and canary. |
| PPL-03/04 | Wrong identity/signer, incomplete signature, inspector failure, frozen malicious cases and all 26 raw aliases retained results; depth-17/130-realized rejected before writers. |
| PPL-05 | Existing final, root missing/replaced/link/nonempty/mode changes, intermediate replacements and held-file substitutions could not report completion; existing bytes were not overwritten. |
| PPL-06 | Mutated/replaced original source after real verification still placed independently expected original retained bytes. |
| PPL-08 | Controlled short write retained one h byte; file close/fsync failures retained five hello bytes and exact two-object inventory. Setup/signal/loss/stall/output/diagnostic/controller interruption stopped dependent effects and reaped children; partial/uncertain state was observed before fixed cleanup. |
| PPL-09 | Fixed input/output/deadline/FD/node bounds, CORE=0, RSS/DATA/CPU/FSIZE limits, wait4 metrics on successful/killed paths and exact saved pipe-input flags passed; process-group absence and diagnostic suppression were checked. |
| PPL-10 | Serial historical/authentication regression and source/case cleanup passed. Keyboard/VGA, clean final halt and native key cleanup were not observed; full case remains incomplete. |

Historical checks independently observed unchanged actual serial TTY attributes.
Placement reads a real canonical TTY and verifies exact flags on each controller
input pipe; its worker metrics include killed-child cleanup, not an assumption
that RLIMIT_RSS proves allocator containment. Final supervisor usage is also
bounded/observed. No core file or skipped placement case was observed.

| Serial build/suite | Cases | Peak controller KiB | Peak child KiB |
|---|---:|---:|---:|
| Dynamic placement | 96 | 5336 | 5336 |
| Static placement | 96 | 4312 | 4312 |
| Dynamic authentication | 64 | 4288 | 4288 |
| Static authentication | 64 | 4252 | 4252 |
| Historical, both inspectors | 90 | Not this harness's metric | 1004 |

Placement took 122.36 seconds; authentication 265.62 seconds. Native fixture
allocation sampled 8596 KiB before test runs, below 20 MiB; final du was not
observed. Host owned allocation sampled 5153865728 bytes, below 20 GiB. Private
captures were 29743 bytes provisioning and 155342 bytes serial, below the 10 MiB
external cap. Per-case native output/capture bounds remained active. Provisioning
exited normally after 1810.08 seconds. Serial reached its hard 60-minute watchdog:
3600.19 seconds including guard polling/exit overhead; this is a stopped run,
not a within-budget clean qualification. No budget was increased.

## Builds and artifact binding

Fixed build scripts used base cc, C99/Os/Wall/Wextra/Werror/pedantic, dynamic/static
variants and actual link maps. Native ldd/readelf observations showed base libc
only for original code and no static interpreter/NEEDED entry; stock signify uses
reviewed base libc/libutil. No cryptographic implementation was copied into Git.
The unchanged inspector SHA-256 is
8009aed82f67ae60c70538579fa493995a87fa6ff1ea324dc23ee79af0847698;
the frozen 35-case manifest remains
052f68eeb8c267e6efe4c44ba6c233efc94b9574fed97a6dcaf1b5985b7f2b23.
The final independent native artifact manifest digest is
5aa6c901a508b011c21e27c0b19063455f234505b047228d024fb4ef5717b617.

| Original placement source | Bytes | SHA-256 |
|---|---:|---|
| placement.h | 1373 | a95cba5404a0c20186ddb1cba80bfedab10f023d993f2ed26ac3ea2f16c695ae |
| placement_plan.c | 4652 | b67886408c6d2a8c984e050907f2ed6f0dc8a64b6be24297284c83d1a2ade35d |
| placement_native.c | 17451 | 88d957a7ea9c932d484968a8c3d1de6d96ba131cf5b8408a028f1b8060d8f669 |
| placement_check.c | 18972 | 067a73cea3530ca1e372c496dd290f0a33234018bd9d003f6645ca66f6ebbe6d |

| Native artifact, relative to its fixed guest research root | SHA-256 |
|---|---|
| payload/inspect | 7f1c86f89cf61bc48a732259cec0aaae00fe95f12e0f74d199872bccd06785c8 |
| payload/inspect-static | 5fc1918f6a98bdae2b01b210110d5e2b5ea80a809e2389bee6d667788005aa8f |
| payload/native-check | af321c01010206624bd8c530218692c4bdc7941a72fc27d565ab7bc91c201200 |
| payload/inspect.map | 0bf8274bf7b4dc4a4328bdf9011cfd0fc4b4f96431e0e46849c37e0e5bf6850f |
| payload/inspect-static.map | 19851cecdb5fdaa169e0e540acf726c1c3ef278e50c0a194d81e4dc2196fded8 |
| payload-auth/inspect | 7f1c86f89cf61bc48a732259cec0aaae00fe95f12e0f74d199872bccd06785c8 |
| payload-auth/inspect-static | 5fc1918f6a98bdae2b01b210110d5e2b5ea80a809e2389bee6d667788005aa8f |
| payload-auth/auth | b0425bc8e440fab543c734d0200cc897a7481f8dcc263d59e3d31961afa83fa7 |
| payload-auth/auth-static | eb65e86cb2a5b19c1c3a3df2d5800c8987cbdd3df16a70ba42e2cd0921199a25 |
| payload-auth/auth-check | bcaec92d893d3b2ce602609069cb8f3881f27e920287ef20d0a9d8b11b85446c |
| payload-auth/auth-check-static | 574389aee843952a1b8e65a40c4f1c5bb120e47c435c645b8bcb5b07a0d2ca57 |
| payload-auth/inspect.map | 337147962ee8595418001755c8ce0f359d734e4a75056eb343fd2d427b7bdeef |
| payload-auth/inspect-static.map | 26b518f06856840f9c9e73d89c53ada22b885d0f4d3309f6b6840cdadeab9e6c |
| payload-auth/auth.map | cd12ba0934d722e8590369840d8a325752db56c7b9bf9a04f95ee6f875aa952b |
| payload-auth/auth-static.map | c59922db6ced17a71337799d697dc7f225833e045d9f95da26df5b09d7d3e245 |
| payload-auth/auth-check.map | e843156b058d1085407dab5083246708b802ae5524464120aaa729b3dac38ac2 |
| payload-auth/auth-check-static.map | ebd4efe67ce2217e01b650c394c60a6de7bfa5240d4d738cb0a6ff24fc2ee265 |
| payload-placement/placement-check | cdb5aecd727c4aeb49e54fb7847b952e9288a5b8694ab255dff2e0bff03765bc |
| payload-placement/placement-check-static | dcbe7ae2a8257dbc3ea043fe5a65bfd64495765213a726bec030590c785aab92 |
| payload-placement/placement-check.map | d6691e94003b3edd54ca906a18c56708db91da58d20b0b8f5c52e2c24b58a2d9 |
| payload-placement/placement-check-static.map | b3c085e170faa3e7297718513f1adcbdaf238607a95f2497081d3e3012452430 |
| stock base signify | f29045c0813293c5d3e6af36729eca0fb427c13b7e70d7201644ba1362b9ab2a |

Native fixed commands were sh auth_native_build.sh/auth_native_run.sh,
sh native_build.sh/native_run.sh and sh placement_build.sh/placement_run.sh.
Corrected placement variants were rebuilt with the same reviewed fixed cc argv,
then independently hashed and source checked. No-context/argument refusals were
checked by the unchanged authentication build harness; host-only refusals and
planner bounds are separate local tooling groups.

## Stopped attempts, tooling errors and disposal

At f58c68503c3d0ac9ce040000bc63db6a4c5d2149, two real crypto checks passed,
then the post-pledge unveil probe correctly received SIGABRT. The test classifier
accepted it only for probes four onward and incorrectly stopped probe three.
The root was independently empty and no member effect occurred. The correction
separately observes EPERM from the unveil lock before pledge and accepts the
pledged syscall denial. The [7.9 unveil manual](https://man.openbsd.org/OpenBSD-7.9/unveil.2)
documents the distinct lock/pledge behavior; native trace, rather than a mock,
supplied the observed denial. No promise or privilege was weakened.

At 1847ad1605a898a6262987c0f8efb991d62684de, confinement and preceding archive
cases passed; check 73/case 65 stopped at a partial-size assertion. The frozen
fixture is nested, while the harness assumed a flat readme path/count. Independent
readback found viewer/readme.txt with exactly one h byte, SHA-256
aaa9402664f1a41f40ebbc52c9993eb66aeb366602958fdfaa283b71e64db123,
and two created objects. Canary and process absence agreed; that case was disposed.
The final correction binds partial/pre-existing/held-file probes to the actual
nested fixture and targets the fsync failure after data-file bytes exist.

A manual static macro lost its C-string quotes; compilation failed before tests
and was repeated with the reviewed script's exact quoting. During the final slow
build, Ctrl-T only echoed, establishing no status result. Ctrl-Z suspended its
static compiler; interactive ksh continued the remaining command chain and printed
a premature build marker with the old static digest. That marker was rejected.
The observed compiler job was resumed with fg; independent no-compiler process
readback and four refreshed binary/map hashes preceded both completed variants.
These tooling events are not skipped/passed native cases. Stock RAM lacked some
installed metadata tools; fixed installed tools were used after provisioning.

The agent scheduled final shutdown too close to the hard deadline. Relink-log
completion and kernel comparison had succeeded, but final ps/du/canary/unmount/
sync/halt were not observed before the watchdog stopped QEMU. Emulator exit zero
is not a halt postcondition. The stopped disk state was treated as uncertain;
no later boot, media replacement, resume or VGA test was attempted.

All owned emulators/clients were stopped and reaped. No host fixture handles,
mounts or endpoints remained. The target's original identity/size/mode was checked
again before fixed host removal. Target, all attached/unattached data ISOs,
source/header staging, synthetic credential/input, raw captures/screenshots and
owned task helpers were removed, and absence observed. Native removal of the four
signing files/private lifetime receipt was not observed; their containing owned
disk was removed during host disposal. No native key-cleanup pass, physical secret
erasure, rollback or clean guest filesystem claim follows. Only sanitized evidence
and previously reviewed stock inputs/tooling remain; this target's authority ends.

Keyboard/VGA qualification, a clean final halt, complete native key lifetime
disposal and PPL-10 remain incomplete. Full parent PTT-04/05, production signer
rotation/revocation/freshness/compatibility, broad archives, external privileged
mutation, upgrade/resume, application readiness and runtime adoption remain
unqualified. A [proposed fresh rerun](payload-placement-rerun-plan.md) supplies a
separate reviewable target and reserves shutdown time without widening mechanisms.
