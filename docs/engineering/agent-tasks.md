# Agent task context for bootweave

Use the [shared compact task packet](https://github.com/gonz0w/suite-standards/blob/e9653b677a3b295d5a7c2d507f86cdc3805bd94e/docs/task-packets.md)
from immutable Suite-Standards revision `e9653b677a3b295d5a7c2d507f86cdc3805bd94e`.
This advisory pointer organizes context; it is not another work authority.
[PROJECT](../../PROJECT.md) composes it with the pinned Agent Temper core and
local delivery rules. [Source provenance](standards-sources.md) identifies retained
private content and hashes. Existing standards pins, stronger local instructions,
reviews and acceptance gates remain unchanged.

## Local path and command index

- Scope the packet to `src/`, `tests/`, `research/` and the relevant `docs/` design packet. Read the applicable
  instructions and relevant design sections; do not load all sibling documentation.
- Run commands from this repository root using admitted tools and the
  [change-specific environment and checks](development.md). The #95/#102 adoption
  has a supported-host documentation gate. Behavior/tooling work retains OpenBSD
  and full `make verify`; the old host bootstrap is not an OpenBSD setup path.
  See `Makefile`, `README.md` and `CONTRIBUTING.md` for the current requirements.
- `make build test-plan` for planner source changes.
- OpenBSD installer/media/native fixtures require their exact disposable-target authority; source/C tests do not qualify installation.

Naming a command does not authorize native effects, downloads, credentials or
retries. Record exact commands actually run, tested base/head/tree, outcomes and
remaining blocked/not-run gates. Keep dirty work, active owners and failed evidence
intact; a focused source check does not replace a required final gate.
