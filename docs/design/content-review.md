# Internal content-review model and plain-text renderer

Status: delegated AP-03–05 continuation after PR #83, 2026-10-20. This scoped
requirements/design/Test Design is independently reviewed before implementation.
The adopted [read-only scope](authenticated-plan.md), [policy binding](policy-binding.md)
and [bounded I/O contract](authenticated-io.md) remain controlling. This increment
is inert and unlinked from production CLI/Makefile sources. Vectivium's boundary
decision and Aegvium's pause are unaffected.

## Requirements and supported result

| ID | Requirement |
| --- | --- |
| CR-01 | Recompute policy/key/exact-definition/tuple/payload-digest binding, then inspect the complete same immutable payload span with the existing restricted-ustar component. No cached observations or caller-supplied success flags. |
| CR-02 | Publish the complete typed plan/count model and deterministic plain-text report only after both stages and formatting succeed. Signature authentication is always explicitly unestablished; no aggregate authenticated/safe/ready verdict. |
| CR-03 | Fixed report states no selected target and no authorized, performed or independently observed operations. No credential, signature, raw key, archive content, parser diagnostic or path opening enters output. Filename remains a label. |
| CR-04 | Refuse invalid/aliased ranges without writes; ordinary disjoint failure clears bounded outputs and the whole result before setting fixed stage causes. No partial model/report or stale prior success. |
| CR-05 | No descriptor/process/allocator/environment/clock/network/device/writer API, verifier launch, public config/CLI, privileged operation, output syscall or persistent receipt. Future caller supplies immutable lifetime, cancellation/original-budget checks and hard timers. |

Policy match and structure success describe bytes relative to the compiled media
policy, not publisher authentication, freshness, installed readiness or authority.
The existing inert compiled policy remains unchanged; tests link distinct const
synthetic policies. There is no signature argument or boolean allowing the caller
to upgrade the fixed unestablished state. A future real authenticated composition
must prove signature success on these same retained bytes in its own trusted entry.
This pure model is not that entry or a token to supply to it.

## Internal API and ordering

Add src/content_review.c/.h with one fused internal entry accepting the existing
const bw_policy_request, caller-owned char output span/capacity, and typed result.
The result holds fixed cause, underlying policy cause, output length, validated
bw_plan and bw_payload_summary. All fields except fixed causes remain zero on
failure. No independent public renderer accepts asserted plan/count/auth states.
Return1 means complete model/report; return0 means refusal. Causes are exactly
NONE, ARGUMENT, POLICY, STRUCTURE and FORMAT. The underlying policy cause is
meaningful only on POLICY refusal and is zero otherwise. Output length excludes
the terminating NUL; required capacity is that length plus one.

The request, output, result, compiled policy and three input spans are pairwise
disjoint. First establish request/output/result/policy ranges before dereferencing
request members. Numeric overflow, positive-length null span, null request/result,
or any overlap rejects without any write. Empty spans are allowed through range
checking and then ordinary argument rejection. As in existing components, callers
must supply valid mapped storage and immutable exclusive lifetime; numeric checks
cannot prove mapping or exclude concurrent mutation.

The output capacity is 1..65536 including its terminating NUL; zero or greater
capacity is an argument refusal that clears a disjoint result but does not touch
the invalid-capacity output. For a valid output capacity, ordinary rejection zeros
the complete output span and result. Preflight has no downstream call.
Positive-capacity NULL output is an invalid range with no writes; NULL output plus
zero capacity is ordinary ARGUMENT refusal clearing the disjoint result only.
Overflow/overlap still rejects without writes even when capacity is invalid.
The stage order is:

1. Clear bounded outputs, validate nonempty adopted input caps and output capacity.
2. Call the real bw_policy_bind on the supplied original spans and local result.
   On refusal preserve only its fixed cause, never a partial plan. No inspector call.
3. Call bw_payload_structure_review with exactly request.payload and its original
   length, without copying, reserializing, reopening, offsetting or substituting it.
4. Build the complete report in fixed 2048-byte local scratch using constant
   snprintf format/unsigned counts. Check formatting and capacity including NUL.
5. Only then copy complete text and publish plan/counts/length with success cause.

Short output, formatting error or truncation yields no text/model. All formatting
uses validated printable v1 fields, literal labels and bounded decimal counts;
there is no ANSI, escaping mode, locale-sensitive grouping, raw diagnostic or
user-provided format. Project name/file are explicitly labeled supplied values.
Repeated calls independently recompute everything. A result is not reusable proof
of an input lifetime; no persistent or externally supplied completion object exists.

