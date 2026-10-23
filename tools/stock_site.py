# SPDX-License-Identifier: BSD-3-Clause
"""Build-host stock site-set assembly. Not an installer or authenticity verdict.

The admitted build owner supplies immutable, already-authenticated package
bytes and exact expected digests. All outputs remain source/unqualified: the
included install.site refuses. No native tool or package is ever executed here.
"""
import hashlib
import io
import json
from pathlib import Path
import re
import shlex
import tarfile

from tools.package_closure import closure

PACKAGE = re.compile(r"[A-Za-z0-9][A-Za-z0-9+_.-]*\.tgz\Z")
ENTRY = re.compile(r"/usr/local/libexec/(?:[A-Za-z0-9_+-][A-Za-z0-9_.+-]*/)*[A-Za-z0-9_+-][A-Za-z0-9_.+-]*\Z")
DIGEST = re.compile(r"[0-9a-f]{64}\Z")
TEMPLATE = Path(__file__).resolve().parents[1] / "installer/stock/install.site"


def setup_literals(setup, packages, authority):
    """Return validated literal fields; all runtime logic stays in the template.

    A digest binds bytes; caller review and authenticated media/package policy
    must authorize the extension and its installation privileges separately.
    """
    if setup is None:
        return {"@SETUP_PATH@": "''", "@SETUP_SHA256@": "''", "@SETUP_ARGUMENTS@": ""}
    if not isinstance(setup, dict) or set(setup) != {"package", "path", "sha256", "arguments"}:
        raise ValueError("unknown or incomplete setup contract")
    package, path, digest, args = (setup[k] for k in ("package", "path", "sha256", "arguments"))
    if not isinstance(package, str) or package not in packages or authority[package] != "consumer":
        raise ValueError("setup requires a selected consumer package")
    if not isinstance(path, str) or len(path) > 192 or not ENTRY.fullmatch(path):
        raise ValueError("setup requires a local libexec entry without aliases")
    if not isinstance(digest, str) or not DIGEST.fullmatch(digest):
        raise ValueError("setup requires an exact executable digest")
    if (not isinstance(args, list) or len(args) > 8 or any(
            not isinstance(arg, str) or not 1 <= len(arg) <= 128
            or any(not 0x21 <= ord(c) <= 0x7e for c in arg) for arg in args)):
        raise ValueError("setup arguments must be bounded printable tokens")
    return {"@SETUP_PATH@": shlex.quote(path), "@SETUP_SHA256@": shlex.quote(digest),
            "@SETUP_ARGUMENTS@": " ".join(shlex.quote(arg) for arg in args)}


def site_set(packages, expected, authority, *, architecture, root_package, setup=None):
    """Return deterministic site80.tgz bytes and measured supplied closure.

    expected and authority have exactly the package filenames as keys. These
    comparisons bind build bytes; they do NOT authenticate their publisher or
    establish dependency completeness. Native pkg_add must still verify its
    signatures against separately admitted OS/consumer trust anchors. Optional
    setup follows the generic package-entry-v1 contract; no product dispatch.
    """
    packages, expected, authority = dict(packages), dict(expected), dict(authority)
    if not packages or set(packages) != set(expected) or set(packages) != set(authority):
        raise ValueError("incomplete inventory")
    inventory = []
    for name in sorted(packages):
        data = packages[name]
        if not isinstance(name, str) or not PACKAGE.fullmatch(name) or ".." in name:
            raise ValueError("nonlocal or ambiguous package name")
        if not isinstance(data, bytes) or not data:
            raise ValueError("missing immutable package bytes")
        digest = hashlib.sha256(data).hexdigest()
        if expected[name] != digest or authority[name] not in ("openbsd", "consumer"):
            raise ValueError("changed bytes or unknown trust domain")
        inventory.append({"name": name, "bytes": len(data), "sha256": digest,
                          "authority": authority[name]})
    measured = closure(packages, architecture, root_package)
    if authority[root_package] != "consumer":
        raise ValueError("root package must use consumer trust")
    literals = setup_literals(setup, packages, authority)
    bootstrap = TEMPLATE.read_bytes()
    if (bootstrap.count(b"@PACKAGES@") != 1 or any(bootstrap.count(key.encode()) != 1 for key in literals)
            or not bootstrap.endswith(b"exit 77\n")):
        raise ValueError("changed source activation gate")
    arguments = " ".join("'/var/db/bootweave/packages/" + p["name"] + "'" for p in inventory)
    bootstrap = bootstrap.replace(b"@PACKAGES@", arguments.encode("ascii"))
    for key, value in literals.items():
        bootstrap = bootstrap.replace(key.encode("ascii"), value.encode("ascii"))
    report = {"contract": "bootweave-stock-site-v1", "root_package": root_package,
              "setup": setup, "release": "8.0", "status": "source-unqualified",
              "packages": inventory, "package_bytes": sum(p["bytes"] for p in inventory),
              "bootstrap_bytes": len(bootstrap), "dependency_completeness": "not established",
              "packing_list_closure": measured,
              "installed_unpacked_bytes": None, "stock_sets_bytes": None,
              "authenticity": "requires admitted site media, separate native package trust and reviewed setup authority"}
    manifest = (json.dumps(report, sort_keys=True, separators=(",", ":")) + "\n").encode()
    output = io.BytesIO()
    # Stable gzip header as well as stable tar metadata; no current host IDs,
    # addresses, keys, credentials or timestamps become appliance defaults.
    import gzip
    with gzip.GzipFile(fileobj=output, mode="wb", mtime=0, filename="") as zipped:
        with tarfile.open(fileobj=zipped, mode="w", format=tarfile.USTAR_FORMAT) as tar:
            def add(name, data, mode):
                member = tarfile.TarInfo(name)
                member.size, member.mode = len(data), mode
                member.uid = member.gid = member.mtime = 0
                tar.addfile(member, io.BytesIO(data))
            for directory in ("var", "var/db", "var/db/bootweave", "var/db/bootweave/packages"):
                member = tarfile.TarInfo(directory)
                member.type, member.mode = tarfile.DIRTYPE, 0o755 if directory in ("var", "var/db") else 0o700
                member.uid = member.gid = member.mtime = 0
                tar.addfile(member)
            add("install.site", bootstrap, 0o500)
            add("var/db/bootweave/closure.json", manifest, 0o600)
            for name in sorted(packages):
                add("var/db/bootweave/packages/" + name, packages[name], 0o600)
    report["site_archive_bytes"] = len(output.getvalue())
    return output.getvalue(), report
