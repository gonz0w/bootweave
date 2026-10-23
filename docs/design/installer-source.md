# Fresh installer: executable-source entry design and behavioral tests

Status: source development under the approved256KiB union/24KiB entry envelope.
The current feature begins with the strict profile compiler/effect plan and the
actual sequential coordinator. No native admission/writer has been qualified or
enabled. Production PLAN_SOURCES, legacy metadata-v1, CLI, refusal/readiness and
all existing runtime/leaf limits stay unchanged. This is not a second simulator.
The same compiler/coordinator source is the future installer path; portable tests
replace only its private native boundary at link time, never via a runtime switch.
No new effectful actor resource envelope is inferred from these data/source caps.

## Profile-v2 grammar and complete planning

This is a separate format from metadata-v1 and requirements-v1. Products keep
runtime policy in their own projects. A new profile is untrusted declaration DATA,
never an authentication verdict, supported-capability claim, disk capability,
clock, cancellation token or prevalidated root request. No profile is installed or
accepted operationally by this source slice. Aegvium/Nomvium/Arxwall/Vectivium test
instances are explicitly inert compatibility examples, not qualified installers.
An unrelated inert product must compile too; no mandatory suite product is added.

Input:1..65536 bytes, ASCII strings without escapes, exact JSON syntax, whitespace
SP/TAB/CR/LF only. No unknown/duplicate/missing keys, null, booleans, floating point,
comments, trailing data/commas, recursion or array overflows. Key order is free.
Required root fields:

| Field | Exact type/shape |
| --- | --- |
| version | Integer2 only |
| project | Lowercase identifier1..32, first letter, remaining letters/digits/underscore/hyphen |
| release | Literal8.0; not a mutation of metadata-v1's historical7.9 policy |
| architecture | amd64 or arm64 DATA; actual board/firmware compatibility must be admitted independently |
| firmware | bios/uefi, with arm64 requiringuefi in this initial grammar; device-specific miniroot/board paths remain future profiles, not excluded product scope |
| handoff | single-stopped or paired-never-started |
| artifacts |3..32 objects with id,kind,file,sha256 strings; exactly one media, one payload, at least one set, remaining optional package entries |
| principals |0..4 objects with name only; dedicated `_project` or `_project_suffix` disabled-login role, not supplied UID/GID/options |
| files |1..64 objects: artifact,member,target,owner,group,mode |
| inputs |0..16 objects: name,target,group,mode; these declare confidential roles, never contain secret values |
| services |1..8 objects: name,entry,template |
| policy | management=site-input,ssh=disabled,network=product-input |
| observability | log=local-bounded,health=product-owned |

Artifact IDs are unique1..32 identifiers; filenames unique1..128 single components;
kind is media/set/package/payload and SHA256 is64 lowercase hex. Checksums are
content labels, not signify verification. Every selected stock set/package is in
the complete artifact closure. File members are1..192 canonical relative paths;
targets1..192 canonical absolute paths. No control/non-ASCII/backslash, empty/dot/
dotdot component, repeated slash or trailing slash. Absolute paths, root traversal,
links/devices/set-ID, overwrite and arbitrary hooks are never inferred.

Placement source must name the one payload. Destinations are below
/usr/local/sbin, /usr/local/libexec or project-specific /usr/local/share, /etc and
/var/db namespaces. Owners are root or declared roles; groups wheel or declared
roles. Regular-file modes are0440/0444/0500/0555/0600/0644 only. Destination equality
and ancestor conflicts refuse, as do duplicate archive members. The native layer
must independently verify the exact authenticated member closure, sizes, exclusive
creation and actual target-relative containment; lexical validation is insufficient.

Private roles are named1..32, unique, below /etc/project/private, modes0440/0600,
root-owned with wheel/declared group. Reject destination equality/ancestor conflicts
with every input/placement. The confidential input transport and exact materials
must be bound by actual admission before writing; a path is not such a channel.
Services use project or project-suffix names and the fixed owned-foreground
template only, with no argv/env/shell/rc fragment. Entry must be a declared root:
wheel0500/0555 executable placement. Duplicate service names/entry aliases refuse. Package
assets/default rc templates remain executor-owned; profile code is never a root
hook. Declaring policy/log/health needs does not implement or authorize product
PF, enrollment, retention, runtime privileges or readiness.

The compiler validates the complete profile first, then emits at most128 ordered
steps: SelectRelease, ConfirmFreshTarget, InstallStock, all CreatePrincipal,
PlaceVerified, BindPrivateInput, ConfigurePolicy, ConfigureObservability,
RegisterService and ObserveAndHandoff. Array order is preserved and displayed;
root planning never trusts a caller's step list. A deterministic bounded16384-byte
review states UNVERIFIED/NO OPERATIONS AUTHORIZED and all declared effects,
artifact identities, permissions, input role names (not values) and handoff kind.
It asserts neither functional health nor a successful installed disk. The actual
coordinator must render the complete review before admission; a syntactically valid
profile whose full review exceeds16384 bytes refuses before any native call. No
truncated review can reach confirmation.

## Actual coordinator and closed authority boundary

