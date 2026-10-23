# SPDX-License-Identifier: BSD-3-Clause
"""Measure supplied OpenBSD packages without extracting or executing them.

This is packing-list closure, not a signature, shared-library compatibility,
installed-allocation or package-script safety verdict. Native pkg_add remains
the package authority. No downloads or production package size cap are added.
"""
import io
import gzip
from pathlib import PurePosixPath
import re
import tarfile
import zlib

NAME = re.compile(r"[A-Za-z0-9][A-Za-z0-9+_.-]*\Z")


def closure(packages, architecture, root_package):
    if architecture not in ("amd64", "arm64") or root_package not in packages:
        raise ValueError("missing exact package root/architecture")
    # Package directories use MACHINE_ARCH; pkg_add accepts both MACHINE_ARCH
    # and MACHINE in @arch. On arm64 these are aarch64 and arm64 respectively.
    package_architecture = {"amd64": "amd64", "arm64": "aarch64"}[architecture]
    compatible_architectures = {architecture, package_architecture, "*"}
    inventory = {}
    for filename, raw in packages.items():
        if not isinstance(filename, str) or not filename.endswith(".tgz") or not NAME.fullmatch(filename[:-4]):
            raise ValueError("nonlocal package")
        try:
            with gzip.GzipFile(fileobj=io.BytesIO(raw), mode="rb") as zipped, tarfile.open(fileobj=zipped, mode="r|") as archive:
                first = archive.next()
                # This limit is for the small packing list, never package data.
                if first is None or first.name != "+CONTENTS" or not first.isfile() or not 0 < first.size <= 262144:
                    raise ValueError("missing bounded first packing list")
                text = archive.extractfile(first).read().decode("utf-8")
                total, members, names = 0, 1, {first.name}
                for member in archive:
                    if member is first:
                        continue
                    path = PurePosixPath(member.name)
                    if str(path) in names or str(path)=="." or path.is_absolute() or ".." in path.parts:
                        raise ValueError("ambiguous archive path")
                    names.add(str(path))
                    total += member.size if member.isfile() else 0
                    members += 1
                # tar EOF alone does not validate gzip CRC/ISIZE or truncated
                # footers. Drain through the actual gzip reader's checked EOF.
                while zipped.read(65536):
                    pass
        except (tarfile.TarError, EOFError, UnicodeError, OSError, zlib.error) as error:
            raise ValueError("malformed package") from error
        declared, arches, dependencies, libraries, predicates = [], [], [], [], []
        for line in text.splitlines():
            if line.startswith("@name "):
                declared.append(line[6:])
            elif line.startswith("@arch "):
                arches.append(line[6:])
            elif line.startswith("@depend "):
                parts = line[8:].split(":")
                if len(parts) != 3 or not all(parts) or not NAME.fullmatch(parts[2]):
                    raise ValueError("malformed dependency")
                # Select the exact default, rather than inventing a spec solver
                # that silently selects another version from a moving mirror.
                dependencies.append(parts[2]+".tgz")
                predicates.append({"pkgpath":parts[0], "pkg_spec":parts[1], "default":parts[2]})
            elif line.startswith("@wantlib "):
                libraries.append(line[9:])
        if declared != [filename[:-4]] or len(arches) != 1 or not compatible_architectures.intersection(arches[0].split(",")):
            raise ValueError("package name/architecture mismatch")
        inventory[filename] = {"archive_bytes": len(raw), "regular_member_bytes": total,
                               "members": members, "declared_architecture": arches[0],
                               "dependencies": sorted(set(dependencies)),
                               "dependency_predicates":predicates,
                               "shared_libraries": libraries}
    order, active, seen = [], set(), set()

    def visit(name):
        if name not in inventory or name in active:
            raise ValueError("missing or cyclic package dependency")
        if name in seen:
            return
        active.add(name)
        for dependency in inventory[name]["dependencies"]:
            visit(dependency)
        active.remove(name)
        seen.add(name)
        order.append(name)

    visit(root_package)
    if seen != set(packages):
        raise ValueError("unrelated package outside root closure")
    return {"architecture": architecture, "root_package": root_package,
            "package_architecture": package_architecture,
            "dependency_order": order, "packages": inventory,
            "archive_bytes": sum(p["archive_bytes"] for p in inventory.values()),
            "regular_member_bytes": sum(p["regular_member_bytes"] for p in inventory.values()),
            "status": "supplied default-name closure; native signatures/pkg-specs/conflicts/libraries/scripts unqualified",
            "installed_allocation_bytes": None}
