# Fresh appliance installation: production direction and complete fit assessment

Source/design direction approved2026-10-23. This defines the first complete
workflow and target-operation interface; it does not activate a writer, alter
existing planner/profile syntax, expand budgets or authorize native effects.

## Deliverable and independence

Boot authenticated installer-only media; select a declarative product profile;
review exact disk destruction and effects; install stock OpenBSD plus authenticated
payload; configure explicit hardening, per-instance private inputs, local logging
and monitoring; read back the installed state; hand off to its real product
startup owner. Ordinary startup thereafter uses the product's own package/service,
with no Bootweave runtime dependency and no other mandatory suite product.
First-boot product function, healthy degraded states and recovery instructions
are acceptance, not inferred from installer exit or a service PID.

The first implementation/qualification candidate is a fresh offline OpenBSD8.0
amd64 single-disk install with explicit BIOS/UEFI selection. This is an initial
profile, **not an amd64-only product scope**. Supported VM/bare-metal and arm64
device-specific paths need concrete firmware/media/board/console profiles and
separate qualification; see [device evidence](openbsd80-baseline.md).
No upgrade, disk reuse, general rollback, on-target compiler, public
download fallback, arbitrary root hook or generic fleet service is included.
Paired never-started images require the extra continuous-custody contract below;
one installed disk never qualifies that handoff.