The exact plaintext is frozen by independent fixtures before execution. Its title
is `UNAUTHENTICATED CONTENT REVIEW - NO OPERATIONS AUTHORIZED`. Required lines
distinguish compiled policy match, definition pin, payload digest, complete archive
structure, `Signature authentication: not established`, project/system/filename,
member/file/content-byte counts and the fixed absence of effects. This is a count
summary of a completely checked archive, not a per-file extraction/effect inventory.
Exactly18 LF-terminated lines appear in the agreed order, with no blank line.
The final three lines are `Operations authorized: none`, `Operations performed: none`
and `Operations independently observed: none`. Those describe this pure component,
not an independent observation of the external host or a source of write authority.

## Budgets and implementation limits

Keep 1KiB key/16KiB definition/1MiB payload and all existing archive bounds unchanged:
128 members, 512KiB total content, 256KiB/file, path255/depth16, fixed uid/gid0 and
0755 directories/0644 files, no links/special/extension types or aliases, complete
padding/two-zero-block trailer/EOF. Reuse the existing components unchanged.

New C/header ceiling is 8KiB. Existing src is65346 bytes, leaving only190 bytes
under its64KiB source-size limit; this scoped increment raises whole-source size
to72KiB, retaining all earlier component ceilings separately. This is source text
accounting, not a runtime resource increase. Additional explicit local objects
fit4KiB, with a compile-time assertion. The existing34KiB inspector workspace,
2KiB policy allowance, caller snapshot/output and all objects must remain inside
the adopted2MiB explicit application-storage ceiling. Native stack/RSS and hard
runtime budgets remain unchanged and unqualified. No new dependency or crypto.

## Independent behavioral Test Design

| ID | Stimulus and independent required observation |
| --- | --- |
| CR-T01 | Separately pinned viewer, DNS and firewall archives: exact independently authored report/model/counts; fixed signature-unestablished and zero-effect lines. |
| CR-T02 | Key/definition/tuple/payload mismatch and unavailable/injected hash failure: no inspector/format call; zero bounded output/model, fixed policy cause only. |
| CR-T03 | Exact-pinned but structurally invalid archive corpus, including final-member/trailer/padding/alias/type/owner/mode/size/depth failures: full refusal and no completed report. Pins are not signatures. |
| CR-T04 | Static test-only wrappers record actual payload pointer and length at policy-hash and real inspector call. Exact span identity and complete digest agree; altered byte/suffix cannot reach successful inspection/report. No runtime callback in product. |
| CR-T05 | Exact required output capacity including NUL succeeds; one short/zero/over-cap and injected snprintf error/truncation refuse with zero result/text. Buffer canaries unchanged outside declared output. |
| CR-T06 | Every pair among request/output/result/policy/key/definition/payload, null/empty/overflow spans and input limits: no-write versus ordinary-clear classifications, no hash/inspection/format for preflight. Sanitizer checks exercise actual alias paths. |
| CR-T07 | ESC/CSI/OSC/NUL/non-ASCII/control metadata with separate exact pins: parser refusal; printable metacharacters/maximal valid fields remain a single labeled ASCII line and never gain format/control meaning. |
| CR-T08 | Same-process success/failure/success and late formatting failure: no stale model/text; production CLI/linkage unchanged; audit new object's references and retained source/runtime budgets. |

The independent reviewer authors golden plaintext/corpus expectations without
calling candidate functions. Test-only adapters link existing real binder/parser/
inspector with Mac CommonCrypto hashing and immutable link-time policies. Native
adapter remains unavailable here. Instrumentation may wrap real calls only in a
separate fixture object; no verifier, generated private key, network or native run.

Reuse the reviewed finite owned launcher behavior from policy tests rather than
create general infrastructure: retain cases/builds/streams/ledgers, live16KiB per
stream bounds, real WNOWAIT/stop-before-wait custody, explicit once-only closes and
publication failure dispositions. New fixtures have at most64 policies/320 calls;
compiler CPU20/fd64/16MiB-file/20s, driver CPU2/fd32/16KiB-file/4s plus5s cleanup,
CORE0 and replaced private environment. Address/UB instrumentation stays active;
Mac leak detection is unavailable. Outer work270/total300s, CPU90/process,fd128,
CORE0,128MiB/file, sampled512MiB owned output and8MiB log; no hard isolation claim.
Review the exact new fixture/envelope before candidate execution. Clean signed
candidate make verify, zero required skips, independent actual source/terminal
review and normal PR/canonical binding precede eligible merge. Preserve originals
and all previous worktrees/evidence. No old native authority is reused.
