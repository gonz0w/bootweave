# Local evidence and PR review

Checks run locally in the foreground. GitHub Actions, hosted/self-hosted runners,
remote success publishers and mandatory remote status checks are not used. GitHub
branch protection cannot prove local tests ran; the maintainer inspects evidence.

For a topic PR, fetch origin, start from current origin/main, inspect the complete
diff and verify a clean candidate. Record base/head/tree, actual Conventional
Commit PR title, command/tool versions and results. Verify every contributor commit
against deliberately trusted signing keys and run tools/check-dco.py BASE_SHA
HEAD_SHA. DCO checks are not signature verification. Run make verify on the clean
identified candidate and confirm neither HEAD nor tracked content changed.
Initial seed has no PR range; inspect its author trailer and signature directly.

Push the scoped topic and open a PR against main with a reviewed body file. Attach
applicable native evidence, failures/skips, uncertainty and fixture teardown.
Inspect evidence before making it public; no private paths, inventories, signing
keys or credentials. New source/base/title requires updated evidence. Leave PRs
open for maintainer review; no automatic merges.

Desired remote settings: PRs, signed commits, linear history, resolved conversations,
blocked deletion/force push and no bypass actors. Solo phase uses zero mandatory
independent approvals. Enable squash merges only, disable auto-merge and delete
merged branches. v* release tags are immutable and separately signed/reviewed.
Versioned ruleset JSON does not apply itself; record observed settings in
[GitHub setup](github.md). No enforcement is claimed before readback.
