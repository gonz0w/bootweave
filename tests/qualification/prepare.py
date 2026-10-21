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
    "oracle_notes.md", "README.md", "runtime-bindings.json", "prepare.py",
    "compiler_guard.c", "native_supervisor.c", "trace_limits.c", "trace_limits.h",
    "trace_model.c", "trace_oracles.json", "host_guard.py", "next-step.md", "accounting.py",
    "coordinator.py", "transport.py", "lifetime.py", "__init__.py"))
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
    flags = ["/usr/bin/cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic"]
    # Compile one translation unit per job so temporary-file accounting does not
    # assume a multi-source driver deletes intermediates promptly. All explicit
    # objects remain retained. No accepted stage runs the older static previews.
    units = common + ["tests/qualification/entry_probe.c", "tests/qualification/limit_probe.c",
                      "tests/qualification/native_supervisor.c"]
    objects = {name: "artifacts/unit-" + str(i).zfill(2) + ".o" for i, name in enumerate(units)}
    auxiliary = [{"name": "unit-" + str(i).zfill(2), "argv": flags + ["-pipe", "-Isrc", "-I.", "-c", name,
        "-o", objects[name]], "purpose": "retained_single_translation_unit", "slot_seconds": 25}
        for i, name in enumerate(units)]
    auxiliary.append({"name": "supervisor", "argv": flags + ["-Wl,-Map=artifacts/nq-supervisor.map",
        objects["tests/qualification/native_supervisor.c"], "-o", "artifacts/nq-supervisor"],
        "purpose": "nine_unmodified_cases_raw_witnesses_only", "slot_seconds": 150})
    for job, driver in ((jobs[0], "tests/qualification/entry_probe.c"),
                        (jobs[1], "tests/qualification/limit_probe.c")):
        output = "artifacts/nq-" + job["name"]
        job["argv"] = flags + ["-Wl,-Map=" + output + ".map",
            *(objects[name] for name in common), objects[driver], "-o", output]
        job["slot_seconds"] = 150
    trace = []
    for label, source in (("start", "src/trusted_start.c"), ("child", "src/signature_platform.c")):
        trace.append({"name": "trace-" + label, "argv": flags + ["-Isrc",
            "-include", "tests/qualification/trace_limits.h",
            "-Dgetrlimit=bw_nq_trace_getrlimit", "-Dsetrlimit=bw_nq_trace_setrlimit",
            "-c", source, "-o", "artifacts/nq-trace-" + label + ".o"],
            "purpose": "disclosed_companion_only_paired_FSIZE_substitution"})
    trace.append({"name": "trace-limits", "argv": flags + ["-c",
        "tests/qualification/trace_limits.c", "-o", "artifacts/nq-trace-limits.o"],
        "purpose": "unrenamed_real_syscalls_only_in_this_object"})
    for static in (False, True):
        name = "trace-closed-" + ("static" if static else "dynamic")
        rest = [s for s in common if s not in ("src/trusted_start.c", "src/signature_platform.c")]
        output = "artifacts/nq-" + name
        trace.append({"name": name, "argv": flags + (["-static"] if static else []) + [
            "-Isrc", "-Wl,-Map=" + output + ".map", *rest,
            "artifacts/nq-trace-start.o", "artifacts/nq-trace-child.o", "artifacts/nq-trace-limits.o",
            "tests/qualification/entry_probe.c", "-o", output],
            "purpose": "modified_binary_trace_only_no_native_case_or_positive_claim"})
    return {"schema": "bootweave-qualification-build-preview-v2",
            "status": "DATA_ONLY_NOT_EXECUTABLE_OR_AUTHORIZED", "canonical": CANONICAL,
            "tree": TREE, "cwd": "/bwq-92166f07", "jobs": jobs,
            "auxiliary_jobs": auxiliary, "trace_jobs": trace,
            "first_stage_jobs": [job["name"] for job in auxiliary] + ["closed-dynamic", "limits-dynamic"],
            "first_stage_build_batch": {"wall_seconds": 1200, "bootstrap_seconds": 120,
                "guarded_slot_seconds": 950, "setup_and_audit_seconds": 130,
                "file_ceiling_bytes": 262144, "guarded_calls": 23, "bootstrap_calls": 1},
            "requires": ["fresh exact native target/effects authority", "exclusive owned artifacts directory",
                         "independently reviewed compiler_guard.c/native_supervisor.c and external controller",
                         "all source/hash checks before and after build", "independent actual native binary/map seals",
                         "frozen final count/time/storage/transport/ownership ledger"],
            "forbidden": ["substitution in production or unmodified-limit jobs", "product CLI readiness override",
                          "private key or credential transport", "automatic compile/run/cleanup"],
            "production_PLAN_SOURCES_unchanged": True}


