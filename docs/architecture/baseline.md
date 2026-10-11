# Architecture direction

Status: Proposed. No runtime interfaces or configuration grammar are adopted.

Separate media construction, configuration/flow presentation, privileged install
operations and product bootstrap. A project author supplies reviewed configuration
and payload metadata; an operator supplies validated installation choices. The
common engine must remain usable without importing DNS or firewall policy.

Prefer OpenBSD's existing installation machinery for OS disk/layout/set work where
native experiments demonstrate safe integration. A stock response file is tied to
installer prompts, not a stable typed API. Mapping and unexpected-prompt behavior
need release-specific qualification. Missing answers must not silently allow
unreviewed destructive defaults.

A custom base-curses renderer is a research candidate for ANSI lightbars and shadow
boxes. Footprint, terminal capabilities and ramdisk integration are unqualified.
Preserve the same validation and confirmation semantics in a plain-text frontend.

Represent planned changes separately from completed steps and verified observations.
Root operations take validated typed requests with bounded execution, trusted tool
paths and explicit targets. Product adapters own product bootstrap semantics and
readiness checks; arbitrary privileged shell embedded in configuration is not a
safe default. If executable extensions are adopted, treat them as separately
reviewed trusted code with explicit authority and provenance.

Payload verification, credential creation, management exposure and durable failure
receipts are core trust boundaries. No default shared password, silent public
network fallback or unverified remote script execution. Install success requires
postconditions, not process exit alone. Destructive disk writes cannot be rolled
back generally; recovery documentation must describe the actual remaining state.

Initial installed systems should follow conventional OpenBSD layouts unless native
research justifies another design. Read-only/two-image storage and full update
orchestration are separate decisions, with product persistence implications.
