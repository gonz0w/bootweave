# Generic public foundation

Status: Accepted scope direction and public name Bootweave, 2026-10-11.

The maintainer requested a new open-source repository that can configure an OpenBSD
installer for any project, following Nomvium contribution/review standards and
using local tests instead of GitHub Actions. Earlier requirements select a text
installer with ANSI/BBS styling and direct-to-installer media.

Use BSD-3-Clause with retained foundation notices, signed Conventional Commits,
author DCO sign-offs, reviewed PRs and local candidate evidence. Target public
source; runtime/media qualification remains native OpenBSD. The common engine
must not own product DNS sharing or firewall policy.

This decision authorizes repository foundation and records its scope. It does not
adopt a runtime language, profile grammar, privilege API or stock-installer adapter,
or authorize installer implementation, a release or live installation.
