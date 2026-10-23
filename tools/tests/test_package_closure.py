# SPDX-License-Identifier: BSD-3-Clause
import importlib.util
import io
from pathlib import Path
import tarfile
import unittest

spec = importlib.util.spec_from_file_location("package_closure", Path(__file__).resolve().parents[1]/"package_closure.py")
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


def package(name, dependency="", arch="amd64", data=b"inert product bytes"):
    output = io.BytesIO()
    with tarfile.open(fileobj=output, mode="w:gz") as archive:
        contents = ("@name "+name+"\n@arch "+arch+"\n"+dependency).encode()
        for filename, raw in [("+CONTENTS", contents), ("bin/inert", data)]:
            member = tarfile.TarInfo(filename)
            member.size = len(raw)
            archive.addfile(member, io.BytesIO(raw))
    return output.getvalue()


class PackageClosureTests(unittest.TestCase):
    def inputs(self):
        return {"archive-indexer-0.0.tgz": package("archive-indexer-0.0", "@depend local/libsample:=:libsample-3.2\n"),
                "libsample-3.2.tgz": package("libsample-3.2")}

    def test_measures_exact_supplied_closure_without_extraction(self):
        data = self.inputs()
        result = module.closure(data, "amd64", "archive-indexer-0.0.tgz")
        self.assertEqual(result["dependency_order"], ["libsample-3.2.tgz", "archive-indexer-0.0.tgz"])
        self.assertEqual(result["archive_bytes"], sum(map(len, data.values())))
        self.assertEqual(result["regular_member_bytes"], 2*len(b"inert product bytes"))
        self.assertIsNone(result["installed_allocation_bytes"])

    def test_refuses_incomplete_wrong_arch_and_unrelated_packages(self):
        data = self.inputs()
        for changed, arch in [({"archive-indexer-0.0.tgz": data["archive-indexer-0.0.tgz"]}, "amd64"),
                              (data, "arm64"), ({**data, "extra-1.tgz": package("extra-1")}, "amd64"),
                              ({**data, "libsample-3.2.tgz": package("other-1")}, "amd64"),
                              ({"archive-indexer-0.0.tgz": b"not an archive"}, "amd64")]:
            with self.assertRaises(ValueError):
                module.closure(changed, arch, "archive-indexer-0.0.tgz")

    def test_refuses_cycle(self):
        data = self.inputs()
        data["libsample-3.2.tgz"] = package("libsample-3.2", "@depend local/archive-indexer:=:archive-indexer-0.0\n")
        with self.assertRaises(ValueError):
            module.closure(data, "amd64", "archive-indexer-0.0.tgz")

    def test_arm64_accepts_native_machine_and_machine_arch_labels(self):
        for arch in ("arm64", "aarch64", "aarch64,amd64", "*"):
            with self.subTest(arch=arch):
                data = {"archive-indexer-0.0.tgz": package("archive-indexer-0.0", arch=arch)}
                result = module.closure(data, "arm64", "archive-indexer-0.0.tgz")
                self.assertEqual(result["package_architecture"], "aarch64")
                self.assertEqual(result["packages"]["archive-indexer-0.0.tgz"]["declared_architecture"], arch)
        for arch, media in (("amd64", "arm64"), ("aarch64", "amd64"), ("arm64", "amd64")):
            with self.subTest(arch=arch, media=media), self.assertRaises(ValueError):
                module.closure({"archive-indexer-0.0.tgz": package("archive-indexer-0.0", arch=arch)},
                               media, "archive-indexer-0.0.tgz")

    def test_refuses_bad_gzip_footer_and_member_aliases(self):
        data=self.inputs()
        raw=data["libsample-3.2.tgz"]
        changed=bytearray(raw); changed[-8]^=1
        for bad in (raw[:-4],bytes(changed)):
            with self.assertRaises(ValueError):
                module.closure({**data,"libsample-3.2.tgz":bad},"amd64","archive-indexer-0.0.tgz")
        output=io.BytesIO()
        with tarfile.open(fileobj=output,mode="w:gz") as archive:
            for name,body in [("+CONTENTS",b"@name libsample-3.2\n@arch amd64\n"),
                              ("bin/x",b"x"),("bin/./x",b"y")]:
                item=tarfile.TarInfo(name); item.size=len(body); archive.addfile(item,io.BytesIO(body))
        with self.assertRaises(ValueError):
            module.closure({**data,"libsample-3.2.tgz":output.getvalue()},"amd64","archive-indexer-0.0.tgz")
