# Installed-target payload research

Original removable C under the [accepted complete slice](../../docs/research/installed-payload-flow.md),
kept with scope, fixes, tests, native qualification and teardown in one follow-up draft after maintainer merges of PR #52/#53/#54/#55/#56/#57/#58.
[Native progress](../../docs/research/installed-payload-native-progress.md) records
compiler provisioning, six revised entry builds, 976 serial/VGA regressions and all
46 installed serial cases, including later readback.
The installed disk namespace adjustment is accepted. Recovery and bounded range
export completed with native clean halt and full ISO readback. Stock installation
and cold OS boot on the serial target passed. The accepted raw-partition replacement
passed serial qualification. VGA handoff stopped before confirmation/writes on an
observer decoding failure; that target remains untouched. All 46 VGA cases and
disposal remain together in one follow-up draft after PR #58 was maintainer-merged; no production runtime/interface is adopted.

Ordered fixed diagnostic labels identify capture/preflight refusals without
weakening the checks or selecting operations; numeric errno is advisory.

root.c compares private canonical observations and normalizes only one exact
empty fd0: record under mandatory host proof of no floppy backing media in the
fixed no-hotplug virtual fixture. Empty cd0/rd0 and sole sd0/DUID rules remain;
the generic storage validator is unchanged. root_native.c uses fixed read-only
raw whole-disk/root-partition and root references, rechecked named block-node
metadata and held filesystem/directory observations to bind an
installed sd0a FFS root to its authorized case. It refuses missing or changed
bindings before member dispatch and observes the binding after placement. The
original confined nobody writer and independent inventory oracle remain in use.

The raw-partition binding replacement and sixteen-launch ceiling are accepted.
Named block metadata binds inode, complete mode, group, link count and flags;
it does not claim a held mounted-block descriptor or physical/hotplug qualification.
All original mount, label, namespace, destination and confirmation gates remain.

The new AUTH_INSTALLED build alone wraps authentication/review/placement with that
gate and uses fixed owned public verification files. Separate archive and console
capabilities remain enforced. Its bounded target prelude reserves 1024 bytes; the
member review reserves 15,000 and its report 256, within the accepted 16 KiB output.
Other component builds retain their existing limits and behavior.

check.c reuses the existing compiled supervisor. Automatic variants have seventeen
cases: positive/cancel, nine independently captured observation substitutions,
snapshot mutation after confirmation, three writer failures and two signature
refusals. Manual variants have five actual-console cases: positive, cancel, snapshot
mutation, changed observed mount flags and partial writer. Separately compiled KEEP
retains one positive case; READBACK independently observes it after another boot
without writing members. None accepts operation selectors or arguments.

fixture_header.py is host development tooling. It checks exported public records,
replaces untrusted comments with fixed text and emits numeric unsigned-byte arrays
only, bounded to 2 KiB. It retains the same decoded public key/signature bytes.
It consumes no private key, runs no command and grants no authority. This private
compiled fixture data is included in the combined source/media budget, separately
from original C; it is not a profile or persisted authorization protocol.

Three additional host groups cover pure authority fields, host refusal/source
ceilings and adversarial public-data packing. Run make test-payload or make verify.
Native build.sh requires OpenBSD, fixed frozen sources and the checked generated
public fixture header. Full source/media/binary identities and native measurements
must precede support claims; host tests cannot qualify the installed-root gate.
