# Payload inspector native evidence

Observed 2026-10-12 under Cam's [accepted native experiment](payload-native-plan.md)
after PR #40 and explicit confirmation of its prepared 8 GiB target and stock
credential limitation. The dynamic and static builds completed 90 checks per console through
serial and keyboard/VGA: 180 final executions, with all
supplied cases and controlled failure checks passing. A directory fixture labelled
alias encoded identical headers; distinct on-wire directory-alias coverage remains
unqualified. The sole disk, credentials, captures and transports were disposed.
This is component research, not payload trust, extraction or installer acceptance.

## Source and platform binding

Implementation base: 961300244b0843681a40f5aa3a73e0cb1546db58, merged PR #40.
Final executed native test source candidate:
788875b01024736aeebb4e7555ef1229d18c1d65, tree
c81c902461db9ff179f6fb0aa3190e459443dafd. The evidence/reproducer changes after that
candidate leave the executed C and shell source bytes unchanged. Clean final
review candidate/tree, signed commits, DCO and verification accompany the PR.

| Original source | Bytes | SHA-256 |
|---|---:|---|
| inspect.c | 9237 | 8009aed82f67ae60c70538579fa493995a87fa6ff1ea324dc23ee79af0847698 |
| native_check.c | 11167 | cde7e5c896d6708f84554fe061a891efaa08ee7d7c9f61b674f00c85076e3903 |
| native_build.sh | 896 | 82da9974bbca7316e4bcda2b6c12182c9e90ccd7e78e1ab31a07ddab672b3eba |
| native_run.sh | 431 | 9ce1123157cb1e6a05d6de5e2c1ba9e6059a74e1aff9bbf71c7f6aa55ac5b6b4 |

The inspector's admission policy and bytes stayed unchanged throughout. All code
is original BSD-3-Clause research. Base libc and installed base cc supply the native
build; Python is host fixture tooling only. No new dependency or runtime choice
is adopted. C/shell source totals 21731 bytes, below the 32 KiB source ceiling.

QEMU 11.1.1, pc-i440fx-11.1/qemu64-v1, TCG, one CPU and 1024 MiB used the
[reviewed executable and firmware identities](../design/native-profile/preflight.md),
rechecked before launches. Each paused guest independently verified the full device
tree, exact raw/backing/access fields, effective arguments and process files.
No NIC, IP listener, sharing, host device, overlay, passthrough or hot-plug was
present. The one stable host disk identity bound initial and each post-write state;
replacement bytes/inodes were not accepted as the same target.

## Provisioning and transport observations

The accepted verifier/key policy reverified stock install79.iso and selected stock
inputs. The first inspected guest had only the approved writable IDE disk, stock
read-only optical bootstrap and nonbootable read-only data CD. Native wd0 readback
reported 16777216 sectors of 512 bytes; QMP showed zero writes before disk answers.

The stock menu reached VGA; a fixed shell redirection used its observed tty00 for
serial supervision. This shell reported no controlling tty/job control. Stock
MAKEDEV created missing wd0/cd1 nodes in RAM. Initial disklabel/mount calls found
missing nodes, and unsupported signify/checklist flags stopped at usage output.
No destructive answer followed those failures. Native ramdisk signify `-V -e`
verified the embedded release manifest, and six native file digests independently
matched its reviewed stock bindings before entering the installer. There was no
verification bypass or autoinstall input.

Manual prompt-by-prompt answers selected synthetic .example.invalid identity, no
DNS, SSH/X or extra user, UTC, unencrypted whole-disk MBR, automatic FFS/swap and
exactly bsd/bsd.rd/base/comp/man. Stock installation verified those sets again,
completed configuration and kernel relinking. Independent readback checked disk
layout, filesystems, hostname, SSH setting, boot-console settings and installed
compiler/account-file metadata. Password database contents were never read.
VGA showed completed disk sync and halt before the bootstrap emulator was quit.

