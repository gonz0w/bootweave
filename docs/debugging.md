# Debugging and diagnostic requirements

Status: Proposed installer behavior. Today only foundation checks are available;
run make verify and inspect its local output for repository problems.

The installer should offer ordinary concise progress and an opt-in diagnostic
view with the same safety controls. Each bounded operation needs an identifier,
phase, timing, outcome, expected/observed postcondition and safe error explanation.
Distinguish validation failure, rejected authorization, execution failure, failed
readiness and uncertain completion. Do not substitute a successful exit code for
an observed installed state.

Default diagnostics must omit secrets, private host inventories and raw answer
files. Filter structured fields before logging; regex cleanup of a raw transcript
is not a reliable primary redaction boundary. Escape terminal controls and bound
line length, buffer size, duration and retained bytes. Never use shell xtrace on
credential-bearing work. Recovery output must identify what is complete and what
requires manual inspection without claiming that disk erasure can be undone.

An explicitly requested support export should show a preview and include only
safe version/source identifiers, bounded operation results and selected synthetic
or sanitized evidence. No automatic telemetry, upload or network callback.
Raw privileged logs require restricted access and a documented cleanup lifetime.

Native Test Design must exercise offline failures, unexpected stock-installer
prompts, disk-full capture, malformed terminal strings, interrupted writes, reboot,
secret redaction and a support bundle that is useful without private information.
