# Payload authentication host evidence

Observed 2026-10-12 under Cam's
[accepted host-only experiment](payload-authentication-spike.md), after PR #43
and the explicit implementation/test-signing question. Implementation base:
d58418b832c21bba61fec84020e7f793b599898e. This is synthetic orchestration research,
not a production signer policy, privileged handoff, native test or extractor.

## Source and trust binding

Original BSD-3-Clause Python development controller: research/payload/authenticate.py,
13627 bytes, SHA-256
a075dc7fa6f7ebab76d2db6abb722c2dd4bc50e38115ab17c54498aadf74ac5c.
The original C inspector stays unchanged at 9237 bytes, SHA-256
8009aed82f67ae60c70538579fa493995a87fa6ff1ea324dc23ee79af0847698.
Native C/shell tooling and all 35 historical fixtures are unchanged. Clean final
candidate/base/tree, signature/DCO and complete local verification accompany the PR.

Host: Darwin arm64, Apple clang 21.0.0 (clang-2100.3.20.102), Python 3.14.7.
The test harness warning-clean compiles the inspector with C99/Wall/Wextra/Werror/
pedantic and pins the resulting owned binary. It rechecks the inspector source
and exact existing signify-osx 1.4 binary/source digests before generating keys.
Verifier binary SHA-256:
4b9b2069c5e5acda9d35ca898ecd5afc050094170cf3c73fa7cf9eb615530946;
signify.c SHA-256:
96006fa573cf1cdeae4b96682ced2ade7750830984346d009bd88e07e942d264.
The [existing source pin/local-use policy](../adr/0002-host-artifact-verifier.md)
and its explicitly accepted synthetic-signing extension apply.

Read-only review covered xopen/readmsg, generate/sign/createsig/verifysimple,
key naming, no-passphrase selection and the generation/signature calls. signify.c
retains Ted Unangst's permission notice; mod_ed25519.c and crypto_api.c retain
their public-domain attribution; the port LICENSE is ISC. This is retained local
tool provenance, not exhaustive redistribution clearance or a cryptographic audit.
No upstream implementation was copied, modified, rebuilt, downloaded or packaged.

Each owned test suite generates two fresh no-passphrase synthetic key pairs through
the unchanged reviewed tool, then signs bounded viewer/malformed/boundary fixtures.
Core output is disabled and umask 077 produces private files. No private-key bytes
enter argv/environment or ordinary logs; the signer receives only owned file paths.
Key files live beneath an owned 0700 test directory, with mode 0600, and are disposed
when the suite ends. They have no production authority. No release key was changed.

Real detached-stdin prerequisites run before the controller cases: expected bytes
verify with the explicit selected key and quiet empty output; changed message bytes
reject. Exact command shapes are signify `-G -n`, `-S -m -` and
`-V -q -p selected.pub -x input.sig -m -` in owned private fixtures.

## Actual checks and independent observations

Focused command: `PYTHONPATH=research/payload .tools/venv/bin/python3 -m unittest
test_authenticate -v`. All eight groups passed. Subsequent focused PAV-06 tool-pin
checks passed; final clean-candidate full verification includes the additional
identity-matched/message-tampering case and is recorded with the PR.