Initial prepared data transport was 675778560 bytes with SHA-256
7326f783900963d64d2be1933080b6a76e3b604b15e3d912869028f001de5467, 45 files.
Adding reviewed test tooling produced 675794944 bytes/49 files, initially SHA-256
e8f32840f98c794093ad3f249ddd5d6cedd9da6fc473bd1409d7994a9b8a56b3.
Before the offline test boot, bounded failure cleanup/static-link inspection
corrections were resealed as SHA-256
2fa1f5ca39555981ed548ab68f98b0afe4286086ade831ad5afa1c3effba51a0.
Every readback matched all 49 frozen files; the original 45 inputs stayed identical.
The host assembled nonbootable data transport only, not installer boot media.
No running guest attachment was replaced.

Each offline boot attached only the same approved post-write disk and that unchanged
read-only test CD. Native source/fixture/manifest hashes matched before compilation.
Stock base startup ran inside the isolated guest; first-boot firmware/patch fetches
failed with no NIC/DNS. Serial boot left VGA unconfigured. A subsequent independently
inspected boot selected pc0 at the observed stock boot prompt and keyboard-booted
GENERIC to ttyC0, with 80x25 wsdisplay. Both boots used the same native binaries.

The first supervisor's sleeping producer timed out while feeding the accepted
512 KiB fixture. Numeric-only failure context identified binary 0, case 4,
exit 2, empty success output. Direct native inspection passed that fixture in
0.35 seconds. Replacing producer sleeps with readiness polling let the complete
suite pass without widening the inspector's deadlines or policy.

An unpaced diagnostic-source transfer lost console input and stopped before its
native hash/compile gate. The incomplete heredoc was cancelled; readback confirmed
the old source unchanged. A paced, bounded base64 channel transferred only reviewed
original test-source/manifest corrections. Their exact native hashes matched before
compilation. The final test manifest digest was
5bab319e48ecaf6ec9011f64d9b591bfa008b323b7265e666e86883660c4c10b.
These were bounded test corrections; stock inputs, inspector, 35 cases, target and
attached data CD were unchanged. Failed attempts remain failures, not passes.

## Native builds, footprint and checks

OpenBSD 7.9 amd64 GENERIC#399, base OpenBSD clang 19.1.7,
target amd64-unknown-openbsd7.9. Warning-clean C99 builds used
`-Os -Wall -Wextra -Werror -pedantic`, adding `-static` for the static inspector
and linker maps for both inspectors. The corrected supervisor was separately
rebuilt with those warning flags after native source verification.

| Native artifact | Bytes | SHA-256 |
|---|---:|---|
| inspect | 13360 | 7f1c86f89cf61bc48a732259cec0aaae00fe95f12e0f74d199872bccd06785c8 |
| inspect-static | 577672 | 5fc1918f6a98bdae2b01b210110d5e2b5ea80a809e2389bee6d667788005aa8f |
| final native-check | 19280 | af321c01010206624bd8c530218692c4bdc7941a72fc27d565ab7bc91c201200 |
| inspect.map | 10146 | 5c6aa0411cc7e4d7d9439ba0f48699b8ba5f7f858d1374cd53ccae7fd9eccdc4 |
| inspect-static.map | 173123 | 33772cd679cfff630e9bba54367e2e4420471f79cab8dca2fbb5332949416651 |

Dynamic inspector/supervisor dependencies were base libc.so.103.0 and ld.so.
Static readelf showed PIE without INTERP/DT_NEEDED; its map referenced base rcrt0,
crtbegin/crtend and libc.a only. No library was added or ramdisk reserve enlarged.
No custom ramdisk, boot image or native artifact export was produced.

For each console, the fixed supervisor ran 35 supplied archives on each binary,
plus ten pipe/signal/output/argument checks per binary: 90 checks. Native wrapper
commands were `sh native_build.sh` and `sh native_run.sh` inside the fixed owned
research directory; the script accepted no operator-selected operation or path.

