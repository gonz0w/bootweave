# SPDX-License-Identifier: BSD-3-Clause
"""Freeze synthetic native planner cases without creating media, disks or guests."""

import copy
import hashlib
import itertools
import json
import os
from pathlib import Path
import random
import shutil
import tempfile

from plan_cases import BASE, encode, expected

ROOT = Path(__file__).resolve().parents[1]


def corpus():
    cases = []

    def add(label, data, model=None):
        cases.append((label, data, 3 if model is None else 0,
                      b"" if model is None else expected(model)))

    for identity, name in [("offline-viewer", "Offline Viewer"),
                           ("dns", "DNS"), ("firewall", "Firewall")]:
        model = copy.deepcopy(BASE)
        model["project"] = {"id": identity, "name": name}
        for order in itertools.permutations(model):
            for flips in itertools.product([False, True], repeat=3):
                ordered = {}
                for key in order:
                    value = model[key]
                    if isinstance(value, dict):
                        flip = flips[["project", "system", "payload"].index(key)]
                        value = dict(list(value.items())[:: -1 if flip else 1])
                    ordered[key] = value
                add("order-" + identity, encode(ordered), model)
    raw = encode(BASE)
    add("whitespace", b" \t\r\n" + json.dumps(BASE, indent=2).encode() + b"\r\n", BASE)
    for ch in range(32, 127):
        model = copy.deepcopy(BASE)
        model["project"]["name"] = "Value " + chr(ch)
        data = encode(model).replace(encode(model["project"]["name"]),
                                     b'"Value \\u%04x"' % ch)
        add("printable-escape", data, model)
    model = copy.deepcopy(BASE)
    model["project"]["name"] = 'Viewer %s %n $(id); `id` \\ " /'
    add("metacharacters", encode(model), model)
    for group in [None, "project", "system", "payload"]:
        node = BASE if group is None else BASE[group]
        for key in node:
            model = copy.deepcopy(BASE)
            target = model if group is None else model[group]
            del target[key]
            add("missing-" + key, encode(model))
            for wrong in [None, True, False, 0, 1, 1.5, [], {}, ""]:
                if type(wrong) is type(node[key]) and wrong == node[key]:
                    continue
                model = copy.deepcopy(BASE)
                target = model if group is None else model[group]
                target[key] = wrong
                add("type-" + key, encode(model))
            needle = json.dumps(key).encode() + b":"
            add("duplicate-" + key, raw.replace(needle, needle + encode(node[key]) + b"," + needle, 1))
            alias = b'"\\u%04x' % ord(key[0]) + key[1:].encode() + b'":'
            add("escaped-key-" + key, raw.replace(needle, alias, 1))
        model = copy.deepcopy(BASE)
        target = model if group is None else model[group]
        target["unknown"] = "x"
        add("unknown-member", encode(model))
    bounds = [
        ("project", "id", ["a", "a" * 63, "a" * 64],
         ["", "a" * 65, "A", "1a", "-a", "a_b", "a.b"]),
        ("project", "name", ["X", "X" * 79, "X" * 80],
         ["", " ", " " * 80, "X" * 81]),
        ("payload", "file", ["a", "A" * 127, "A" * 128, "a._-9"],
         ["", "A" * 129, ".a", "../a", "/a", "a/b", "a b"]),
        ("payload", "sha256", ["0123456789abcdef" * 4],
         ["a" * 63, "a" * 65, "A" * 64, "g" * 64]),
        ("system", "release", ["7.9"], ["7.8", "7.90", "", "7.9 "]),
        ("system", "architecture", ["amd64"], ["arm64", "AMD64", "amd64 "]),
    ]
    for group, key, good, bad in bounds:
        for value in good + bad:
            model = copy.deepcopy(BASE)
            model[group][key] = value
            add("bound-" + key, encode(model), model if value in good else None)
    for ch in list(range(32)) + [127, 128, 255]:
        add("raw-control", raw.replace(b"Offline Viewer", bytes([ch])))
    for escape in ["\\u0000", "\\u001b", "\\u007f", "\\u0080", "\\ud800", "\\udfff",
                   "\\ud800\\udc00", "\\n", "\\b", "\\f", "\\r", "\\t", "\\x41"]:
        add("decoded-control", raw.replace(b"Offline Viewer", escape.encode()))
    for value in [b"0", b"2", b"-1", b"+1", b"01", b"1.0", b"1e0", b"1" * 1000]:
        add("version", raw.replace(b'"schema_version":1', b'"schema_version":' + value))
    for data in [b"", b"{}", b"[]", b"null", b"\xef\xbb\xbf" + raw, raw + b"x",
                 raw + b"{}", b"// comment\n" + raw, raw[:-1] + b",}",
                 b"{" * 1000, b" " * 16384,
                 raw.replace(b"Offline Viewer", "caf\u00e9".encode())]:
        add("malformed", data)
    for end in range(len(raw)):
        add("truncation", raw[:end])
    for length in [16383, 16384, 16385]:
        add("input-bound", raw + b" " * (length - len(raw)), BASE if length <= 16384 else None)
    generator = random.Random(20261015)
    for _ in range(256):
        at = generator.randrange(len(raw) + 1)
        add("mutation", raw[:at] + bytes([generator.choice([0, 1, 27, 127, 128, 255])]) + raw[at:])
    return cases


