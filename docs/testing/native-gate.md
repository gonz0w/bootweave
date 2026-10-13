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
