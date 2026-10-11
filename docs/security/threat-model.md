# Security and trust boundaries

Status: Proposed runtime controls; foundation checks are implemented separately.

Protect disks and existing data, credentials, media/payload authenticity, installed
management boundaries and the integrity of diagnostic evidence. Project files,
archives, mirrors, network replies, operator input and terminal capabilities are
untrusted. A signed project is authenticated, not automatically safe to execute.

| Boundary | Required control and evidence |
|---|---|
| Build inputs to media | Pin provenance; verify signatures/digests; reject path traversal, link escapes and unreviewed executable hooks. |
| Operator to root installer | Typed validation; explicit stable target identity and destructive confirmation; no input-to-shell interpolation. |
| Project configuration to privileges | Bounded declarative flow; separate authority for reviewed executable extensions. |
| Payload to target filesystem | Verify before extraction/use; safe ownership, permissions, paths and target-root handling. |
| Bootstrap to running services | Unique credentials, restrictive bindings, least privilege and independent readiness postconditions. |
| Inputs to terminal/logs | Escape control sequences; secret-safe structured fields; bounded capture and explicit export. |
| Failure to recovery | Durable completed/uncertain state; safe reboot/resume rules; no invented atomic rollback. |

Secrets should be kept out of argv, transcripts, shell tracing, ordinary temporary
files and default evidence. Any necessary secret file requires restrictive mode,
minimal lifetime and explicit cleanup semantics. Debug mode cannot disable payload
verification or access controls. Interrupted operations and low disk/memory must
leave useful bounded evidence without leaking sensitive values.

Do not raise securelevel or apply generic tuning without checking installer and
product requirements. Qualify actual OpenBSD confinement and privilege separation;
Linux/macOS approximations are not native evidence. Threat-driven native tests must
cover terminal escape injection, malicious archives/profiles, substituted targets,
failed signatures, credential exposure and incomplete bootstrap.