A dedicated single-threaded source entry accepts raw profile bytes and copies
its validated profile privately. No caller supplies a provider, owner, boolean
signature result, confirmation receipt, FD/PID, origin or clock. It recompiles the
complete plan. A private platform constructor must eventually authenticate the
original artifacts/publisher policy, bind confidential inputs, hold the actual
exclusive target(s), immutable plan, one original clock/cancellation lifetime and
finite qualified resources. The normal constructor now refuses with no syscalls.
No capability is minted by parsing, checksums, a rendered report or tests.

Before any intent or target write, actual preflight must validate support for EVERY
required step, the complete artifact/role/input/service/policy closure and target
compatibility, budgets and safe bounded original stock driver. Unknown/unimplemented
operations fail the whole plan; there is no shell fallback or post-install skip.
Arxwall's unresolved PF/enrollment authority and absent real paired handoff are
examples of pre-write blockers. Operator destructive confirmation must bind the
actual original target/geometry/firmware and full effect/material identities; the
constructor cannot reconstruct it from supplied paths/hashes/receipts.

The actual sequential coordinator checks the same private owner live boundary
before/after each intent and operation. ConfirmFreshTarget is the one actual
confirmation step, not a second prompt after a receipt. The final handoff boundary
performs its last original-clock/cancellation check before actual one-way transfer;
there is no subsequent target access/release. Each write-capable
step has durable intent before submission; progress distinguishes intended,
submitted and independently observed completion. Cancellation/expiry after an
observed operation still stops dependent work and retains that observed prefix.
No renewal, retry, fallback, automatic rollback, deletion or cleanup of a partial
installation occurs. Failed intent durability, unknown operation/Wait/Close or
changed identity retains uncertainty; later observations cannot erase it.

The result reports a retention request, never proof all resources were retained.
Success means the actual ObserveAndHandoff completed, not traffic-ready. For paired
products both original fresh never-started targets must remain exclusively owned
through installation/readback to one exact descriptor/generation/full-digest/actual
capability attempt. No export/start/duplication or substituted owner. Partial or
unknown handoff retains every remaining actual owner, not a retry token. The
platform retains failed/partial owners; untouched known admission failure may
release original handles once. Normal source cannot reach any of these effects.

## Component accounting before execution

New complete charges, shared source fully per consumer and once in union:
profile/parser install_profile.c/.h <=16KiB; effect-plan/UI render
install_plan.c+the full profile header <=8KiB; coordinator,
private boundary and full shared profile header <=32KiB; entry/refusing platform
and full shared private/profile headers are additionally charged to the approved
24KiB full entry. No runtime logic in tests/tools/generated payloads. Existing
planner24KiB, material/signature18KiB, capture8KiB, review/output16KiB remain
unchanged. Whole runtime C/H regardless of directory <=256KiB. Additional native
components receive their approved production-design allowances before source is
added; no byte allowance proves actual privilege, allocation or deadline fit.

## Independent behavioral TDD and finite portable effects

Freeze literal expectations before dispatch. Tests drive the actual compiler,
plan renderer and coordinator through a separately linked private synthetic
platform. The production refusal links separately and is tested too. No provider
is selectable in a product binary. No native syscall, root, disk/image, service,
network, signer/key or security-setting effect. The ordinary focused source invocation reuses the existing retained launcher for
born direct test children and bounded streams/waits, with one240s fixture admission
origin and no new admission after any unresolved child. This is source testing,
not a clean-host/group-containment or native proof. Do not run the historical
outer scripts that invoke the denied process observer. All new build/input/output
files are retained under one fresh.tools directory, not deleted. No old attempt
or aggregate full-suite authority is reused.

| Case | Required observation |
| --- | --- |
| Four consumers plus unrelated product | Same generic implementation; complete literal operation order and safe review, no Aeg dependency for Vect/Aeg core |
| Malformed/truncated/oversized/duplicate/unknown/wrong-type data | No output publication, native admission or mutation; source output unchanged on parse failure |
| Closure/role/mode/path/destination/service errors | Refuse complete profile before first intent, not after partial installation |
| Every array/string maximum and over-maximum | Bounded deterministic accept/refuse; no unchecked access, ASan/UBSan controls |
| Native normal constructor | Refuse before any target/process/credential/security/fixture operation, unchanged old CLI/link |
| Admission/preflight/confirmation failures | No install operation; unsupported ANY required step rejects entire plan before writes |
| Each operation's intent/submit/observation failure | Exact attempted/observed prefix and original failing phase; no retry, following action, fabricated rollback or lost uncertainty |
| Original live cancellation/expiry before/after EVERY transition | No renewed allowance; preserve observed completion even when late, stop dependent work |
| Partial single/paired handoff and release uncertainty | Retained actual owner semantics; one attempt, no substituted authority or cleanup success claim |
| Repeat/null/invalid output and forged plan | Refuse safely; coordinator one attempt, renderer recomputes ordered plan rather than trusting caller fields |

Before candidate execution obtain independent design/source/fixture review. Run
only affected bounded retained tests and safe content/lint/secret checks. Full
make verify remains blocked by held genuine-authentication fixture effects; this
slice grants no waiver or old-head full-pass claim. Exact OpenBSD8.0 media/package/
compiler/loader/driver closure, actual privilege/confinement/resources/durations,
original target/child/material ownership, installed product function and fresh
native authority remain required before native admission or CLI activation.
