# Native UI ramdisk experiment

Cam confirmed continuation after the native UI PR and its stated ramdisk milestone,
2026-10-11. This authorizes the bounded research tooling and native media experiment;
it does not adopt C or implement an installer engine. Issue #4 / UI-10 remains the
scope. PR #31 is merged. In direct response to the exact prepared target approval
request, Cam instructed “keep going” and delegated routine decisions. This authorizes
the inventoried new disposable target; no additional confirmation is required.

The prepared sole target is .tools/ramdisk-ui/target.raw: new, exclusively owned,
0600, 8 GiB logical, initially zero allocated bytes, SHA-256
ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25.
A private receipt binds inode/device/size; substitution or changed attachments
invalidates confirmation. No backing chain or host device is permitted. Requested
writes are manual stock OpenBSD 7.9 amd64 installation on this target and native
build files within it. No existing disk, live network or host service is involved.

Use the previously verified stock ISO, signed sets and reviewed QEMU/firmware route.
Manual provisioning follows [the preceding experiment](native-ui-build.md),
including refusal of signature bypass, observed guest wd0 mapping and synthetic
private credentials. Freeze one CPU/1024 MiB, TCG pc-i440fx-11.1/qemu64-v1, no NIC,
sharing or passthrough; raw target ide.0 unit 0, read-only optical ide.1 unit 0.
Inspect paused configuration, open files and endpoints before every continuation.

[Original build tooling](../../research/ramdisk/README.md) compiles the source-bound
UI statically using base cc, reruns seven pipe/twelve PTY tests, uses base rdsetroot
and vnconfig on only its new private root.fs, adds the UI and original login profile,
and builds BIOS-only optical media with base mkhybrid. Reviewed signed stock bsd.rd
and ISO-bound cdbr/cdboot are inputs. No upstream source build/hook or package is run.
The profile runs the simulation once, reports its outcome and opens a research shell;
it invokes no installer and never retries the UI automatically.

Authenticated release-source inspection reviewed rdsetroot size/extract/insert,
vnconfig unused-device allocation, amd64 ramdisk Makefile/list and kernel reservation.
RAMDISK_CD declares MINIROOTSIZE=7360 sectors (3768320 bytes). Native rdsetroot/df
must establish actual capacity; insufficient space rejects a candidate before
boot. Routine stripping stays within the spike; reserve enlargement or a kernel
rebuild requires a new major decision. Base mkhybrid is a GNU/GPL build tool already supplied by
OpenBSD; it is not added to the ramdisk. Static libc/compiler objects must be recorded
from the native linker map; full redistribution/adoption licensing remains deferred.
Artifacts are private research output and are not distributed or released.

Limits: 15 minutes to prompt, 60 minutes per guest run, 20 GiB owned host allocation,
10 MiB private capture and 16 MiB UI/profile growth. No claimed reproducibility,
physical serial, EFI or general hardware support. Native compile/media results
cannot be replaced by a host build. A failed/partial image never becomes boot input.
Freeze native binary/kernel/ISO hashes and sizes before optical attachment.

Boot the resulting image in fresh diskless VGA and serial fixtures with only one
read-only optical attachment, no writable target/NIC/sharing/passthrough. Observe
root on rd0a and direct simulation startup, verify embedded source-bound binary,
then test completion, cancel, malformed/oversized input, EOF/timeout and tty restoration
at known and narrow declared dimensions. Report unknown dimensions honestly.
Catchable signals/resize and forced-kill restoration require owned console supervision;
unavailable cases remain unqualified. Record original media as control, exact hashes,
resource measurements, failures and console transport limits. No install prompt is
submitted in either boot fixture.

Stop and inspect owned processes, clients, mounts/vnode attachments and endpoints
before disposal. Delete only owned credentials, targets, transfers, artifacts and
captures after sanitized evidence is retained. Cleanup failures remain incomplete.
Planned actions are not performed or observed results; no rollback claim follows.

## Prepared inputs and host checks

Development base: 355617eb64d576145f93b2ce3b3c8a17ac55b991 (merged PR #31).
Read-only transfer ramdisk-source.iso: 5031936 bytes, SHA-256
0718022abf2908b4fe457c69b9e19d88dca1ed728141b695e13e8cec915dac8a.
The private source receipt binds every input, including build.sh/profile and
original UI/native tests. The input bsd.rd is 4848172 bytes, SHA-256
6f0974bf92e28e2a97594987cfd1db135fc2fb4aea00f3f3e35ca6f70448f034;
its signed manifest is the previously verified release SHA256.sig. ISO-bound cdbr
is 2048 bytes, SHA-256
171ab173143bc41dde2cd5944435040614035bd788a40caf34593ffc2332a753;
cdboot is 90328 bytes, SHA-256
beb4e1952b2cb7d40e25fee22d142cc74889204685d1d3f20a40166d56cc45f5.
These bootloader leaf digests are host extraction identities, not separately signed
upstream artifacts. Extraction initially rejected cdboot at a 64 KiB bound; the
reviewed directory entry established its 90328-byte extent, then exact bounded
extraction succeeded. No unverified guest code executed.

Prepared signed-sets data ISO: 672909312 bytes, SHA-256
2be86cb08ab9e55493c30d5a81bc9b2f2509772d8c25ad5ac54bced58c8dd4bc.
All selected files passed the accepted host signify check. Local make verify passed
14 foundation and 13 host UI tests, script/profile lint and secret scanning on the
development worktree. Native compilation, provisioning, image build and boot tests
had not run at preparation time. The target was checked empty before authorization
and again before first launch. See [actual native outcomes](ramdisk-ui-native-evidence.md), including rejected
builds, corrected shell compatibility and the final image identities.
