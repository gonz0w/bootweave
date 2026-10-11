# Host-only media verifier: review proposal

Status: Proposed, 2026-10-11. Owner/acceptance authority: Cam.
[Issue #10](https://github.com/gonz0w/bootweave/issues/10).
No installation, build, verifier execution or trust-root adoption has occurred.
Related [verification routes](provenance.md) and [EDD](EDD.md).

## Concrete candidate and provenance

Candidate: jpouellet/signify-osx 1.4, a macOS port of OpenBSD signify.
The [tag](https://github.com/jpouellet/signify-osx/tree/1.4) resolves through the
read-only contents/ref API to commit 9a654d698f5dc957ac407a3156a638bb3b77add2.
Homebrew formula record at commit e570f1a1d5c61c0407ff419a5d484420bf032ffe
identifies source archive SHA-256
5aa954fe6c54f2fc939771779e5bb64298e46d0a4ae3d08637df44c7ed8d2897.
This identifies a proposed source pin, not an authenticated artifact already checked.
Do not follow moving HEAD or run its source-refresh targets.

Reviewed tagged README, Makefile, LICENSE and regression runner as inert text.
The port ships upstream source plus portability helpers; Makefile compiles that
source and substitutes a no-op for pledge. Its source-refresh target fetches mutable
CVS source, which is excluded from this proposal. README's stated macOS test history
is older than this host; modern host compatibility is unmeasured.

Tagged LICENSE contains an ISC permission notice for the port. Bundled upstream
files retain their own notices and require file-level review before approval.
No source or binary would be redistributed in Bootweave media. The host port lacks
OpenBSD confinement semantics and is not a runtime dependency or native test tool.
The alternative aperezdc portable project targets Linux/libbsd; it is not the same
package and is not selected for this macOS proposal.

## Bounded proposed work after acceptance

Review the pinned archive and build inputs before executing code. Build only in an
owned ignored development-tool directory without root/global installation. Do not
run source refresh, install targets, package-manager hooks or remote scripts. Record
compiler/toolchain, artifact digest and license/provenance decisions. If dependency
or host compatibility requirements exceed this scope, stop for updated review.

Run reviewed upstream regressions in an owned disposable test directory. Their runner
uses OpenSSL command-line checksum helpers and shell constructs: record the actual
shell/OpenSSL versions and observed results, not just the presence of tests. Tests
create synthetic keys with no production authority; private keys never enter Git,
public evidence or ordinary logs and fixtures must be cleaned.

Proposed extra verification cases:

| Case | Expected result |
|---|---|
| Valid upstream synthetic signature/checksum fixtures | Signature and selected file bytes accepted. |
| Mutated message/signature and wrong synthetic key | Verification rejects; no media boot follows. |
| Missing/truncated/malformed signature or selected file | Explicit failure, not partial success. |
| Signed checksum list with altered selected file | Rejected file identity; no fallback. |
| Comparison with OpenBSD base signify when independently available | Record agreement/disagreement and exact versions; do not claim comparison ran before access exists. |

Proposed ceilings: 100 MiB source/tool/test allocation, ten-minute build, five-minute
regression run and 1 MiB retained safe output. No network during build/tests; retrieval
is a separate reviewed HTTPS step. No signing of project artifacts or media, no real
private keys, services, guest disks or native launches. Remove only owned synthetic
fixtures/tool outputs; preserve sanitized digests and outcomes. A failed regression
or incomplete cleanup blocks release-media verification.

## Key bootstrap and release verification

A working host verifier does not authenticate its own release key. Obtain maintainer
acceptance of key provenance separately. Prefer a key already held by a trusted
OpenBSD system; otherwise document independent official-source acquisition and the
remaining HTTPS/publisher trust assumptions. Do not silently trust keys bundled with
the port or the same mirror serving media. Pin the reviewed key identity before
checking OpenBSD release signatures.

Only after tool tests and key acceptance, verify release SHA256.sig and exact source/
media bytes. This would upgrade provisional source findings to authenticated-input
evidence, not native runtime evidence. Verify a corrupted copy as a negative control.
No custom cryptographic verifier code or weaker digest-only fallback is permitted.

## Proposed design change and approval

If accepted, amend native-profile EDD's pre-boot verification requirement to allow
this explicitly reviewed host port solely for artifact verification, retaining base
signify as the preferred route. Record scope/date/source of acceptance and an ADR
for the host dependency, including updates and removal. Do not change the design
merely because this proposal merged. No bootstrap script or dependency pin is added
by this PR; the source pin above is research until adopted.

Approval should name: host-only pinned build/regression scope, allowed tool storage,
license/provenance review, key-bootstrap route and the EDD exception. Guest boot and
installation remain separately gated. Until that record exists, the OpenBSD-base
verifier requirement remains in force.