def jobs_header(build):
    """Fixed reviewed argv arrays, not a caller command parser or authority."""
    jobs = build["jobs"] + build["auxiliary_jobs"] + build["trace_jobs"]
    if len({job["name"] for job in jobs}) != len(jobs):
        raise ValueError("Duplicate fixed build job")
    if len(jobs) > 40 or any(len(job["argv"]) >= 96 for job in jobs):
        raise ValueError("Fixed build table exceeded")
    rows = ["/* Generated source-only fixed jobs; no execution authority. */",
            "#define NQ_BUILD_COUNT " + str(len(jobs)),
            "static const char *const nq_build_names[] = {" +
            ",".join(json.dumps(job["name"]) for job in jobs) + "};",
            "static const long long nq_build_ms[] = {" +
            ",".join(str(job.get("slot_seconds", 180) * 1000) for job in jobs) + "};",
            "static char *const nq_build_argv[][96] = {"]
    for job in jobs:
        if job["argv"][0] != "/usr/bin/cc":
            raise ValueError("Unexpected fixed compiler")
        rows.append("{" + ",".join(json.dumps(arg) for arg in job["argv"]) + ",0},")
    return ("\n".join(rows + ["};", ""]).encode())


def oracle_header(raw):
    """Necessary local stop gates; independent native acceptance stays external."""
    if hashlib.sha256(raw).hexdigest() != "610a161c0c0d64bbebdd35d1584a0ec5234186bec3b2c453f50ffbbf40381f73":
        raise ValueError("Independent native oracle bytes changed")
    oracle = json.loads(raw)
    cases = oracle["cases"]
    fields = oracle["entry_record"]["bounded_numeric_fields"]
    exits, sizes, entries, limits = [], [], [], []
    for case in cases:
        expected = case["expected"]
        exits.append(str(expected["exit_status"]))
        sizes.append(str(expected.get("final_fd3_size", -1)))
        record = expected.get("record")
        entries.append("0" if record is None else json.dumps("NQ_ENTRY " +
            " ".join(str(record[field]) for field in fields) + "\n"))
        if "actual_soft_hard_pairs" in expected:
            limits.append(json.dumps("NQ_LIMITS " + expected["profile_token"] + " " +
                " ".join(str(soft) + ":" + str(hard) for soft, hard in expected["actual_soft_hard_pairs"])))
    rows = ["/* Generated necessary stop gates, not independent native proof. */",
            "static const int nq_expected_exit[9] = {" + ",".join(exits) + "};",
            "static const long long nq_expected_fsize[9] = {" + ",".join(sizes) + "};",
            "static const char *const nq_expected_entry[9] = {" + ",".join(entries) + "};",
            "static const char *const nq_expected_limits[2] = {" + ",".join(limits) + "};", ""]
    return "\n".join(rows).encode()


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
    files["BUILD-JOBS.h"] = jobs_header(build)
    files["ORACLE-GATES.h"] = oracle_header(files["tests/qualification/oracles.json"])
    # These roles are never an authenticated fixture or a signing policy.
    # The nine-case closed/unmodified stage cannot reach cryptographic use.
    files["inputs/definition.bin"] = files[FIXTURES[0]]
    files["inputs/payload.bin"] = files[FIXTURES[1]]
    files["inputs/signature.bin"] = b"unsigned closed-entry fixture\n"
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
            parent = directory
            for part in Path(name).parts[:-1]:
                parent = parent / part
                parent.mkdir(mode=0o700, exist_ok=True)
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
