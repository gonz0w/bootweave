# SPDX-License-Identifier: BSD-3-Clause
"""Seal the separate evidence-only source bundle; never provision media/guests."""

import hashlib
import os
from pathlib import Path
import shutil
import tempfile

from native_cases import ROOT, artifacts


def prepare():
    files = artifacts()
    for name in ["src/main.c", "src/plan.c", "src/plan.h", "src/plan_io.c", "src/native.c",
                 "tests/native_check.c", "tests/native_trace.c", "tests/trace_limits.c",
                 "tests/native_trace_build.sh"]:
        files[Path(name).name] = (ROOT / name).read_bytes()
    files["SHA256"] = ''.join(f"SHA256 ({name}) = {hashlib.sha256(data).hexdigest()}\n"
                               for name, data in sorted(files.items())).encode()
    directory = Path(tempfile.mkdtemp(prefix="planner-trace-preparation-", dir=ROOT / ".tools"))
    try:
        for name, data in files.items():
            fd = os.open(directory / name, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
            with os.fdopen(fd, "wb") as stream:
                stream.write(data)
    except BaseException:
        shutil.rmtree(directory)
        raise
    return directory


if __name__ == "__main__":
    import sys
    if len(sys.argv) != 1:
        raise SystemExit("Preparation accepts no arguments; no media or guest is created.")
    print(prepare())
