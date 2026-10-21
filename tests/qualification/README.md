# Nontrace native qualification source preparation

This is preparation for canonical `92166f07fd79c9d356ef3e1ed704eaeeed8653e4`,
tree `79060329e9d06283a95c247c9245bbdd60ac9081`. It authorizes no native work.
All 34 runtime C/header files, their complete shared-helper accounting and the
production four-source planner link remain unchanged. Normal native entry still
refuses, readiness remains zero and no review CLI is activated.

## Prepared sources and evidence boundary

- `entry_probe.c` calls the existing closed entry once and emits the actual 29
  semantic observation fields to an exclusive nonblocking fd2 receipt pipe.
  Fields follow `oracles.json` exactly. Inconsistent return/observation, oversized
  formatting or incomplete single-write receipts fail with 70, never retry.
  Signature observations are data, not instructions to signal, close or recover.
- `limit_probe.c` calls the unmodified process guard and optionally the unmodified
  child-limit adapter in the same process. It emits all six actual soft/hard
  readbacks and raw original REAL/PROF timer fields through writable nonblocking
  FIFO fd1. This is not a real verifier-child, child-alarm or custody test.
  Separate fixed file-size modes write only the caller-owned empty 0600 regular
  fd3: sixteen 4096-byte X blocks then one byte for parent FSIZE65536, or one byte
  for child-profile FSIZE0. Expected native exit5 must come from the unmodified
  guard's actual SIGXFSZ handler. An unexpected return fails; no fake signal,
  close/removal, partial-write retry or success marker is supplied.
- `forwarder.c` is a separate qualification object routing to the existing native
  candidate on OpenBSD only, while readiness remains zero. It is never in the
  product link. The build preview may prepare it with the inert compiled policy;
  no native execution case or cryptographic positive is supplied for it here.
- `oracles.json` is an independently authored nine-case requirement catalogue,
  not observed native results. The note records its independent provenance.

Both probe mains return quiet 77 on non-OpenBSD, even when `BW_NQ_SOURCE_CHECK`
exposes otherwise inactive bodies for host syntax checking. That macro cannot
activate the body on the Mac and is absent from native build previews. Host
syntax and refusal checks satisfy zero native cases.

The source prepper accepts no arguments, imports no candidate C, has no process,
signing, target, media, mount, credential or cleanup API, and creates only a new
owned source directory under `.tools/entry-qualification-source-*`. It checks
the exact canonical runtime closure, rejects changed, oversized, unstable or
symlink source inputs, and writes private source copies, a complete manifest and
typed build-preview arrays. Any partially written namespace remains retained.
Its stable-file checks assume trusted coordinator quiescence; they do not claim
hostile-filesystem confinement. No executable recipe is generated.

## Requirements before future use

Every native case needs a separately reviewed fresh-process supervisor and exact
target/effects authority. The supervisor must independently observe original PID
custody, raw streams/EOF/consuming wait, native account/limit/timer prerequisites,
exact input/OFD/output roles and terminal absence. File-size outcomes additionally
need complete independently held fd3 identity, initial emptiness, final exact
length/bytes/hash and termination. A numeric exit5 alone proves no signal cause.
Do not recover authority from sampled PIDs, signal after wait consumption/loss,
retry numeric closes, or remove uncertain state.

`BUILD-PREVIEW.json` is data only. No native compiler/custody executor is supplied
or authorized. In particular, no historical helper hash or old `/planner` cwd is
silently treated as a new build guard. Before use, bind the complete new executor,
compiler/source/build flags, output/maps, exact oracle/count/slot/byte/transport
ledger, public signed fixture and private key custody, and independent seals.
Native output hashes are observed only after approved installation/build and
before dependent tests. No private key or guest credential belongs in media.

The held trace choice remains separate. No setrlimit/getrlimit renaming,
virtualization or trace companion occurs in these sources or build previews.
Unmodified readbacks/file-size enforcement can establish those measured caps and
outcomes; they cannot establish syscall absence/order, a complete exact-binary
trace, the actual locked loader/signify/FD handoff, cryptographic acceptance,
whole-session confinement or a future production entry/CLI qualification.

All runtime limits, failed evidence, primary worktrees and Vectivium holds remain
unchanged. A passing portable preparation check is not native approval, native
teardown or disposal authority.
