# SPDX-License-Identifier: BSD-3-Clause
"""Independent data oracles for unauthenticated content review.

Authored from the adopted byte/format contract before candidate execution. This
module uses no Bootweave imports, filesystem, subprocess, signer or archive
parser. Its small USTAR encoder constructs stimuli; it never decides acceptance.
Counts and stage expectations below are explicit, not obtained from a candidate.
Only the test fixture may link the returned synthetic const-policy definitions.
"""
import base64
import hashlib
import itertools
import json


KEY = (b"untrusted comment: Bootweave independent content corpus\n" +
       base64.b64encode(b"Ed" + bytes(40)) + b"\n")

REPORT_TEMPLATE = (
    "UNAUTHENTICATED CONTENT REVIEW - NO OPERATIONS AUTHORIZED\n"
    "Compiled policy: matched\n"
    "Definition SHA-256: matched compiled pin\n"
    "Payload SHA-256: matched definition\n"
    "Archive structure: complete restricted ustar\n"
    "Signature authentication: not established\n"
    "Project ID (policy-bound): {id}\n"
    "Project name (supplied): {name}\n"
    "Requested release (policy-bound): {release}\n"
    "Requested architecture (policy-bound): {architecture}\n"
    "Payload file (supplied label): {file}\n"
    "Archive members: {members}\n"
    "Regular files: {files}\n"
    "Content bytes: {content_bytes}\n"
    "Target: unselected; no device identity collected\n"
    "Operations authorized: none\n"
    "Operations performed: none\n"
    "Operations independently observed: none\n"
)

# All21 unordered relationships. Labels name storage spans, not trusted tokens.
ALIAS_PAIRS = tuple(itertools.combinations(
    ("request", "output", "result", "policy", "key", "definition", "payload"), 2))
ALIAS_EXPECTATION = {"return": 0, "output_disposition": "unchanged",
                     "result_disposition": "unchanged",
                     "calls": {"policy": 0, "hash": 0, "inspect": 0, "format": 0}}


def _octal(value, width):
    text = format(value, "o").encode("ascii")
    if len(text) >= width:
        raise ValueError("independent stimulus field too wide")
    return text.rjust(width - 1, b"0") + b"\0"


def _checksum(data):
    """Update only the first512-byte header, never its body/padding."""
    data[148:156] = b" " * 8
    data[148:156] = format(sum(data[:512]), "06o").encode("ascii") + b"\0 "


def _member(name, body=b"", kind=b"0", prefix="", mode=None, uid=0, gid=0):
    name, prefix = name.encode("ascii"), prefix.encode("ascii")
    if not 0 < len(name) <= 100 or len(prefix) > 155 or len(kind) != 1:
        raise ValueError("independent USTAR stimulus field width")
    header = bytearray(512)
    header[:len(name)] = name
    header[100:108] = _octal((0o755 if kind == b"5" else 0o644)
                            if mode is None else mode, 8)
    header[108:116] = _octal(uid, 8)
    header[116:124] = _octal(gid, 8)
    header[124:136] = _octal(len(body), 12)
    header[136:148] = _octal(0, 12)
    header[156:157] = kind
    header[257:263] = b"ustar\0"
    header[263:265] = b"00"
    header[345:345 + len(prefix)] = prefix
    _checksum(header)
    return bytes(header) + body + bytes((-len(body)) % 512)


def _archive(*members):
    return b"".join(members) + bytes(1024)


def _alter(member, offset, replacement, repair=True):
    data = bytearray(member)
    if not 0 <= offset <= len(data) - len(replacement):
        raise ValueError("independent mutation out of range")
    data[offset:offset + len(replacement)] = replacement
    if repair:
        _checksum(data)
    return bytes(data)


def _definition(payload, project_id="offline-viewer", name="Offline Viewer",
                filename="viewer.tar", release="7.9", architecture="amd64"):
    model = {"schema_version": 1,
             "project": {"id": project_id, "name": name},
             "system": {"release": release, "architecture": architecture},
             "payload": {"file": filename,
                         "sha256": hashlib.sha256(payload).hexdigest()}}
    raw = json.dumps(model, ensure_ascii=True, separators=(",", ":")).encode("ascii") + b"\n"
    plan = {"id": project_id, "name": name, "release": release,
            "architecture": architecture, "file": filename,
            "sha256": model["payload"]["sha256"]}
    return raw, plan


