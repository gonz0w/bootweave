# Payload inspector host evidence

Observed 2026-10-12 under Cam's accepted
[Phase A scope](payload-inspection-spike.md). Base:
028a2e951cb1e42c6a0e44f51a674f46bea10b96, merged PR #38.
Original research source: research/payload/inspect.c, 9237 bytes, SHA-256
8009aed82f67ae60c70538579fa493995a87fa6ff1ea324dc23ee79af0847698.
Clean signed candidate/base/tree and full verification outcomes accompany the PR.

The [format source review](payload-source-findings.md) binds offsets, framing and
encodings to authenticated OpenBSD 7.9 release source. The reader is original
BSD-3-Clause C with base libc; no upstream code or third-party dependency is added.
Host: Darwin arm64, Apple clang 21.0.0 (clang-2100.3.20.102), target
arm64-apple-darwin27.0.0, Python 3.14.7. Command: `make test-payload`; cc flags:
`-std=c99 -Wall -Wextra -Werror -pedantic`. Warning-clean compilation passed.

All nine host regression groups passed, covering PAY-01 through PAY-09 at the
experimental admission boundary. Tests combine host tarfile-generated ustar
fixtures, corrected-checksum mutations and a separately constructed fixed header.
The independent byte fixture's checksum was corrected during test development;
it then passed without using the encoder/mutation helper. An initial diagnostic
assertion incorrectly treated fixed punctuation as echoed input; that assertion
was narrowed while private/control-bearing text exclusion remained covered.

Observed acceptance includes reordered files/directories, implicit parents, exact
255-byte joined paths, full-width name/prefix fields, 128 members, 256 KiB per file,
512 KiB aggregate content and exactly 1 MiB framed input. Excess bounds, unsafe or
ambiguous paths/trees, all excluded type bytes, unreviewed metadata, malformed
numbers/checksums, truncation, nonzero padding, concatenation and trailing garbage
rejected. Empty streams/archives and directory-only archives rejected. Content
stayed opaque; numeric summaries contained no names or payload bytes.

Stalled input stopped after the ten-second whole-stream deadline within the
twelve-second observed bound. Excessive declared content rejected while its input
writer remained open. A valid archive supplied in 17-byte writes completed.
INT/TERM/HUP could not complete. Broken output failed; a filled output pipe stopped
within the five-second write deadline (observed four-to-seven-second bound).
Owned pipe checks verified restoration of O_NONBLOCK, not all platform flags;
the host exposes an additional unrelated flag after descriptor use, as also
handled by the existing console tests. No exact native flag-restoration pass is
claimed. Required counts came only after complete framing/EOF; partial output
remains possible on I/O failure and must be paired with the process result.

The focused run observed peak inspector RSS of 1933312 bytes through independent
wait4 observations across supplied-buffer cases, below 32 MiB. Streaming/deadline
cases were separately supervised; their per-process RSS was not measured by that
helper. Source is below 32 KiB and a compile-time application-array/stack allowance
is below 64 KiB; this is not a native footprint measurement. The final focused run
sampled peak owned file allocation of 1110016 bytes, below 20 MiB, and checked
capture sizes against 1 MiB. Full candidate observations are retained in local
command evidence/PR. Platform time -l
could not query kern.clockrate inside the host sandbox; wait4 supplied the RSS
measurements instead. No tooling or privilege exception was needed.

Supplied-buffer fixtures were independently checked unchanged after reading.
The owned existing-file canary remained untouched; rejected shell-like arguments
created no effect. No archive member was extracted or executed. Tests checked
directory inventories, closed owned files/pipes, waited or terminated children,
and removed temporary binaries/fixtures. Only original source, tests and sanitized
evidence are versioned; private source-review files remain ignored.

No native guest, media build, disk target, signature-trust integration, extractor,
hook, application or service ran. The host cases do not complete parent PTT-04/05
or qualify native parsing, immutable verified-input lifetime, containment during
effects, ownership/modes, race resistance or application readiness. Research is
evidence until adopted; no public payload/profile or runtime contract follows.
