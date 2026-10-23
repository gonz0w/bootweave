# Appliance product requirements v1: source integration handoff

The confirmed profiles are **Aegvium, Nomvium, Arxwall and Vectivium**. Vectivium
uses its independent package/code, not a fork. Every product remains independently
usable; Bootweave is installation integration, and Aegvium is optional for Vect.
This interface lets product owners supply concrete placement/private-input/service/
observability needs while the common executor is developed. It is requirements
data, **not an executable installer schema, root API, qualification provider or
authority token**. No alternate private installer is needed or adopted.

The first source instance is Vectivium's
`packaging/openbsd/appliance-profile.json`, contract
`bootweave-product-requirements-v1`. It declares only static requirements:

| Field | Semantics |
| --- | --- |
| product, release_direction, status | Product ID and intended release, with source/qualified status stated explicitly. No unsupported platform default. |
| implementation, runtime_requires_bootweave, runtime_requires_aegvium | Shared product code and actual optional dependency policy; a selected required security feature still refuses on failure. |
| install_assets | Fixed absolute asset path, owner-role and exact octal mode to request from PlaceVerified. Data does not authorize root execution, overwrite, links or ownership repair. |
| protected_roots | Fixed private namespaces; actual owner/mode/identity and target-relative containment are independently verified. Never reopen a caller path as target authority. |
| site_inputs | Named needed inputs; no secrets, real addresses, operator values, defaults or scripts embedded. Private values use separately bound confidential input channels. |
| typed_operations_required | Closed capability names from the production design. Unknown/unimplemented operations fail before effects; no shell fallback. |
| setup_order | Reviewable dependency order, not executable code or root commands. The executor owns sequencing and revalidates the complete plan. |
| observability | Product audit emission/health meaning and finite limits, independent of base retention/rotation acceptance. Never infer healthy traffic from process ownership. |
| updates_and_recovery, acceptance_required | Required unfinished completion work and functional/failure evidence. No installer receipt can substitute for product function. |
| authority | Explicitly describes the lack of runtime authority in this source data. |

Build coordination must bind exact product source/payload hashes, roles, limits,
release/architecture/board/firmware/media and publisher policy before privileged
use. This file format is not accepted by the existing16KiB planner-v1 parser and
must never be fed to the root executor as a prevalidated request. The real
installer parsing, complete effect plan, capability transport and writer remain
subject to the approved256/24KiB development envelope and later exact qualification. No helper
or callback may interpret these fields into arbitrary root actions.

Product owners can complete their own role/input/audit/health source now. Bind
those results to the concrete typed operations below without waiting for an
unavailable installer provider:

- Aegvium: PlaceVerified(S/I/loader/module), CreatePrincipal(disabled role),
  BindPrivateInput(inspector roles/backend trust), ConfigureObservability and
  RegisterService, then genuine continuous paired-target ObserveAndHandoff.
  Both fresh target owners remain exclusively held, never started/exported/
  duplicated, through the one attempted descriptor/generation/full-digest handoff.
  Any unknown or partial owner remains retained; files/FD equality/receipts do not
  recreate freshness. No particular frontend product is required.
- Nomvium: PlaceVerified(P/M/entry/helper/patched DNS closure), BindPrivateInput
  and actual publisher enrollment/incarnation ownership, ConfigurePolicy for
  qualified resolver/authority/filtering paths, ConfigureObservability and
  RegisterService. Actual filtering, cache and local DNS acceptance remain required;
  the one-record publisher is not the finished DNS appliance.
- Arxwall: authenticated assets, explicit interface roles/management reachability,
  private enrollment and approved firewall lifecycle plus logging/health. Its
  unresolved guardian/compiler/verifier/PF authority blocks those dependent
  bindings; ConfigurePolicy does not grant that authority by naming the product.
- Vectivium: shared package/immutable configuration, per-instance network/backend/
  domain/TLS inputs, own-service commissioning/health and ordinary product startup.
  Optional Aeg integration has separate compatibility/qualification; no core
  dependency on Aeg or a Bootweave runtime is introduced.

Initial qualification may target an amd64 VM; supported bare-metal/arm64 device
paths remain required extensibility work. Upstream board support is distinct
from a qualified appliance profile, including exact firmware, storage, NIC,
console, resource and protection evidence. Preserve all7.9 historical receipts;
new8.0 acceptance must be source/target-bound.