def _policy(definition, project_id="offline-viewer"):
    return {"profile": 1, "key": KEY, "key_length": len(KEY),
            "digest": hashlib.sha256(definition).digest(),
            "id": project_id.encode("ascii"), "release": b"7.9", "arch": b"amd64"}


def _refusal(cause, policy_cause="NONE", policy=1, hashes=2, inspect=0,
             format=0, clear_output=True):
    return {"return": 0, "cause": cause, "policy_cause": policy_cause,
            "report": b"", "length": 0, "model": None, "summary": None,
            "cleared": True,
            "output_disposition": "zeroed" if clear_output else "unchanged",
            "result_disposition": "zeroed_except_causes",
            "calls": {"policy": policy, "hash": hashes,
                      "inspect": inspect, "format": format}}


def _valid(label, payload, summary, project_id="offline-viewer",
           name="Offline Viewer", filename="viewer.tar"):
    definition, plan = _definition(payload, project_id, name, filename)
    report = REPORT_TEMPLATE.format(**plan, **summary).encode("ascii")
    return {"name": label, "key": KEY, "definition": definition,
            "payload": payload, "policy": _policy(definition, project_id),
            "expected": {"return": 1, "cause": "NONE", "policy_cause": "NONE",
                         "report": report, "length": len(report), "model": plan,
                         "summary": summary, "cleared": False,
                         "output_disposition": "complete_text_then_NUL",
                         "result_disposition": "complete_model",
                         "calls": {"policy": 1, "hash": 2, "inspect": 1, "format": 1}}}


def valid_cases():
    """Nine successes with independently specified counts, not parsed counts."""
    viewer = _archive(_member("viewer", kind=b"5"),
                      _member("viewer/readme.txt", b"offline viewer\n"))
    maximum_name = 'Literal %n %s ${HOME} $(id); <>&| "quotes" \\ '.ljust(80, "x")
    long_prefix = "a" * 77 + "/" + "b" * 76
    long_path_member = _member("c" * 100, b"P", prefix=long_prefix)
    depth16_member = _member("/".join(["d"] * 15 + ["note"]), b"D")
    one_empty = _archive(_member("empty.dat"))
    return (
        _valid("viewer", viewer, {"members": 2, "files": 1, "content_bytes": 15}),
        _valid("dns", _archive(_member("etc/resolver.conf", b"dns")),
               {"members": 1, "files": 1, "content_bytes": 3},
               "sample-dns", "Sample DNS", "resolver.tar"),
        _valid("firewall", _archive(_member("rules", kind=b"5"),
                                     _member("rules/policy.txt", b"block\n"),
                                     _member("rules/empty")),
               {"members": 3, "files": 2, "content_bytes": 6},
               "sample-firewall", "Sample Firewall", "rules.tar"),
        _valid("zero-length-file", one_empty,
               {"members": 1, "files": 1, "content_bytes": 0}, "empty-viewer"),
        _valid("maximal-fields-literal-metacharacters", viewer,
               {"members": 2, "files": 1, "content_bytes": 15},
               "v" + "a" * 63, maximum_name, "p" + "x" * 123 + ".tar"),
        _valid("exact-file-and-content-caps",
               _archive(_member("a.bin", bytes((0, 255)) * 131072),
                        _member("b.bin", b"b" * 262144)),
               {"members": 2, "files": 2, "content_bytes": 524288}),
        _valid("exact-member-cap", _archive(*(_member("m%03d" % n) for n in range(128))),
               {"members": 128, "files": 128, "content_bytes": 0}),
        _valid("exact-path-and-depth-caps", _archive(long_path_member, depth16_member),
               {"members": 2, "files": 2, "content_bytes": 2}),
        _valid("exact-archive-cap", one_empty + bytes(1048576 - len(one_empty)),
               {"members": 1, "files": 1, "content_bytes": 0}),
    )