def artifacts():
    cases = corpus()
    # Hex escapes keep untrusted fixture bytes out of C tokens and string syntax.
    lines = ["/* SPDX-License-Identifier: BSD-3-Clause */",
             "/* Generated synthetic fixtures; not an engine input contract. */",
             "struct native_case { const unsigned char *data; size_t length;",
             "    int status; const char *output; size_t output_length; };" ]
    outputs = list(dict.fromkeys(case[3] for case in cases))

    def literal(data):
        if not data:
            return '""'
        return "\n".join('"' + ''.join('\\x%02x' % ch for ch in data[at:at + 32]) + '"'
                         for at in range(0, len(data), 32))

    for index, output in enumerate(outputs):
        lines.append(f"static const char expected_{index}[] = {literal(output)};")
    manifest = []
    for index, (label, data, status, output) in enumerate(cases):
        values = ",\n".join(','.join(str(ch) for ch in data[at:at + 32])
                              for at in range(0, len(data), 32)) if data else '0'
        lines.append(f"static const unsigned char input_{index}[] = {{\n{values}\n}};")
        manifest.append({"index": index, "label": label, "input_bytes": len(data),
                         "input_sha256": hashlib.sha256(data).hexdigest(), "exit": status,
                         "output_bytes": len(output), "output_sha256": hashlib.sha256(output).hexdigest()})
    lines.append("static const struct native_case native_cases[] = {")
    for index, (_, data, status, output) in enumerate(cases):
        lines.append(f"    {{input_{index}, {len(data)}, {status}, expected_{outputs.index(output)}, {len(output)}}},")
    trace_index = next(i for i, case in enumerate(cases) if case[0] == "metacharacters")
    lines.extend(["};", f"#define NATIVE_CASE_COUNT {len(cases)}",
                  f"#define NATIVE_TRACE_CASE {trace_index}", ""])
    return {"native_cases.h": "\n".join(lines).encode(),
            "cases.json": (json.dumps(manifest, indent=2) + "\n").encode()}


def prepare():
    (ROOT / ".tools").mkdir(exist_ok=True)
    files = artifacts()
    names = ["src/main.c", "src/private_cli.h", "src/plan.c", "src/plan.h", "src/plan_io.c", "src/native.c",
             "tests/native_check.c", "tests/native_probe.c", "tests/native_faults.c", "tests/native_compile.c",
             "tests/plan_faults.c", "tests/plan_unit.c", "tests/native_build.sh",
             "tests/native_run.sh"]
    for name in names:
        files[Path(name).name] = (ROOT / name).read_bytes()
    files["SHA256"] = ''.join(f"SHA256 ({name}) = {hashlib.sha256(data).hexdigest()}\n"
                               for name, data in sorted(files.items())).encode()
    directory = Path(tempfile.mkdtemp(prefix="planner-preparation-", dir=ROOT / ".tools"))
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
