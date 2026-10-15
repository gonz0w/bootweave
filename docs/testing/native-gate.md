# Native installer acceptance

Status: Full native installer acceptance remains incomplete. The bounded
[confirmed-handoff experiment](../research/installer-handoff-native-evidence.md)
observed verified stock installations and offline OS boots on two approved
disposable targets. Payload extraction, generic project readiness and broader
profile/hardware acceptance remain unqualified; the evidence records its shutdown
and terminal limitations.

The [bounded payload component](../research/payload-native-evidence.md) has native
dynamic/static builds and 90 checks each on serial and keyboard/VGA; distinct
on-wire directory-alias coverage, payload trust/extraction and runtime adoption
remain unqualified. Its disposable target was removed.

The [authenticated placement rerun](../research/payload-placement-rerun-evidence.md)
completed the accepted component slice: 192 placement, 128 authentication and 90
historical checks per actual serial/keyboard-VGA console, native key cleanup,
observed clean halts and full disposal. The signal test observer requires measured
child execution; resource/confinement guards remain intact. This does not complete
generic installer extraction/integration, application readiness or parent PTT-04/05.

The [authenticated review/placement composition](../research/payload-authorization-native-evidence.md)
completed 976 required checks across serial and keyboard/VGA: full effect review,
fresh one-use confirmation, context substitution refusals and independently observed
partial state. Exact terminal gates, native key cleanup, clean halts and full disposal
passed. Earlier compile/console failures remain recorded. This completes only its
accepted research slice, with no full parent PTT or production integration claim.

The [installed-root slice progress](../research/installed-payload-native-progress.md)
records six revised native entry builds, 976 serial/VGA build regressions and all
46 installed serial cases, including retained positive and later readback.
The private empty-fd0 adjustment is accepted. Media export remained incomplete and
the earlier build hard stop was followed by approved recovery/export, persisted key
absence, native clean halt and full original ISO readback; serial stock installation
and cold OS boot passed. The accepted raw-partition replacement passed complete
serial qualification with clean halts/full target hashes. The accepted VGA observer
retry passed fresh confirmation and native input checks, then stock extraction
panicked. Earlier failed optical/Cocoa preflights remain recorded without qualification.
The corrected stock-only AHCI control passed both listings/native halt. The later
accepted headless handoff preflight independently passed authentication/full hashes,
both listing-only tar passes and clean native halt, followed by all-target/media
integrity and closure. The approved one-time original VGA reset passed full zero
hash/zero allocation with identity preserved. The corrected launch 24 completed
stock verification/install, foreground relink/configuration, native halt and complete
integrity/closure. Cold boot 25 reached the actual ttyC0 login page but stopped on a
host prompt-matching error before runtime/payload input. Cleanup-only root login/
sync/halt and full hash/identity/closure passed; first-boot changes remain, with no
claim of worker absence/kernel-copy agreement from launch 25. Corrected launch 26
independently established runtime/root binding, worker absence and matching kernel
copies, then passed 34 automatic VGA cases. The first 28-line keyboard review
exceeded the 25-row display; the host stopped before any review answer/confirmation.
Cleanup forced termination. All original target/media hashes, identities and owned
closure passed, but native clean halt and full qualification remain false.
Nine host prompt groups cover complete overlapping views, full variant hashes and
exact return to the current page; native scrollback remains unproven. Twenty-six of
twenty-seven launches are consumed. Probe/exact stopped-fixture cleanup/full payload
rerun and later readback at 27/28, ceiling 28, are proposed and not accepted. No reset
or reinstall is proposed. Complete installed-root, Cocoa-window, physical-hardware
and parent installer acceptance and original exact disposal remain pending.

Local make verify checks foundation text/configuration, ShellCheck, safeguards and
secret scanning. Every runtime/media profile additionally needs disposable native
OpenBSD evidence using exact source, release, architecture, firmware and boot mode.
Cross-builds and host fixtures do not establish native support.

An approved scoped Test Design must cover boot to installer; keyboard and serial
flows; plain-text equivalence; read-only planning; stable disk selection and explicit
confirmation; signed offline/online payloads; filesystem extraction boundaries;
accounts and service exposure; reboot and readiness; loss of network; interrupted
writes and bootstrap; safe diagnostic export; and recovery from partial completion.

Use virtual disks first and separately qualified disposable hardware where needed.
Never use live gateway/DNS/NAS networks. Record teardown and isolation. Verify
that destructive actions cannot reach the host disks or neighboring networks.
Budgets for RAM, media size, logging and operation timeouts need measured native
results. Generality needs a generic application fixture independently of the DNS
and firewall examples. Every claimed profile needs its own actual evidence.

Record clean candidate, tree and base, exact command/tool versions, signatures,
actual results, skips, failures and uncertainty. Acceptance and public release need
explicit maintainer review. A foundation PR is not installer readiness.
