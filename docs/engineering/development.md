# Local development

Use the Python version in `.python-version` and run make tools hooks, then make
verify. Bootstrap installs checksum-pinned ShellCheck/Gitleaks and hash-pinned
PyYAML into ignored .tools/. Reviewed prebuilt tool artifacts currently cover
macOS/Linux AMD64/ARM64 development hosts. An unsupported host fails explicitly;
this limitation is not an OpenBSD installer compatibility claim.

check validates all working content, including committed files, required documents,
UTF-8/line endings, local links, configuration parsing, duplicates in YAML and
obvious credential patterns. It prohibits GitHub Actions workflows. lint runs
ShellCheck; test runs safeguards in disposable Git repositories; secrets runs
Gitleaks. make verify aggregates these. No installed services or native disks are
created by these checks.

Hooks are local and bypassable. Configure trusted signer identities deliberately
outside versioned source; never download arbitrary signing keys and trust them
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

For an already observed, separately approved run, using synthetic example paths:

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

Eight host regression groups run through `make verify`, covering substitutions,
ambiguous/malformed replies, the exact byte limit, private/control-text output and
a shell-fragment canary with no effect. No new native run was performed for this
tool, and previous private-guard native evidence does not qualify this new checker.
