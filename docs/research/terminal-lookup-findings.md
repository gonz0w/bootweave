# Release terminal lookup and dimension review

Source evidence for [issue #4](https://github.com/gonz0w/bootweave/issues/4),
2026-10-11. Continues the [ramdisk inventory](ramdisk-ui-source-findings.md) under
[UI research scope](text-ui-runtime-plan.md); no candidate adopted or prototype run.
Clean source/base: f5f6e6064aca69c73ab5723e9c67e73127a0f77f.
Tree: 89805dcdb4b3c95662f7d7a6dd7b973bd4d4b877.

Rehashed the previously authenticated release src.tar.gz to
fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b.
The existing [signature verification and trust policy](../design/native-profile/release-input-evidence.md)
apply to these identical bytes. Read regular selected members via Python tarfile,
wrote only fixed owned temporary paths, then inspected with rg/sed. No source
script/Makefile, compiler, guest, dependency install or media modification executed.

## Curses lookup: root is not a trusted-path policy

Release term.h explicitly enables both NCURSES_USE_DATABASE and NCURSES_USE_TERMCAP.
ncurses_cfg.h enables USE_HOME_TERMINFO, USE_ROOT_ENVIRON and HAVE_ISSETUGID.
nc_access.h maps use_terminfo_vars to _nc_env_access unless both root and setuid
environment access are enabled. access.c rejects elevated issetugid execution when
USE_SETUID_ENVIRON is not defined, but its ordinary-root rejection is compiled out
when USE_ROOT_ENVIRON is defined. Thus ordinary root execution does not by itself
suppress environment-dependent database lookup in this source configuration.
This is a source derivation; effective candidate compile flags and native execution
remain to be checked. Do not assume setuid and ordinary root have identical behavior.

The database iterator considers configured terminfo paths alongside TERMINFO,
TERMINFO_DIRS and HOME-derived .terminfo when environment access is allowed.
It also considers absolute TERMCAP and TERMPATH in the termcap branch; the termcap
reader examined previously additionally handles inline entries. home_terminfo.c
caches the expanded home path. No complete search-precedence contract or safe
post-initialization environment change is claimed from this selected review.
A candidate must establish its environment and trusted database locations before
library initialization, then verify effective lookup and file access natively.

## Special pager lookup is a different implementation

The stock ramdisk pager's local termcap.c reads TERMCAP only when issetugid is false.
If absent it selects /usr/share/misc/termcap directly. An absolute TERMCAP selects
paths; the non-absolute branch consults TERMPATH or a HOME-derived/default path.
This concrete code is narrower than its broad fallback comment: HOME/TERMPATH are
not consulted in the initial absent-TERMCAP branch. Running as ordinary root is
not an issetugid guarantee. The local parser is separate from full ncurses and its
findings must not be applied to a future curses prototype without native checks.
The existing stock-menu observations did not exercise this pager or its lookup.

## Dimensions and resource bounds

lib_setup.c can obtain dimensions from terminal capabilities, terminal ioctl and
LINES/COLUMNS according to use_env/use_tioctl settings. Environment values can
override measured dimensions in the inspected path. Nonpositive results fall back
to capabilities and eventually 24 rows/80 columns. That fallback does not measure
a serial endpoint's dimensions or prove prompts fit the actual display.

getenv_num.c uses strtol and rejects absent, negative, unparsed/trailing and
nonrepresentable values in its inspected checks. This is not an application-specific
maximum size or display-buffer budget. A candidate needs independent bounded row/
column and allocation arithmetic, safe behavior for zero/missing ioctl dimensions,
and native evidence for oversized values, resize, narrow screens and invalid TERM.
Do not use inherited LINES/COLUMNS or fallback 80x24 as qualification evidence.

## Selected license closure and limits

The three local pager helpers termcap.c, tputs.c and tgoto.c each carry Berkeley
three-clause redistribution notices: retain source notices, reproduce notices in
binary distribution material, and do not imply endorsement. Together with the
previously inspected more.c, this closes header inspection for the four directly
named C files in the special pager Makefile. It does not clear libc, headers,
generated morehelp.h, more.help, termtypes data or the full transitive binary.
No pager code is copied into Bootweave.

The inspected curses lookup/sizing sources carry permissive ncurses notices with
copyright/permission retention and non-endorsement conditions. This review is not
a complete libcurses/generated-table/license inventory or an adoption ADR. Any
future binary/media distribution still needs its own complete provenance/notices.

## Evidence and next review target

| Release source member | SHA-256 |
|---|---|
| lib/libcurses/term.h | cd42136e986dac12acb49e6d96822da8e1469bd92c5c7dee6df6a2dd5b81d156 |
| lib/libcurses/ncurses_cfg.h | bab4b8fb513eba229290a94f08cdbeb1c5de877d797a8a9ac4757ef44bae916e |
| lib/libcurses/nc_access.h | 858da717a16511569cf51779d9cdd113dd71231772bd176d4abd5dd663fd7c85 |
| lib/libcurses/tinfo/access.c | dee80ea536afa01bd4486aa06f5ea8fa7abc01ca5aeafe1c5a018a33b517b957 |
| lib/libcurses/tinfo/db_iterator.c | 557598d0b4a16eb9e96904c13f378ce1f55459706b8916d5f6be3cde1b6f7333 |
| lib/libcurses/tinfo/home_terminfo.c | 652b3a992355b47bac5e40cf9333fc3ec5a1a4e051f11feadd423b88ec432aa3 |
| lib/libcurses/tinfo/lib_setup.c | 61633a92dc49ba79b4dbc9d9688f74b9eb18b76f3214967025285dd5a958d01a |
| lib/libcurses/tinfo/getenv_num.c | 82272d77ae7624532fabd9907e33be0959f832ce5b05f8e38a60296fa64121cc |
| distrib/special/more/termcap.c | 6939d0c2c75598cce2de34eeecafb6035d0abfd0c420ccb2744faf4b8c756e15 |
| distrib/special/more/tputs.c | 4912e34610e7be4056d931a2a5f715cf63428647da5e61176f425a274e6ecb42 |
| distrib/special/more/tgoto.c | 1be603d5b785b4353c3f9ec93af78215f48ddae18054cac7f955113e812af854 |
| distrib/special/more/pathnames.h | fcf456be470000e724f9550af57ab1106025761e2d37434c14a42814bc7a5f0e |

| Candidate review case | Evidence required before qualification |
|---|---|
| Unset/invalid TERM and absent trusted database | Readable plain fallback or safe stop; no confirmation |
| TERMINFO/TERMINFO_DIRS/TERMCAP/TERMPATH/HOME substitution | Effective reviewed lookup policy; no project-selected capability file |
| Ordinary root versus elevated execution | Exact compiled policy and observed lookup, without granting runtime privilege |
| Missing, zero, oversized or inconsistent dimensions | Bounded layout/allocations; actual dimensions separate from defaults |
| Resize and cached lookup after initialization | Observed behavior and terminal restoration; no pending confirmation survives uncertainty |

These are inputs to UI-03/04/05/07/09/10, not passed test cases. All UI tests remain
unrun. Next: freeze one minimal synthetic plain-text prototype's exact code/input
scope, transport, guest commands, resource caps and cleanup in a reviewable spike
plan. This review authorizes no implementation, native launch, disk write or durable
runtime choice. C candidates additionally require native build provisioning and
complete dependencies; shell candidates must qualify the SMALL ksh behavior.

No new native fixtures/processes/endpoints needed cleanup. Temporary source copies
remain private review material. make verify supplies local foundation/document
checks only; it does not qualify terminal behavior.
