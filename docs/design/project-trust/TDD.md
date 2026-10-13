# Generic project trust and privilege: Test Design

Status: Proposed, 2026-10-11. Owner: Cam; acceptance pending. No complete parent test case has run.
Related [PRD](PRD.md) and [EDD](EDD.md).
The narrower accepted Phase A is the
[read-only payload inspection spike](../../research/payload-inspection-spike.md).
Its twelve host groups and [bounded native results](../../research/payload-native-evidence.md)
neither accept this parent design nor qualify trust/extraction. The later native
authentication experiment covers the distinct on-wire directory alias gap without changing the frozen historical native stimuli.
The [accepted authentication experiment](../../research/payload-authentication-spike.md)
has [eight host groups](../../research/payload-authentication-host-evidence.md) for
a narrow synthetic slice of PTT-04. Privileged race resistance, compatibility/
freshness policy and extraction remain unqualified.
The [accepted native authentication experiment](../../research/payload-authentication-native-plan.md)
has [component evidence](../../research/payload-authentication-native-evidence.md):
128 authentication/alias checks plus 90 historical checks on each console.
Its exact target and private keys were disposed. This completes no parent case
and adopts no runtime/trust/extraction interface.

The [proposed placement slice](../../research/payload-placement-spike.md) maps
authenticated file effects, unprivileged writer restrictions and independent
containment/partial-state observations to PTT-04/05. Its fresh exact target and
new authority await acceptance; no placement case has run.

## Requirement mapping

| Requirement | Test ID and stimulus | Expected evidence |
|---|---|---|
| PT-01 | PTT-01: malformed, duplicate/conflicting, oversized, unsupported and authenticated-but-dangerous definitions | Rejection before effects; precise bounded error, no implicit authority. |
| PT-02 | PTT-02: inspect/edit/cancel a plan, change artifact or target | No disk/file effects; changed plan cannot reuse confirmation. |
| PT-03 | PTT-03: unknown operations, metacharacters, out-of-range arguments, target substitution between validation and execution | Rejection at executor, no command interpolation, no wrong-target writes. |
| PT-04 | PTT-04: tampered payload, wrong signer, stale compatibility, verification failure | No privileged extraction/use or network trust downgrade. |
| PT-04 | PTT-05: traversal, absolute paths, escaping links, duplicate entries, special files, dangerous modes, expansion limits and extraction races | Target containment and policy enforced throughout effects; bounded failure state observed. |
| PT-05 | PTT-06: synthetic secret in input, validation failure, interruption and diagnostic export | No ordinary output/argv/log disclosure; restricted channel/file lifetime and cleanup observed. |
| PT-06 | PTT-07: ESC/CSI/OSC, control bytes, malformed text and excessive output across console modes | Escaped/rejected data cannot spoof confirmation; required summary remains readable. |
| PT-07 | PTT-08: exit-zero/failed postcondition, process loss, partial extraction and bootstrap failure | Planned/performed/observed states differ accurately; uncertainty reported, dependent work stopped. |
| PT-08 | PTT-09: unreviewed hook, changed executable identity, unauthorized adapter request | Rejected before execution; reviewed code cannot exceed granted scope. |
| PT-09 | PTT-10: independent document-viewer fixture plus DNS/firewall author scenarios | Same generic boundary applies; product pairing/network policy remains external. |
| PT-10 | PTT-11: debug mode, missing authorization/evidence, unavailable confinement | Controls unchanged; no support claim or unsafe fallback. |

## Levels and independent oracles

After scoped implementation approval, unit/property tests may cover parser bounds,
validators and escaping. Contract tests must independently assert rejection and
absence of effects rather than duplicate implementation decisions. Native component
and failure tests establish actual privilege, filesystem, secret-channel and console
behavior. Host tests cannot qualify these boundaries.

For planning/rejection, compare complete disposable target bytes and filesystem
inventories before/after and inspect operation records independently. Test effects
only on approved guest fixtures; never attach real host devices to test substitution.
Use two inventoried disposable targets only if a separately reviewed plan authorizes
them; otherwise test substitution through rejected/unlaunched configuration or a
non-destructive harness. The first native profile currently permits one target.

Archive fixtures are synthetic, owned and authenticated for testing; test keys have
no production authority. Never execute malicious hooks to show they should have
been rejected. Inspect mounts/paths, file contents, ownership/modes and outside-root
sentinels independently for extraction tests. Signature checks and directory scans
alone cannot establish containment during races.

Use unique synthetic secret sentinels; inspect private capture, argv, ordinary logs,
export preview and authorized-consumer observations without publishing raw values.
Core/memory exposure checks require reviewed native tools and honest limitation
reporting. Use local synthetic readiness markers for the unrelated application;
DNS/firewall cases are design reviews until separate isolated product tests exist.

## Fixtures, budgets and authorization

Use the [native profile](../native-profile/PRD.md) and its preflight/teardown. Exact
source/head/tree/base, backend/firmware, verified media, payload fixtures and run
commands must be recorded. Fix parser/runtime/mechanism choices before runnable
cases are authored; this document supplies no speculative configuration examples.

Apply PRD input/output ceilings and approved per-payload expanded-byte/file/time
budgets. Test at, below and above each bound; exceeded limits stop dependent effects.
Measure peak memory, operation time and evidence size. Disk-write and target-root
fixture creation require reviewed target confirmation; service/network tests require
separate explicit scope. Use no real credentials, host sharing or live networks.

Coordinate console tests with [UI research](../../research/text-ui-runtime-plan.md)
and stock integration failure cases with
[installer research](../../research/installer-integration-plan.md).
No debug or resource failure may bypass trust/authorization gates.

## Failure, cleanup and evidence

Inject tampering, output loss, interruption and resource exhaustion only within the
reviewed disposable fixture. Separate actions submitted from independent observations;
record partial/uncertain state and no rollback promise. Stop dependent mutations;
do not auto-resume. Verify owned processes/endpoints stopped, secret files removed
where applicable, fixtures detached/deleted and no listeners/attachments remain.
Incomplete cleanup or unobservable postconditions cannot be reported as a pass.

Report each PTT case as passed, failed, skipped or not run with source identities,
versions, commands, actual observations, limits and teardown. Publish only reviewed
escaped evidence, no raw secrets/private inventories. Current PTT-01 through PTT-11
status: not run; local make verify checks foundation and bounded host research.
The narrower accepted spikes have separate native evidence. Accepted designs,
actual native results and maintainer review precede any compatibility/support claim.
