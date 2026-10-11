# Stock installer integration: bounded research plan

Status: Proposed, 2026-10-11. Owner and acceptance authority: Cam.
[Issue #3](https://github.com/gonz0w/bootweave/issues/3).
Acceptance date/source: pending. Documentation preparation is authorized; native
execution, media modification and disk writes are not authorized by this plan.
The merge of a planning PR alone does not identify permitted destructive targets.

## Outcome and boundaries

Determine whether OpenBSD 7.9 amd64 installation machinery can underpin a generic
text installer while preserving reviewed target confirmation and honest failure
state. Produce source-bound findings and a recommendation, not an integration API,
configuration grammar, runtime choice or production implementation.

Depend on the [first native profile](../design/native-profile/PRD.md), its
[engineering boundaries](../design/native-profile/EDD.md) and
[Test Design](../design/native-profile/TDD.md). Before native research, record
explicit acceptance, exact execution configuration and passed fixture/isolation
preflight. Issues #4 and #5 separately own renderer and project-authority design.
No DNS/firewall policy, network discovery, service installation, product accounts,
upgrade support or general recovery orchestration is part of this experiment.

## Initial evidence and questions

Primary documentation inspected 2026-10-11:

| Source | Documented behavior | Research implication |
|---|---|---|
| [7.9 autoinstall(8)](https://man.openbsd.org/OpenBSD-7.9/autoinstall.8) | Unanswered questions use installer defaults; local ramdisk response files can trigger automation. | Missing answers cannot be assumed to stop safely. Identify every default and entry trigger from pinned source before booting modified media. |
| [7.9 install.site(5)](https://man.openbsd.org/OpenBSD-7.9/install.site.5) | Executable site hooks run in the installed root at installation completion. | Hooks are privileged reviewed code; chroot alone is not a security sandbox. |
| [7.9 INSTALL.amd64](https://cdn.openbsd.org/pub/OpenBSD/7.9/amd64/INSTALL.amd64) | bsd.rd embeds installer tools; install79.iso includes offline OS sets. | Begin with offline stock media; record verified inputs and exact release source. |

These are manual findings, not native observations or proof of safe integration.
No source revision/file digest has yet been inspected for this issue. Do not use
mutable FAQ/current-tree behavior as a release contract.

## Research phases and authority

Phase A is source inspection only. Acquire authenticated release source using the
native profile's trusted verification procedure. Record archive digests and exact
paths/file digests for installer entry, shared question handling, disk operations,
set verification/extraction and site hooks. Inspect ramdisk/media build machinery
and license headers. No downloaded source or hook executes on the host.

Produce a complete question ledger for the selected path: exact prompt, matching
rule, default, condition, validation/retry behavior, authority, first possible disk
write and behavior when the answer is absent or ambiguous. Include disk selection,
partitioning, set selection, verification failures, console changes and installation
completion. An incomplete ledger is a stop condition for automated testing.

Phase B is a separately authorized native stock-media observation. Use a fresh
isolated fixture, verified media, no network and no site hooks. Read-only boot/menu
inspection follows the native baseline. Installation choices remain prohibited
unless a reviewed run explicitly permits writing the inventoried disposable disk.
No wrapper or response-file generator is implemented in this phase.

Phase C would test local response files and tiny reviewed media changes. Before
execution, a new reviewed run plan must specify exact source patch/media digest,
response bytes, guest target inventory, allowed write extent, stop/kill method and
expected prompts. Build modified media inside a disposable OpenBSD build guest,
not with host cross-build tools. Unverified modified media cannot inherit upstream
signature claims; document derivation and verify its separately trusted artifact
identity. No network transport or public mirror fallback is permitted.

Phase D would examine one inert generic site-set marker and one reviewed synthetic
hook with bounded behavior. Requires separate acceptance of artifact verification,
extraction paths/ownership/link boundaries, privileges and target confirmation.
No real payload, private credentials or persistent service is used. Malicious
archive execution is deferred to the scoped trust design in issue #5.

## Target confirmation and state

For any proposed write run, review only a freshly owned disposable disk with no
backing chain. Confirm its host attachment identity, size and guest mapping against
the effective emulator configuration; an sdN label alone is insufficient. Before
submitting the first destructive choice, the operator confirms that exact inventory
and the allowed partition/format/extraction scope. Changed attachments invalidate
confirmation. Omission, cancellation or mismatch stops input before writes.

The question ledger is research material, not an engine safety boundary. Stock
installer behavior must be observed, including unsafe defaults, without pretending
it enforces Bootweave's future policy. Experiments that might invoke defaults capable
of writes require explicit authorization for that target even when testing failure.
No reviewed plan means no unattended boot into autoinstall.

Record intended actions, submitted inputs and independent observations separately.
Observe disk labels/filesystems and expected file markers after authorized writes;
reboot readiness requires a separately defined postcondition. A zero exit or hook
return does not establish a usable system. Interruption can leave partitioned,
formatted, partially extracted or uncertain state. Never claim general rollback;
retain evidence, stop and discard the fixture rather than automatically resume.

## Test Design and acceptance mapping

| ID | Question/test | Expected evidence and gate |
|---|---|---|
| IR-01 | Enumerate entry triggers and complete selected-path prompt/default ledger | Pinned source paths/digests and exact questions; source review prerequisite. |
| IR-02 | Compare manual answers with exact and omitted response matches | Observed prompt/default behavior, including destructive defaults; phase C/write authorization required. |
| IR-03 | Test changed or ambiguous question matching | No claim of fail-closed behavior without observations; stop automation if source path is uncertain. |
| IR-04 | Review wrong-disk, changed attachment and cancelled confirmation cases | Preflight refuses unapproved target; never expose host devices to demonstrate failure. |
| IR-05 | Test missing sets and verification failure offline | Record actual failure/retry path and possible bypass prompts; never submit a verification bypass. |
| IR-06 | Observe verified generic site marker/hook success and deliberate bounded hook failure | Independent target postconditions and actual hook authority/failure propagation; phase D only. |
| IR-07 | Interrupt one explicitly authorized install at a reviewed stage | Planned/submitted/observed/uncertain state and safe fixture disposal; no resume/rollback claim. |
| IR-08 | Compare keyboard and serial confirmation/failure visibility | Exact console transcripts sanitized; renderer equivalence remains issue #4. |
| IR-09 | Verify limits, isolation and teardown for every launched run | Native profile budgets, stopped processes, removed owned fixtures and absent listeners. |

Source review can satisfy only source findings. Host simulations cannot qualify
installer behavior. Native cases require separate fresh fixtures and actual results
reported as passed, failed, skipped or not run. Missing source findings or gates
block the affected run rather than invent expected stock behavior. Initial result:
IR-01 through IR-09 not run; cited manuals supply starting hypotheses only.

## Limits, failure and cleanup

Use the native profile's RAM, disk, media, allocation, duration and capture limits.
Measure modified-media growth and each operation duration; no resource minima or
performance guarantee is established. Review any additional build-guest resource
budget and attachments before phase C. Use a maximum of one target disk and one
research case per launched installer; new cases receive fresh disks.

Stop on unexpected prompts, lost console, unknown target mapping, verification
failure, unauthorized device/network discovery or exceeded limits. Halt input,
terminate only the owned process and mark uncertain writes honestly. Debugging does
not relax verification, authorization or isolation. Capture remains private,
bounded and secret-free; publish only escaped, manually reviewed excerpts.
Teardown follows the native profile: close endpoints, stop guests, remove only owned
fixture files after sanitized evidence retention, and confirm no processes/listeners
or attachments remain. Incomplete cleanup is an incomplete experiment.

## Alternatives and decision deliverable

Compare a bounded front end over stock machinery, a minimal reviewed upstream-source
patch and a custom implementation. Assess safety of defaults, release maintenance,
OS set verification, privilege separation, native footprint and license/provenance.
Do not select an option solely because autoinstall can run unattended. Third-party
installer reuse requires separate file-level license/provenance review; none is
copied here. Preserve an unmodified stock control image for comparison.

Publish exact base/head/tree, release/source/artifact/backend identities, commands,
versions, ledger, case outcomes, resources, skips, uncertainty and teardown. Record
whether stock behavior can meet proposed requirements, which controls require an
adapter, and what remains unsupported. Adoption requires reviewed scoped
PRD/EDD/Test Design and an ADR for durable architecture/dependency choices. This
research plan alone supplies no future public API or implementation permission.

[Provisional source findings](installer-source-findings.md) now record selected
7.9 source bytes and prompt/default paths. Signature verification and the complete
ledger remain outstanding; no native test is marked passed.
