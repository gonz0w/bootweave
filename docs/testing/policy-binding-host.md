# Portable compiled-policy and digest evidence

On 2026-10-20, the independently reviewed internal
[policy-binding increment](../design/policy-binding.md) passed eleven focused
portable groups on the connected Mac ARM64. The exact source bindings, original
failure and corrected result are retained in
[results.json](policy-binding-evidence/results.json),
[first log](policy-binding-evidence/focused-r1.log) and
[corrected log](policy-binding-evidence/focused.log).

The common component compares exact compiled key/definition identity, unchanged
strict v1 parsing, project/system tuple and full retained payload digest. One
compiled inert synthetic policy is used; no real publisher or private key exists.
The separate fixture links the real Mac base CommonCrypto SHA-256 adapter and
independent hashlib/known-vector oracles. The normal non-OpenBSD adapter refuses
as unavailable. No new SHA/signature implementation or dependency was introduced.
The material module's existing pure public-record grammar is reused; its dormant
I/O functions are never invoked by binding. Object references were checked.

Exact/changed/reordered definitions, key bytes/comments/lengths, malformed linked
policies, strict syntax/system refusal, different project identity, payload
suffixes/NULs/full-cap bytes and first/last digest changes have independent
expected results. All fifteen request/input/output/compiled-policy alias pairs
and invalid ranges are checked with no-write canaries, including separate address
and undefined-behavior instrumentation. Injected hash-adapter failure and repeated
match/refusal/match expose no partial plan or cached success. A deliberately
invalid archive can match: this stage grants no structure or signature verdict.

The corrected run took14.359 seconds in unittest and14.684 seconds in the outer
envelope:171 invocations,43 compile/link steps,121 byte-driver invocations,
two object-symbol inspections and five intentional launcher-failure controls.
All171 direct waits were consumed with no lost wait;341 written stream files
were retained. One injected stdout publication failure is explicitly unconfirmed,
with its retained-byte hash and the other stream still recorded. Overflow on
either pipe, a four-second timeout, a nonzero partial-output exit and that write
failure cannot pass. These failed control rows are distinct from regressions.

The first run failed two fixture cases: this Mac sanitizer rejects leak detection,
and strict C99 rejects an empty `{}` initializer. The corrected fixture uses
`{0}` and explicitly disables the unsupported leak option. Address/undefined
behavior checks remain enabled; no leak-detection qualification is claimed.
All134 original rows, outputs, builds and the failed outer receipt remain intact.
No product source changed between the two focused runs.

Each fresh run used a replaced minimal environment, private HOME/tmp, no inherited
credentials or network request, work270/total300 seconds, CPU90/process, fd128,
CORE0,128MiB/file and sampled512MiB owned-output/8MiB-log ceilings. Compilers use
CPU20/fd64/16MiB-file/20-second limits; drivers use CPU2/fd32/16KiB-file/four seconds,
with five seconds cleanup. Live nonblocking collection keeps16KiB/stream and
records any overflow and incomplete EOF. This is a trusted finite portable check,
not hard memory isolation or a general hostile-worker boundary. Builds and case
data are retained rather than deleted.

The outer original leader stayed unreaped through its final owned-group stop
attempt. Darwin refused that signal while only the zombie leader was observed;
the sole actual consuming wait and later empty group are separately recorded.
No signal occurred after consumption or against recovered/historical IDs.

New C/header sources total7843 bytes; whole src remains65346 bytes under the
unchanged64KiB ceiling. Source assertions reserve1024 bytes for explicit common
objects and require native SHA2_CTX plus that allowance to fit2048 bytes. No
native stack/RSS, timing, libc behavior or confinement result is claimed.

Production CLI/Makefile linkage is unchanged. Signature success, complete archive
inspection, trusted immutable composition, cancellation/hard timers, plain review
rendering, native qualification and real publisher/distribution remain separate
gates. No target, extraction, privilege, deployment or native approval follows.
Full clean-candidate make verify, independent terminal/source review and normal
signed/DCO PR/canonical checks are recorded separately at delivery; this focused
result alone is not full verification or authenticated-product acceptance.
