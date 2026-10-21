# First native prerequisite stage: preparation proposal

Status: source preparation only; not an execution-ready packet or native authority.
Runtime is canonical `92166f07fd79c9d356ef3e1ed704eaeeed8653e4`, tree
`79060329e9d06283a95c247c9245bbdd60ac9081`. The 34-file runtime closure is sealed
by `runtime-bindings.json`. Production limits, entry refusal, readiness zero and
the four-source planner link remain unchanged. The adopted trace method changes
acceptance: companion traces cannot prove the exact unmodified binaries' syscall
sequence or absence of calls. This first stage uses no trace companion at all.

The bounded first stage would install stock OpenBSD 7.9/amd64 on one new 8 GiB
fully allocated raw file, then make one installed-guest launch and collect nine
unmodified dynamic-component cases. It establishes limit readbacks, parent/child
file-size outcomes and five closed-entry refusals. It excludes positive crypto,
actual verifier-child/alarm custody, locked loader/signify descriptor handoff,
confinement activation, static/set-ID copies, planner/VGA, companion traces and
product entry/CLI qualification. No keypair or signed fixture is needed: the
signature role contains only the literal unsigned closed-entry fixture.

The fixed future target basename is
`bootweave-qualification-92166f07-runtime-a1`; only `build.raw`, `data.iso`, private
environment/capture directories and distinct `qmp1/ser1` then `qmp2/ser2` sockets
belong there. Existing retained images, primary worktrees and Vectivium are
excluded. Stock installer and host tool/firmware pins are historical source
references from `current-tools-inputs.json` in the external r1 proposal. They
require fresh stable full readback, executable/dependency provenance and identity
seals before an eventual launch; their old hashes are not current admission.

Proposed finite ceilings, all below or equal to the prior proposal:

| Scope | Ceiling and ownership |
| --- | --- |
| Campaign | One original 10,800-second clock created before any target/media effect; two ordered single-use launches; no reset/retry authority |
| Each launch |3,600 seconds including 2,700 work and 900 closure; paused QEMU 11.1.1, fixed i440fx/qemu64, single TCG CPU, 1,024 MiB guest RAM, no network/shared mounts/passthrough |
| Host resources |20 GiB newly allocated aggregate; sampled QEMU RSS 2 GiB; raw five-channel capture 10 MiB per launch, 20 MiB campaign |
| Guest build | One original 1,200-second batch: bootstrap 120, 20 retained object slots of 25 each, three link slots of 150 each, setup/audit 130; 24 total compiler calls, no retries |
| Guarded compile | CPU 120, DATA 512 MiB, STACK 8 MiB, NOFILE 128, CORE 0, FSIZE 256 KiB; fixed generated argv, five seconds of each slot reserved for stop/wait |
| Native collection | One original 900-second work/suite window with 910-second hard alarm; nine fresh children, 20 seconds each including five for finalization; CPU 180, DATA 64 MiB, STACK 8 MiB, NOFILE 64, CORE 0, FSIZE 8 MiB |
| Guest allocation |32 MiB qualification tree, with 27 MiB nontrace and 4/1 MiB raw/decoded trace reserves unused by this stage; all compiler outputs/logs, temporary high-water bytes, inputs, cases and receipts must be charged |

Per-file compiler ceilings and sampled observations alone do not prove the guest
aggregate or compiler temporary high-water bound. The final coordinator/allocation
ledger must enforce that bound before this proposal can become execution-ready;
the stated 27 MiB is a ceiling, not a demonstrated fit or a silent allowance.

`compiler_guard.c` holds its original unreaped compiler/group until the last
authorized stop before one consuming wait. `native_supervisor.c` holds each
original child similarly, recording held input/executable/output identities and
fixed descriptor roles, with necessary oracle gates before another case. Separate
material/descriptor/terminal audits remain required. The dormant host guard holds only original
direct child PIDs; sampled PIDs never grant signal authority. Lost/consumed waits
seal all further signal rights. No retry of an uncertain numeric close, guessed
PID/group, recovered ownership or global process kill is permitted.

The following bindings must be finished and independently accepted before asking
for native execution authority:

1. Exact candidate commit/tree, clean source bundle plus all generated-header
   hashes, complete compiler/tool/dynamic-loader closure and fixed bootstrap argv.
2. Reviewed original-clock coordinator/watchdog and every bounded QMP/serial byte
   transport, paused device/block/PCI/handle/listener inventory, explicit resume
   admission and no new guest work after the work deadline.
3. Fixed provisioning/credential input custody and stock account/OS verification;
   independently checked guest compiler/binary/map/descriptor/output identities
   observed at staged build gates, before dependent cases.
4. Enforced host/guest whole-allocation and temporary high-water accounting,
   complete compiler-descendant/terminal evidence, actual raw witnesses and
   independent nine-case oracle interpretation. `collection_result=0` is not pass.
5. Normal halt, original emulator consuming wait, scoped terminal observations,
   stopped full-image readback, independent final failure/retention reduction and
   immutable execution packet review. A missing later-stage hash is an explicit
   blocking staged seal, never a guessed value or automatic permission to continue.

Fresh exact authority must cover new-file allocation, stock installation and
same-target writes, one isolated guest root credential, existing stock nobody
child-only privilege drop, actual per-process resource/timer/signal calls and
ordinary compiler/entry scratch effects. No host/global security configuration,
package/network/service changes, credential reuse, signing-key creation,
production activation, deployment or disposal is proposed. Native calls remain
unexecuted. Success and failure both retain the full new namespace; uncertainty
stops dependent work and retains partial bytes, journals and original custody
records. No cleanup/disposal approval is requested by this proposal.

Separately, changed-head `make verify` is held because its unchanged baseline
generates two host test signing keypairs and disposes newly created temporary
fixtures. Complete current coverage needs either an explicit exception for those
exact effects or an independently reviewed equivalent retained/public-fixture
revision. The focused preparation selector is useful partial evidence and cannot
replace that gate. PR #92 stays draft while applicable checks or native gates remain
unsatisfied. There is no remaining trace-method decision; exact native execution
approval comes only after these concrete bindings are complete.
