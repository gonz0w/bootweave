# Authenticated payload placement experiment

Accepted, 2026-10-12. Owner/acceptance authority: Cam. The user's “yes,continue”
accepted implementation/native testing on the exact target, stock provisioning,
data ISO, disposable keys and documented synthetic credential limitation after PR #47.
Implementation base: 9ff2ad75d88a6bbbaf2162c941c517471f7a98d6, merged PR #47.
Preparation base: b7c97ea320fe40cb876ee0db49cb6ef4b4a67af2, merged PR #46.
Related [issue #5](https://github.com/gonz0w/bootweave/issues/5),
[parent PRD/EDD/Test Design](../design/project-trust/PRD.md) and
[completed native authentication](payload-authentication-native-evidence.md).
This acceptance authorizes only the bounded effects below; it does not accept
the parent design or introduce a production installer interface.

## Outcome and bounded authority

Build original C research that places authenticated synthetic regular files and
directories into one fresh, private, empty guest directory. Prove that rejected
archives create nothing, accepted contents/owners/modes match independent expected
observations, and interruption or changed containment cannot yield completion.
This extends the existing retained-byte authentication/inspection experiment into
bounded file effects, a narrow slice of PTT-04/05. It does not accept the parent
design, production trust policy, a public installer API or general extraction.

Requested acceptance covers original implementation and fixed test harnesses,
a nonbootable read-only source/data ISO, stock provisioning and offline boots on
the one exact target below, two new disposable native test signing pairs, one
synthetic stock root credential, and private case directories on that disk.
It additionally permits the trusted guest harness to create owned directories,
set their fixed test owner, fork a writer and irreversibly drop that child's
privileges to the existing stock nobody UID/GID 32767 before member effects.
It permits fixed failure probes in the same child and controlled filesystem
substitution by the trusted guest test supervisor. No account is added or changed.

Every placement case gets a new directory under /root/payload-placement/work,
inside an exclusively owned root-only parent. The harness fixes its root identity
and independently expected inventory. Archive names never choose an OS target,
parent root, user/group, command, executable or policy. No existing deployment,
installed OS directory, application, hook, package, product service, live network,
custom boot media, release signing, new dependency, automatic install retry or
resume. Guest stock startup is allowed only inside the no-NIC disposable fixture.
No destructive installation rollback is claimed.

## Exact prepared target and boot effects

Sole target: .tools/payload-placement-native/build.raw, a new exclusively created
regular file under an owned 0700 directory, mode 0600, 8589934592 bytes,
zero allocated bytes. Initial SHA-256:
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25.
Immutable private target-receipt SHA-256:
f4a4b1321aa8566c42d953f955da1f38bf2e5bceff9943d4c0538298d3ac1672.
The receipt binds host device/inode, size/mode/allocation and complete digest.
Preparation flags are guest started=false, guest write authorized=false,
credential created=false, data ISO created=false and placement implemented=false.
Keep that history unchanged; record later exact acceptance/effects separately.
Prior targets were disposed and grant no authority here.

Installed QEMU/BIOS/VGA pins and authenticated stock install79.iso were rechecked
against the [reviewed platform](../design/native-profile/preflight.md) and
[ADR 0002](../adr/0002-host-artifact-verifier.md). Explicit-key signify -C again
verified the signature and selected media. Preparation executed no stock source,
mounted no media, and started no guest. No native placement pass exists.

After explicit acceptance, use QEMU 11.1.1, pc-i440fx-11.1, qemu64-v1, TCG,
one CPU and 1024 MiB. Sole writable raw ide-hd: confirmed target at ide.0/unit 0.
Bootstrap stock install79.iso is raw read-only ide-cd at ide.1/unit 0;
sealed data ISO is raw read-only ide-cd at ide.1/unit 1. Private Unix QMP/serial
and local VGA only: no NIC/IP listener, host sharing/passthrough, backing file,
overlay, hot-plug or reopen. Start paused; inspect complete block/device/PCI trees,
effective argv, process files and listeners before CPU execution. Re-observe exact
wd0 capacity and pause/revalidate unchanged target immediately before disk answers.
Any identity/isolation/prompt mismatch stops dependent work.

As in the completed authentication experiment, first use the stock shell and
read-only data CD to verify the embedded release manifest with native explicit-key
signify, then all six fixed stock inputs before installation. Permit manual
whole-disk unencrypted MBR/automatic FFS and swap, bsd/bsd.rd/base/comp/man,
synthetic .example.invalid hostname, UTC, no interface/DNS, SSH/X disabled,
no additional user, game/X/site sets or install.site. Stock installation must
independently verify the selected sets; do not bypass verification.
Observe installed layout/configuration/compiler metadata, completed kernel
relinking and clean sync/halt. Bind the same device/inode to complete observed
post-write digests for later offline serial/VGA boots. No source/data ISO swap
while the guest runs. Missing postconditions mean uncertain state, not retry.

Fresh synthetic credentials and exactly two guest-generated signing pairs begin
only after acceptance. The stock installer encrypt plaintext-argv limitation is
explicitly requested again for this synthetic guest, not a production secret
channel. Exclude the credential from host argv/environment and ordinary logs;
submit only at observed echo-off prompts, check private captures before viewing,
and dispose input/captures. Signing uses fixed private-file paths, never secret
bytes in argv/environment. Reuse the same two pairs across binaries/consoles,
verify their private identity receipt, then independently observe disposal.

## Engineering mechanism to test

Keep one bounded owned snapshot from capture through identity, real explicit-key
base-signify verification, unchanged inspector acceptance, planning and writing.
No reopen or pathname-selected recapture of original input. Refactor only private
research internals as needed to carry that buffer into a fixed compiled placement
phase after authentication and inspection; preserve the default standalone refusal
and original acceptance policy. There is no serialized request/schema, configurable
callback, operation flag, shell or payload-selected executable.

Before any member effect, an original bounded in-memory planner must validate the
complete snapshot and derive canonical paths, types, content offsets and lengths.
Compare its counts with the real inspector result and independently supplied
expected fixture inventory. Do not run general tar/pax extraction after a scan.
The planner enforces the frozen ustar encodings and tree/ownership/type rules in
[payload source findings](payload-source-findings.md). Links, special/extended
records, absolute/traversal paths, duplicate/alias trees, conflicting types,
unapproved metadata, padding/trailer/EOF failures and expansion violations reject.

Placement is intentionally narrower: at most 128 realized objects including
implicit parents, depth at most 16, path at most 255 bytes, input at most 1 MiB,
512 KiB total/256 KiB per-file content. Derive every implicit directory before
writing; excessive depth/realized inventory rejects before launch. Sort the
validated plan for parent-before-child creation; archive order cannot change
policy. Archive uid/gid/modes/timestamps are never applied. Fixed created owner:
32767/32767; directories 0700, regular data files 0600, no execute or special bits.

The trusted parent opens and validates the fresh root using O_DIRECTORY and
O_NOFOLLOW, confirms exact empty inventory/device/inode/mode and binds authority
to that directory reference. It creates only this root before authenticated
member effects and assigns the fixed child owner. Before writing, the forked
original child moves cwd into the bound root, closes every unrelated descriptor
and has only bounded pipes plus the authorized root descriptor. It installs and
locks unveil for that directory with rwc, clears supplementary groups, sets all
real/effective/saved group and user IDs to 32767, verifies those IDs, then reduces
pledge to stdio/rpath/wpath/cpath/fattr. Native prerequisite probes must confirm
outside-root opens, privilege reacquisition and forbidden execution/network/device
operations are denied. Network probes stop at socket creation, with no bind,
connect, send or packet; execution probes name only the pinned stock true tool,
never archive content. Unexpected success stops dependent work.
No member effect precedes complete successful setup;
unsupported restriction or denial observability stops without a weaker fallback.

Walk paths one component at a time with directory-relative operations;
open directories using O_DIRECTORY/O_NOFOLLOW, compare held identities and
same-device/type/owner/mode against the plan before use. Final regular files use
O_CREAT/O_EXCL/O_NOFOLLOW, never truncation or overwrite. Existing files,
directories, links or changed root/inventory reject; directories created earlier
by this writer are reused only when their recorded identities agree. Never pass
an archive-supplied multi-component path directly to open. Bound open descriptors
and close every owned reference. Observe write/close errors and identity, type,
link count, exact bytes/size/mode/owner postconditions; fsync owned files/directories
within deadlines before completion. Do not infer crash durability from fsync alone.

The privileged parent never follows archived paths to create members. Its separate
read-only oracle walks from the held target reference without following links,
checks exact final inventory/content hashes/ownership/modes and root binding, and
compares independent canaries outside the case root. Completion needs child exit,
reaping, bounded fixed output, oracle agreement and unchanged outside state.
Any failure reports fixed planned/submitted/observed counts and incomplete or
uncertain state. Partial files stay observable until fixed harness cleanup;
no automatic cleanup is presented as rollback or successful extraction.

## Evidence prerequisites and source facts

The [7.9 open manual](https://man.openbsd.org/OpenBSD-7.9/open.2) specifies
O_NOFOLLOW for the final component, O_DIRECTORY and directory-relative openat.
The [stat manual](https://man.openbsd.org/OpenBSD-7.9/stat.2) specifies held-file
fstat and directory-relative fstatat with AT_SYMLINK_NOFOLLOW. These require
component traversal and native race tests, not just lexical normalization.
The [unveil manual](https://man.openbsd.org/OpenBSD-7.9/unveil.2) documents directory
identity tracking and locking; [pledge](https://man.openbsd.org/OpenBSD-7.9/pledge.2)
restricts promised syscall classes. Their composition with held descriptors,
privilege dropping and mutation is a native prerequisite, not an established pass.
No chroot containment guarantee is substituted: its
[manual caveats](https://man.openbsd.org/OpenBSD-7.9/chroot.2) explicitly include
outside hierarchy changes and inherited directory capabilities.

The existing signed src.tar.gz, release signature, exact host verifier and key
again matched the accepted pins; explicit-key signify -C verified src.tar.gz.
Four bounded regular members were read to fixed private filenames without general
extraction/execution. SHA-256 identities:

| Release member | Bytes | SHA-256 |
|---|---:|---|
| include/unistd.h | 18064 | 4e5550917ee8d873109873960df3c5792ba0c513ee76aa7f5de71d97a802bdd8 |
| lib/libc/sys/setresuid.2 | 2884 | 37a777dd95ba6246c6e3fd49e6beb7ba07cb375ae05d8b943d91fc71111bfb3d |
| etc/master.passwd | 4124 | 957ff685f315fde20dc0f0f4e0c2478ca880754fb658f9365d2546c72317ee00 |
| etc/group | 1239 | ec3c88d6b88af4836f8707b70fc24aa95f62ced905db5073e6d5e5713fee8c4f |

unistd.h revision 1.113 declares group/user triplet getters/setters, setgroups,
pledge and unveil. setresuid.2 revision 1.10 describes both UID/GID triplets;
privileged setters can change all IDs, whereas unprivileged setters cannot choose
an unrelated identity. The stock nobody user and group both use 32767; nogroup
is different and is not silently substituted. Native installed account metadata
must agree before setup, without reading credential hashes. unistd.h retains the
Regents' BSD three-clause notice, the manual Sheldon Hearn's two-clause notice.
Private excerpts were removed after recording these facts; only their sanitized
hash receipt remains. No upstream source or host verifier is copied into guest media.
This is limited source/API evidence, not kernel confinement or license-audit proof.
The web UID manual failed retrieval; the authenticated release source supplied it.
An initial receipt reader assumed revision markers in every member and failed on
the account files; optional markers corrected this metadata-only preparation.

## Test Design, budgets and stop conditions

Use the unrelated offline document-viewer data fixture and owned synthetic
archives only. Expected inventories/hashes come from the trusted test supervisor,
not controller summaries or reparsing its claimed output. New test-only fixed
barriers/doubles cannot be selected by archive/CLI/environment. Do not execute
archived content, malicious hooks or downloaded programs. Confinement denial
probes are reviewed internal functions in separately forked disposable children.

| Case | Required serial and keyboard/VGA observation |
|---|---|
| PPL-01: prerequisites | Real stdin signature positive/negative pass first; fixed owner IDs, dropped saved IDs/groups, locked unveil and exact pledge behavior observed before any member creation. |
| PPL-02: positive placement | Authenticated ordinary/nested/zero-length/boundary data creates exactly independent expected files, bytes, owners/modes and no outside changes. |
| PPL-03: trust/identity | Tamper, wrong signer/identity, incomplete signatures or failed inspector never launch writer or create members. |
| PPL-04: archive/tree limits | All frozen malicious cases and 26 raw aliases retain inspector results; additional placement depth/implicit-parent limits reject before effects. Authenticated links/devices/special bits/traversal cannot acquire write authority. |
| PPL-05: root and path substitution | Changed/missing/nonempty/symlink root, pre-existing destination and intermediate symlink/file replacement fail closed; controlled barriers exercise changes before opening and after holding a reference. Outside canary bytes/inodes/owners/modes/link counts remain unchanged; observer-induced access-time changes are excluded. |
| PPL-06: retained identity | Source changed/replaced after verification cannot replace placed bytes; actual created content hashes match independent original expected data. |
| PPL-07: confinement | Known outside-file openat/absolute paths, retained unrelated descriptors, privilege regain and forbidden operation probes deny without effects; unexpected success stops all placement. Controlled rename probes must expose stale binding or deny use, never qualify escaped writes as success. |
| PPL-08: partial/failure | Controlled short write, close/fsync/setup failure, signals and process loss stop dependent effects, preserve exact observed partial inventory, reap children and never report completion. |
| PPL-09: resources/output | Input/output/FD/realized-node/deadline boundaries hold, core files are absent, wait4 usage is recorded for successful and killed children, missing metrics block passes, controlled secret/control diagnostics never reach ordinary output. |
| PPL-10: regression/disposal | Historical 90 and authentication 128 checks per console rerun; source/binary/maps, exact flags/TTY and canaries agree; all owned keys, files, guests, endpoints, captures, media and target removed with absence observations. |

Original new placement/controller/test C ceiling: 64 KiB additional, at most
128 KiB combined with existing controller/harness/inspector C. No copied crypto,
package, parser library or dependency adoption. Build base-libc dynamic/static
variants with C99/Os/Wall/Wextra/Werror/pedantic and actual link maps. Non-OpenBSD
and standalone no-context/argument execution refuse before effects. Host checks
establish tooling only and remain limited to nonprivileged owned temporary files.

Maintain 64 MiB controller/32 MiB child observed RSS ceilings and native DATA
limits, CORE=0, CPU=15, FSIZE=1 MiB; 15-second child and 45-second whole operation,
4 KiB shared output and at most 160 owned descriptors. Record wait4 usage even on
cleanup paths; do not infer RLIMIT_RSS allocator containment. Private fixture
allocation at most 20 MiB, capture at most 1 MiB/case. External guest limits:
15 minutes to prompt, 60 minutes/run, 10 MiB host capture and 20 GiB owned host
allocation; source/data ISO at most 1 GiB. Missing postconditions, failed
confinement, budget breach, observability loss or secret exposure stop dependent
work; no budget increase, weakened promises or unsigned fallback.

Run make verify on a clean signed/DCO candidate before native use. Freeze every
new source/fixture/manifest; preserve historical stimuli and original inspector
policy, seal and independently read back the entire nonbootable data ISO, then
verify native transferred files before execution. Native results record exact
base/head/tree, release/compiler/tools, build maps/binary identities, failed/skipped
cases, measurements, independent observations and remaining gaps.
Observe kernel relinking complete before planned halts. Stop/reap owned children,
guests and clients, verify no handles/mounts/listeners, remove keys/credential,
case roots and private inputs/captures/staging/media/target; preserve only sanitized
evidence. Physical erasure, crash atomicity, deployment/upgrade/resume, concurrent
external root mutation, application readiness, broader archives and general
privileged extraction remain unqualified. No missing parent case is declared done.

Acceptance must explicitly name this new implementation/placement authority,
stock credential limitation and exact fresh target. Ordinary fixes inside accepted
bounds need no repeated permission; other targets/effects, changed trust/dependency,
privileged mutation outside owned fixtures or increased budgets return to review.
The original preparation receipt remains unchanged. A separate private acceptance
receipt binds this approval to its digest and exact target; prior authentication
acceptance supplies no authority for another target or wider effects.
