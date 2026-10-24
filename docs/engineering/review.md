# Local evidence and PR review

Read [PROJECT](../../PROJECT.md), [development prerequisites](development.md),
the owning issue and comments. Checks and hooks run locally in the foreground in
the environment required for the change. The #95/#102 documentation adoption uses
its recorded supported-host checks; installer/behavior/tooling gates remain OpenBSD.
GitHub Actions, hosted/self-hosted runners,
remote success publishers and mandatory remote status checks are not used. GitHub
branch protection cannot prove local tests ran; the maintainer inspects evidence.

For a new feature or closely related issue slice, fetch origin and start from current
origin/main. Keep related scope/design, implementation, fixes, tests, native
qualification, cleanup and evidence on that branch in one draft PR. When continuing,
reuse the branch and update the existing PR; phase transitions do not require new
PRs. Review larger decisions within the draft while preserving scoped implementation
and exact disk target authorization. Mark it ready only when the agreed deliverable
and required verification, native qualification and fixture teardown are complete.

Use a separate bounded read-only reviewer for accepted consequential work. The
owner authorized this review delegation; it grants no implementation, native or
merge authority. Record whether review was peer-agent, human or specialist, its
source identity and findings. It does not replace required native/specialist gates.

Inspect the complete diff and verify a clean candidate. Record base/head/tree, actual
Conventional Commit PR title, command/tool versions and results. Verify every contributor commit
against deliberately trusted signing keys and run tools/check-dco.py BASE_SHA
HEAD_SHA. DCO checks are not signature verification. Run the required checks for
the change on the clean identified candidate: the recorded documentation gate for
#95/#102, otherwise the applicable focused checks and full `make verify`. Confirm
neither HEAD nor tracked content changed.
Initial seed has no PR range; inspect its author trailer and signature directly.

Push the scoped topic and open a PR against main with a reviewed body file. Attach
applicable native evidence, failures/skips, uncertainty and fixture teardown.
Inspect evidence before making it public; no private paths, inventories, signing
keys or credentials. New source/base/title requires updated evidence. Leave PRs
open for maintainer review; no automatic merges. Before an authorized squash,
review the exact proposed author/message/DCO, current head/base/tree and acceptance.
After integration, verify the canonical source/signature and reconcile completed
issues and parent outcomes. A changed relevant input invalidates dependent evidence;
missing verification stays unavailable rather than becoming a waiver.

Desired remote settings: PRs, signed commits, linear history, resolved conversations,
blocked deletion/force push and no bypass actors. Solo phase uses zero mandatory
independent approvals. Enable squash merges only, disable auto-merge and delete
merged branches. v* release tags are immutable and separately signed/reviewed.
Versioned ruleset JSON does not apply itself; record observed settings in
[GitHub setup](github.md). No enforcement is claimed before readback.
