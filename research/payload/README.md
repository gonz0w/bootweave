# Read-only payload inspection research

Original BSD-3-Clause C under the accepted
[Phase A scope](../../docs/research/payload-inspection-spike.md). This removable
experiment reads stdin and checks a narrow uncompressed ustar subset. It has no
extraction, filesystem path-opening, shell, subprocess, network or privilege backend.
No argument is accepted. Archive names and content never appear in diagnostics.

`make test-payload` compiles the source with the installed host cc and runs twelve
structural groups plus eight authentication groups when the exact reviewed local
verifier is available on macOS. `make verify` includes them. Missing verifier or
another host platform explicitly skips authentication; a mismatching verifier fails.
Python is host research tooling only;
the inspector uses base libc. Temporary fixtures/binaries are owned and removed.
[Host outcomes and limits](../../docs/research/payload-host-evidence.md) are recorded
separately from native qualification.
No native build or run is authorized by Phase A, and host checks cannot qualify
OpenBSD execution, payload trust or extraction containment.
The separately [accepted native experiment](../../docs/research/payload-native-plan.md)
has [source-bound native evidence](../../docs/research/payload-native-evidence.md):
dynamic/static builds and 90 checks on each of serial and keyboard/VGA. Its exact
target was disposed; this record authorizes no new guest or disk.

After host compilation, supply an owned synthetic archive through stdin. The
executable emits only a fixed summary with member/file/content-byte counts after
the complete input and trailer pass. Exit 0 means inspection passed; exit 2 means
rejected/incomplete; exit 3 means I/O/setup/cleanup failure. A failed output write
can leave a partial summary; the process result must also be checked. No printed
result authorizes extraction or installation.

Accepted headers/encodings are frozen in the
[authenticated source review](../../docs/research/payload-source-findings.md).
The experiment rejects links, special/extended types, unsafe paths, duplicate or
conflicting trees and unreviewed ownership/modes. It bounds input at 1 MiB, members
at 128, content at 512 KiB total/256 KiB per file and path length at 255 bytes.
Two zero trailer blocks, complete zero padding and EOF are required. Some valid
broader archives deliberately reject; this is not a general tar implementation.

Input has a ten-second whole-stream deadline and output a five-second deadline.
The reader temporarily enables nonblocking I/O, restores saved descriptor flags
on controlled exit and does not alter terminal attributes. Host checks observe
restoration of the O_NONBLOCK bit; the bounded native pipe suite additionally
checked exact saved flags.
Catchable INT/TERM/HUP stop work, SIGPIPE cannot grant completion, and the owned
supervisor bounds each case. Process RSS is measured independently with wait4 for
the supplied-buffer cases; streamed/deadline cases have separate host observations.

Authentication, immutable verified-input lifetime, distinct on-wire directory
alias qualification, broader native behavior, extraction
containment/races, ownership effects and application readiness remain separate
gates. Never follow an inspection pass with unconstrained extraction. No public
project/payload schema or C runtime choice is adopted by this research.

The original fixed native supervisor and build/run scripts operate only inside the
accepted disposable guest directory and refuse host execution. They have no disk,
installer or extraction backend. For reproducible host fixture preparation, run
`.tools/venv/bin/python3 research/payload/native_fixtures.py`. It creates one owned
private directory under .tools with the exact 35 synthetic archives and test
checklists; it neither launches a guest nor supplies target authorization. Remove
the generated directory after review/use. The `alias-dir` recipe produces identical
directory headers; distinct slash/no-slash wire encodings were not tested natively.
PAY-12 constructs exact raw directory headers independently of tarfile, verifies
their name/prefix fields and checks six individual admissions and twenty ordered
duplicate/alias rejections on the host. These include combined names and split
ustar prefix/name fields. The original 35 native fixtures stay frozen.

The [accepted authentication experiment](../../docs/research/payload-authentication-spike.md)
now has an original Python host controller and
[eight test groups](../../docs/research/payload-authentication-host-evidence.md).
It checks independently expected length/digest and an explicit pinned test key,
then feeds the retained immutable payload bytes to the unchanged C inspector.
The trusted host test harness supplies context and tool identities; there is no
operator-selected key/path/command interface. Direct execution refuses without
trusted context and rejects arguments. Run `make test-payload`; no new tool is
downloaded or installed. Disposable signing keys exist only during owned tests
and are removed afterward. Python is not an OpenBSD installer/runtime choice.
Darwin DATA/RSS resource-limit calls are unavailable; child RSS is observed through
wait4 and controller/test-process RSS through getrusage. Neither establishes kernel
memory containment. Native auth, signer rotation/revocation, freshness/compatibility,
privileged file-race resistance and extraction require further scoped evidence.

The separately [accepted native authentication experiment](../../docs/research/payload-authentication-native-plan.md)
has original C/base-libc research in auth_native.c and a fixed native supervisor.
It captures one bounded buffer, checks independently supplied expected identity,
verifies with explicit-key base signify, and then inspects retained bytes. Only
the trusted compiled test harness supplies context. The standalone entry refuses
without context; neither it nor the native scripts run on the host. Test hooks
exist only in AUTH_TEST builds and cannot be selected by payload/CLI/environment.
The [native evidence](../../docs/research/payload-authentication-native-evidence.md)
records dynamic/static builds, 128 authentication/alias checks and 90 historical
checks on each of serial and keyboard/VGA, followed by key/fixture disposal.
The 26 distinct raw alias cases close that narrow native gap. No installer API,
production trust policy, extraction or privileged race/confinement guarantee is adopted.

The [accepted placement slice](../../docs/research/payload-placement-spike.md) adds
an original complete-buffer planner and fixed compiled native placement phase. The
default authentication build still stops after inspection; placement builds pass
the same retained snapshot directly to the planner and child writer. No archive
pathname is reopened, no general extractor runs and no callback is configurable.
The trusted harness supplies an empty held root and separate synthetic expected
inventory. New implicit directories count toward 128 objects and depth is limited
to 16. Archive owners/modes/timestamps never become write policy.

placement_native.c targets OpenBSD only: fixed nobody IDs/groups, locked unveil,
pledge, component-relative no-follow traversal, exclusive creation and fixed
0700/0600 directories/files. Native prerequisite probes precede all member effects;
failed or unobservable confinement stops the experiment. A read-only independent
oracle checks expected paths/hashes, owners/modes, root binding and outside canary
identity/content. Failure observations preserve partial/uncertain state until fixed
fixture disposal; cleanup is not rollback. placement_check.c has only compiled
synthetic probes/barriers, never payload/argument/environment selected operations.

Three host groups check original/frozen rejection policy, additional planner
bounds/contents and native entry refusals/source ceilings. They perform no native
file effects or confinement qualification. placement_fixtures.py prepares trusted
synthetic bytes and private expected test records without extracting files. Native
build/run scripts use the fixed approved guest roots, verify source manifests and
reuse the authentication suite's exact two native signing pairs across builds and
consoles. Native execution/evidence is pending; no production trust, installer
API, application readiness or runtime adoption follows this work.
