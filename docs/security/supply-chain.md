# Supply chain and dependency policy

Prefer OpenBSD base runtime facilities. Third-party runtime dependencies require
an ADR covering necessity, license, provenance, updates, footprint, security and
removal. Configuration includes trusted build code only through an explicit
reviewed boundary; downloading and executing unverified scripts is prohibited.

Foundation development uses Python and pinned PyYAML, ShellCheck and Gitleaks.
Tools stay in ignored `.tools/`; bootstrap checks archive hashes and hash-pinned
binary wheels. These pins are inherited from the inspected Nomvium foundation,
not a claim of ongoing vulnerability clearance. Review updates through PRs and
local verification. No Go/runtime language is selected merely to match another repo.

[Foundation provenance](../../tools/foundation-provenance.json) records inspected
source-file digests before adaptation. Retained BSD Cloud, EdgeWall and Nomvium
copyright notices cover inherited files; Bootweave changes use the same license.
Exact upstream OpenBSD file notices and artwork licenses must be retained when
code/art is incorporated. BBS references inspire original art, not copied assets.

Future media distribution needs externally verifiable signed artifacts and exact
OS/payload manifests. A modified ramdisk cannot authenticate its own integrity.
OS release images, firmware redistribution rights, application packages and
project/media signing keys are separate review subjects. No signing private key
belongs in the repository or generated public evidence.