The parent supplied reviewed product requirements at Aegvium revision
`2d52769a0fb4a44456aa1698f07c9a8beac7f257` and Nomvium revision
`9d8c25fe412c2952804b378b6dd5bea552752a11`:
[Aegvium profile needs](https://github.com/gonz0w/aegvium/blob/2d52769a0fb4a44456aa1698f07c9a8beac7f257/docs/milestones/M2/BOOTWEAVE-APPLIANCE-PROFILE.md),
[Nomvium profile needs](https://github.com/gonz0w/nomvium/blob/9d8c25fe412c2952804b378b6dd5bea552752a11/docs/milestones/M2/bootweave-appliance-requirements.md).
These are consumer requirements, not authority issuers or parallel installers.
The owner subsequently confirmed Aegvium, Nomvium and Arxwall, then adopted
Vectivium as a fourth appliance alongside its independent package. One generic
installer and each product's existing codebase serve these four profiles; there
is no Vectivium fork or mandatory Aegvium dependency. Arxwall enrollment/PF
authority remains unresolved and is not settled by this product clarification.

Acceptance is product function, not publisher mechanics or configuration files:
Aegvium protects as a standalone WAF on supported VMs/bare metal with no required
Vectivium frontend; Nomvium provides actual ad/bad-traffic filtering, local
authoritative DNS/cache and useful DNS features on qualified VM/device profiles;
Arxwall provides a complete usable firewall with understandable modern management;
Vectivium provides routing/load-balancing/TLS as a package or appliance. Each
requires hardened defaults, minimal site-specific setup, real protection, useful
logs/health and update/recovery acceptance. This promises no absolute security,
unsupported hardware, new Nom filtering contract or unresolved Arx authority.

## Constrained profile and executor interface

The existing16KiB read-only metadata-v1 format and its7.9 advisory meaning stay
unchanged. A new installer-profile version is separate. Below are concrete typed
operation semantics, not final JSON syntax or caller-supplied command templates.
Base-C remains the implementation direction; no new runtime language/framework.
The unprivileged UI validates and reviews; a small root executor independently
validates the exact complete request. Product-supplied code never runs inside it.

| Typed operation | Accepted inputs and executor-owned effects |
| --- | --- |
| SelectRelease | Exact release/architecture, authenticated installer/set/package manifests, expected digests and explicit selected set/package closure. No network fallback or version coercion. |
| ConfirmFreshTarget | Actual exclusively held target, disk attachment and geometry/firmware identity, bounded partition plan and fresh confirmation over full effects/artifact identities. A path, hash, FD number or receipt alone is insufficient. |
| InstallStock | Fixed qualified stock installer driver and exact answer vocabulary; never product-provided shell/answer interpolation. Its target is the confirmed original owner. Monitor actual child/console and installed postconditions; partial/unknown outcomes stop. |
| PlaceVerified | Original authenticated payload bytes and exact bounded member list; descriptor-relative exclusive placement beneath the actual mounted target; root chooses allowed owner/type/mode. Single-link regular files/directories only; no links/devices/set-ID or overwriting existing files. Distinguish executable placement from authority to execute it. |
| CreatePrincipal | Explicit dedicated disabled-login role and exact secondary-group policy; allocate/verify target-local UID/GID and bind subsequent placements to that actual role. Refuse conflicts; no arbitrary account command/options or host account effects. |
| BindPrivateInput | Closed product role/name, one per-instance secret channel and validated public counterpart. Input is never argv/environment/log text. No shared image credential, silent generation or adoption of credential policy. Source may define channel; actual enrollment/signing remains separately authorized. |
| ConfigurePolicy | Exact profile-owned service/interface/PF/admin/SSH/time/update settings supported by enumerated executor operations. No arbitrary rc.conf, PF fragment, sysctl or root command. Unsupported setting refuses the entire plan before writes. |
| ConfigureObservability | Bounded base local logging/rotation and declared read-only product-health contract; exact private file/descriptor ownership and retention caps. Do not require an external collector, disclose request bodies/secrets, or treat a generic PID as healthy. Product owns audit emission/health semantics. |
| RegisterService | Fixed product executable/service identity and ordering from the reviewed payload closure; install only the reviewed built-in template/closed parameters. Enable only after complete validation/commissioning; no supplied install.site/root script, auto crash loop or readiness shortcut. |
| ObserveAndHandoff | Independently read actual installed file/account/policy/service identities and distinguish configured/started/functionally checked/uncertain. Transfer the actual target owner to the exact product receiver once; no receipt can recreate authority. |

Profiles express required capabilities and values, not root control flow. Unknown
or duplicate operations, bad bounds, unsupported combinations, changed media,
target identity or reviewed values invalidate confirmation before effects. A
runtime adapter is separately reviewed product source with its own privilege,
resource and readiness contract; signing a profile cannot add an executor operation.
No profile can turn unsupported operations into arbitrary shell fallbacks.

## Fresh-target custody and product bindings

One actual media/install owner retains both original never-started targets for a
paired product, from creation through placement/durability/readback and one
attempted frontend-owned handoff. Do not export, duplicate, start or reconstruct
either owner from paths, descriptor equality, hashes or persisted receipts. Bind
exact descriptor, fresh generation and full pair digest plus actual capability.
On partial handoff or unknown close/writer state, retain both partial owners and
uncertainty; no retry from replacement authority. AGPAIR1 stays control-only;
new CONTROL/DATA descriptors do not supply application/backend trust by themselves.

Aegvium needs its fixed S/I/loader/module closure, disabled `_aegvium` role,
root:role0440 private inputs, protected launch intent/checkpoints and actual sole-S
original-child ownership. Its current null audit streams and missing health
authority are product integration gaps, not evidence supplied by Bootweave.
Any qualified frontend may implement the handoff; Vectivium is optional.

Nomvium needs protected P/M/entry/helper and patched NSD/Unbound closure, target
DNS paths, publisher enrollment and continuously owned writer/incarnation state.
Model A accepts an operator-held administrator secret and stores its verifier;
this document creates neither. Restore/clone must exclude the former writer and
establish fresh instance identity outside restored storage. No counter/hash can
detect unannounced rollback or manufacture fresh enrollment. Product runtime
limits805306367 bytes and export134217727 bytes are unchanged and unqualified;
installer/build allocation is a distinct budget, not borrowed runtime headroom.

## Hardening, logs, monitoring and failure

Default to no unselected service, no default/shared administrator secret, explicit
management reachability and independent product runtime. Do not silently enable
SSH, autostart or a public management endpoint because installation succeeded.
Show required private inputs and exact management consequences before confirmation.
Logging uses bounded event codes and sanitized fields: operation, phase, result,
public artifact IDs and explicitly safe product health. Private addresses, raw
installer answers, passwords, PEMs, cookies, DNS query values and bodies are not
default logs. Rotation/storage/write-failure behavior must be qualified. Logging
failure is reported distinctly from product health; no invented success receipt.

Stop dependent work on cancellation, deadline, ownership mismatch, failed fsync,
unknown Wait/Close or failed readback. Preserve intended, submitted, completed,
observed and unknown separately. No automatic disk retry, cleanup, fallback,
resume or rollback after ambiguity. Show bounded offline diagnostics and exact
manual recovery options without claiming physical erasure or known containment.
One original operation cancellation/deadline and actual descriptor owner survive
all phases. New privileged executor limits require a complete independent native
resource design; existing read-only leaf limits remain unchanged.

## Whole-feature source fit: decision required before runtime implementation

At base `04fa08df064e3f26d5e5b72b4db2f89ea34065ba`, the runtime closure is
100932/102400 bytes and fully charged authenticated entry16342/16384. Margins are
1468 and42 bytes. Planner14708, capture8093, signature17162, review/output12782
and material16635 are existing separate charges; shared helpers count in full to
each consumer and once in the union. No dormant native helper is excluded.

Existing research alone contains22442 bytes of placement planner/native writer,
17265 of review/native review,14405 of installed check/root/native root and
26170 of handoff/console/PTY/snapshot sources (including host-only research).
Those are evidence of implementation scale, not production code we can simply
copy or a measured minimum. The retained research lacks the complete production
privilege/interface/account/service/policy/secret/custody integration above.
No complete maintainable fit into the1468 remaining bytes has been demonstrated;
the measured research scale gives no credible basis to start that full runtime
implementation under the current cap. Moving code cannot remove its accounting.
Code golfing is inappropriate.

Recommend **one production planning package: total256KiB and full entry24KiB**,
all other existing leaf component caps and runtime limits unchanged. Keep the
current100/16KiB caps enforced until that decision. New source allocation:

These count **source-text bytes of Bootweave-owned runtime C/header code**,
including comments/whitespace, dormant native code and full shared helpers. They
are not an ISO/archive/payload format, binary-size, disk, RAM, CPU, timer or file
descriptor limit. Product binaries and stock OpenBSD are separately authenticated
dependency closures; moving Bootweave runtime code into a payload, generator or
another directory cannot evade its accounting. Four declarative profiles share
one engine envelope, not four copies of the allowance.

The100/16 constraints came from the owner's approved authenticated read-only
entry/native/CLI source package, recorded in
[the trusted-entry design](trusted-entry-output.md) and
[native accounting](native-entry-source.md). `tests/test_plan.py` enforces the
whole cap; `tests/test_native_entry.py` and `tests/test_entry_ownership.py` enforce
full entry/leaf charges. These are review-scope limits for that feature, not
measured hardware capacity or an OpenBSD security threshold. No guard changes.

| Complete new responsibility | Planning allowance |
| --- | ---: |
| Strict installer profile, compatibility and complete effect plan | 16KiB |
| Root executor, exact transport, stock driver, secret lifecycle and failure state | 32KiB |
| Target attachment/mount/firmware ownership and independent installed readback | 16KiB |
| Authenticated confined executable placement and ownership binding | 24KiB |
| Closed account/hardening/service configuration operations | 16KiB |
| Safe local diagnostics, log/rotation and product-health integration | 8KiB |
| Actual fresh single/paired target handoff | 8KiB |
| Complete UI/CLI and shared integration additions | 8KiB |
| Correction reserve after existing100KiB plus new128KiB | 28KiB |

These are whole-feature planning allowances, **not proved minima or permission to
consume them**. Entry grows by at most8KiB within the128KiB new union allowance;
shared entry/transport/profile helpers are still fully charged to every consumer.
Tests, docs and development tools cannot hide runtime logic. New components must
get explicit separate cap checks when the package is adopted; the union remains
the final bound. Review actual measured source before each implementation slice.
If even this package cannot fit without architectural sprawl, stop at the complete
fit rather than requesting repeated tiny increases. Alternative: keep100/16KiB
and the read-only product; that does not deliver the approved fresh installer.

Increasing a text allowance alone adds no root operation, permission or runtime
resource. Implementing the approved installer **does materially increase trusted
code and attack surface**: a disk/extraction/account/service executor and private
input lifecycle replace a solely read-only outcome. Source/component review,
closed operations, independent failure tests and exact native privilege/resource
qualification must address that expansion; a byte cap is not proof of security.
Compiled size/allocations may grow and must be measured separately. Existing
read-only timers/resource limits stay enforced; the new installation actor needs
its own finite qualified envelope before execution and cannot borrow those limits
as sufficiency evidence.

Decision: adopt256/24 as a **maximum source-development planning envelope**, with
the listed component guards and measurements required before code, or retain
100/16 and continue design/fit work while the full writer remains blocked. The
evidence supports leaving the old envelope; it does not prove256/24 is minimal
or sufficient. No extra ISO capacity, credentials, privileged/native attempt,
security setting, deployment or acceptance waiver follows from either choice.

## Behavioral Test Design and remaining gates

Use an unrelated inert application first alongside real consumer compatibility.
Portable/intercepted tests must cover the actual executor coordinator, not a second
simulator: unsupported operation before effects, parser/control injection, exact
target substitution, duplicate/conflicting members, permissions/account conflict,
changed private input, every write/readback/fsync/close failure, cancellation at
each transition, partial stock install, failed first boot, log-full/unavailable,
unknown health and partial paired handoff. Assert actual effects and retained
owners, no automatic repair/retry or authority from receipts. No host disk/socket/
account/security effects in portable tests.

Independent design/fixture/source review precedes exact native proposal. Native
acceptance then needs authenticated8.0 media and complete compiler/package closure,
qualified hardware/firmware/console, explicit fresh disposable target authority,
bounded storage/CPU/memory/fd/time accounting, original-child/secret/confinement
proof, independent installed function and truthful retention/cleanup observations.
The approved trace-companion method remains accurately limited; production
refusal/readiness do not change. Existing missing genuine authentication fixtures,
denied observer, unknown shared-host containment and retained failed attempts
remain separate blockers; this design does not clear or reuse them.
