# Entry trace evidence: owner decision

Status: recommendation only, not adopted. Native execution/effects and the trace
method remain held. Routine isolated nontrace harness/oracle source preparation
continues independently; see [prepared sources](../../tests/qualification/README.md).
The source baseline is canonical `92166f07fd79c9d356ef3e1ed704eaeeed8653e4`.

Recommend explicitly amending the planned complete exact-unmodified-component
trace requirement to use labeled source-correlated companions plus separate
unmodified-limit evidence. **That changes an acceptance criterion**, not merely
test implementation. It grants no native authority or runtime/CLI activation.

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

The proposed new companion would replace only the fixed FSIZE set/get calls in
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
| Source-correlated companions, if accepted and later qualified | Complete PID-correlated syscall/return/signal/terminal records for the disclosed modified binaries; bounded ordering/absence observations within that coverage | Not exact-unmodified-binary traces or actual65,536/0 file-size enforcement |

Complete kernel records are required for the **companion channel**. Independent
readback/file-size probes supply the separate **unmodified-limit channel**; neither
a numeric exit5 nor an unchanged canary fills missing syscall records. All trace
budgets, complete-record/loss checks, independent review and failure retention stay
mandatory. No partial trace, wider path, larger runtime budget or product bypass
becomes acceptable.

## Alternative and decision question

No practical exact-unmodified observer is established in the current approved tool
recipe. An external observer would need to prove recording independent of tracee
FSIZE, exact object/instruction and fork/exec/PID correlation, complete start/stop
coverage, loss detection, custody/export and fit within the same budgets. That
needs a separate bounded feasibility investigation; it is not a proven drop-in
alternative or permission for a new tool, kernel/VM instrumentation or security
setting change. Keeping the current criterion leaves the trace gate held.

> Approve replacing complete exact-unmodified-component traces with explicitly
> labeled companions using fixed 4 MiB FSIZE and virtualized readback, paired with
> separate unmodified-limit enforcement and production-refusal checks, while
> keeping current budgets and native approval gates?

Acceptance would adopt this evidence-method amendment only. It would still require
independent companion source/oracle review, complete execution bindings and fresh
exact target/effects authority. Until an answer, no companion implementation or
native dispatch is cleared. All historical failures and Vectivium holds persist.
