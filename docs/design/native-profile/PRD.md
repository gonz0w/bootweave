# First native qualification profile: requirements

Status: Proposed, 2026-10-11. Owner and acceptance authority: Cam.
Related [EDD](EDD.md), [Test Design](TDD.md), and
[issue #2](https://github.com/gonz0w/bootweave/issues/2).
Acceptance date/source: pending explicit maintainer review. This document approves
no execution; accepting this scope must separately identify permitted experiments
and any disposable target writes.

## Outcome and scope

Give contributors one reproducible, isolated OpenBSD guest baseline before custom
installer integration or renderer experiments. A running OpenBSD guest supplies
native OS evidence even when its CPU is emulated; host execution and cross-builds
do not. Emulation timing cannot establish hardware performance.

Propose OpenBSD 7.9 release amd64, legacy BIOS optical boot, stock install79.iso,
QEMU pc machine with software CPU emulation, one CPU and 1 GiB guest RAM. Use a
read-only optical image and one new 8 GiB virtual disk; no network adapter.
Keyboard/VGA and a separate serial-console run are required. Exact emulator
version, versioned machine type, CPU model and firmware digest must be frozen in
a reviewed execution plan before launch. This is a bounded proposed profile, not
support for arbitrary QEMU defaults, UEFI, arm64 or physical hardware.

The first experiment stops at stock installer interaction without installing.
A later explicitly authorized experiment may install the OS to the confirmed
throwaway disk. No Bootweave media, renderer, application payload, service
bootstrap, upgrade or recovery implementation is included here.

## Requirements

| ID | Acceptance |
|---|---|
| NP-01 | Record exact guest, host tooling, firmware, media and source identities; verify inputs before boot. |
| NP-02 | Reviewed attachment inventory exposes only the disposable disk and read-only media; networking and host sharing are absent. |
| NP-03 | Boot and interact through keyboard and serial independently; record actual plain-text and low-color behavior without asserting custom UI equivalence. |
| NP-04 | Stop before writes absent separate reviewed target confirmation; never use a host device or existing guest disk. |
| NP-05 | Capture bounded, sanitized outcomes, unexpected prompts, timeouts and uncertainty; confirm fixture teardown. |
| NP-06 | Publish native results only for the exact tested profile; keep later installer acceptance gates outstanding. |

## Resources and evidence

Proposed experiment limits, not measured minimum requirements: 1 GiB guest RAM,
8 GiB logical disk, 2 GiB optical-media cap, 20 GiB host fixture allocation cap,
15 minutes to reach the installer prompt, 60 minutes per run, and 10 MiB private
console capture cap. Stop and report if a limit is reached; changes require review.
Measure actual image size, elapsed times, guest memory observations where available,
host process peak memory, disk allocation and capture size. Distinguish unavailable
measurements from zero. CPU emulation results describe that host/backend only.

## Safety and unresolved decisions

Use synthetic names only, no accounts or secrets in the initial boot experiment.
No bridge, NAT, host-only network, shared directory, clipboard, USB/device passthrough
or remote display listener. Backend/firmware provenance and licensing, release
errata review, trustworthy release key acquisition, exact console settings and host
capacity remain preflight decisions. If serial or isolation cannot be established,
stop rather than silently change backend or profile. No general disk rollback is
claimed. Issues #3–#5 require their own scoped acceptance after this baseline.

The [execution preflight](preflight.md) records host tooling observations and
remaining trust/configuration/authorization gates; no guest has launched.