| Check | Actual observation |
|---|---|
| 8 supplied positives, each binary | Exact fixed member/file/content counts; reordered/empty-file/path/member/content/input limits admitted only after complete EOF/trailer checks. |
| 27 supplied negatives, each binary | Nonzero rejection, empty success output and only fixed whitelisted diagnostics. Malformed/truncated framing, unsafe paths/tree, excluded metadata/types and excess bounds rejected. |
| Directory duplicate fixture | Both input spellings were encoded with the same trailing slash by tarfile. Duplicate canonical header rejection passed; distinct on-wire slash/no-slash alias rejection was not run. |
| Partial input/EOF/excess/stall | Seventeen-byte producer writes admitted; 511-byte truncation rejected; oversized body rejected while its writer stayed open within three seconds; stalled input stopped within the nine-to-thirteen-second observed bound. |
| INT/TERM/HUP | No successful completion; exact saved pipe flags restored. |
| Broken/full output | Exit 3, no false completion; full pipe retained only its original sentinel bytes and stopped within four-to-seven seconds. |
| Arguments/effects | Shell-like argument rejected; existing canary and its complete work-directory inventory unchanged. No member extraction or archive-derived operation ran. |
| Serial and keyboard/VGA | Same fixed trusted summary and 90 checks, exit 0; exact terminal attributes unchanged. This is not renderer, archive-interaction or screen-fit qualification. |
| Input postconditions | All 35 inputs and source/test manifests reverified unchanged after each final run. Binary identities matched before the VGA run and at final readback. |

Independent wait4 child-lifetime peak RSS was 944 KiB on serial and 936 KiB on VGA,
below 32 MiB. All cases checked exact native F_GETFL restoration on retained shared
pipe descriptions. The supervisor bounded captures at 4 KiB, each case at 15
seconds and child data/RSS/CPU/file/core resource limits; every wait/reap completed.
The inspector retained 1 MiB input, 128 members, 512 KiB aggregate/256 KiB per-file,
255-byte paths and its compile-time application-buffer ceiling. These bounds do
not establish arbitrary program containment or generic kernel RSS-limit enforcement.

The post-run [host recipe](../../research/payload/native_fixtures.py) regenerated
all 35 exact archive digests, 2806787 total bytes, the original case-manifest digest
052f68eeb8c267e6efe4c44ba6c233efc94b9574fed97a6dcaf1b5985b7f2b23 and the
unchanged archive/source checklist digest
f8fe3c7eb5defd70d2586825709e3a17182488a79db36276ccdab8fb7fe6e22e.
Its generated JSON/checklists are test fixtures, not a public project contract.
The recipe was added after native disposal; regeneration does not claim another
native run or close the distinct on-wire alias gap.

## Budgets, teardown and limits

Observed emulator runs: 981.93 seconds provisioning, 1124.69 seconds serial build/
tests/corrections and 488.52 seconds keyboard/VGA. All were below the 60-minute
run limit and reached prompts before the 15-minute prompt limit. The supervisor
checked owned allocation and ordinary capture caps throughout; a late independent
sample was 5130756096 allocated bytes, below 20 GiB. Sampled captures were 954811,
157057 and 914276 bytes respectively, below 10 MiB each. These are samples, not
claimed historical peaks; no large-capture exception was used.

Both installed boots' stock relink logs reported completion and no reorder_kernel
process remained before serial halt. VGA completion was also read back before halt.
Serial sync/poweroff and subsequent VGA poweroff ended the emulators with status 0.
The bootstrap VGA halt was independently observed before QMP quit. No interrupted
relink or automatic destructive resume/retry occurred in this experiment.

The accepted stock plaintext-argv limitation applied only to the synthetic guest
credential. No stock argv trace/input echo or password database content was
captured. Private serial captures excluded the credential; VGA password input
showed no echo. Protected input/credential files and stock-generated guest keys
remained outside Git. This does not satisfy the production secret-channel gate.

Teardown observed all emulator/client sessions stopped, former emulator PIDs
absent, no owned open handles or host mount, then removed the one exact disk,
credentials, input queues, captures, staging and all task-owned data transports.
Regenerated host recipe files were also removed. Prepared-target authority ended
with disposal. Only original code and sanitized evidence remain; partial disk
installation has no general rollback.

Payload authenticity, immutable verified-input lifetime, distinct on-wire directory
alias qualification, extraction containment/races, ownership effects, application
readiness, runtime/public-contract adoption and the broader native profile remain
unqualified. Parent issue #5 and PTT-04/05 remain incomplete. No Actions, runner,
hosted status publisher, live network, host service installation or release ran.
