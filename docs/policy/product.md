# Product and compatibility contracts

These local clauses were extracted from [PROJECT.md](../../PROJECT.md) without changing their
requirements. Source identity is the consumer revision containing this extraction.
Command paths and literal configuration paths remain relative to the repository root.
Selected applicability is recorded only in root `policy.toml`; linked contracts retain
their existing scope. The owner changes required policy through the existing process.

## Product and technology

Bootweave is a generic third-party OpenBSD installer, independent of the OpenBSD
project and every appliance it can install. Product setup, private inputs, roles,
service policy and runtime belong to consumers. Do not add named-product dispatch
or a required appliance/suite runtime dependency.

The owner selected stock OpenBSD 8.0 installer/packages plus a small reviewed
generic bootstrap in [#5](https://github.com/gonz0w/bootweave/issues/5) and
[#96](https://github.com/gonz0w/bootweave/issues/96). That is an adopted direction,
not completed product acceptance. The separately held stock-source and OpenBSD
tooling changes remain with their owning PR/issue; this method change imports none
of them. Existing planner/custom trust source and 7.9 research retain their exact
scope and evidence. No production installer, broad hardware or release is claimed.

Keep existing C99 implementation contracts, POSIX shell and bounded inert metadata.
Python 3.14.7 and the pinned lint/secret/YAML tools are development inputs, not
selected installed-system dependencies. Prefer OpenBSD base tools where suitable.
Do not treat a host prototype or proposed parser/UI as a native runtime choice.
New languages, dependencies, services, abstractions or maintained upstream patches
need a present requirement, simpler-alternative comparison, provenance/license and
maintenance/removal rationale. Keep the BSD-3-Clause license and imported notices.

Project definitions, payloads, operator strings and terminal output are untrusted.
Preserve bounded parsing, escaping, explicit trust roots and source authenticity;
digests alone are not authentication. Never interpolate untrusted input into shell
commands. Package scripts and build hooks are reviewed executable code. Preserve
the adopted installation/trust boundary, explicit operation authorization and
consumer ownership; debug mode cannot bypass safety.

Show the exact destructive target and effects, require fresh operator confirmation,
and invalidate it on changed inputs. Planned, performed and observed states differ.
Unknown or partial results need recovery instructions, not an invented rollback or
automatic destructive retry. Keyboard, serial, low-color and plain-text paths remain
required; decoration cannot obscure prompts or enable control-sequence injection.
