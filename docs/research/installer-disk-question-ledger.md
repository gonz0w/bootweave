# OpenBSD 7.9 amd64 disk-question ledger

Source inspection, 2026-10-11, for [issue #3](https://github.com/gonz0w/bootweave/issues/3).
Related [research scope](installer-integration-plan.md) and
[source identities](installer-source-findings.md). This is a bounded disk-stage
ledger, not the complete installation path, an answer file or a runtime contract.
No installer source was executed, disk attached or guest launched for this review.

## Verified inputs and selected path

Repeated the accepted [verifier/key policy](../adr/0002-host-artifact-verifier.md):
rehash verifier/key, then signify -C with explicit key, source SHA256.sig and
src.tar.gz. Signature and archive checksum passed. Re-read only the three fixed
regular-file archive members and compared them byte-for-byte with inspected files;
all matched. Archive/member hashes remain those in the source findings. No extraction
of archive paths or execution of upstream hooks occurred.

The selected source path is do_install in install.sub, root selection followed by
unencrypted amd64 disk configuration. Encryption, additional disks and editing are
reviewed as branches to exclude from a future minimal single-disk experiment. The
preceding hostname/network/account/console/timezone stages and later sets/completion
are outside this ledger and still prevent complete-path automation approval.

Locations below refer to the authenticated files. Prompt text omits displayed default
brackets and trailing spaces, which _ask adds dynamically. Variables in braces are
explanatory placeholders, not configuration syntax. Table behavior is source-derived;
none is a newly observed native outcome.

## Shared response behavior

install.sub 734–753 scans the response file in order, ignores comments/empty or
nonmatching entries, folds question/key case and permits a key to match within a
question at its blank-delimited boundaries with a blank/question-mark suffix.
This is broader than literal whole-question equality. The first matching entry sets
the answer and returns; the remaining input is copied for later questions. Earlier
nonmatching entries are retained. No duplicate-key or ambiguity rejection occurs
in this helper; a later duplicate remains available to a later question. An unmatched question uses a nonempty supplied default; otherwise
err_exit is called. This must be tested natively before relying on matching semantics.

_ask (764–793) assigns the supplied default after an interactive empty reply. Its
exclamation escape supports a shell or evaluated command. ask repeats _ask when
interrupted by the dmesg update path and logs the answer. ask_yn (806–824) accepts
case-folded y/yes/n/no only; its default is no unless the caller changes it. Invalid
answers retry interactively and exit in autoinstall. Prefix matching in the disk
callers below is a separate rule and must not be confused with ask_yn validation.

## Disk-stage questions and effects

| Location | Prompt and condition | Default and validation | Effect / absent-answer boundary |
|---|---|---|---|
| install.sub 2433–2475 | `Which disk is the root disk? ('?' for details)` | First positional disk in the discovered list is passed as _ask's default; disks reported below 1G are moved later. `?` displays details. Device/DUID translation must name a discovered disk; installation does not require an existing OpenBSD root. | Empty interactive input selects the default. Automated missing answer also uses it when present. Selection sets ROOTDISK/ROOTDEV/SWAPDEV and creates device nodes; it does not independently confirm host fixture identity or write scope. |
| install.sub 3125–3170 | `Encrypt the root disk with a (p)assphrase or (k)eydisk?`, when amd64 permits softraid boot, bioctl exists and no softraid volume already exists | `no`; n/N prefixes skip. p/P or k/K prefixes select encryption branches. Other replies repeat this loop. | Missing/empty defaults skip encryption. Selecting encryption invokes platform partitioning, writes a RAID auto-layout and calls bioctl before normal configure_disk. Target confirmation must precede this branch too. Keydisk requires extra storage authority; passphrase inputs require separate secret handling. |
| install.md 51–86 | `Use (W)hole disk MBR, whole disk (G)PT or (E)dit?`; if an OpenBSD area exists, inserts `, (O)penBSD area` before ` or (E)dit?` | `whole`; changes to `gpt` when dmesg matches efifb or ACPI 5–9, then to `OpenBSD` if an existing area is found. w/W, g/G, e/E and o/O prefixes dispatch; other values repeat. | Missing/empty answers can select a whole-disk operation on a fresh disk. MBR calls fdisk -iy; GPT calls fdisk -gy -b 532480. These are destructive calls before disklabel/newfs; there is no separate target-confirmation question in these branches. |
| install.md 78–81 | `An EFI/GPT disk may not boot. Proceed?`, only when g/G selected and MDEFI is not y | ask_yn default `no`; only y/yes proceeds | No returns to partition-choice loop. This is a bootability warning, not stable-target/write-scope confirmation. MDEFI=y omits this question. |
| install.sub 450–464 | `URL to autopartitioning template for disklabel?`, root disk in autoinstall only | `none`; other values enter a fetch path | Missing answer defaults to none and proceeds to normal layout choice. Non-none may fetch input and invoke disklabel -T with -w -A. Exclude this branch from an offline stock-media experiment; do not assume missing answers stop. |
| install.sub 466–477 | `Use (A)uto layout, (E)dit auto layout, or create (C)ustom layout?`, root disk without a successful template layout | `a`; a/A writes auto-layout, e/E enters auto-layout editor, c/C returns to caller for manual editor; other values repeat | Missing/empty selects disklabel -F with -w -A. The preceding disklabel -h -A displays a proposed layout; display is distinct from the subsequent write. Editing has its own tool prompts not enumerated here. |
| install.sub 3240–3253, 834–865 | `Which disk do you wish to initialize?`, after root disk configured and eligible disks remain; display adds `(or 'done')` | Explicit `done`, overriding ask_which's ordinary first-item default; only eligible disk or done accepted | Missing/empty chooses done. An empty eligible list returns done without prompting. Additional selection enters configure_disk; exclude it in a one-target run. Invalid autoinstall answer with response file exits; interactive answer retries. |

The root-selection default is the first positional argument passed to _ask, not
an explicit size/identity guarantee. Discovery and DUID translation are reviewed in
install.sub 315–351 and 384–394. Attachment changes or extra disks can change the
list/default. Guest labels alone do not satisfy Bootweave's future authorization.

## Write order and failure state

Normal unencrypted path: get_rootinfo, encrypt_root returning without encryption,
then configure_disk. configure_disk calls md_prep_disklabel; amd64 calls
md_prep_fdisk before disklabel_autolayout, then invokes a manual disklabel editor
if no generated fstab exists. Choosing the existing OpenBSD area avoids the
whole-disk fdisk branches but still leads to later disklabel/filesystem operations;
it is not a read-only install choice.

configure_disk (481–538) checks for a root mount point and duplicate mount points
only after partition/layout handling. It then invokes newfs for each FFS entry and
records filesystem entries. A later validation failure can therefore follow earlier
disk writes. The install loop retries configure_disk on failure; retry is neither
rollback nor proof the target is unchanged. The echoed partitioning completion and
function return are not independent disk postconditions. Failure propagation from
fdisk/disklabel/newfs and the active traps needs separate native review.

A future run must confirm independently inventoried host file, guest mapping and
allowed partition/label/format scope before entering any write branch. Planned layout,
submitted choice and observed disk/filesystem state need separate evidence. No target
is identified or authorized by this document, and no ready-to-run answer file is supplied.

## Remaining acceptance work

IR-01 now has a more precise disk-stage ledger but remains incomplete. Review the
preceding setup/secret questions, set-source and verification/extraction paths,
completion/hook behavior and all selected-path dependent commands. Editors,
encryption and template downloads require separate scope if later included.

Proposed native cases, after exact disposable target acceptance: fresh-disk MBR/GPT
defaults; existing-area selection; omitted and ambiguous response matches; invalid
prefixes; explicit encryption exclusion; and disk-stage interruption with independent
postconditions. Do not execute these cases merely because this document merges.
IR-02 through IR-09 receive no new pass. No integration or runtime is adopted.
