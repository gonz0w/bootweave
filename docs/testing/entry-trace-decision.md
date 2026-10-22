# Entry trace evidence: adopted method

Status: evidence method approved by the owner on 2026-10-21 at 01:59:26 UTC.
Native execution/effects remain held. Isolated companion and nontrace source
preparation may continue; see [prepared sources](../../tests/qualification/README.md).
The source baseline is canonical `92166f07fd79c9d356ef3e1ed704eaeeed8653e4`.

Amend the planned complete exact-unmodified-component trace requirement to use
labeled source-correlated companions plus separate unmodified-limit evidence.
**This changes an acceptance criterion**, not merely test implementation. The
owner approved the explicitly stated tradeoff below; it grants no native
authority or runtime/CLI activation and supplies no qualification result.

## Why there is a decision

The production authenticated entry remains unavailable: the product retains its
legacy planner link, native boundary refusal, readiness zero and inert policy.
A test-policy/forwarder build cannot prove a positive production capability.
Separately identified unmodified components can test the real limits, but complete
kernel traces of their actual execution are not currently established.

The [historical planner trace](project-plan-trace-amendment.md) actually failed at
its 8 KiB file-size limit. Its accepted companion exception covers that planner
only. No current authenticated-entry native trace has run. The entry parent now
uses FSIZE65,536 and its verifier child FSIZE0, leaving complete unmodified-limit
trace feasibility unproven; this is not a newly observed native failure.

The approved companion design replaces only the fixed FSIZE set/get calls in
separately identified evidence objects. It would check the exact expected parent
65,536/child0 requests, set and verify a real 4 MiB kernel limit, and report the
original requested limit to the companion through a fixed per-process readback
state. A set-only wrapper fails today's exact readback checks. Unexpected calls,
state, OS errors or an inherited actual ceiling too low for this method must
refuse. Other limits, timers, paths, code and failure checks remain unchanged.
Every changed observation and its source/object/build binding must be disclosed.

## What each evidence channel could establish

| Channel | Supported claim | Remaining limitation |
| --- | --- | --- |
| Exact closed production build | Current planner behavior and authenticated-entry refusal/readiness0 | No available positive authenticated production entry |
| Unmodified-limit component probes | Actual soft/hard values, real owned-file size enforcement and observed termination/outcomes | No complete syscall-attempt absence/order, global confinement or atomic correlation with a modified trace binary |
| Source-correlated companions, after qualification | Complete PID-correlated syscall/return/signal/terminal records for the disclosed modified binaries; bounded ordering/absence observations within that coverage | Not exact-unmodified-binary traces or actual65,536/0 file-size enforcement |

Complete kernel records are required for the **companion channel**. Independent
readback/file-size probes supply the separate **unmodified-limit channel**; neither
a numeric exit5 nor an unchanged canary fills missing syscall records. All trace
budgets, complete-record/loss checks, independent review and failure retention stay
mandatory. No partial trace, wider path, larger runtime budget or product bypass
becomes acceptable.

## Decision record and remaining gates

No practical exact-unmodified observer is established in the current approved tool
recipe. An external observer would need to prove recording independent of tracee
FSIZE, exact object/instruction and fork/exec/PID correlation, complete start/stop
coverage, loss detection, custody/export and fit within the same budgets. That
needs a separate bounded feasibility investigation; it is not a proven drop-in
alternative or permission for a new tool, kernel/VM instrumentation or security
setting change. This is why the accepted method deliberately gives up that
exact-unmodified trace claim rather than asserting that separate probes prove it.

The owner answered **yes** to accepting instrumented test copies with a 4 MiB
file-size allowance and simulated readbacks, paired with unmodified components
demonstrating their real limits and outcomes. The approved question explicitly
said this replaces exact-unmodified-binary traces, cannot prove their exact
syscall sequence or absence of calls, keeps production limits unchanged, and
does not approve native execution or activation.

Companion source/oracle review, complete execution bindings and fresh exact
target/effects authority remain mandatory before native dispatch. Complete trace
coverage/loss checks and separate unmodified evidence cannot be omitted. No
production cap, refusal, readiness or runtime source changes follow from this
adoption. Native boot, execution, security changes, activation and disposal remain
held. All historical failures and Vectivium holds persist.
