# OpenBSD 7.9 set and completion ledger

Source inspection, 2026-10-11, for [issue #3](https://github.com/gonz0w/bootweave/issues/3).
Extends the [disk-stage ledger](installer-disk-question-ledger.md) under the
[research scope](installer-integration-plan.md). No upstream installer code, installed
program or hook executed; no guest, mount, target write or network operation occurred.

## Identity and selected path

Repeated the accepted [host-verifier policy](../adr/0002-host-artifact-verifier.md):
checked verifier/key hashes, verified the source signature and archive checksum, then
compared the three fixed regular-file members with inspected bytes. All passed.
Archive/member identities remain in [source findings](installer-source-findings.md).
Locations below refer to install.sub with SHA-256
517ff57809632e8106c267483d766f3901d98b2e9ae06fa95949f7e21a824df7.
No general archive extraction or script execution was used for source inspection.

Selected source path: installation from stock optical media after the disk stage,
through set selection/verification/extraction and finish_up. HTTP, NFS, upgrades,
custom site sets and executable hooks are excluded from a future minimal experiment;
their presence/effects must still be checked before launching that experiment.
Preceding network/account/secret setup remains outside this ledger. No complete-path
approval or ready-to-run answer file follows from this document.

Prompt text omits the default brackets/trailing spaces added by ask. Braced names
below explain dynamic values; they are not proposed configuration syntax. Shared
matching/default/logging rules remain those in the disk ledger. All outcomes here
are source-derived, not newly observed native behavior.

## Set questions and defaults

| Location | Prompt / condition | Default, validation and effect |
|---|---|---|
| 2795–2869 | `Location of sets? ({optical devices} {methods} or 'done')` | Defaults first from CGI_METHOD, then HTTP if interfaces exist, optical cd0 when no earlier default exists, otherwise disk; unavailable default is replaced by an available method. Thus optical is not an unconditional preference. With no interfaces, no prior method and cd0 present, optical is the source-derived default. Prefix dispatch accepts c/C, d/D, h/H, n/N; exact done checks missing required sets. Invalid automated choice exits; interactive choice loops. Re-inventories optical devices each loop. |
| 2117–2148 | `Which {device} partition has the install sets?`, only when multiple filesystem partitions exist without a usable c partition | ask_which defaults to first listed partition; c or a sole partition is selected without a question. Selected source is mounted read-only at /mnt2. Mount failure returns; no successful-source claim follows. |
| 2046–2062 | `Pathname to the sets? (or 'done')` | SETDIR defaults to release/architecture (7.9/amd64 for the selected release). Exact done returns. Checks directory under /mnt2 first, /mnt second, then absolute root; invalid path retries interactively and exits automatically. This is not confinement to optical media: later review must confirm the chosen directory and attachment. |
| 1672–1675 | `INSTALL.{architecture} not found. Use sets found here anyway?`, when INSTALL.amd64 absent | ask_yn default no; refusal returns without installing these sets. Missing metadata must stop a future bounded stock-media test rather than authorize this continuation. |
| 1530–1568 | `Set name(s)? (or 'abort' or 'done')` | Default done retains preselected available sets; exact abort clears selection. Space-separated names/patterns add sets, leading minus removes, all becomes wildcard. Unmatched patterns do not have an explicit invalid-choice error here. Missing automated answer defaults to done. Selection is not itself verification or extraction. |
| 1756–1765 | `Fetching of {set} failed. Continue anyway?` | Default no; refusal cleans prefetch state and returns, or exits automatically. Yes removes this failed set from selected/unverified lists and proceeds with others; it does not successfully install that set. |
| 1772–1780 | `Checksum test for {set} failed. Continue anyway?` | Default no; yes leaves the set unverified. A subsequent aggregate unverified-sets prompt can still occur. No bypass answer is permitted in future verification-failure research. |
| 1787–1793 | `{issue}. Continue without verification?` | Default no. Issue covers missing/unfetchable SHA256.sig, signature failure, prefetch failures or remaining unverified sets. Refusal cleans prefetch state and returns, or exits automatically. Yes permits extraction without the expected verification; missing answers default to refusal, not a verification downgrade. |
| 1840–1848 | `Installation of {set} failed. Continue anyway?` | Default no; refusal returns/exits after potentially partial extraction. Yes continues with later sets. Neither answer restores earlier disk/filesystem contents. |
| 1490–1502 | `Are you *SURE* your install is complete without '{required set}'?`, when done requested and a required set remains in DEFAULTSETS | Default no for each missing required set. Refusal returns to set-location loop or exits automatically; yes accepts omission. The quiet check suppresses questions and reports missing sets. This checks bookkeeping, not complete filesystem or boot readiness. |
| 3741–3751 | `Exit to (S)hell, (H)alt or (R)eboot?`, interactive completion only | Default reboot unless MDHALT=y selects halt. h/H and r/R prefixes exec halt/reboot; s/S breaks to shell; other values loop. Automated child exits earlier and its parent owns reboot. Reboot requires separate scoped authority and postconditions. |

## Selection, authentication and extraction

Initialization at 3649–3692 selects the release base key, SETDIR and ordered set
lists. DEFAULTSETS preselects kernels and base/comp/man/game/X sets when available;
SANESETS requires the normal kernel and base (multiprocessor defaults differ).
ALLSETS places generic site79.tgz last, and do_install appends a hostname-specific
site set. install_files also preselects an available hostname-specific site set.
A generic site set is selectable rather than universally preselected. Future stock
control runs must check the actual available and selected list, not assume that
choosing done excludes every executable extension.

install_files (1638–1793) uses signify with the release key to verify SHA256.sig,
then compares each fetched checksum with the signed checksum entry. Missing metadata
and failed verification have distinct branches above. Files fetched via file://
are treated as local: fetch calls run as root, checksum calculation uses the helper
unprivileged path, and successful verification discards the fetched bytes. During
extraction local files are fetched again. This source flow is not proof that verified
and extracted bytes cannot change; a future test needs immutable reviewed source
attachments and independent identities. Nonlocal sets use prefetch storage.

Extraction at 1821–1863 streams tgz through tar into /mnt, including embedded etc
archives from installed base/xbase, and writes kernel/BUILDINFO files separately.
Fetch helpers' reduced privileges do not make destination extraction unprivileged.
Tar containment, path/link/ownership semantics and pipeline failure interpretation
remain separate native research. Authentic OpenBSD set verification is not a public
project-archive safety contract. Successful set installation removes its name from
DEFAULTSETS; that bookkeeping is what sane_install subsequently checks.

## Completion authority and state

The disk stage and mount_fs precede install_sets (3300–3312); reaching a refusal or
failure prompt here can already mean partitioned, formatted or partly populated
storage. do_install later writes configuration, account/password databases and keys
before finish_up. No general rollback is claimed.

finish_up (2937–3083) operates on the installed root: enables swap where possible,
creates device nodes, installs bootblocks, writes the kernel checksum and first-boot
commands, may run installed fw_update, relinks a kernel through a chrooted shell/make,
queues response-file mail and executes an executable install.site in chroot. Site-hook
failure calls err_exit after storing randomness. Chroot is not a privilege sandbox.
Kernel relink reports failure text; the final congratulation text alone does not
establish every preceding operation succeeded or prove the system will boot.

The installed rc.firsttime includes fw_update and, for release amd64, syspatch -c;
response mail may retain a mode-600 log until successful delivery (prep_root_mail,
224–237). Even an offline install prepares later network-capable work. Any future
reboot must retain the no-network attachment boundary and review these first-boot
actions; offline set selection does not suppress their creation.

Autoinstall uses /tmp/ai/ai.done as a completion marker, retains its protocol for
root mail and execs reboot (3086–3100). That marker is a stock control-flow signal,
not an independent installed-system postcondition. INT/EXIT/TERM trap behavior and
partial failure need native observation; no outer exit status is treated as readiness.

## Remaining gates

IR-01 has disk and set/completion source ledgers but is still incomplete: preceding
terminal/hostname/network/account/secret/timezone questions and dependent command
semantics need review. Native cases remain unrun. Proposed later cases include
missing INSTALL metadata, omitted selection, missing/changed signature, failed fetch,
checksum failure, partial extraction and required-set omission, always stopping at
bypass prompts. Hook and reboot cases need their own scope rather than being added
implicitly to a set test. Disk target confirmation remains required before writes.

No installer integration, response schema, hook contract, runtime or installed
service is adopted. Local verification qualifies documentation/foundation tooling.
