# Host verifier build and regression evidence

Observed 2026-10-11 for [issue #10](https://github.com/gonz0w/bootweave/issues/10).
Scope: bounded host-only work in [review proposal](host-verifier-review.md).
Authorization source: maintainer said “continue with issues” immediately after the
explicit request to approve that build/regression scope. This authorizes the bounded
experiment, not release-key adoption, EDD exception, permanent dependency or guest boot.

## Inputs and review

Retrieved signify-osx 1.4 source archive; SHA-256 matched the proposed formula pin:
5aa954fe6c54f2fc939771779e5bb64298e46d0a4ae3d08637df44c7ed8d2897.
Checked archive member paths, regular-file/directory types and size before extraction
into ignored owned tooling. Reviewed build/test commands before running; no refresh,
install/global package change or upstream source modification occurred.

Port LICENSE and representative upstream notices were inspected; bundled source is
retained locally with notices. This experiment is not a completed redistribution
license review. No source/binary is included in Bootweave or its media. Build/test
commands had no retrieval step; local compilation needs no new runtime package.
No OpenBSD confinement equivalence is claimed for the no-op pledge host shim.

## Actual host results

Host: macOS ARM64. Apple clang 21.0.0 (clang-2100.3.20.102), OpenSSL 3.6.3.
Local make test built the unmodified verifier in under one minute. Compiler emitted
four warnings: two old-style function definitions, one fixed-size initializer warning
and one differing-element-size division warning. Build success is not a security
audit or assertion those warnings are harmless.

First upstream regression run failed at checksum parsing: OpenSSL emitted SHA2-256
labels which the helper expected as SHA256. The local regression helper was adapted
to normalize SHA2-256/SHA2-512 labels as well as the older labels. No cryptographic
or verifier source changed. Adapted make test completed successfully, including
explicit_bzero and upstream signature, forged-message, embedded-checksum and signed
compressed-archive cases. This is an adapted regression pass, not an unmodified
upstream test pass.

Five additional bounded cases rejected with nonzero status: mutated message, wrong
synthetic key, truncated signature, malformed signature and missing signature.
Generated synthetic key/result directories were removed and absence checked.
Published evidence omits key values and private test paths. The reviewed source
archive contains upstream public regression fixtures, including non-production test
key material, only in ignored tooling; it is not committed or used as release trust.

| Retained artifact | SHA-256 |
|---|---|
| Local verifier binary | 4b9b2069c5e5acda9d35ca898ecd5afc050094170cf3c73fa7cf9eb615530946 |
| Adapted regression helper | 1af48b9d41a3da9273eed2046d287c0e764bdf3a36ee8338edf19b44f572968a |

Total observed source/tool/log bytes: 742096, below the proposed 100 MiB ceiling.
Build/test runs completed below their ten/five-minute ceilings; output below 1 MiB.
Raw logs remain ignored. Binary digest identifies this build only, not a portable
or upstream-signed distribution. No background process/service was installed.

## Remaining gates

No OpenBSD base-signify cross-check was available. No release key was accepted and
no OpenBSD source/media signature verified. Synthetic tests establish limited host
behavior, not authenticated release inputs or native qualification. Source findings
remain provisional. Permanent host-verifier adoption needs dependency ADR/review and
explicit EDD exception; key provenance is separate. Native configuration and guest
run authorization remain pending. Nothing in this evidence authorizes disk writes.
