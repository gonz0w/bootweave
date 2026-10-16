# Installer author guide

Status: Read-only metadata planner implemented; native qualification pending.
The broader installation profile/extension format remains a separate design.

## Read-only metadata quick start

Build with `make build`. On OpenBSD, as a normal unprivileged user, run:

```sh
bin/bootweave plan < project.json
```

Native support has not yet been qualified. The normal macOS/Linux build supports
`--help` and `--version`, but refuses `plan` with exit 77. `make test-plan` uses a
separate private portable harness; it does not enable production planning on hosts.
No package or runtime interpreter is needed by the C executable.

An example `project.json` definition is:

```json
{
  "schema_version": 1,
  "project": {"id": "offline-viewer", "name": "Offline Viewer"},
  "system": {"release": "7.9", "architecture": "amd64"},
  "payload": {
    "file": "viewer.tar",
    "sha256": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
  }
}
```

The digest above is a synthetic claim. The planner neither opens the named payload
nor measures/authenticates it. Every successful plan says the target is unselected
and authorized, performed and independently observed operations are all absent.
The output is a human advisory, not an installation request or machine interface.

All fields are required. Unknown or duplicate members reject. Keys must use the
literal spelling above. Values are bounded printable ASCII; names must contain a
non-space character. IDs start with lowercase a–z and contain only lowercase
letters, digits and hyphens (64 bytes maximum). Names allow 80 bytes. Payload
basenames start with an ASCII letter/digit and contain only letters, digits, dot,
underscore and hyphen (128 bytes maximum). Digests have exactly 64 lowercase hex
characters. Only the requested release `7.9` and architecture `amd64` are accepted;
that metadata acceptance does not claim an installation support profile.
See the [complete version-1 contract](../design/project-plan.md) for escape rules,
limits and exits. Version 1 is frozen; changed meanings require a reviewed version.

Supply a regular file of at most 16384 bytes through a read-only descriptor at
offset zero. Piped/terminal/device/read-write input and input/output file aliases
refuse. Do not put the definition or its pathname in command arguments. Do not put
credentials or private operator data in public display metadata. Failures emit
fixed diagnostics without echoing rejected content. A failed output can leave a
partial display; only exit zero establishes complete output, never authority.

The planner changes no terminal attributes, uses no color, and emits full labeled
fields for keyboard, serial and plain displays. Narrow displays can wrap lines.
It creates no files/processes/sockets and opens no paths; caller-selected output
descriptors and filesystem read access times remain outside that statement.

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

See the proposed [project trust design](../design/project-trust/PRD.md) for broader
requirements, privilege boundaries and Test Design. Only the read-only planner's
metadata syntax is adopted; installation profile syntax is still undecided.
