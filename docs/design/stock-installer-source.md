# Generic stock installer/bootstrap source path

Owner: #5/#96, existing PR94 batch. The adopted production path uses the stock
OpenBSD installer and packages plus a small reviewed bootstrap. The2026-10-23
product-independence correction supersedes the earlier suite-name dispatcher.
Bootweave is a third-party general-purpose installer, with no appliance repository
or runtime dependency. Consumer-specific profiles, scripts, roles, commissioning,
lineage and setup instructions belong in the consumer repository. Prior C/controller
research and historical evidence remain intact; no old result qualifies this path.

## Generic configuration and optional extension

`tools/stock_site.py:site_set` accepts exact immutable local package bytes, their
expected SHA256, explicit `openbsd` or `consumer` trust-domain labels, architecture
and an exact `root_package`. There is no product ID, name prefix or product allowlist.
The root must have consumer trust. `tools/package_closure.py` checks the reachable
acyclic supplied default-name dependency closure and measures archive/member bytes
without extraction or execution. Media arm64 maps to package directory aarch64;
accepted packing-list architecture retains its original value. Package predicates,
shared-library/script compatibility, signatures, installed allocation and supported
boards remain native qualification, not inferred from this metadata.

The optional `package-entry-v1` setup object has exactly four fields:

| Field | Validated source contract |
| --- | --- |
| package | Exact selected package filename with consumer trust. This declares which package supplies setup; ownership of the installed entry still needs native validation. |
| path | Canonical ASCII path below `/usr/local/libexec/`, at most192 bytes; no empty/dot/parent components, shell syntax or URLs. |
| sha256 | Exact lowercase64-hex executable digest, supplied from the reviewed built artifact. A digest is not publisher trust or execution authorization. |
| arguments | At most8 nonempty printable ASCII tokens, each at most128 bytes; shell-quoted individually, never evaluated as a command template. No secrets in this public configuration. |

Unknown/missing fields refuse. `setup=None` selects package-only installation.
An unrelated [archive-indexer example](../../examples/stock-site/archive-indexer.json)
demonstrates the same interface. Its inert digest is illustrative and cannot
qualify a real executable. A build owner calls `site_set(packages, expected,
authority, **configuration)` after independently admitting each artifact and the
extension's code/effects. Bootweave does not fetch an appliance repository or
import its Python/runtime. Consumers can use other installers and ordinary package
startup independently.

The assembler renders deterministic `site80.tgz` bytes in build-host memory,
containing exact packages, a closed report and `installer/stock/install.site`.
Python is build-host tooling only. The bootstrap uses local-only pkg_add with
normal signatures and a fixed cleared child environment. The optional setup entry
must be a regular non-symlink single-link root:wheel file, mode0500 or0555, with the
admitted executable digest before its one attempted invocation. It receives only
the selected arguments in a fixed environment. No shell interpreter, environment
hook, eval, guessed role transition or suite-specific dispatch is supplied by
Bootweave. A reviewed consumer may implement its own setup in that package entry;
that executable and any interpreters/dependencies receive installation privileges
and require explicit review/authentication under the adopted extension policy.

The target owner must prevent concurrent path/parent/executable mutation and
independently establish actual containment, ownership and full executable closure.
The shell's checks do not eliminate check/use races or turn paths/hashes into
custody. Missing private inputs, role/lineage prerequisites or failed consumer setup
leave a partial installation, never product readiness. The consumer owns these
requirements and its boot/use/restart/protection acceptance. Bootweave reports
installation/setup observations separately. Fixed event codes suppress raw package
and setup diagnostics, with no shared secrets or implicit issuer/default policy.

## Refusal, failure and acceptance

The public install.site entry **unconditionally exits77** with no environment
activation switch. The legacy planner/CLI refusal stays unchanged. No media/ISO,
package, key, credential, disk, service, security setting or network effect is
performed by these source tools. Authentic stock media/sets, separate native
package and consumer trust, exact freshly confirmed target, private input delivery,
original-child/resource retirement and real install/boot/use/restart evidence remain
mandatory. A one-attempt directory refuses repeats; it is not a crash-proof journal
or rollback. No automatic retry, disposal or reconstructed target authority exists.
The initial stock interpreter environment also requires native admission; clearing
child environments cannot retroactively establish it.

Proportional tests exercise deterministic assembly, exact inventory and package
closure, malformed extension refusal, literal argv, package-only operation and an
unrelated consumer. Interception replaces every effectful bootstrap function with
shell builtins, exercising actual stop-on-failure sequencing without package/root
operations; a normal-entry test verifies exit77. Shell syntax and ShellCheck cover
the template and rendered example. C/header plus runtime-shell source remains
charged to256KiB total and24KiB entry; component/runtime limits are unchanged.
Full make verify remains held on genuine authentication-fixture effects; focused
portable checks do not replace that required gate or native acceptance.

Central boundary: [Suite-Standards5b5a36e](https://github.com/gonz0w/suite-standards/blob/5b5a36ea63989357c62eba132bec6f59c9e4f1e3/docs/standards.md#product-independence-and-bootweave-boundary).
Stock references: [install.site](https://man.openbsd.org/OpenBSD-8.0/install.site.5),
[autoinstall](https://man.openbsd.org/OpenBSD-8.0/autoinstall.8),
[pkg_add](https://man.openbsd.org/OpenBSD-8.0/pkg_add.1).