def structure_cases():
    """Every archive is pinned exactly; no pin is a signature or safety verdict."""
    good = _member("good.txt", b"x")
    stimuli = (
        ("no-file-inventory", _archive()),
        ("directory-only", _archive(_member("only", kind=b"5"))),
        ("late-checksum", _archive(good, _alter(_member("last.txt"), 0, b"X", False))),
        ("nonzero-content-padding", _archive(_alter(good, 513, b"x"))),
        ("single-trailer-block", good + bytes(512)),
        ("member-after-trailer", _archive(good) + _member("late.txt")),
        ("partial-trailing-byte", _archive(good) + b"x"),
        ("symbolic-link", _archive(_member("link", kind=b"2"))),
        ("hard-link", _archive(_member("link", kind=b"1"))),
        ("device-type", _archive(_member("device", kind=b"3"))),
        ("fifo-type", _archive(_member("fifo", kind=b"6"))),
        ("extension-type", _archive(_member("extension", kind=b"x"))),
        ("file-mode", _archive(_member("mode", mode=0o755))),
        ("numeric-uid", _archive(_member("owner", uid=1))),
        ("numeric-gid", _archive(_member("group", gid=1))),
        ("owner-name", _archive(_alter(_member("owner"), 265, b"root"))),
        ("group-name", _archive(_alter(_member("group"), 297, b"wheel"))),
        ("parent-traversal", _archive(_member("../bad"))),
        ("absolute-path", _archive(_member("/bad"))),
        ("empty-path-component", _archive(_member("a//b"))),
        ("depth-over-cap", _archive(_member("/".join(["d"] * 17)))),
        ("duplicate-file", _archive(_member("same"), _member("same"))),
        ("directory-wire-alias", _archive(_member("d", kind=b"5"),
                                          _member("d/", kind=b"5"), good)),
        ("prefix-wire-alias", _archive(_member("a/b"), _member("b", prefix="a"))),
        ("file-as-ancestor", _archive(_member("a"), _member("a/b"))),
        ("late-file-parent", _archive(_member("a/b"), _member("a"))),
        ("member-over-cap", _archive(*(_member("m%03d" % n) for n in range(129)))),
        ("file-over-cap", _archive(_member("large", b"x" * 262145))),
        ("content-over-cap", _archive(_member("a", b"x" * 262144),
                                      _member("b", b"x" * 262144), _member("c", b"x"))),
        ("hidden-name-tail", _archive(_alter(_member("ab"), 3, b"hidden"))),
        ("invalid-size-numeric", _archive(_alter(good, 124, b"-0000000001"))),
        ("wrong-magic", _archive(_alter(good, 257, b"notar\0"))),
        ("directory-with-data", _archive(_member("dir", b"x", kind=b"5"), good)),
    )
    result = []
    for label, payload in stimuli:
        definition, _ = _definition(payload)
        result.append({"name": label, "key": KEY, "definition": definition,
                       "payload": payload, "policy": _policy(definition),
                       "expected": _refusal("STRUCTURE", inspect=1)})
    return tuple(result)


