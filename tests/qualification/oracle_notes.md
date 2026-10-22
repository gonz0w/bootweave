# Independent qualification expectations

This is test preparation, not native evidence or permission to run a fixture.
The nine literal cases in [oracles.json](oracles.json) were authored from the
adopted contracts and public observation headers before reading any new
qualification driver. No qualification driver was imported, compiled or run.
The starting canonical commit is `92166f07fd79c9d356ef3e1ed704eaeeed8653e4`.

The frozen R2 catalogue is 16,531 bytes, SHA256
`610a161c0c0d64bbebdd35d1584a0ec5234186bec3b2c453f50ffbbf40381f73`.
It contains two fresh-process readback cases, two file-size cases and five
closed-entry refusals. Expectations are literals, never captured driver output.

The [guard contract](../../docs/design/trusted-entry-output.md) supplies the
parent caps 4/67108864/2097152/32/0/65536 and child-profile caps
2/33554432/2097152/16/0/0. The stated inherited-bound precondition makes both
soft and hard readbacks exact. Record six actual pairs and raw timer fields.
REAL must remain positive and at most 15 seconds; PROF positive and at most
4 seconds; both intervals must be zero. A particular remaining value is not an
expectation. The child profile is a limit-only call in the same process after
the parent guard; it proves no real verifier child, child timer or launch custody.
Its readback fd1 must be an owned nonblocking FIFO, not a regular file under
the zero file-size limit.

The parent file-size schedule is sixteen 4096-byte writes of `X`, followed by
one byte which must not return. The child-profile first one-byte write must not
return. Both require the real guard's fixed exit5 handler, empty stdout/stderr
and an actual consuming wait showing normal exit5. The supervisor must verify
the caller-owned regular fd3 object, initial emptiness and final complete bytes:
65536 `X` bytes or empty, respectively. The hashes were calculated independently
from those fixed byte strings. Source-bound write counts and exit5 alone are
insufficient without independent file and termination evidence. Short writes,
unexpected errors, extra writes, timeout, missing wait or signal termination fail.

The [entry contract](../../docs/design/trusted-entry-output.md#closed-complete-entry-ownership-coordinator)
and [public enums](../../src/trusted_start.h) fix REJECTED0, FINISHED2,
capture NOT_TAKEN-2 and causes CONFINEMENT5, START1 or INPUT2. Missing original
roles0/3/4 produce known close-error masks1/2/4, respectively, with unknown mask0.
All five cases require empty stdout, final-boundary-failed0 and untouched zero
authenticated/output/signature observations. The stderr record projects exactly
the 29 ordered decimal observation fields in the catalogue, prefixed NQ_ENTRY
and terminated by LF. The function return must equal observation.result; it is
not an additional serialized field. Rusage and struct padding are not projected.
The actual normal boundary remains an unconditional refusal. No forwarded
positive native case, cryptographic case, readiness override or trace interposer
is included.

R1, SHA256 `acaa117110fb019f3e7d3c605be3f6ee897092dfe97a5dc93789e988afd78888`,
is preserved outside the checkout in coordinator history
`bootweave-qualification-harness-preparation-history-r1/independent-oracles-r1.json`.
Its bytes were verified before correction. R2 changes only the two literal
child_limit_adapter_result values from1 to0. A narrow independent check of the
canonical callee confirmed that `bw_sv_child_limits` returns `fixed_limits`,
whose successful six-resource completion returns0. Its canonical signature
caller checks for failure-1. Guard success remains1. This correction came from
the existing API, not a new driver result; no other expected value changed.

JSON syntax, duplicate keys, case/mode uniqueness, literal caps, enum/close masks,
zero projections and fixed-byte hashes were checked without source execution.
Portable syntax, source-closure or unsupported-host refusal checks satisfy none
of the nine native cases. Exact source/tool/profile bindings, fresh authority,
an independently reviewed supervisor and actual retained observations remain
required before any native acceptance claim. No held trace method is adopted.
