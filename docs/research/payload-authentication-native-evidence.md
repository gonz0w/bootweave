# Native payload authentication evidence

Bounded experiment completed 2026-10-12 local time (final guest observations
crossed 2026-10-13 UTC). Owner/acceptance authority: Cam. Related
[issue #5](https://github.com/gonz0w/bootweave/issues/5),
[accepted exact-target plan](payload-authentication-native-plan.md),
[host evidence](payload-authentication-host-evidence.md) and
[parent Test Design](../design/project-trust/TDD.md).

## Outcome and identities

Original C/base-libc research authenticated a single retained synthetic payload
with explicit-key OpenBSD base signify before inspection. Dynamic/static native
builds passed 64 authentication/alias cases each on serial and keyboard/VGA:
256 checks total. The unchanged historical harness passed 90 checks on each
console: another 180 checks. No test failure or skip occurred in those completed
suites. Both consoles preserved TTY state; independent pipe/child/filesystem
observations passed. This qualifies this synthetic component experiment only.
No installer runtime, public API, production signer policy or extraction is adopted.

Implementation base: d72eaec20f799cd6a974749ccc024d0e041ed994 (merged PR #45).
Initial implementation: 378eafdd7cfe15b26917eda0c1554e5cd5e44194.
Native source candidate: d021826ccf15bc482ce8b9ce9b3284ee4ba7199e,
tree 181f564be164a8de0f347914da7418bd69fbf705. Both commits had deliberately
trusted SSH signatures and author DCO sign-offs. Clean make verify passed all
68 local groups, zero skips, before native use; tracked content stayed unchanged.
The second commit corrected key lifetime before any native key generation:
exactly two test pairs total, reused across both binaries and consoles.
Documentation-only evidence updates follow that tested candidate; final PR
head/tree and full-range signature/DCO/local checks belong in the PR receipt.

The reviewed local platform was QEMU 11.1.1, pc-i440fx-11.1, qemu64-v1,
TCG, one CPU and 1024 MiB with the exact executable/BIOS/VGA pins in the plan.
Native release: OpenBSD 7.9 GENERIC#399 amd64; base clang 19.1.7,
amd64-unknown-openbsd7.9. No package or dependency was installed.
Original controller/supervisor source is BSD-3-Clause; stock signify/libc
provenance and upstream notices are recorded in the accepted source review.
No upstream cryptographic implementation was copied into repository code.

## Transport and permitted stock provisioning

The prepared exact 8 GiB target was rechecked by complete digest, host
identity/size/mode, then paused full QMP block/device/PCI observations, effective
arguments and process handles/listeners. Immediately before destructive answers,
the guest was paused again and the unchanged target and inventory rechecked.
Guest wd0 reported 16777216 sectors of 512 bytes. It was the sole writable target.
Raw stock optical and data media were read-only, without backing chains. No NIC,
IP listener, host sharing, passthrough, overlay, hot-plug or reopen was used.

The initial nonbootable data ISO was 675975168 bytes, SHA-256
4212a8952200e04588080ef95e053bd669d91c3fab2c18836096703e40553ddb.
The key-lifetime correction produced a separately sealed final ISO, 675977216
bytes, SHA-256 b77807e1d19ed0d6c6315b76a5080e6417f750bb19774ce3df4751d90968dcf9.
Independent full readback found exactly 83 files, preserved all 75 original
nonmanifest files, and added seven original tool files. No boot record existed;
both media were below 1 GiB. The attached initial ISO was never replaced while
running; the final ISO was attached only after observed stock sync/halt and exit.
These were host data transport, not custom native installer media.

Before installation, native ramdisk signify -V -e with the stock explicit release
key verified the embedded manifest. Its extracted payload digest was
50bec66f28426a22b9c9436f6a87cf3e7029e636bb915ba1d0db80d638881b87.
Native SHA-256 of bsd, bsd.rd, base79.tgz, comp79.tgz, man79.tgz and INSTALL.amd64
matched that authenticated manifest before any destructive answer. The key
matched its separately reviewed pin. The stock installer then independently
verified the signature and selected sets again.

Permitted manual answers wrote whole-disk unencrypted MBR, automatic FFS/swap,
bsd/bsd.rd/base/comp/man, UTC and a synthetic .example.invalid host; no interface,
DNS, SSH, X, additional user, game/X/site set or install.site. Stock installation
and unique-kernel relinking explicitly completed. Independent readback observed
8 GiB capacity, FFS root/usr/home plus swap, sshd_flags=NO, com0 at 9600,
credential-file metadata only and native compiler identity. Stock sync/halt was
observed on VGA before emulator exit. The same host device/inode was bound to
its complete post-write digest for both later boots; preparation history remained
immutable and performed/postcondition facts were separate.

The first installed boot reported clean filesystems. Stock offline fw_update and
syspatch failed name resolution; no network fallback or product readiness claim
followed. Stock first-boot host keys are separate from the two synthetic signing
pairs. The explicitly accepted stock encrypt plaintext-argv limitation remains
unsuitable for the proposed production secret channel. The synthetic credential
never entered host argv/environment, ordinary logs, Git or published evidence.
All private serial captures were checked for its exclusion before inspection and
removed at scope end.

## Native build and binding

Only the read-only data CD was mounted. Every source/fixture digest and the
historical tool manifest passed native readback before copying/building.
Both owned research roots were private; no archive member was extracted or run.
The inspector was unchanged: 9237 bytes, SHA-256
8009aed82f67ae60c70538579fa493995a87fa6ff1ea324dc23ee79af0847698.
The exact 35 historical archives and cases.json stayed frozen; manifest digest
052f68eeb8c267e6efe4c44ba6c233efc94b9574fed97a6dcaf1b5985b7f2b23.
Alias-manifest digest: e7fb400fb68a1d9ebf3bb1d123e287f86fd543b9e4ba2c1465994a62df44207d.

Fixed scripts built with base cc, C99, Os, Wall/Wextra/Werror/pedantic and link
maps, with dynamic and static variants. ldd observed only base libc.so.103.0
for the controller/supervisor, and libc.so.103.0/libutil.so.21.0 for stock signify.
The static map identified libc.a including sha2.o; readelf showed no interpreter
or NEEDED entry (static PIE still has relocation/dynamic metadata).

| Native artifact | File bytes | SHA-256 |
|---|---:|---|
| inspect | 13360 | 7f1c86f89cf61bc48a732259cec0aaae00fe95f12e0f74d199872bccd06785c8 |
| inspect-static | 577672 | 5fc1918f6a98bdae2b01b210110d5e2b5ea80a809e2389bee6d667788005aa8f |
| auth | 23296 | 2069c5db491b4615c09cb0bbf95029c229cbe64d3ab24e095ba02bc28319bde1 |
| auth-static | 815064 | 1d28a510e89636faa458c981f5d9272f6cd7c30575834d33db9ad55e277fd91e |
| auth-check | 45536 | 6682f4f2ecd70ca6b0599864430982d62919f441a1ed06cd2e1fd282b704529f |
| auth-check-static | 848464 | 84fb7329a0c4d9427d2ef4443856ec5ee4b075266e3b818d3af0a0218ab6fb74 |
| base signify | Recorded stock identity | f29045c0813293c5d3e6af36729eca0fb427c13b7e70d7201644ba1362b9ab2a |

| Link map | SHA-256 |
|---|---|
| inspect.map | 36023b971419b4477ea1cc513f72b9a6f2b8cfdf4428afad93439c586255967f |
| inspect-static.map | 583dfce30b8ab8e5b9811e79163e1a268761f43ba4537c8055fd5981c103bfc9 |
| auth.map | 78882b95a377fecd873b3d25bce0e671809bf550e1215ee1ae2c768ee38f92e5 |
| auth-static.map | 8882b51c81722d3d0dda90e5ccdb460b6b8b69bd7976e401c0f6f0bd3519f3ba |
| auth-check.map | 36ba1b8f962f3a8d37eb427c0ce8bc16459c86859d46cf09ff87c9aed1eb9b05 |
| auth-check-static.map | 1e22a583b6d3efe165a7b3396e17b426b276e26eb7f50bf51f70153dcbbea91b |

The historical rebuild produced identical dynamic/static inspector digests;
native-check digest was af321c01010206624bd8c530218692c4bdc7941a72fc27d565ab7bc91c201200.
After VGA tests, independent native readback of both inspectors and authentication
supervisors still matched their serial-build hashes.

Executed fixed commands were sh auth_native_build.sh and sh auth_native_run.sh
in the authentication root, and the unchanged sh native_build.sh / sh native_run.sh
in the historical root. Actual serial and VGA keyboard input launched the suites;
VGA results were directly observed, not inferred from serial invocation. Standalone
native refusal returned 77 without context, arguments returned 2, and supervisor
arguments returned 2 before effects.

## Observed cases and resources

Each authentication binary first passed real base-signify generation/signing and
positive/changed-message detached stdin verification before dependent controller
cases. Mock success did not supply this prerequisite or normal authentication
acceptance. Trusted test-only barriers/doubles had no CLI, environment or payload
selection path and are absent from production-style standalone builds.

| Scope | Observation on both consoles |
|---|---|
| PAN-01/02 | Explicit selected signer/expected identity; valid altered archive with matching new digest still rejected its old signature; wrong identity/key and corrupt/missing/truncated signatures denied inspection/completion. |
| PAN-03/04 | Control/shell/key-hint comments neither selected trust nor appeared in diagnostics; oversized/malformed records/context rejected; authentic traversal archive verified then failed inspection. |
| PAN-05 | Real verification preceded source mutation/replacement. A separately reviewed consumer double independently hashed bytes actually received through the child pipe; original retained bytes won. Normal real-inspector cases were separate. |
| PAN-06 | Empty/exact/excessive/unfinished input, full input consumption, trusted tool-digest mismatch and caller-environment injection preserved bounds and authority. |
| PAN-07 | Actual child failure/signal/stall/closed pipes, shared output cap, controlled private-key diagnostics and invalid inspector output yielded bounded rejection; observed launch ordering, child absence/reaping and saved flags agreed. |
| PAN-08 | Directory/key modes, exact inventory, source/canary and identity checks rejected changed postconditions; injected extra-file cleanup was independently observed; signatures were removed between suites and the same two pairs retained privately until final disposal. |
| Alias gap | Six distinct raw encodings admitted alone; all twenty ordered conflicting directory pairs rejected after real authentication, including slash/no-slash and prefix/name aliases. |
| Historical regression | All 35 unchanged archives plus original signal/pipe/output/deadline cases passed for both inspectors, 90 checks per console. |

| Console/build | Cases | Controller peak KiB | Child peak KiB |
|---|---:|---:|---:|
| Serial dynamic authentication | 64 | 4260 | 4260 |
| Serial static authentication | 64 | 4260 | 4260 |
| VGA dynamic authentication | 64 | 4384 | 4384 |
| VGA static authentication | 64 | 4372 | 4372 |
| Serial historical (both inspectors) | 90 | Not this harness's metric | 920 |
| VGA historical (both inspectors) | 90 | Not this harness's metric | 1044 |

Native DATA/RSS limits, CORE=0, CPU/FSIZE/deadlines and bounded capture were set by
the reviewed code. Peaks came from wait4, not RLIMIT_RSS allocator containment.
Published child peaks cover successful collection paths; killed-child cleanup
wait4 data is not aggregated into that peak field. This does not qualify a general
memory-exhaustion guarantee or every failure-path peak. Worker resources, actual
reaping/absence and rejection were independently checked. No core was observed.
Both consoles reported unchanged TTY attributes; each case independently checked
exact saved input flags and canary/work/private-file postconditions.

External observed run durations were 2073.37 seconds for provisioning, 535.88 for
serial build/tests, and 482.55 for VGA, all below 60 minutes. End-of-run owned
allocation was 4404281344 bytes, below 20 GiB; combined private serial capture was
82403 bytes. Separate 15-minute prompt, 10 MiB capture and allocation watchdogs
remained active. These are observed values, not maximum hardware/support promises.

## Actual preparation errors and disposal

RAM device nodes initially did not exist; direct MAKEDEV execution lacked execute
permission. Running the fixed base script with sh created the needed nodes before
read-only verification. An initial sh /install invocation failed because /install
is the crunch executable; direct /install entered manual installation. Uppercase S
at the completed install exit prompt simply repeated it; lowercase s entered the
shell. A password-prompt matcher correctly refused a trailing-space mismatch
without sending a secret. A VGA helper rejected a comma during a fixed read-only
process command; it was extended to validate all input before key events and the
known unfinished command completed. A guessed relink-log path was absent; the
stock kernel-directory log supplied the actual completion marker. Host ps needed
sandbox escalation, and QEMU removed its pidfile on normal exit; stopped process
identities were checked directly. None of these failures was reported as a native
suite pass or used to repeat destructive installation.

Installer kernel relinking finished before its halt. Serial startup relinking
finished, its installed/copy hashes matched, and sync/halt completed before VGA
boot; that subsequent relinked kernel boot was observed. VGA relinking explicitly
finished with no relinker/test children before final halt. Ctrl-T sent once during
installer relinking only echoed ^T on the redirected shell; no interruption or
status result is claimed, and later completion was observed independently.

The fixed native cleanup script removed all four test key files, the directory and
private lifetime receipt after both consoles. Independent ls readback observed both
directory/receipt absent. Final VGA sync and halt were observed; all three emulators
and private serial clients exited and were reaped normally. Host lsof found no
fixture handles/endpoints, and no fixture mount remained. The same exact target,
both data ISOs, staging, synthetic credential/input, raw captures/screenshots,
private source copies and task helpers were then removed and absence checked.
Only sanitized evidence and earlier independently verified stock inputs/tools
remain. This target's authority ends here; disposal is not destructive-install
rollback or physical secret erasure.

Parent PTT-04/05 and the full trust design remain incomplete. Production signer
rotation/revocation/freshness/compatibility, root/key/executable races, privilege
separation/confinement, broad archive formats, safe extraction, ownership effects,
application readiness and runtime adoption remain unqualified. Another root
process or debugger/memory exposure is outside the trusted disposable boundary.
No future schema, service, dependency or installer interface follows from these
results.
