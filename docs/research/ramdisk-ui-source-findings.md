# OpenBSD 7.9 installer-ramdisk UI source review

Source-only evidence for [issue #4](https://github.com/gonz0w/bootweave/issues/4),
2026-10-11. Scope: [text UI research plan](text-ui-runtime-plan.md). Documentation
and authenticated source inspection only; no prototype, build, media modification,
guest launch or language/library adoption occurred.

## Provenance and inspected bytes

Clean source/base: 426c62dcb036609907cbe74ee55ab7acd095f487.
Tree: e7c9a465548a61bd11b35587df7c3818f892833b.
Rehashed src.tar.gz to
fb305c553059b48e8ee64539f392b783cb38a67865823f6f6a94f3b220a1268b
and repeated the accepted host signify -C check with explicit source filename and
public key under [ADR 0002](../adr/0002-host-artifact-verifier.md).
Verification returned zero. [Release-input evidence](../design/native-profile/release-input-evidence.md)
records the archive/signature/key identities and trust limits.

Read selected regular archive members with Python tarfile, copied exact bytes to
owned temporary review paths and inspected with rg. No archive-controlled path was
used as an extraction destination; no source script or Makefile executed. File
SHA-256 binds these findings to release bytes rather than mutable manuals:

| Release archive member | SHA-256 |
|---|---|
| distrib/amd64/ramdisk_cd/Makefile | eed808f070bb6c1fdf2816ca9dfd8e3b84d54794408f92152b518683cb91d9bb |
| distrib/amd64/ramdisk_cd/list | 3a654d3e105c908d5c802fd14149c51f465ad0f391819eca453b32acf930b5a0 |
| distrib/miniroot/makeconf.awk | 51b92d77764a99523bd84fb682b0c3329cbdbfa4032a5cb5feaf75cb437fad81 |
| distrib/miniroot/list2sh.awk | 686f49ae66dbc4400845f6bd429240d42f12635188753690380d9ee6d7b8b0fe |
| distrib/special/ksh/Makefile | ca7cbd24e55b630d3bd260a49e854b7330f9d87822c5a8142a01d29a7bf5fb0a |
| distrib/special/sysctl/Makefile | 054b731123e8961eec088972a48dd63792685bd2c4b9a4d006900e6cf55d8b33 |
| distrib/special/sysctl/sysctl.c | 732a1c65760d80410b29e12ee75bd871d3fbde227b817290dcdc03ffea98d06e |
| distrib/special/more/Makefile | c137200c4e57ac4dbbb7b0f415b679b5e0fb75ab0736b978c0d247ecc4e249c7 |
| distrib/special/more/more.c | d3769e8876264e95a67dc9b1c97996aaea77f7dd46e0c809f98283a5daa5f660 |
| lib/libcurses/Makefile | 3727077c012dc2f712733701634beada7697d6c6221e10ac0fa30eb2496250c2 |
| lib/libcurses/ncurses_cfg.h | bab4b8fb513eba229290a94f08cdbeb1c5de877d797a8a9ac4757ef44bae916e |
| lib/libcurses/tinfo/read_termcap.c | c2e50f07e2b3c12b2bd5957e7a79b0269da4f3b8726f5fab950cb0edff814eb5 |
| lib/libcurses/tinfo/MKfallback.sh | 20487e2b94d4d26b1fd2dc64e0eb1e1330d8566e693b54aaa3ec40e135a056b4 |
| share/termtypes/Makefile | 06150f0e502359420599d4791e1eb6d43d4eeb41db848512f04498f477f4e2db |

## Ramdisk build boundary

The amd64 ramdisk_cd Makefile selects its local list and builds instbin through
crunchgen, generates mr.fs from that list, and embeds mr.fs into bsd.rd via
rdsetroot. This is the selected stock RAMDISK_CD source path, not an executed build
or proof that a future customized release preserves identical contents.

The list supplies ksh/sh, stty, arch, dmesg and sysctl, but does not select uname,
a C compiler, tic, infocmp, standalone curses shared-library files or a terminfo
tree. It requests only vt100, vt220 and dumb entries in usr/share/misc/termcap.
list2sh.awk implements TERMCAP by running build-host tic against termtypes.master,
then filters the generated termcap output. Thus build-host tic use does not establish
that tic exists in the installer guest. makeconf.awk's default crunched-library list
is stubs, util, m, tls, ssl, crypto, event, c and z; it does not name curses.

The list selects the special more implementation and exposes it under both more
and less names. Its Makefile compiles more.c with local tputs.c, termcap.c and
tgoto.c rather than linking full libcurses. Therefore the presence of a less-named
command is not proof of installed-system less or curses availability. Its source
has a dumb fallback when TERM is missing or capability lookup fails; behavior and
terminal restoration of that pager were not exercised in these native observations.
Full transitive objects/size and all pager helper licenses remain unreviewed.

The special ksh build defines SMALL. Shell behavior/features must be measured in
this variant rather than inferred from full installed ksh. No shell implementation
choice or privileged-shell contract follows from it being in the stock ramdisk.

## Explaining the existing native observations

The [serial stock-media evidence](../design/native-profile/serial-evidence.md)
recorded uname not found and no hw.physmem output. The list lacks uname.
The special sysctl source has a fixed name table with hw.machine but not hw.physmem;
its argument loop produces no diagnostic for an unmatched name and exits zero.
This explains the observed omission under the selected build path. An exit status
alone cannot establish that every requested observation was returned.
The earlier native record did not infer a cause; this subsequent source review adds
that explanation. dmesg supplied actual kernel/memory facts in the native record.

## Curses and terminal-data implications

Installed-system libcurses builds a wide-character ncurses implementation and aliases
termlib, termcap, ncurses and ncursesw library names to curses artifacts. Its build
FALLBACK_LIST is empty; MKfallback.sh generates no matching built-in fallback entries
for that case. Static linking alone must not be assumed to supply terminal data.
The configuration names /usr/share/terminfo and /usr/local/share/terminfo, while
share/termtypes installs compiled terminfo and a termcap file in the full system.
Those full-system targets are separate from the ramdisk list above.

read_termcap.c contains environment-dependent TERMCAP/TERMPATH and home-directory
lookup branches. Their effective compiled/runtime selection is not qualified here.
Any candidate must review the effective library configuration, environment and
trusted lookup locations before initialization. Terminal capability bytes produce
control output, so uncontrolled lookup paths cannot be treated as ordinary project
text. A future review must also inspect TERMINFO/TERMINFO_DIRS behavior, dimensional
bounds and fallback availability in the exact candidate build.

Inspected ncurses configuration/termcap/fallback headers carry permissive permission
notices with retained copyright/permission and non-endorsement conditions. The special
sysctl header carries ISC-style terms; special more.c combines ISC-style and
three-clause Berkeley notices. These are selected-file observations, not complete
license clearance for libcurses, generated tables, pager helpers, termtypes data or
transitive code. No upstream source is copied into Bootweave or redistributed here.
A build/adoption decision still needs that complete inventory and notices.

## Candidate gates

| Candidate | Source finding | Required next evidence |
|---|---|---|
| C with curses | Full library/data are not selected as standalone ramdisk files; no built-in fallback list | Native compile/link, exact dependency/data inventory and licenses, custom ramdisk build/boot and UI cases |
| C with plain line input | No compiler selected in stock ramdisk | Separately reviewed disposable native build environment, bounded input/terminal handling and exact media inclusion |
| Shell with plain prompts | SMALL ksh is selected | Native feature/input/EOF/secret/terminal tests; no arbitrary-shell privilege interface |

All UI-01 through UI-10 remain not run. Stock menu readability and com0 interaction
are a control baseline only. No candidate is recommended or adopted from absence,
source inspection or host tooling. Native build-guest provisioning and prototype
implementation require their reviewed scope; writable targets need separate explicit
confirmation. The next source deliverable is complete candidate license/dependency
and terminal-lookup review before preparing a concrete minimal prototype run plan.

No guests, sockets, disks or processes were created by this review. Selected source
copies remain private temporary evidence; downloaded source/tool storage is separate
from disposable guest fixtures. Local make verify checks documentation/foundation
only and supplies no new native qualification.

[Follow-up terminal lookup review](terminal-lookup-findings.md) resolves selected
compile-time lookup controls, dimension handling and direct pager-helper notices.
Full candidate/transitive license and native qualification remain outstanding.
