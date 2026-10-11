# OpenBSD installer research

Inspected 2026-10-11. Evidence and candidate directions; no code reuse or runtime
architecture is adopted. Confirm release-specific source in each native experiment.

| Source | Relevance and limit |
|---|---|
| [OpenBSD install FAQ](https://www.openbsd.org/faq/faq4.html) | bsd.rd, installer media, site sets and install.site; installer-only ISO does not need a general live appliance. |
| [autoinstall(8)](https://man.openbsd.org/autoinstall.8) | Response files can automate stock installation; question text/default behavior needs exact release qualification. |
| [Installer architecture paper](https://www.openbsd.org/papers/eurobsdcon2023-kn-installer.pdf) | Ramdisk build machinery and boot path; custom UI inclusion needs a native spike. |
| [UTMFW](https://github.com/sonertari/UTMFW) | Existing downloadable OpenBSD appliance with modified installer; source/license scope needs review before reuse. |
| [FuguIta](https://fuguita.org/) | Active live OpenBSD construction/persistence reference; live model differs from our scope. |
| [resflash](https://stable.rcesoftware.com/resflash/) | Read-only/two-image builder reference, not a guided installer; persistence/update semantics differ. |
| [OPNsense installer](https://github.com/opnsense/installer) | FreeBSD-specific installation mechanics; useful UX reference, substantial OpenBSD port. |
| [curses(3)](https://man.openbsd.org/curses.3) and [wscons(4)](https://man.openbsd.org/wscons.4) | Base text UI candidate; ANSI/color/line drawing differs from DOS CP437 artwork assumptions. |

Preferred research direction: a custom text flow over verified OpenBSD machinery,
conventional installed layout initially, generic bounded project configuration,
original ANSI/BBS appearance with serial/plain fallback. Parser, runtime language,
privilege boundary and release-specific integration remain unresolved. Product
bootstrap owns optional DNS peer pairing and firewall role/configuration semantics.
