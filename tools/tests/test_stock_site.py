# SPDX-License-Identifier: BSD-3-Clause
"""Synthetic package bytes only. No package, installer, signer or native runs."""
import hashlib
import importlib.util
import io
from pathlib import Path
import re
import subprocess
import tarfile
import unittest

from tools.tests.test_package_closure import package

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location("stock_site", ROOT / "tools/stock_site.py")
stock = importlib.util.module_from_spec(spec)
spec.loader.exec_module(stock)


class StockSiteTests(unittest.TestCase):
    def inputs(self):
        data = {"libsample-1.0.tgz": package("libsample-1.0"),
                "archive-indexer-1.0.tgz": package("archive-indexer-1.0", "@depend local/libsample:=:libsample-1.0\n")}
        return data, {n: hashlib.sha256(v).hexdigest() for n, v in data.items()}, {
            "libsample-1.0.tgz": "openbsd", "archive-indexer-1.0.tgz": "consumer"}

    def test_measured_deterministic_stock_bootstrap(self):
        inputs = self.inputs()
        blob, report = stock.site_set(*inputs, architecture="amd64", root_package="archive-indexer-1.0.tgz")
        self.assertEqual(blob, stock.site_set(*inputs, architecture="amd64", root_package="archive-indexer-1.0.tgz")[0])
        self.assertEqual(report["package_bytes"], sum(map(len, inputs[0].values())))
        self.assertEqual(report["site_archive_bytes"], len(blob))
        self.assertIsNone(report["installed_unpacked_bytes"])
        with tarfile.open(fileobj=io.BytesIO(blob), mode="r:gz") as archive:
            script = archive.extractfile("install.site").read().decode()
            self.assertTrue(script.endswith("exit 77\n"))
            self.assertIn("/usr/sbin/pkg_add -I '/var/db/bootweave/packages/archive-indexer-1.0.tgz' '/var/db/bootweave/packages/libsample-1.0.tgz'", script)
            self.assertIn("Package-only installation.", script)
            self.assertNotIn("@PACKAGES@", script)
            self.assertNotIn("@SETUP_", script)
            self.assertEqual(archive.getmember("install.site").mode, 0o500)
            for name, data in inputs[0].items():
                self.assertEqual(archive.extractfile("var/db/bootweave/packages/"+name).read(), data)

    def test_refuses_injection_changed_and_incomplete_inventory(self):
        for name in ("../x.tgz", "x;id.tgz", "x'bad.tgz", "https://x/x.tgz", "-x.tgz", "x y.tgz"):
            with self.assertRaises(ValueError):
                stock.site_set({name: b"x"}, {name: hashlib.sha256(b"x").hexdigest()}, {name: "consumer"}, architecture="amd64", root_package=name)
        data, hashes, authority = self.inputs()
        for changed in ({}, {**hashes, "archive-indexer-1.0.tgz": "0"*64}):
            with self.assertRaises(ValueError):
                stock.site_set(data, changed, authority, architecture="amd64", root_package="archive-indexer-1.0.tgz")
        with self.assertRaises(ValueError):
            stock.site_set(data, hashes, {k: "trusted-because-caller-said-so" for k in data}, architecture="amd64", root_package="archive-indexer-1.0.tgz")

    def setup(self):
        return {"package": "archive-indexer-1.0.tgz",
                "path": "/usr/local/libexec/archive-indexer/setup",
                "sha256": "a"*64, "arguments": ["initialize"]}

    def test_unrelated_consumer_setup_is_digest_bound_and_literal(self):
        data, hashes, trust = self.inputs()
        setup = self.setup()
        setup["arguments"] = ["$(id)", "a'b", ";echo", "`id`", "--prepare"]
        blob, report = stock.site_set(data, hashes, trust, architecture="amd64",
                                     root_package=setup["package"], setup=setup)
        self.assertEqual(report["setup"], setup)
        with tarfile.open(fileobj=io.BytesIO(blob), mode="r:gz") as archive:
            script = archive.extractfile("install.site").read().decode()
        self.assertIn('/bin/sha256 -q "$setup_path"', script)
        self.assertIn("0:0:500:1|0:0:555:1", script)
        self.assertNotIn("@SETUP_", script)
        syntax = subprocess.run(["/bin/sh", "-n"], input=script, text=True,
                                capture_output=True, timeout=2, check=False)
        self.assertEqual((syntax.returncode, syntax.stderr), (0, ""))
        # Execute only the rendered argv with a builtin receiver. No target
        # command, stat, hash tool or environment loader can run in this test.
        literals = stock.setup_literals(setup, data, trust)
        body = "capture() { printf '%s\\n' \"$@\"; }\n" + "capture " + literals["@SETUP_PATH@"] + " " + literals["@SETUP_ARGUMENTS@"] + "\n"
        result = subprocess.run(["/bin/sh", "-s"], input=body, text=True,
                                capture_output=True, timeout=2, check=False)
        self.assertEqual((result.returncode, result.stdout.splitlines(), result.stderr),
                         (0, [setup["path"]]+setup["arguments"], ""))

    def test_invalid_extension_refuses_before_archive_creation(self):
        bad = [dict(self.setup(), extra=True), dict(self.setup(), package="libsample-1.0.tgz"),
               dict(self.setup(), sha256="0"*63), dict(self.setup(), arguments=["x"]*9),
               dict(self.setup(), arguments=["x"*129]), dict(self.setup(), arguments=["a\nb"]),
               dict(self.setup(), arguments="initialize")]
        for path in ("/bin/sh", "/usr/local/libexec/../bin/x", "/usr/local/libexec//x",
                     "/usr/local/libexec/x';id", "/usr/local/libexec/./x"):
            bad.append(dict(self.setup(), path=path))
        for setup in bad:
            with self.subTest(setup=setup), self.assertRaises(ValueError):
                stock.site_set(*self.inputs(), architecture="amd64",
                               root_package="archive-indexer-1.0.tgz", setup=setup)

    def test_actual_bootstrap_control_flow_stops_without_retry(self):
        # Test-copy only: replace EVERY effectful function with fixed shell
        # builtins. The real coordinator body runs; no package/OS/root function
        # can execute. This is not a provider or switch in the installed entry.
        source = stock.TEMPLATE.read_text()
        self.assertTrue(source.endswith("exit 77\n"))
        body = source[:-len("exit 77\n")] + '''
fault=$1
bw_prepare() { printf 'prepare\\n'; if [ "$fault" = prepare ]; then return 1; fi; }
bw_packages() { printf 'packages\\n'; if [ "$fault" = packages ]; then return 1; fi; }
bw_setup() { printf 'setup\\n'; if [ "$fault" = setup ]; then return 1; fi; }
bw_log() { printf 'log:%s:%s\\n' "$1" "$2"; }
bw_bootstrap
'''
        prefix = "prepare\nlog:packages:started\npackages\n"
        expected = {
            "prepare": (1, "prepare\nlog:prepare:failed\n"),
            "packages": (1, prefix+"log:packages:failed\n"),
            "setup": (1, prefix+"log:setup:started\nsetup\nlog:setup:failed\n"),
            "none": (0, prefix+"log:setup:started\nsetup\nlog:setup:completed\n"),
        }
        for fault, (code, output) in expected.items():
            result = subprocess.run(["/bin/sh", "-s", "--", fault], input=body,
                                    env={"PATH": "/usr/bin:/bin", "LC_ALL": "C"},
                                    text=True, capture_output=True, timeout=2, check=False)
            self.assertEqual((result.returncode, result.stdout, result.stderr), (code, output, ""))

    def test_installed_entry_cannot_activate(self):
        result = subprocess.run(["/bin/sh", str(stock.TEMPLATE)], capture_output=True,
                                env={"PATH": "/usr/bin:/bin", "LC_ALL": "C"}, timeout=2, check=False)
        self.assertEqual((result.returncode, result.stdout, result.stderr), (77, b"", b""))

    def test_runtime_source_still_fits_approved_review_envelope(self):
        c = sum(p.stat().st_size for p in (ROOT/"src").rglob("*") if p.suffix in (".c", ".h"))
        shell = sum(p.stat().st_size for p in (ROOT/"installer/stock").rglob("*") if p.is_file())
        self.assertLessEqual(c + shell, 256*1024)
        entry = ["trusted_start.c", "trusted_start.h", "entry_platform.c", "entry_native.c",
                 "entry_openbsd.c", "entry_cli.c", "private_entry.h", "private_cli.h",
                 "private_limits.h", "private_start_material.h", "main.c", "install_entry.c",
                 "install_platform.c", "private_install.h", "install_profile.h"]
        # Charge the retained controller's private gate only when this exact
        # source includes it. A required missing header still fails stat below.
        if re.search(r'^\s*#\s*include\s*"private_install_gate\.h"',
                     (ROOT/"src/install_platform.c").read_text(), re.MULTILINE):
            entry.append("private_install_gate.h")
        self.assertLessEqual(sum((ROOT/"src"/n).stat().st_size for n in entry) + shell, 24*1024)
