# SPDX-License-Identifier: BSD-3-Clause
"""Seal qualification SOURCE only; cannot compile, sign, launch or dispose.

No caller paths, environment overrides, process APIs or native artifacts. Failed
output namespaces remain retained. Source reads assume coordinator quiescence;
stable identity/hash checks are not a hostile-filesystem confinement claim.
"""
import hashlib
import json
import os
from pathlib import Path
import stat
import tempfile

ROOT = Path(__file__).resolve().parents[2]
CANONICAL = "92166f07fd79c9d356ef3e1ed704eaeeed8653e4"
TREE = "79060329e9d06283a95c247c9245bbdd60ac9081"
RUNTIME_BINDING_SHA256 = "6d537a176ef58cac2ea0ad4b91822dea32bf51928220c265cfb7bcc995bad797"
MAX_FILE = 262144
MAX_SOURCE = 1048576
HELPERS = tuple("tests/qualification/" + name for name in (
    "entry_probe.c", "limit_probe.c", "forwarder.c", "oracles.json",
    "oracle_notes.md", "README.md", "runtime-bindings.json", "prepare.py"))
FIXTURES = ("tests/fixtures/policy-definition.json", "tests/fixtures/policy-payload.txt")


def identity(value):
    return (value.st_dev, value.st_ino, value.st_mode, value.st_uid, value.st_gid,
            value.st_nlink, value.st_size, value.st_mtime_ns, value.st_ctime_ns)


def read_source(name):
    parts = Path(name).parts
    if not parts or Path(name).is_absolute() or any(p in (".", "..") for p in parts):
        raise ValueError("Invalid fixed source name")
    current = ROOT
    for part in parts[:-1]:
        current /= part
        if not stat.S_ISDIR(current.lstat().st_mode):
            raise ValueError("Source parent is not an ordinary directory")
    path = ROOT / name
    before = path.lstat()
    if not stat.S_ISREG(before.st_mode) or before.st_size > MAX_FILE:
        raise ValueError("Source must be a bounded ordinary file")
    fd = os.open(path, os.O_RDONLY | os.O_NOFOLLOW)
    with os.fdopen(fd, "rb") as stream:
        opened = os.fstat(stream.fileno())
        data = stream.read(MAX_FILE + 1)
        after = os.fstat(stream.fileno())
    if not (identity(before) == identity(opened) == identity(after) == identity(path.lstat())):
        raise ValueError("Source identity changed")
    if len(data) != before.st_size or len(data) > MAX_FILE:
        raise ValueError("Source size changed")
    return data


def recipe(runtime):
    common = [name for name in sorted(runtime) if name.endswith(".c") and
              name not in ("src/main.c", "src/entry_cli.c")]
    jobs = []
    for static in (False, True):
        variant = "static" if static else "dynamic"
        for label, driver, forward in (
                ("closed", "entry_probe.c", False),
                ("limits", "limit_probe.c", False),
                ("forwarded-inert", "entry_probe.c", True)):
            sources = [name for name in common if not (forward and name == "src/entry_platform.c")]
            if forward:
                sources.append("tests/qualification/forwarder.c")
            output = "artifacts/nq-" + label + "-" + variant
            argv = ["/usr/bin/cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic"]
            if static:
                argv.append("-static")
            argv += ["-Isrc", "-Wl,-Map=" + output + ".map", *sources,
                     "tests/qualification/" + driver, "-o", output]
            jobs.append({"name": label + "-" + variant, "argv": argv,
                         "purpose": "unmodified_limits_or_closed_refusal" if not forward else
                         "source_prepared_only_no_frozen_native_case_or_positive_signature_claim"})
    return {"schema": "bootweave-nontrace-qualification-build-preview-v1",
            "status": "DATA_ONLY_NOT_EXECUTABLE_OR_AUTHORIZED", "canonical": CANONICAL,
            "tree": TREE, "cwd": "/bwq-92166f07", "jobs": jobs,
            "requires": ["fresh exact native target/effects authority", "exclusive owned artifacts directory",
                         "reviewed bounded compiler/custody executor; none supplied by this preview",
                         "all source/hash checks before and after build", "independent actual native binary/map seals",
                         "frozen final count/time/storage/transport/ownership ledger"],
            "forbidden": ["trace or get/setrlimit substitution", "product CLI readiness override",
                          "private key or credential transport", "automatic compile/run/cleanup"],
            "production_PLAN_SOURCES_unchanged": True}


def sources():
    raw = read_source("tests/qualification/runtime-bindings.json")
    if hashlib.sha256(raw).hexdigest() != RUNTIME_BINDING_SHA256:
        raise ValueError("Canonical runtime binding bytes changed")
    binding = json.loads(raw)
    if (binding["canonical"], binding["tree"]) != (CANONICAL, TREE):
        raise ValueError("Wrong canonical runtime binding")
    expected = binding["files"]
    actual = {str(path.relative_to(ROOT)) for path in (ROOT / "src").rglob("*")
              if path.suffix in (".c", ".h")}
    if set(expected) != actual:
        raise ValueError("Runtime closure differs from canonical source")
    files = {}
    for name, row in expected.items():
        if not name.startswith("src/") or Path(name).suffix not in (".c", ".h"):
            raise ValueError("Unexpected runtime name")
        data = read_source(name)
        if len(data) != row["bytes"] or hashlib.sha256(data).hexdigest() != row["sha256"]:
            raise ValueError("Runtime bytes differ from canonical source")
        files[name] = data
    for name in HELPERS + FIXTURES:
        files[name] = read_source(name)
    if sum(map(len, files.values())) > MAX_SOURCE:
        raise ValueError("Sealed source allocation exceeded")
    return files, recipe(expected)


def prepare():
    files, build = sources()
    files["BUILD-PREVIEW.json"] = (json.dumps(build, sort_keys=True, indent=2) + "\n").encode()
    manifest = {name: {"bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()}
                for name, data in sorted(files.items())}
    files["SOURCE-MANIFEST.json"] = (json.dumps({"canonical": CANONICAL, "tree": TREE,
        "source_only": True, "files": manifest}, sort_keys=True, indent=2) + "\n").encode()
    if sum(map(len, files.values())) > MAX_SOURCE:
        raise ValueError("Complete sealed bundle allocation exceeded")
    tools = ROOT / ".tools"
    if not stat.S_ISDIR(tools.lstat().st_mode):
        raise ValueError("Source-only tools parent is not an ordinary directory")
    directory = Path(tempfile.mkdtemp(prefix="entry-qualification-source-", dir=tools))
    try:
        for name, data in sorted(files.items()):
            path = directory / name
            path.parent.mkdir(mode=0o700, parents=True, exist_ok=True)
            fd = os.open(path, os.O_WRONLY | os.O_CREAT | os.O_EXCL | os.O_NOFOLLOW, 0o600)
            with os.fdopen(fd, "wb") as stream:
                stream.write(data)
    except BaseException as error:
        raise RuntimeError("Source preparation failed; partial namespace retained: " + str(directory)) from error
    return directory


if __name__ == "__main__":
    import sys
    if len(sys.argv) != 1:
        raise SystemExit("Source preparation accepts no arguments; it cannot execute a recipe.")
    print(prepare())
