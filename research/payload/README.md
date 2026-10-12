# Read-only payload inspection research

Original BSD-3-Clause C under the accepted
[Phase A scope](../../docs/research/payload-inspection-spike.md). This removable
experiment reads stdin and checks a narrow uncompressed ustar subset. It has no
extraction, filesystem path-opening, shell, subprocess, network or privilege backend.
No argument is accepted. Archive names and content never appear in diagnostics.

`make test-payload` compiles the source with the installed host cc and runs nine
synthetic test groups. `make verify` includes them. Python is test tooling only;
the inspector uses base libc. Temporary fixtures/binaries are owned and removed.
[Host outcomes and limits](../../docs/research/payload-host-evidence.md) are recorded
separately from native qualification.
No native build or run is authorized by Phase A, and host checks cannot qualify
OpenBSD execution, payload trust or extraction containment.

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
restoration of the O_NONBLOCK bit; full native flag behavior remains unqualified.
Catchable INT/TERM/HUP stop work, SIGPIPE cannot grant completion, and the owned
supervisor bounds each case. Process RSS is measured independently with wait4 for
the supplied-buffer cases; streamed/deadline cases have separate host observations.

Authentication, immutable verified-input lifetime, native behavior, extraction
containment/races, ownership effects and application readiness remain separate
gates. Never follow an inspection pass with unconstrained extraction. No public
project/payload schema or C runtime choice is adopted by this research.
