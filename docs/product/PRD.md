# Installer framework requirements

Status: Proposed design direction. Repository foundation authorized 2026-10-11;
installer implementation is not approved by this document.

## Outcomes and scope

An installer author can package an unrelated OpenBSD application into branded,
installer-only boot media using documented, versioned configuration. An operator
can review and install it through a safe text flow with clear diagnostic outcomes.
There is no selected configuration schema or implemented interface yet.

| ID | Intended acceptance |
|---|---|
| INST-01 | Media boots directly into the installer on an explicitly qualified native profile. |
| INST-02 | Generic project configuration controls branding, bounded steps, payload and services without an engine fork. |
| INST-03 | Planning cannot modify disks; the selected disk and destructive scope require explicit confirmation. |
| INST-04 | Media/payload trust is checked before privileged use; failures cannot silently downgrade verification. |
| INST-05 | Secure account, management and service defaults are explicit, project-configurable and tested. |
| INST-06 | Partial installation and failed postconditions report honest state and documented recovery choices. |
| INST-07 | Structured, bounded diagnostics support offline troubleshooting without exposing secrets or terminal control sequences. |
| INST-08 | ANSI/BBS styling has keyboard, serial, low-color and plain-text equivalents. |
| INST-09 | An independent author can follow the docs to create and test a generic profile in a disposable OpenBSD fixture. |
| INST-10 | Builds and review record source, dependency, license, verification and native compatibility evidence. |

No general live environment, public fleet control plane, runtime product policy,
configuration replication protocol or universal disk rollback is included initially.
Install/upgrade/recovery behavior must be designed separately; an install engine
must not accidentally treat existing disks as fresh targets during an upgrade.

## Product examples and independence

A DNS project may ask whether an existing secondary should be paired and then
configure its product-owned relationship. This installs one DNS server; pairing
semantics and secrets remain with that product. A firewall may ask interface roles
and management reachability using its owning product's validated contract.
An unrelated generic application must be an independent native acceptance fixture.
These examples constrain extensibility; they do not define engine APIs.

## Unresolved design

Native release/architecture matrix; storage and firmware policy; runtime language
and renderer; profile grammar and versioning; extension privileges; service/TLS
bootstrap; signing identities; artifact reproducibility; recovery and upgrade
boundaries. Define measured resource budgets in each scoped Test Design.
