# Isolated authentication fixture policy

Direction adopted by the owner on 2026-10-22: use two isolated, non-production
test signing pairs for the genuine signed-archive corpus. This replaces waiting
for an unavailable public corpus. It does **not** authorize generation, signing,
native execution, cleanup or activation. The exact proposed effects are data in
[the effects packet](../../tests/qualification/auth-fixture-effects.json).

The useful result is reusable public test inputs for the existing PAV01–PAV08
real authentication path: selected-key accepted 2,048-byte archive, selected-key
1,536-byte traversal archive, selected-key accepted archive padded to exactly
1,048,576 bytes, and an other-key signature for the small accepted archive. Four
signatures and two public records are required. Release signatures, representation
fixtures, skips and verifier doubles cannot replace these cryptographic outcomes.
No production trust anchor or media policy is selected.

## Custody and effects

The proposed attempt owns one new exclusive private namespace, never an old
fixture. Create its directories as 0700 and files as 0600 with umask077. Two
unencrypted test-only secret keys remain in its private subdirectory; only the
exact four signer children may subsequently read them. The coordinator records
mode, owner and stable descriptor identity, never secret bytes or secret-key
digests. Keep keys out of Git, public manifests, logs, agents, media and network.
Public corpus means reusable non-secret records, not world-readable permissions.
The owner retains the complete namespace after success, failure or interruption
until a separate explicit disposal decision; no automatic expiry or deletion.
No existing file is overwritten and no attempt is retried.

The unchanged tool's key generation uses O_EXCL, but detached signature output
uses O_TRUNC|O_NOFOLLOW. Each signing call therefore gets a distinct freshly
created private output directory and an absent fixed filename. It relies on a
trusted quiescent coordinator and no competing same-user writer; this is not
hostile-filesystem isolation or O_EXCL signature creation. A changed namespace,
unknown writer or lost custody blocks dispatch. Do not patch the cryptographic
tool or borrow authority from the historical signing experiment.

Use one original 300-second clock including 60 seconds reserved for closure;
2 generation +4 signing +6 verification children, each at most15 seconds,
serially, no retries. Each15-second child window includes stop/wait, EOF and
stream close. Only0/1/2 are inherited at exec; disclosed child file opens are
separate. Retain the actual original child handle through its last
direct-PID stop and sole consuming wait. Observe status, complete bounded streams,
EOF and resource use. A sampled PID, zero signal or path match grants no rights.
Missing evidence is uncertainty. No broad signal, recovered PID authority or
automatic removal is permitted. Detailed limits and exact argument arrays are
in the packet. These are proposed limits, not measured resource compliance.

## Acceptance and remaining gates

All four signatures must verify with their own signer. Two additional controls
require normal exit1, empty stdout and the exact bounded diagnostic for a changed
message (signature verification failed) or the other signer (checked against
wrong key). A crash, signal, IO/resource failure, missing stream or wrong
diagnostic is failed/uncertain evidence, never a successful negative control. The corpus is sealed only
after stable public byte/length/hash and tool readback, genuine verification,
complete child/custody evidence and independent review. No private key is needed
for later consumers. An opaque non-secret diagnostic canary replaces the old
private-key diagnostic read without weakening suppression coverage.

The packet is not executable admission: the bounded retained signer coordinator,
complete tool/dependency and executor seals, and independent final review remain
unfinished. The shared-host lost-process hold is unresolved; the denied observer
was not retried. Exact generation/signing effects require fresh explicit approval
after those bindings exist. The retained full-coverage adapters must preserve all
original positive/negative, lifetime, permission and failure coverage across the
complete make verify closure. Merely producing the corpus does not complete them.

PR #92 remains draft. No full pass on its latest head is claimed; old passes stay
bound to their historical heads. Production source limits, readiness zero,
unconditional normal entry refusal, planner link and native qualification gates
are unchanged. The adopted trace method supplies no exact-binary trace claim.
