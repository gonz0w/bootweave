# Local development

Follow [PROJECT](../../PROJECT.md) and the selected issue before executing any
command. Bootweave's OpenBSD runtime, build and behavior-test requirements are
local project choices. Agent Temper prescribes no operating system or toolchain.

## Documentation adoption checks

The owner directed completion of #95/#102 on October 24 with checks appropriate to
this documentation/method-metadata change. Its executable source, hooks, tests,
tool pins and dependencies are unchanged. On the existing supported development
host, from the repository root, use the pinned Python and previously admitted
ShellCheck, Gitleaks and PyYAML inputs:

```sh
make check lint secrets
git diff --check
```

Also check the bundle's exact upstream bytes, complete inventory, source/license,
local links and anchors, and a representative issue-to-completion reader path.
Obtain separate policy/source review. Run unchanged content/secret and commit-
message hooks, verify signatures and actual-author DCO, and bind the results to
final base/head/tree and message. This finite adoption replaces the former
unconditional full-suite/OpenBSD requirement for its source-only checks. Do not
run installer fixtures or create test keys for prose. Report full `make verify`
and native tests as not run/not applicable to this batch, not as passing results.

## Behavior and tooling prerequisites

Runtime execution, builds, behavior tests and cross-build validation remain
OpenBSD-only under #5/#96. Use an explicitly authorized target, admitted native
tool versions, exact source and issue effects/resource limits. The interpreter is
pinned in `.python-version`; Python is development-only. The published bootstrap
still installs Mac/Linux ShellCheck/Gitleaks artifacts and PyYAML under `.tools/`;
it is not an OpenBSD setup path. The OpenBSD adaptation remains separately held
work. Do not import it into this documentation batch or run foreign binaries.

Read Makefile and the owning issue for command effects. The full aggregate has
genuine authentication fixture requirements; keys/signatures and native inputs
need their existing authority. Do not provision a VM, create credentials, change
system tools or run native fixtures merely because a command appears here.
Report missing prerequisites and stop dependent execution.

For behavior, tools, tests, hooks, build or dependency changes, focused checks
assist iteration; the final gate remains `make verify` on the clean candidate.
Record OS/tool/source identity, commands, exits, actual results and unavailable
lanes. On failure retain evidence, diagnose within scope and rerun affected checks.
Do not skip assertions, fabricate authentication positives or claim an unrun pass.

check validates all working content, including committed files, required documents,
UTF-8/line endings, local links, configuration parsing, duplicates in YAML and
obvious credential patterns. It prohibits GitHub Actions workflows. lint runs
ShellCheck; test runs safeguards in disposable Git repositories; secrets runs
Gitleaks. make verify aggregates these and the other targets listed in Makefile.
Read the actual recipes and fixture requirements; this description grants no
additional effects.

Hooks use the applicable environment above. They are bypassable by Git, which
does not grant permission to bypass them. Configure trusted signer identities
deliberately outside versioned source; never download arbitrary signing keys and trust them
silently. Use git commit -S -s and the [review procedure](review.md).
Python is development-only. [Native research build instructions](../../research/handoff/README.md)
apply to separately approved disposable fixtures; no production installer command
is provided.

## Saved native block observations

`tools/check-native-blocks.py` checks one saved QMP `query-block` reply against an
explicit reviewed inventory. It makes the path/access/raw-format/backing-depth
comparisons used by the private [handoff preflight](../research/installer-handoff-native-evidence.md)
repeatable in local tooling. It reads stdin and writes a fixed summary; it does not
open the named attachment files, connect to QEMU or launch a guest. Python remains
a development dependency only.

On an authorized OpenBSD check host, for an already observed, separately approved
run, using synthetic example paths:

```sh
python3 tools/check-native-blocks.py \
    --expect-read-only media /synthetic/boot.iso \
    --expect-read-only target /synthetic/target.raw \
    < .tools/reviewed-query-block.json
```

Use the exact device IDs and absolute paths from that run's reviewed inventory.
`--expect-writable DEVICE PATH` matches observed writable access; it supplies no
write authorization. Inventories contain one to eight distinct device IDs/paths.
Paths are printable ASCII and at most 4096 characters; IDs are at most 64
characters. The saved reply is limited to 64 KiB. Duplicate JSON keys, malformed
or nonstandard JSON, missing/extra/duplicate attachments, mismatched access/paths,
non-raw images and reported backing chains reject with exit 2. Image metadata and
block-child metadata must agree with the reviewed raw attachment. Unrelated QMP
fields can remain in the reply; private values never enter diagnostics.

The [official QMP reference](https://www.qemu.org/docs/master/interop/qemu-qmp-ref.html#blockdeviceinfo)
was checked on 2026-10-12 for the existing response fields. This is a narrow check
of supplied observations, not validation of every QMP field or authentication of
their origin. Reply freshness/provenance, paused state, exact disk
device/inode/size/digest, network isolation, process/mount cleanup and target
confirmation remain separately observed run gates. This tool does not prove an
attachment stayed unchanged after the snapshot.

Eight regression groups are included in `make verify`, covering substitutions,
ambiguous/malformed replies, the exact byte limit, private/control-text output and
a shell-fragment canary with no effect. No new native run was performed for this
tool, and previous private-guard native evidence does not qualify this new checker.
