# Authenticated payload placement: completed native rerun

Recorded 2026-10-12 local time; completion 2026-10-13 UTC. Owner and acceptance
authority: Cam. Related [issue #5](https://github.com/gonz0w/bootweave/issues/5),
[accepted exact-target rerun](payload-placement-rerun-plan.md),
[mechanisms/Test Design](payload-placement-spike.md) and
[earlier stopped qualification](payload-placement-native-evidence.md).

## Outcome and candidate

The fresh approved experiment completed **820 checks**: 192 placement, 128
native authentication and 90 frozen historical checks on each of actual serial
and keyboard/VGA. Every final suite passed, with zero failures or skips. Native
signing-key cleanup, independent absence checks, completed kernel relinking,
clean sync/halt on both consoles, emulator/client reaping and complete owned
fixture disposal were observed. The earlier partial experiment remains a
separate historical record; its 410 checks are not counted in this rerun.

Native source candidate: a9f4e684c8645b711f1b67b25f04f35112f662ca;
tree aff21a393b774a587ed1b6339cb0f5cc0f73c7cb. All ten contributor commits from
9ff2ad75d88a6bbbaf2162c941c517471f7a98d6 passed deliberately trusted SSH signature
verification and author DCO checks. Clean make verify passed 71 local groups
(22/13/3/10/23), zero skips, before corrected source transport/native execution.
Python 3.14.7, ShellCheck 0.11.0 and gitleaks 8.30.1 were development tools only.

PR #48 was merged before this rerun. The consolidated follow-up starts from its
squash on main, adde1777a802729f199eda394d60f13e67b671c4, and carries only the
acceptance, diagnostic, correction and completed evidence. Replayed commits retain
DCO and have fresh signatures; final clean PR head/tree and local verification
are recorded in the PR receipt. No agent merged a PR or used hosted checks.

This qualifies the accepted placement research slice, including bounded PPL-10
console/regression/cleanup requirements. Production trust/freshness/rotation,
installer integration, application readiness, general root races/crash atomicity,
broader archive support, parent PTT-04/05 and runtime adoption remain unqualified.

## Stopped attempts and bounded correction

Before the final suites, dynamic authentication stopped at check 35, mode 11
(verifier stall), on the signal-cleanup assertion. A diagnostic-only supervisor
commit added fixed numeric failure/reaping/peak metrics. Its first dynamic run
passed 64 checks; a repeated run reproduced failure AUTH_RESOURCE (10), reaped=1,
child peak=0 KiB. Independent native process/case inspection found no retained
authentication/signify child or case files. These two stopped attempts and the
64-case diagnostic run are excluded from the final 820 count. No missing RSS
metric was accepted and no test was skipped.

The test observer had signaled on the parent's fork notification, which does not
prove that the child ran. The correction adds AUTH_TEST-only scheduled-child
notifications. Before emitting one, the fixed child hook requires observed
positive getrusage(RUSAGE_SELF) RSS within one second, still inside the original
15-second child/45-second operation limits and 32 MiB child ceiling. The supervisor
binds scheduled and parent launch PIDs/stages, requires matching masks and signal
delivery, and signals the controller after the verifier runs. Existing positive wait4 metrics,
resource/deadline/reaping/confinement assertions remain enforced. The hook is
absent from ordinary standalone builds; no production runtime API was added.

Each correction had clean local verification and signed source/hash binding before
native use. Fixed gzip/base64 stdin frames used native uudecode/gzip, never a CD
swap or new target. Diagnostic frame: 14399 bytes; corrected frame: 28077 bytes,
129.05 seconds, below the 32768-byte/240-second transport bounds. The final 1688-byte
fail-fast rebuild recipe copied exact compile lines from the reviewed build scripts;
SHA-256 0fc5edb391b40db81bf26c2dd0835923b10cf68bcaf2f834ca63a13fd4278782.
Existing work-root initialization was omitted; no signing pair was recreated.
Actual completion, hashes and prompt preceded final tests.

Two host inspection mistakes were corrected without weakening checks: a source
manifest was initially checked outside its relative-file root, and RAMdisk stock
verification/device commands needed their actual base-tool form/device node.
Neither failed command counted as a passed postcondition. Final complete native
source/fixture checks ran in each fixed root and passed.

VGA evidence used observed 95-character ASCII glyph training and bounded screen
snapshots. Calibration and temporary single-client QMP contention were corrected
before accepting results. During final inspection, timed synthetic key release
caused repeated blank prompts and a dropped quote; the unfinished observational
input was canceled with no test active. Explicit key-down/key-up events and short
screen pages then retained the affected postconditions. Full completed test result
lines were retained, including wrapped metrics; no result was inferred from an
emulator exit or an unreadable screen.

## Exact target, provisioning and isolation

Cam explicitly approved the new sole regular 8 GiB target and synthetic stock
credential limitation. Its initial complete digest and immutable private receipt
are in the accepted rerun plan; preparation flags remained unchanged, with
acceptance/performed/observed receipts kept separately. Exact identity/size/mode
and complete digest were checked before execution and immediately before disk
answers, with zero allocated bytes and observed block writes. Guest wd0 reported
16777216 512-byte sectors. A preselection boot-prompt recovery stopped before disk
answers; the target was independently unchanged. Its elapsed time remained in the
original provisioning clock rather than resetting the budget.

All existing QEMU 11.1.1/firmware/verifier/release-key/media pins from the original
record were rechecked, including explicit-key stock signature verification.
Platform stayed pc-i440fx-11.1, qemu64-v1, TCG, one CPU and 1024 MiB. Complete paused
block/device/PCI/process/handle inventories showed the exact sole writable raw
IDE target, read-only stock/data CDs for install and target/data only for later
boots, no backing chain/overlay, NIC, IP listener, host sharing or passthrough.
The guest used stock base tools and services; no additional dependency or product
service was installed and no live network was modified.

Native base signify verified the stock manifest; its extracted payload SHA-256 was
50bec66f28426a22b9c9436f6a87cf3e7029e636bb915ba1d0db80d638881b87. All six fixed stock
input hashes matched, and the installer independently verified bsd/bsd.rd/base/comp/man.
Approved manual whole-disk unencrypted MBR/automatic FFS/swap, UTC, synthetic
.example.invalid hostname and no interface/DNS/SSH/X/additional user were observed.
Compiler was base clang 19.1.7 on OpenBSD 7.9 GENERIC#399 amd64; stock nobody IDs
were 32767. Credential metadata alone was inspected. The accepted stock encrypt
plaintext-argv limitation was confined to the synthetic guest; credential bytes
never entered host argv/environment or public evidence and were excluded from
private captures before disposal. Stock startup host keys were separate from the
exact two native test signing pairs reused across all attempts/binaries/consoles.
Offline startup name resolution failed without a NIC; no fallback/readiness claim
followed.

One nonbootable read-only ISO, 682022912 bytes, was retained unchanged throughout:
SHA-256 1764bb643ffe93fcb9ebb2e0d2a6df1ddfd4b7cae0550e160a099d2a3222f935.
All 212 staged files had complete independent Joliet exact-name/byte readback and
ISO9660 complete byte-inventory readback. No credential or private signing key was
in this media. The frozen 35 historical archives, 26 raw aliases and five placement
stimuli/independent inventories remained unchanged.

## Results, measured budgets and postconditions

| Console / suite | Checks | Peak controller KiB | Peak child KiB |
|---|---:|---:|---:|
| Serial dynamic authentication | 64 | 4792 | 4792 |
| Serial static authentication | 64 | 4280 | 4280 |
| Serial dynamic placement | 96 | 4284 | 4284 |
| Serial static placement | 96 | 4288 | 4288 |
| Serial historical, both inspectors | 90 | Not this harness's metric | 956 |
| VGA dynamic authentication | 64 | 4296 | 4296 |
| VGA static authentication | 64 | 4292 | 4292 |
| VGA dynamic placement | 96 | 4296 | 4296 |
| VGA static placement | 96 | 4288 | 4288 |
| VGA historical, both inspectors | 90 | Not this harness's metric | 920 |

Actual TTYs were /dev/tty00 (24x80) and /dev/ttyC0 (25x80), canonical input at 9600
baud. Both consoles independently retained unchanged TTY state and exact input
pipe flags. Dynamic/static standalone gates refused without context (77) and with
arguments (2). These gates are separate from the frozen 410 checks per console.
Real explicit-key stdin crypto, locked unveil/pledge and dropped IDs/groups,
independent final/partial content/inventory/owner/mode checks, retained snapshot
mutation/replacement, malicious archives/aliases, resource/signal/failure cleanup
and outside canaries passed as defined in the accepted PPL cases. Prerequisite
pledge diagnostics were expected negative probes, not bypasses. Final case roots
contained only their original 14-byte canaries; no core or owned test/compiler/
relinker processes remained. All three canary hashes stayed
ed129299dfd52cfea27ea531effc7879cdfc558c0014fd3c406156c6a6854163.

All original limits stayed active, including 64/32 MiB controller/child RSS/DATA,
CORE=0, CPU=15, FSIZE=1 MiB, 15/45-second deadlines, 4 KiB shared output, 160 FDs,
128 realized objects/depth 16, 20 MiB guest fixture, 1 MiB case capture, 1 GiB ISO,
20 GiB owned host allocation, 10 MiB host capture and 15-minute prompt/60-minute
run caps. Final serial fixture allocation was 8608 KiB; VGA was 8600 KiB after key
cleanup and final inspection. Host allocation sampled 3795075072 bytes, below
20 GiB; this is a sample, not a claim about an unobserved peak. Private text
captures were 64705 bytes provisioning, 114040 serial, 141 VGA boot-selection and
513236 VGA screen-text observations. Bounded image/socket/metadata captures also
remained inside the enforced host cap.

Provisioning exited normally at aggregate elapsed 1939.36 seconds, serial at
2408.42 seconds and VGA at 1399.72 seconds. Each observed clean sync/halt and
emulator reaping occurred before the 45-minute scheduling gate and inside the
unchanged hard cap. No shutdown reserve was consumed by new builds/suites.
RAMdisk ps could not provide installed-kernel process evidence; no absence claim
came from it. Foreground installer relink completion, kernel-copy agreement and
actual halt were observed; complete installed-kernel process snapshots supplied
the later serial/VGA absence checks. Relink logs completed and both /bsd and
relink copies agreed before each halt:

| Phase | Both kernel copies SHA-256 |
|---|---|
| Provisioning | 9e851de048a4eac016e089d65a1e9a52b3ed56a894d9687265e73aa4c799226f |
| Serial | 4da42fab6312f679390931bb39dc34a8ea049637d22ad62a1fa3452a59dc3078 |
| VGA | 3336c98305690d3f19c7b52d776d356096661eb07a415e62ae81fbfe311c2b95 |

Complete same-identity post-halt target digests bound the later boots and disposal:
provisioning 1e3cf87e6b8eccc48075366f00c9ec798531d07f7344be2bdef3cb271d6f3e94;
serial 15a44e19f84e22088fc71347b48a5db70e8af23da3eaedf2ec311380d7937ac0;
VGA 9e4598bbf60647e9b039fefedb4899ef7aedff9e738fe8447967251c2f06b04f.
Needed optical mounts were removed; final VGA had only root/home/usr disk mounts.

## Source and artifact binding

The four original placement-source hashes and unchanged inspector/frozen case
manifest remain those in the earlier record. Corrected original sources:

| Source | SHA-256 |
|---|---|
| auth_native.c | cf355bb2603b8fa64aac57d19efb3012ebc9091f16b86c862755c1ee657c5e08 |
| auth_native_check.c | 98e2c0beb090e381c75e0667595a9fbe8f9e8e9f8d33274ba6ede0eab0d3ddb4 |

Independent final native source manifests:
authentication 1f884995a09605e4621f1c7cbe233ae5d43181692094f3e7c539f2a023bd8ef1;
placement 30be99c8cd74862e4e28fac5ef8dd8375e47e28598effeb03c9585d87f4270ad.
Final native source/fixture checks passed in all three fixed research roots.

Base cc used the reviewed C99/Os/Wall/Wextra/Werror/pedantic dynamic/static compile
lines and native maps. Existing planner strcpy linker warnings occurred alongside
its checked length guard; no compiler error was ignored. Native ldd/readelf showed
base libraries only for original code and no static interpreter/NEEDED entry.
No cryptographic implementation or significant dependency was added.

The independent 21-artifact manifest SHA-256 is
d046424a4bfa8fb07019cb9f034fbee18a7e23648ee772fc898d6e2d6b1b6200.
Every listed binary/map matched on VGA before and after the suites. Paths below
are relative to the fixed guest research roots, not host/private inventories.

| Native artifact | Binary SHA-256 | Link-map SHA-256 |
|---|---|---|
| payload/inspect | 7f1c86f89cf61bc48a732259cec0aaae00fe95f12e0f74d199872bccd06785c8 | 47230a495bf12a213f249be5e9a92c90b15aaf56b3c8bc19f8ee7df0c8fdb7bc |
| payload/inspect-static | 5fc1918f6a98bdae2b01b210110d5e2b5ea80a809e2389bee6d667788005aa8f | cb0d02347c37587f2ae1f9ccb8f4fd49d3bd79212cb45bdcea28fae9cdd6dde8 |
| payload/native-check | af321c01010206624bd8c530218692c4bdc7941a72fc27d565ab7bc91c201200 | Not generated by reviewed build |
| payload-auth/inspect | 7f1c86f89cf61bc48a732259cec0aaae00fe95f12e0f74d199872bccd06785c8 | 581fe3fa1322dd69e675ac32f023d5968d338e7c4ef095a27b40b6f9f4eb22d6 |
| payload-auth/inspect-static | 5fc1918f6a98bdae2b01b210110d5e2b5ea80a809e2389bee6d667788005aa8f | e45d05f12a9ca850d3c7f7b7022720986109e699724f22a365aae367a5713a6a |
| payload-auth/auth | b0425bc8e440fab543c734d0200cc897a7481f8dcc263d59e3d31961afa83fa7 | 827f9f761003efe2bba99d5590981ac9d2baa3722b747972c9c350327b573225 |
| payload-auth/auth-static | eb65e86cb2a5b19c1c3a3df2d5800c8987cbdd3df16a70ba42e2cd0921199a25 | a1cf5763e9ac4a4d25b39cdb9ff866825dcc2dc778349f96279c2375200737b4 |
| payload-auth/auth-check | 37051e5388f6ac1bf936a40ede3bbbbdd4399f6922af7d055dad9d2ac8c2e7e1 | 242d2b501de573ed82542822813f61a6b6ec20030d2907efb25178bb5973d251 |
| payload-auth/auth-check-static | 8a6db1aea2b5fb56f486edd03a6e3f37ebe943ea67eedcbb3650e47d4db01a5e | b7aa4fc67c3f6fbf5857a4d2df71340925dd8998152a2f7d15d77044c69644a4 |
| payload-placement/placement-check | 99ba059ab76c2bc8baa330c38ed92a31bbfce5d024e857b3fddac402e53e18ad | 9205771e618661e16f8b0518cb0fc40143650942c0b5c526e6dfd6a48cf1d0ad |
| payload-placement/placement-check-static | 8118389141cba0f4e6ff23c5f03f5b46d59c5f78e6ec1bc16d07b4ded9259f5e | 560c4341c3b9055d76e8b6312ef1f684501f5221f671adc3c7645084bb57faf1 |

## Cleanup and scope end

The fixed native auth_native_cleanup.sh verified sources and removed exactly
selected.pub/selected.sec/other.pub/other.sec and the private lifetime receipt.
Independent VGA no-file/no-symlink checks confirmed all five absent and the key
directory removed. No extra signing pair was generated. This native observation
precedes host disposal and is not inferred from removing the disk image.

Actual completed sync/halt, emulator/client/observer reaping, no target/media
handles, no fixture mounts or Unix listeners and exact target identity/digest
were checked. The owned target, source ISO/staging, credential and input files,
private captures/screens/socket remnants and 16 created helpers were removed;
independent absence checks passed. Only sanitized facts remain. No rollback or
physical-erasure guarantee is claimed. This exact-target scope is complete and
ended with disposal; it supplies no authority for another destructive target.
