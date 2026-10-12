# SPDX-License-Identifier: BSD-3-Clause
"""Host-only saved-observation tests; no QEMU, target files or native runs."""
import copy
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
TOOL = ROOT / "tools/check-native-blocks.py"


def block(device, path, readonly):
    return {"device": device, "inserted": {
        "file": path, "ro": readonly, "drv": "raw", "backing_file_depth": 0,
        "image": {"filename": path, "format": "raw", "virtual-size": 8589934592},
    }}


class NativeBlockTests(unittest.TestCase):
    def setUp(self):
        self.reply = {"return": [block("media", "/synthetic/boot.iso", True),
                                  block("target", "/synthetic/target.raw", False)]}
        self.args = ["--expect-read-only", "media", "/synthetic/boot.iso",
                     "--expect-writable", "target", "/synthetic/target.raw"]

    def run_check(self, reply=None, args=None, raw=None):
        if raw is None:
            raw = json.dumps(self.reply if reply is None else reply).encode()
        return subprocess.run([sys.executable, str(TOOL),
                               *(self.args if args is None else args)],
                              input=raw, capture_output=True, timeout=3)

    def rejected(self, **kwargs):
        result = self.run_check(**kwargs)
        self.assertEqual(result.returncode, 2, result.stdout + result.stderr)
        self.assertEqual(result.stdout, b"")
        self.assertTrue(result.stderr.startswith(b"Native block observation rejected:"))
        self.assertNotIn(b"Traceback", result.stderr)
        return result

    def test_reviewed_raw_attachments_and_order(self):
        for reply in (self.reply, {"return": list(reversed(self.reply["return"])),
                                   "id": "query-block"}):
            result = self.run_check(reply=reply)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stdout,
                             b"Block observation matches 2 expected raw attachments.\n")
            self.assertEqual(result.stderr, b"")
        diskless = {"return": self.reply["return"][:1]}
        self.assertEqual(self.run_check(reply=diskless, args=self.args[:3]).returncode, 0)

    def test_target_path_access_format_and_backing_substitution(self):
        for key, values in (("file", ("/synthetic/other.raw", None, 5)),
                            ("ro", (True, "false", 0, None)),
                            ("drv", ("qcow2", "host_device", None)),
                            ("backing_file_depth", (1, -1, False, 0.0, "0", None))):
            for value in values:
                with self.subTest(key=key, value=value):
                    reply = copy.deepcopy(self.reply)
                    reply["return"][1]["inserted"][key] = value
                    self.rejected(reply=reply)
        reply = copy.deepcopy(self.reply)
        del reply["return"][1]["inserted"]["ro"]
        self.rejected(reply=reply)

    def test_missing_extra_duplicate_and_uninserted_devices(self):
        variants = [[], self.reply["return"][:1],
                    self.reply["return"] + [block("other", "/synthetic/other.raw", True)],
                    [self.reply["return"][0], self.reply["return"][0]],
                    [self.reply["return"][0], {"device": "target"}],
                    [self.reply["return"][0], {"device": []}],
                    [self.reply["return"][0], None]]
        for value in variants:
            with self.subTest(value=value):
                self.rejected(reply={"return": value})

    def test_inconsistent_image_and_backing_metadata(self):
        for key, value in (("filename", "/synthetic/other.raw"), ("format", "qcow2"),
                           ("backing-filename", ""), ("full-backing-filename", ""),
                           ("backing-filename-format", "raw"), ("backing-image", {})):
            reply = copy.deepcopy(self.reply)
            reply["return"][1]["inserted"]["image"][key] = value
            self.rejected(reply=reply)
        for change in ("missing-image", "missing-format", "backing-file"):
            reply = copy.deepcopy(self.reply)
            item = reply["return"][1]["inserted"]
            if change == "missing-image":
                del item["image"]
            elif change == "missing-format":
                del item["image"]["format"]
            else:
                item["backing_file"] = ""
            self.rejected(reply=reply)
        for children in (None, {}, [None], [{}], [{"child": "backing"}]):
            reply = copy.deepcopy(self.reply)
            reply["return"][1]["inserted"]["children"] = children
            self.rejected(reply=reply)
        reply = copy.deepcopy(self.reply)
        reply["return"][1]["inserted"]["children"] = [{"child": "file", "node-name": "file0"}]
        self.assertEqual(self.run_check(reply=reply).returncode, 0)

    def test_ambiguous_malformed_and_nonstandard_JSON(self):
        for raw in (b'{"return":[],"return":[]}',
                    b'{"return":[{"device":"target","device":"media"}]}',
                    b'{"return":[],"id":NaN}', b'{"return":[],"id":Infinity}',
                    b'{"return":[],"id":-Infinity}', b'\xff', b'{',
                    b'[' * 1500 + b']' * 1500):
            with self.subTest(raw=raw[:80]):
                self.rejected(raw=raw)
        for value in (None, [], {"return": {}}, {"error": {"desc": "private"}},
                      {"return": self.reply["return"], "error": {}}):
            self.rejected(raw=json.dumps(value).encode())

    def test_exact_input_size_boundary(self):
        raw = json.dumps(self.reply).encode()
        self.assertEqual(self.run_check(raw=raw + b" " * (65536 - len(raw))).returncode, 0)
        self.rejected(raw=raw + b" " * (65537 - len(raw)))

    def test_expected_inventory_requires_explicit_unique_paths_and_modes(self):
        variants = [[], self.args[:2], self.args + self.args[:3],
                    ["--expect-read-only", "bad\x1b", "/synthetic/boot.iso"],
                    ["--expect-read-only", "media", "relative.iso"],
                    ["--expect-read-only", "media", "/synthetic/\x1b.iso"],
                    ["--expect-read-only", "media", "/"],
                    ["--expect-read-only", "d" * 65, "/synthetic/boot.iso"],
                    ["--expect-read-only", "media", "/" + "a" * 4096],
                    self.args[:3] + ["--expect-writable", "target", "/synthetic/boot.iso"],
                    sum((["--expect-read-only", f"d{i}", f"/synthetic/{i}.raw"]
                         for i in range(9)), [])]
        for args in variants:
            with self.subTest(args=args):
                self.rejected(args=args)
        self.rejected(reply={"return": []}, args=[])
        args = sum((["--expect-read-only", f"d{i}", f"/synthetic/{i}.raw"]
                    for i in range(8)), [])
        reply = {"return": [block(f"d{i}", f"/synthetic/{i}.raw", True) for i in range(8)]}
        self.assertEqual(self.run_check(reply=reply, args=args).returncode, 0)
        path, device = "/" + "a" * 4095, "d" * 64
        self.assertEqual(self.run_check(reply={"return": [block(device, path, True)]},
                                       args=["--expect-read-only", device, path]).returncode, 0)

    def test_no_private_or_control_text_in_diagnostics_and_no_path_execution(self):
        private = "operator-private\x1b[2J"
        result = self.rejected(args=["--unknown", private])
        self.assertNotIn(private.encode(), result.stderr)
        reply = copy.deepcopy(self.reply)
        reply["return"][1]["inserted"]["file"] = "/" + private
        result = self.rejected(reply=reply)
        self.assertNotIn(private.encode(), result.stderr)
        self.assertNotIn(b"/synthetic", result.stdout + result.stderr)
        # A shell interpolation regression would create this owned canary.
        with tempfile.TemporaryDirectory(prefix="block-data-") as directory:
            canary = Path(directory) / "unexpected-effect"
            path = f"/synthetic/$(touch {canary});`touch {canary}`.raw"
            reply["return"][1] = block("target", path, False)
            args = self.args[:3] + ["--expect-writable", "target", path]
            self.assertEqual(self.run_check(reply=reply, args=args).returncode, 0)
            self.assertFalse(canary.exists())


if __name__ == "__main__":
    unittest.main()