| Case | Observation |
|---|---|
| PAV-01 | Actual signature verification preceded unchanged C inspection; exact 1-member/1-file/5-byte summary. Both child inputs matched the independently expected digest. |
| PAV-02 | Wrong identity, malformed/missing/corrupted signature and a second valid signer stopped without an inspector launch or success. Real direct transport checks independently rejected changed message bytes. |
| PAV-03 | Changed key hints, control sequences and shell-like comments could not change the explicit key or emit supplied text; malformed records stopped before child launch. |
| PAV-04 | A signed and identity-matched traversal archive verified, then the unchanged inspector rejected. Result retained verified state but reported no completion or summary. |
| PAV-05 | Mutating/replacing the original source after verification did not change inspection input. A separately executed trusted observer hashed bytes actually received through its pipe after a same-length valid source replacement; they matched the retained original, not the replacement. This observer is test evidence, not cryptographic evidence. |
| PAV-06 | Exact 1 MiB input admitted; empty/over-limit input, excessive records, invalid context/tool digest and arguments rejected. Injected PATH/options/locale did not change fixed child arguments/environment. |
| PAV-07 | Controlled child errors, invalid success summaries, excess output, broken/full input and stalls stopped; owned children were reaped. Changed private inventory/modes blocked dependent execution. Even a controlled diagnostic containing disposable private-key bytes produced no returned secret or summary. |
| PAV-08 | Private file/directory modes, unchanged canary/inventory and input postconditions passed. INT/TERM/HUP delivered after an actual child launch caused interruption and reaping; tracked owned PIDs were independently observed absent. An unfinished input stopped within the real ten-second deadline, observed between nine and twelve seconds. |

The controller retains one immutable bytes snapshot, compares expected length and
standard-library SHA-256, then uses two owned pipes. It never opens archive member
paths or reopens the original source. The trusted host harness owns expected
identity/key context and executable pins; there is no operator-supplied context or
command interface. Direct execution refuses with exit 77, and arguments with exit 2.
Unsigned retry, comment-based key lookup, shell and network lookup do not exist.
Key/signature records are capped at 2 KiB; child output totals at 4 KiB. Only fixed
results/exact bounded numeric summaries can leave the controller. Private key and
signature copies are not public contracts or Git fixtures.
Capture restores saved descriptor flags; host tests check the O_NONBLOCK bit,
without claiming exact native flag qualification for this new controller.

## Resources, observed failure and cleanup

The last complete focused run took 11.553 seconds. Host test-process lifetime peak
RSS was 36929536 bytes, below the 64 MiB controller ceiling; that conservative
measurement includes the harness and is not isolated controller-only RSS. Independent
wait4 child-lifetime peak was 18022400 bytes, below 32 MiB. Sampled owned fixture
allocation was 73728 bytes, below 20 MiB; this sample is not a historical peak.
The controller source is below 32 KiB. Additional final-candidate measurements
accompany local verification, without replacing these dated focused observations.

The initial run stopped before key generation in preexec_fn: Darwin rejected DATA
and RSS setrlimit calls with ValueError. A separate owned unsandboxed process showed
the same failure. A soft-only attempt inside the sandbox also failed; no automatic
approval was rejected.
Darwin now uses measured RSS completion ceilings. Independent child observations
confirmed CORE 0, CPU 15 seconds and FSIZE 1048576 bytes. RSS/DATA kernel enforcement
is unavailable here; neither measured rejection nor those other limits establishes
generic memory confinement. Input, child and workflow deadlines remain ten,
fifteen and forty-five seconds respectively; controlled stalls used shorter trusted
supervisor deadlines without changing runtime defaults.

Every controlled failure path closes owned pipes, kills/reaps unfinished children
and removes its private-directory public-key/signature snapshots. Successful completion also requires
unchanged private file contents/modes/inventory and completed directory cleanup.
Tests verified input and canary unchanged, known PIDs absent, test directory deletion
and no leftover authentication/signing fixtures. Temporary binaries, signatures and
all task-owned private keys were removed. No disk, media, guest, host service, live
network, extraction, hook or application ran. Deletion is lifetime cleanup rather
than physical erasure or resistance to host administrators/debuggers/memory access.

Authentication is explicit-key synthetic host evidence only. OpenBSD base behavior,
native directory aliases, privileged key-file/executable races, production signer
rotation/revocation, compatibility/freshness, privileged verified-input lifetime,
extraction containment/races and application readiness remain unqualified. PTT-04/05
and issue #5 remain incomplete. Python is not adopted for an OpenBSD runtime.
All checks ran locally; no Actions, runners or hosted status publisher was used.