def policy_cases():
    """Fixed order: preflight/policy refusals must not reach real inspection."""
    baseline = valid_cases()[0]
    result = []

    def changed(label, cause, hashes, *, key=None, definition=None, payload=None,
                pinned=None, stage="POLICY", policy_calls=1, hash_fail_at=0):
        result.append({"name": label, "key": baseline["key"] if key is None else key,
                       "definition": baseline["definition"] if definition is None else definition,
                       "payload": baseline["payload"] if payload is None else payload,
                       "policy": dict(baseline["policy"] if pinned is None else pinned),
                       "hash_fail_at": hash_fail_at,
                       "expected": _refusal(stage, cause, policy=policy_calls, hashes=hashes)})

    changed("wrong-key", "KEY", 0, key=KEY.replace(b"corpus", b"Corpus"))
    changed("definition-whitespace-old-pin", "DEFINITION", 1,
            definition=b" " + baseline["definition"])
    changed("payload-byte-before-inspection", "PAYLOAD", 2,
            payload=b"X" + baseline["payload"][1:])
    changed("payload-truncated-suffix-before-inspection", "PAYLOAD", 2,
            payload=baseline["payload"][:-1])
    changed("payload-zero-extension-before-inspection", "PAYLOAD", 2,
            payload=baseline["payload"] + b"\0")
    definition, _ = _definition(baseline["payload"], project_id="other-viewer")
    changed("different-project-tuple", "TUPLE", 1,
            definition=definition, pinned=_policy(definition))
    for label, name in [("escape", "bad\x1b"), ("csi", "bad\x1b[2J"),
                        ("osc", "bad\x1b]0;title\x07"), ("nul", "bad\0"),
                        ("non-ascii", "caf\u00e9"), ("newline", "bad\nline")]:
        definition, _ = _definition(baseline["payload"], name=name)
        changed("metadata-" + label, "SYNTAX", 1,
                definition=definition, pinned=_policy(definition))
    for label, definition in [
            ("duplicate-field", baseline["definition"].replace(
                b'"schema_version":1', b'"schema_version":1,"schema_version":1')),
            ("unknown-field", baseline["definition"].replace(b'"name":', b'"unknown":')),
            ("definition-nul-suffix", baseline["definition"] + b"\0")]:
        changed(label, "SYNTAX", 1, definition=definition, pinned=_policy(definition))
    definition, _ = _definition(baseline["payload"], release="7.8")
    changed("unsupported-requested-release", "SYNTAX", 1,
            definition=definition, pinned=_policy(definition))
    changed("invalid-compiled-profile", "POLICY", 0,
            pinned=dict(baseline["policy"], profile=2))
    changed("invalid-compiled-key-format", "POLICY", 0,
            pinned=dict(baseline["policy"], key=b"not a key", key_length=9))
    changed("hash-adapter-first-refusal", "HASH_UNAVAILABLE", 1, hash_fail_at=1)
    changed("hash-adapter-second-refusal", "HASH_UNAVAILABLE", 2, hash_fail_at=2)
    changed("empty-key-preflight", "NONE", 0, key=b"", stage="ARGUMENT", policy_calls=0)
    changed("empty-definition-preflight", "NONE", 0, definition=b"",
            stage="ARGUMENT", policy_calls=0)
    changed("empty-payload-preflight", "NONE", 0, payload=b"",
            stage="ARGUMENT", policy_calls=0)
    changed("key-over-cap-preflight", "NONE", 0, key=b"k" * 1025,
            stage="ARGUMENT", policy_calls=0)
    changed("definition-over-cap-preflight", "NONE", 0, definition=b" " * 16385,
            stage="ARGUMENT", policy_calls=0)
    changed("payload-over-cap-preflight", "NONE", 0, payload=bytes(1048577),
            stage="ARGUMENT", policy_calls=0)
    return tuple(result)


def formatting_cases():
    """Capacity includes NUL; these are driver requests, never a policy upgrade."""
    vector = valid_cases()[0]
    length = vector["expected"]["length"]
    return (
        {"name": "exact-capacity", "capacity": length + 1, "fault": "none",
         "expected": vector["expected"]},
        {"name": "one-short", "capacity": length, "fault": "none",
         "expected": _refusal("FORMAT", inspect=1, format=1)},
        {"name": "zero-capacity", "capacity": 0, "fault": "none",
         "expected": _refusal("ARGUMENT", policy=0, hashes=0, clear_output=False)},
        {"name": "over-capacity", "capacity": 65537, "fault": "none",
         "expected": _refusal("ARGUMENT", policy=0, hashes=0, clear_output=False)},
        {"name": "format-negative", "capacity": 2048, "fault": "negative",
         "expected": _refusal("FORMAT", inspect=1, format=1)},
        {"name": "format-truncated", "capacity": 2048, "fault": "truncated",
         "expected": _refusal("FORMAT", inspect=1, format=1)},
    )
