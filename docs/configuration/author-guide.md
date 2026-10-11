# Installer author guide

Status: Design preparation. No runnable profile format or example command exists.
This guide states the information authors will need; it is not a schema.

## Prepare a project definition

Describe your application, support owner, license and source provenance. Select
an exact proposed OpenBSD release/architecture and hardware boot profile. Identify
verified OS sets, application packages, configuration files and firmware needs.
List minimum disk/memory/network requirements and distribution rights.

Describe branding and original artwork with terminal-safe fallbacks. Write the
operator's steps: required/optional inputs, conditions, defaults, help text,
validation, secrets and the final change summary. Include an unrelated generic
service example when evaluating the engine's extension contract.

Specify accounts, service enablement and bindings, management access, TLS trust,
SSH policy and required readiness checks. Disabled services and restricted
management are the default direction; an application-specific exception needs a
reason and tests. Do not assume installing a generic webserver configures your
application's administration endpoint.

Document what the installer configures once and what the product owns thereafter.
For optional peer setup, specify the owning product's existing pairing contract,
credential lifecycle, connectivity checks and pending/unreachable behavior. Do not
introduce product replication semantics in the common engine.

## Qualification and distribution

Use disposable disks and isolated networks. Test standalone/offline paths, invalid
input, interrupted installation, wrong disk identity, payload tampering, lost
network access, secret handling and readiness failure. Keep private inventories
out of reports. Follow [native acceptance](../testing/native-gate.md).

Before distribution, document exact build inputs, licenses, signature verification,
known limits, supported profiles, recovery procedure and how to request help.
The intended future quick start must include a fully working generic profile,
reference documentation, migration rules and troubleshooting after its design is
accepted. Do not copy speculative configuration snippets into production tooling.

See the proposed [project trust design](../design/project-trust/PRD.md) for
requirements, privilege boundaries and Test Design. No profile syntax is adopted.
