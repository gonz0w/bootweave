# OpenBSD 7.9 installer: provisional source findings

Inspected 2026-10-11 for [issue #3](https://github.com/gonz0w/bootweave/issues/3).
Related [bounded research plan](installer-integration-plan.md).
Read-only source inspection; no upstream code was executed or adopted.

## Source identity and trust limitation

Retrieved [7.9 src.tar.gz](https://cdn.openbsd.org/pub/OpenBSD/7.9/src.tar.gz)
and [release SHA256.sig](https://cdn.openbsd.org/pub/OpenBSD/7.9/SHA256.sig) over HTTPS.
Archive size observed from server metadata: 204451355 bytes. Calculated SHA-256:
fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b.
It matches the checksum text carried in SHA256.sig. The signature was NOT verified;
trusted verifier/key provenance remains outstanding under issue #10. Consequently
these findings are provisional, not authenticated release qualification or evidence
permitting privileged use. Reconfirm all identities after signature verification.

Read selected regular-file members through Python's tar reader into fixed temporary
filenames; no general archive extraction, archived paths or scripts executed. Reject
non-regular or oversized selected members. Initial guessed install.sh/auto_install
members were absent; shared installer logic is in the files below. No claim is made
that guessed filenames are OpenBSD interfaces.

| Archive member | SHA-256 |
|---|---|
| distrib/miniroot/install.sub | 517ff57809632e8106c267483d766f3901d98b2e9ae06fa95949f7e21a824df7 |
| distrib/miniroot/dot.profile | 925fcc92d671714fedb1f5cc5b2a08839e84b894d25396645cea373119241b82 |
| distrib/amd64/common/install.md | a20bbf368739cf794e8db9ec117425326e3cf94255a9a854e0e2d1a647d8d66d |

install.sub's embedded source revision is 1.1286 dated 2026-03-25. Its inspected
header contains a two-condition redistribution notice. No source is copied into
Bootweave; reuse needs retained notices and complete file-level review.

## Selected prompt/default ledger

Locations refer to the exact files/digests above; this is a partial ledger, not a
complete execution path or a stable API. Exact emitted text can include runtime
values. No native expected outcome has been confirmed.

| Location | Observed source behavior | Safety implication/test |
|---|---|---|
| dot.profile 103–128 | Netboot or local response files enable a timed unattended entry path. | No local automation file or netboot assumption may enter the baseline; IR-01. |
| install.sub 734–753 | Response matching folds case, consumes a matching response, otherwise uses a nonempty default or errors. | Missing answers are not uniformly fail-closed; duplicate/ambiguous matching needs full-path review; IR-02/03. |
| install.sub 764–793 | Interactive empty response takes the default; special exclamation input enters a shell or evaluates a command. | Do not forward untrusted project/operator text to raw stock prompts; typed adapter boundary required; IR-03. |
| install.sub 806–824 | Yes/no questions default to no unless caller supplies yes; invalid automated answers exit. | Review defaults per call, not per helper; IR-02. |
| install.sub 2433–2475 | Root-disk selection derives defaults from discovered disks and accepts disk names or DUID translation. | Guest labels/discovery are not independent stable-target authorization; IR-04. |
| amd64 install.md 51–89 | Disk layout prompt chooses among whole-disk MBR, GPT and editing; whole-disk cases invoke fdisk writes. | First destructive boundary must be reviewed in platform-specific code, before these calls; IR-01/04. |
| install.sub 486–538 | Disk configuration invokes machine-specific labeling and creates FFS filesystems with newfs. | OS effects occur before readiness; cancellation afterward cannot undo them; IR-07. |
| install.sub 1731–1789 | Signed checksum verification precedes set checking; failure paths can ask to continue despite checksum or signature problems. | Never submit a bypass response; capture exact offline failure branch; IR-05. |
| install.sub 3054–3063 | Executable target-root site hook runs under chroot; nonzero return triggers failure. | Review executable authority separately; test independent marker/failure state; IR-06. |
| install.sub 3086–3102 | Autoinstall collects a log, checks a completion marker and reboots after completion. | Terminal/process exit is not independent installed readiness; capture/log and reboot authority need design; IR-07/08. |

## Answer logging, extraction and termination paths

Additional inspection uses the same member digests above. Source authentication
still remains pending. These observations do not assert a vulnerability in the
stock installer or compliance with Bootweave's stronger boundaries.

| Location in install.sub | Observed source behavior | Follow-up acceptance implication |
|---|---|---|
| 621–625, 796–801 | Ordinary ask responses are appended to a response record. | Classify every call before permitting capture; do not export raw answer records. |
| 888–917 | Password helper disables interactive echo and uses a separate input path; automated password output is a placeholder. | Do not claim ordinary response logging captures all passwords; audit callers/consumers and interruption paths independently. |
| 921–938 | Interactive passphrase prompt explicitly echoes; automated path prints a placeholder. | Offline/no-network baseline avoids network secrets; future secret UX requires a separately reviewed channel. |
| 224–237, 3055, 3097 | Response/autoinstall records are copied into target var/log with mode 600 and scheduled for root mail on first boot; removal follows successful mail. | Restricted logs remain persistent data. No transcript is safe by default, and failed delivery can retain the copy. |
| 1821–1848 | Set contents stream through tar into /mnt; extraction failure offers a continuation choice. | Signature authentication does not establish arbitrary project-archive containment. Review native tar semantics and independent target postconditions before adopting payload extraction. |
| 64–67, 564–575 | Error helper requests exit 1; installed traps kill the dmesg listener, restore echo and request exit 0. | Trace which trap is active at each failure; an outer process status alone is an unreliable success oracle. |
| 3086–3102 | Automated completion uses an explicit marker before reboot. | Observe marker, actual filesystem state and readiness separately; no automatic retry/resume from process status. |

The trap interaction is a source-derived hypothesis requiring native confirmation;
no shell was run to simulate OpenBSD ksh semantics. This is not a claim that every
failure returns zero. Similarly, the tar invocation is not proof of either safe
or unsafe archive handling; determine exact native tool behavior and destination
policy using approved synthetic fixtures. No malicious archive or secret input
was executed for this review.

## Conclusions and remaining research

The inspected stock code intentionally supports interactive administration and
unattended installation. Inference: passing arbitrary inputs or incomplete answer
files into it cannot by itself enforce Bootweave's proposed trust and confirmation
requirements. A future integration needs a reviewed boundary around entry triggers,
answers, disk identity, verification bypass and observed postconditions. This is
not a selected wrapper design or reason to replace upstream machinery prematurely.

IR-01 has partial provisional source observations only; no test is marked passed.
Complete the selected-path ledger, platform layout defaults, response ambiguity,
complete secret-consumer/log filtering, native tar semantics and active-trap failure behavior after trusted
source verification. Inspect all dependent source revisions; do not infer safe
behavior from a single helper. IR-02 through IR-09 remain not run. Native experiments
still need accepted scope, verified media, disposable targets and explicit authority.

Research receipts remain outside Git. Public evidence includes only source identities
and paraphrased observations, no raw answer files, private hosts or control sequences.
No native fixture or process was created; native teardown was unnecessary. Local
checks qualify documentation/foundation tooling only.

A subsequent [candidate-key consistency check](../adr/0002-host-artifact-verifier.md)
accepted the downloaded source signature/checksum. The key is not yet adopted, so
these findings remain provisional rather than authenticated native evidence.
