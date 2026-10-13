# Authenticated payload review and placement: native evidence

Status: Accepted bounded research slice completed, 2026-10-13. Owner: Cam.
Related [scope, exact target and Test Design](payload-authorization-flow.md),
[issue #5](https://github.com/gonz0w/bootweave/issues/5) and
[implementation PR #51](https://github.com/gonz0w/bootweave/pull/51).

The original C experiment now connects retained-byte authentication, complete
planning, plain console review, fresh one-use confirmation and the existing confined
writer. Authentication alone cannot launch placement. Confirmation binds the full
plan, payload/key/tool identities and held empty root; dispatch rechecks them.
Archive input and console descriptors are separate capabilities. Terminal restoration
and queue checks must pass before dispatch. Completion requires independent final
inventory and outside-canary agreement, with separate planned/submitted/observed
counts. No production runtime, trust policy, configuration, privilege protocol,
installer integration or application readiness is adopted.

## Authority and source binding

Cam approved the complete #50 slice, its exact fresh 8 GiB target, two native signing
pairs, synthetic stock credential exception and 90-second interactive ceiling.
The reviewed scope head was bab77601ae95a6a6c17a5a7626a8b973ebed6e2a;
implementation began after Cam merged it, from
96711a7bfb213d7c6f56ab0a70f07193f162efc2. All implementation, corrections, tests,
qualification, teardown and evidence remain in PR #51.

The first implementation was efdd795a8efbfd0d91ded9260d30e2c876fd09cd. Native
compilation/console corrections were incremental signed commits on that branch.
The final qualified implementation is 1f998e7dbffb0878bcbd0778ab0e5212490be93a,
tree d0a81147c0257e785eedcabda33c27212a7250d8. Subsequent evidence edits change
no qualified executable source. New C including its header is 32,729 bytes within
32 KiB; combined experiment C/headers are 142,054 bytes within 160 KiB. Existing
component source ceilings remain enforced.

Before execution, source manifests, fixed stimuli, expected inventories and commands
were frozen and independently checked. The nonbootable read-only data ISO contained
219 independently read-back files, 715,130,880 bytes, SHA-256
bcbd1405eab4a725cb029db8a3316cb1d45edb31df9179d454e1b9004c320be4.
It held the initial implementation and frozen historical fixtures. Later corrections
used fixed bounded serial byte frames, independently hashed in the guest before
compilation and manifest updates. There was no running media replacement. Final
sources and compiled artifacts were rechecked after all suites.

## Observed checks

Local make verify passed 78 groups (22/13/3/10/30), with no failures or skips;
content, links, ShellCheck and Gitleaks passed. Seven new payload groups cover
portable bindings and owned host PTYs with a synthetic dispatch recorder. Host
execution cannot invoke the native writer or establish OpenBSD qualification.
Development tools: Python 3.14.7, ShellCheck 0.11.0 and Gitleaks 8.30.1.

| Required native checks | Serial | Keyboard/VGA |
|---|---:|---:|
| Frozen historical inspector | 90 | 90 |
| Authentication and raw directory aliases | 128 | 128 |
| Confined placement and independent failure oracle | 192 | 192 |
| New automatic composition, 35 each dynamic/static | 70 | 70 |
| Actual-console composition, 4 each dynamic/static | 8 | 8 |
| Total passed, no skips | 488 | 488 |

All 976 required checks passed. Automatic composition includes eighteen PTY
action/input/terminal cases, eleven postconfirmation context substitutions,
three partial writer failures, two signature refusals and forced-death restoration
with positive measured execution. Actual-console cases separately exercised fresh
confirmation, cancellation, snapshot mutation after confirmation and short writer
failure. The trusted compiled supervisor independently checked results, reaping,
resources, terminal attributes/flags, inventory and canaries.

A positive two-object case reported planned=2, submitted=2, observed=2 and complete=1.
Cancellation and snapshot substitution launched no writer and left the member root
empty. The injected short writer reported planned=2, submitted=2, observed=2 but
complete=0 and a failure: object counts alone cannot establish content completion.
The unexpected preexisting-object substitution reported the independently observed
object and uncertainty, rather than describing that changed root as empty.

The original 410 serial regressions ran in the serial boot. To preserve its scheduling
budget, final corrected serial composition ran through the existing stock tty00 getty
in the already planned VGA boot, before VGA suites through ttyC0. Both gettys were
already enabled by stock configuration. No extra boot, clock reset, service change,
new target or signing pair was introduced; suites sharing case roots ran sequentially.
VGA input used actual emulated keyboard events, with observed complete review and
fresh case/revision phrases. Native supervisor results established passes independently
of the host screen observer.

## Earlier failures and observation corrections

These are separate from the 976 final required passes. Two earlier automatic rounds
passed 70 cases each: 140 additional passes, not substitutes for final qualification.
There were no omitted failed native attempts:

- The first native composition compile failed because the test supervisor's console
  descriptor was not initialized before a short-circuit condition. It was initialized
  explicitly; native warnings remain errors.
- The first actual-console attempt held a /dev/tty proxy across a new session and
  could not display the review. It refused before member writes. Independent empty
  root observation and owned case cleanup followed.
- The next positive attempt wrote and independently observed the two intended
  objects, but its trusted parent rejected exact terminal restoration. It is an
  unqualified completion despite the child's complete report. Independent readback
  found the expected 0700 directory, 0600 file, nobody ownership and five-byte
  content; only owned members/source fixtures were then removed. Cleanup is not
  rollback.

The supervisor now retains the actual console descriptor before archive stdin is
installed and the child creates its session. It checks an empty input queue with
FIONREAD before saving active terminal attributes. OpenBSD initially exposed lflag
0x200005cb, then 0x5cb: the difference was PENDIN (0x20000000), a transient canonical
input state. No terminal flags are masked or ignored; the exact active baseline and
all saved attributes, control characters, speeds and descriptor flags remain gates.
Host PTYs separately exhibited a sticky first-write macOS flag and establish their
active owned baseline before the controller. This is not a production console policy.

Host observation had separate recoverable failures. A persistent single-client QMP
connection initially prevented keyboard control; short locked connections fixed it.
Cursor-blink duplicates reached an observer's conservative 1 MiB text guard at
1,049,540 bytes; lossless compressed history was retained, and content-change
observation prevented repetition. The accepted aggregate 10 MiB host capture bound
was never exceeded. Cursor normalization affects only observation deduplication,
not native terminal checks. One host serial reader stopped before parsing the final
packet; the complete retained capture established all four passes and exit zero,
and the reader was corrected. No native pass was inferred from missing output.

## Native environment and limits

OpenBSD 7.9 GENERIC#399 amd64 used base clang 19.1.7, C99/Os/Wall/Wextra/Werror/
pedantic. Dynamic binaries used libc.so.103.0 and test-only libutil.so.21.0; ldd
was inspected. Static PIE link maps/readelf had no NEEDED entries. No additional
runtime dependency or language adoption follows. Native base signify SHA-256:
f29045c0813293c5d3e6af36729eca0fb427c13b7e70d7201644ba1362b9ab2a.

Pinned QEMU 11.1.1 used pc-i440fx-11.1, qemu64-v1, TCG, one CPU, 1024 MiB, one
exact raw IDE target and read-only optical inputs. Paused effective argv, block,
PCI/device trees, open files and listeners showed no NIC, IP listener, host sharing,
passthrough, backing chain, overlay or extra writable disk. Immediately before
stock destructive answers, exact target identity/digest and observed wd0 capacity
matched the accepted zero-allocated 8 GiB file. Host device/inode are private.

The stock ramdisk's explicit-key base signify verified the release manifest; six
selected input hashes matched it before provisioning. The stock installer separately
verified its selected sets. No verification was bypassed. The stock compiler guest
used whole-disk MBR/FFS/swap, bsd/bsd.rd/base/comp/man, no NIC, SSH, X, added account,
site or game sets. Initial cd1 selection in the later boot refused; observed device
enumeration identified the sole remaining source optical as cd0 before copying.
The synthetic credential used the expressly accepted stock encrypt limitation;
it never entered Git, public evidence or captures. Private input files were kept
separate and disposed. Private text/compressed captures and owned screen images
were checked for credential exclusion before evidence publication.

Original component deadlines stayed 45 seconds whole/15 seconds child. Only the
compiled review composition used 90 seconds whole, 60 review, ten per input line,
five output and at least fifteen remaining before writer dispatch. Resource guards
retained 64/32 MiB controller/child RSS and native DATA, CPU=15, CORE=0, FSIZE=1 MiB,
160 descriptors, 1 MiB input, 512 KiB total/256 KiB member content and path/depth
limits. The new review admitted at most sixteen effects, 1024-byte lines, 64 KiB
input work and 16 KiB output. Missing positive measurements blocked passes.
Largest measured controller/child RSS across required suites was 4564 KiB, below
both ceilings; final serial automatic controller/child peaks were 1428/1368 KiB,
and VGA automatic peaks 1436/1384 KiB.

Install, serial and planned VGA launches lasted 1512.202, 2588.584 and 1899.824
seconds respectively, including cleanup, below the 45-minute scheduling gate and
60-minute hard stop. No launch clock was reset. Final native fixture/log allocation
was 11,376 KiB within 20 MiB. Final owned host allocation was 3,844,149,248 bytes
within 20 GiB; private run capture including screens was 3,454,175 bytes within
10 MiB. The sealed ISO remained below 1 GiB.

## Artifact identities

SHA-256, rechecked natively after the suites:

| Final composition artifact | Digest |
|---|---|
| review-auto-dynamic | 9dec599492e019dacf42a269bfab678f63527b990bdf917e95767916358564c7 |
| review-auto-dynamic.map | 1162c48fa2031d4cc914902b1d7084cf38f1b07ddaefc9d68d3a0ee8907c8d2f |
| review-manual-dynamic | 4d1c8f820463b7ffbffe09b51222e03be161c196179eede8a09432551ebe5d18 |
| review-manual-dynamic.map | 183732d747b5130b075a6158a4e1e4ff05cbe7074af9e2cb76b7259a09d402cf |
| review-auto-static | 2294f42b6e76319ec90ea73a3a297e1162cf026356b829703fdb593093a059c9 |
| review-auto-static.map | e0e8a45a80264f73091cbb350e874a04a18b3fee351895285bf9c39175d159c6 |
| review-manual-static | e2ab8696e445cce53f0de352e2a9895b15c7e02820357eaef5c7e90e042e19c5 |
| review-manual-static.map | fddb99acb63e452273acc594a2b488c7ddf4cf1b8dfc42c91e0fba31ee5c70c4 |

## Postconditions and disposal

Exactly two fresh native signing pairs were reused across consoles/builds. Their
private lifetime receipt was checked at setup; independent final ls observed both
public/private pairs, key directory and receipt absent before halt. All case roots
contained only their unchanged outside canary; no case, moved-root, source or core
artifact remained. All three canaries retained SHA-256
ed129299dfd52cfea27ea531effc7879cdfc558c0014fd3c406156c6a6854163.
Final complete process observation found no owned test, writer, compiler or kernel
relinker. Both authorized idle root shells and base gettys were accounted for.

Kernel relinking completed on each launch and /bsd agreed with its relink output.
The matching kernel-copy SHA-256 identities were:

| Launch | Digest |
|---|---|
| install | 6705245e05c8b58593835519a708cbf37b88f27187dba2054bebd7402ad1805a |
| serial | 74551d00b77f6810657bfa713d813d7fc700eadbeff00e58f45a867de8817ad8 |
| VGA | 36c094e5df1a5cd0a3156d91cc629f94471b2844e5499deffdff8c4c7823116e |

Stock install halt was visibly observed before QMP exit; serial halt powered QEMU
off normally. Actual VGA keyboard sync and halt -p produced a native guest-shutdown
QMP event and normal emulator exit. No timeout or forced stop substituted for a
clean halt. The final relinked kernel was not booted again; subsequent boot remains
unqualified.

| Clean post-halt target | Full SHA-256 |
|---|---|
| install | 867e122794df6b7c97e1fc499f9709bd095f12c2ebd9c8b02cf92fd7f1c0fec6 |
| boot | df4f528853d8057d19b29206933b0cfd68f1165ab1146a63ea767504b4b09435 |
| vga | d4832287061a5bab8cdccfb206faf32ced52efa6d5dfc2137750c0756451c1c2 |

Each post-halt digest retained the exact approved host identity/size/mode/link count
with no target/media handles. Before disposal, owned guests, clients and observers
were reaped; full fixture open-file, mount and Unix-listener observations were empty.
The exact raw target, sealed ISO, staging, credential/input files, captures/screens,
private receipts, sockets and 22 new helpers were removed. Sanitized evidence was
retained outside the fixture. This is logical disposal, not physical erasure or
installation rollback. Exact-target authority has ended.

Plain review wraps at forty columns and presents sequential full effects. Owned PTYs
covered the sixteen-object maximum and dimensions; actual VGA used the two-object
flow and scrolling text. This does not qualify a paged interface, single-screen
usability, physical operator presence, every paste pattern, general durability or
full parent PTT cases. Production design and broader privileges still need review.
