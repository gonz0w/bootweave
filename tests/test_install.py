# SPDX-License-Identifier: BSD-3-Clause
"""Actual installer compiler/coordinator; private synthetic boundary, no native authority."""
import copy
import hashlib
import json
from pathlib import Path
import tempfile
import time
import unittest
import test_policy

ROOT = Path(__file__).resolve().parents[1]
BASE = {
    "version": 2, "project": "viewer", "release": "8.0", "architecture": "amd64",
    "firmware": "uefi", "handoff": "single-stopped",
    "artifacts": [{"id": "media", "kind": "media", "file": "install80.iso", "sha256": "1"*64},
                  {"id": "base", "kind": "set", "file": "base80.tgz", "sha256": "2"*64},
                  {"id": "app", "kind": "payload", "file": "viewer.tgz", "sha256": "3"*64}],
    "principals": [{"name": "_viewer"}],
    "files": [{"artifact": "app", "member": "bin/viewer", "target": "/usr/local/libexec/viewer",
               "owner": "root", "group": "wheel", "mode": "0500"}],
    "inputs": [{"name": "site", "target": "/etc/viewer/private/site", "group": "_viewer", "mode": "0440"}],
    "services": [{"name": "viewer", "entry": "/usr/local/libexec/viewer", "template": "owned-foreground"}],
    "policy": {"management": "site-input", "ssh": "disabled", "network": "product-input"},
    "observability": {"log": "local-bounded", "health": "product-owned"}}
# Literal source-independent required effect order; not derived from an observed run.
STEPS = ["1.0", "2.0", "3.0", "4.0", "5.0", "6.0", "7.0", "8.0", "9.0", "10.0"]


def encoded(p):
    return json.dumps(p, separators=(",", ":")).encode("ascii")


class InstallerTests(unittest.TestCase):
    @classmethod
    def execute(cls, command, **kwargs):
        if time.monotonic()-cls.origin > 240 or cls.quarantine:
            raise AssertionError("source run admission closed; retain unresolved ownership")
        return test_policy.PolicyTests.execute.__func__(cls, command, **kwargs)

    @classmethod
    def setUpClass(cls):
        cls.origin = time.monotonic()
        cls.directory = Path(tempfile.mkdtemp(prefix="installer-source-retained-", dir=ROOT/".tools"))
        cls.ledger = cls.directory/"observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1",
                   "ASAN_OPTIONS": "detect_leaks=0:abort_on_error=1"}
        cls.calls = cls.builds = 0
        cls.cache, cls.quarantine = {}, []
        cls.flags = ["/Library/Developer/CommandLineTools/usr/bin/clang", "-isysroot", "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
                     "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g", "-I", str(ROOT/"src")]
        src = [str(ROOT/"src"/n) for n in ("install_profile.c", "install_plan.c", "install_executor.c", "install_entry.c")]
        cls.driver, cls.normal = cls.directory/"modeled", cls.directory/"normal"
        for binary, extra in [(cls.driver, []), (cls.normal, ["-DBWI_NORMAL", str(ROOT/"src/install_platform.c")])]:
            cls.execute(cls.flags+extra+src+[str(ROOT/"tests/install_driver.c"), "-o", str(binary)], compile_job=True)
        paths = [*ROOT.joinpath("src").glob("*.[ch]"),ROOT/"tests/install_driver.c",Path(__file__),ROOT/"tests/test_policy.py"]
        manifest = {str(p.relative_to(ROOT)): {"bytes":p.stat().st_size,"sha256":hashlib.sha256(p.read_bytes()).hexdigest()} for p in paths}
        (cls.directory/"source-manifest.json").write_text(json.dumps(manifest,sort_keys=True)+"\n")

    def run_mode(self, mode, model=BASE, faults=("-",0,1,"-",0,1), raw=None, normal=False):
        args = [str(self.normal if normal else self.driver),mode]
        if not normal: args += list(map(str,faults))
        return self.execute(args, data=encoded(model) if raw is None else raw).decode("ascii")

    def test_four_products_and_independence(self):
        for product in ("viewer","aegvium","nomvium","arxwall","vectivium"):
            p=json.loads(encoded(BASE).decode().replace("viewer",product))
            if product=="aegvium": p["handoff"]="paired-never-started"
            self.assertEqual(self.run_mode("parse",p),"PLAN "+" ".join(STEPS)+"\n")
            text=self.run_mode("render",p)
            self.assertTrue(text.startswith("UNVERIFIED INSTALLER EFFECT PLAN - NO OPERATIONS AUTHORIZED\n"))
            self.assertIn("root:wheel 0500 exclusive",text)
            self.assertIn("root:_"+product+" 0440; confidential bytes separately bound",text)
            self.assertIn("not traffic readiness",text)
            self.assertNotIn("signature accepted",text)

    def test_structural_and_complete_preflight_rejections(self):
        bad=[]
        for field in BASE:
            p=copy.deepcopy(BASE);del p[field];bad.append(encoded(p))
            p=copy.deepcopy(BASE);p[field]=None;bad.append(encoded(p))
        raw=encoded(BASE)
        bad += [raw.replace(b'"version":2',b'"version":2,"version":2'),raw+b'{}',raw[:-1]+b',}',
                raw.replace(b'"version":2',b'"version":2.0'),raw.replace(b'"version":2',b'"version":20'),
                raw.replace(b'"project"',b'"\\u0070roject"'),raw.replace(b'"viewer"',b'"view\\ner"',1),
                raw.replace(b'"project":"viewer"',b'"project":"../viewer"'),b'{'*65536,raw+b' '*(65537-len(raw))]
        for data in bad: self.assertEqual(self.run_mode("parse",raw=data),"REJECTED-UNCHANGED\n")
        self.assertEqual(self.run_mode("prefixes"),"PREFIXES-REJECTED-UNCHANGED\n")
        self.assertTrue(self.run_mode("parse",raw=raw+b' '*(65536-len(raw))).startswith("PLAN "))

    def test_closed_row_schemas_and_pointer_canaries(self):
        cases=[]
        for field in ("artifacts","principals","files","inputs","services","policy","observability"):
            row=BASE[field][0] if isinstance(BASE[field],list) else BASE[field]
            for key in row:
                for value in (None,):
                    p=copy.deepcopy(BASE);r=p[field][0] if isinstance(p[field],list) else p[field]
                    r[key]=value;cases.append(encoded(p))
                p=copy.deepcopy(BASE);r=p[field][0] if isinstance(p[field],list) else p[field]
                del r[key];cases.append(encoded(p))
                old=encoded(row);needle=json.dumps(key).encode()+b":"
                duplicate=old.replace(needle,needle+encoded(row[key])+b","+needle,1)
                cases.append(encoded(BASE).replace(old,duplicate,1))
            p=copy.deepcopy(BASE);r=p[field][0] if isinstance(p[field],list) else p[field]
            r["unknown"]="unsupported";cases.append(encoded(p))
        for data in cases:self.assertEqual(self.run_mode("parse",raw=data),"REJECTED-UNCHANGED\n")
        self.assertEqual(self.run_mode("canaries"),"CANARIES-REFUSED\n")

    def test_unsafe_paths_roles_modes_and_service_refuse(self):
        cases=[]
        for value in ("/etc/passwd","/etc/viewer/../passwd","/etc//viewer/x","/etc/viewer/x/","/etc/viewer/.","/etc/viewer/x;id"):
            p=copy.deepcopy(BASE);p["files"][0]["target"]=value;cases.append(p)
        for field,values in {"owner":["nobody","_other"],"group":["root","_other"],"mode":["4755","0777","0755"],
                             "member":["/bin/x","../x","a/../b"],"artifact":["missing","base"]}.items():
            for value in values:
                p=copy.deepcopy(BASE);p["files"][0][field]=value;cases.append(p)
        for field,value in [("template","shell"),("entry","/bin/sh"),("name","sshd")]:
            p=copy.deepcopy(BASE);p["services"][0][field]=value;cases.append(p)
        for field,value in [("mode","0644"),("target","/etc/viewer/x"),("group","_unknown")]:
            p=copy.deepcopy(BASE);p["inputs"][0][field]=value;cases.append(p)
        p=copy.deepcopy(BASE);p["principals"][0]["name"]="_viewerother";cases.append(p)
        p=copy.deepcopy(BASE);p["files"] += [dict(p["files"][0],member="other",target="/usr/local/libexec/viewer/sub")];cases.append(p)
        p=copy.deepcopy(BASE);p["inputs"][0]["target"]="/etc/viewer/private/site";p["files"][0]["target"]="/etc/viewer/private";cases.append(p)
        p=copy.deepcopy(BASE);p["artifacts"] += [p["artifacts"][0]];cases.append(p)
        p=copy.deepcopy(BASE);p["policy"]["network"]="raw-pf";cases.append(p)
        for p in cases: self.assertEqual(self.run_mode("parse",p),"REJECTED-UNCHANGED\n")
        for mode in ("short","forged"):self.assertEqual(self.run_mode(mode),"OUTPUT-UNCHANGED\n")

    def test_representative_text_maxima_and_one_over(self):
        # text() is shared; cover each capacity class and a namespace-bound role.
        pairs=[]
        for n in (32,33):
            p=json.loads(encoded(BASE).decode().replace("viewer","v"*n))
            p["principals"]=[];p["inputs"][0]["group"]="wheel";pairs.append((p,n==32))
        for n in (32,33):
            p=copy.deepcopy(BASE);p["artifacts"][2]["id"]="a"*n;p["files"][0]["artifact"]="a"*n;pairs.append((p,n==32))
        for n in (32,33):
            p=copy.deepcopy(BASE);role="_viewer_"+"r"*(n-8)
            p["principals"][0]["name"]=role;p["inputs"][0]["group"]=role;pairs.append((p,n==32))
        for n in (128,129):
            p=copy.deepcopy(BASE);p["artifacts"][0]["file"]="a"*n;pairs.append((p,n==128))
        for n in (64,65):
            p=copy.deepcopy(BASE);p["artifacts"][0]["sha256"]="a"*n;pairs.append((p,n==64))
        for n in (192,193):
            p=copy.deepcopy(BASE);p["files"][0]["member"]="m"*n;pairs.append((p,n==192))
        for n in (192,193):
            p=copy.deepcopy(BASE);stem="/usr/local/libexec/";target=stem+"t"*(n-len(stem))
            p["files"][0]["target"]=target;p["services"][0]["entry"]=target;pairs.append((p,n==192))
        for p,accepted in pairs:
            output=self.run_mode("parse",p)
            self.assertTrue(output.startswith("PLAN ") if accepted else output=="REJECTED-UNCHANGED\n")

    def test_array_maxima_and_bounds(self):
        for field,maximum,make in [
            ("artifacts",32,lambda i:{"id":f"pkg{i}","kind":"package","file":f"pkg{i}.tgz","sha256":"a"*64}),
            ("principals",4,lambda i:{"name":f"_viewer_{i}"}),
            ("files",64,lambda i:dict(BASE["files"][0],member=f"file{i}",target=f"/etc/viewer/f{i}")),
            ("inputs",16,lambda i:dict(BASE["inputs"][0],name=f"input{i}",target=f"/etc/viewer/private/i{i}")),
            ("services",8,lambda i:dict(BASE["services"][0],name=f"viewer-{i}"))]:
            p=copy.deepcopy(BASE)
            while len(p[field])<maximum:
                i=len(p[field]); item=make(i)
                if field=="services":
                    item["entry"]=f"/usr/local/libexec/viewer{i}"
                    p["files"].append(dict(BASE["files"][0],member=f"bin/service{i}",target=item["entry"]))
                p[field].append(item)
            self.assertTrue(self.run_mode("parse",p).startswith("PLAN "),field)
            p[field].append(make(maximum))
            self.assertEqual(self.run_mode("parse",p),"REJECTED-UNCHANGED\n",field)

    def parse_result(self, text):
        lines=text.splitlines();parts=lines[0].split()
        self.assertEqual(parts[0],"RESULT");self.assertEqual(parts[-2:],["REPEAT","0"])
        return list(map(int,parts[1:8])),lines[1].split()[1:]

    def test_coordinator_normal_refusal_and_success_source_only(self):
        result,events=self.parse_result(self.run_mode("execute"))
        self.assertEqual(result,[4,8,10,10,10,0,0])
        self.assertEqual([e[1:] for e in events if e.startswith("S")],STEPS)
        self.assertEqual([e[1:] for e in events if e.startswith("I")],STEPS[2:])
        self.assertNotIn("R",events);self.assertNotIn("U",events)
        normal=self.run_mode("execute",normal=True)
        self.assertEqual(normal,"RESULT 1 0 0 0 0 0 0 REPEAT 0\nNORMAL-PLATFORM-REFUSAL\n")
        for kind in ("A","P"):
            for value in (0,2):
                r,e=self.parse_result(self.run_mode("execute",faults=(kind,1,value,"-",0,1)))
                self.assertEqual(r[1:4],[0,0,0]);self.assertFalse(any(x.startswith(("I","S")) for x in e))
                self.assertEqual(r[0],1 if value==0 else 3)

    def test_each_operation_and_intent_failure_preserves_exact_prefix(self):
        for kind,total in (("S",10),("I",8)):
            for nth in range(1,total+1):
                for value in (0,2):
                    r,e=self.parse_result(self.run_mode("execute",faults=(kind,nth,value,"-",0,1)))
                    submitted=nth if kind=="S" else nth+1
                    observed=nth-1 if kind=="S" else nth+1
                    self.assertEqual(r[2:4],[submitted,observed])
                    self.assertEqual([x[1:] for x in e if x.startswith("S")],STEPS[:submitted])
                    self.assertEqual(r[0],3 if value==2 else (2 if (kind=="I" or nth>=3) else 1))
                    self.assertEqual(e[-1],"R" if (value==2 or kind=="I" or nth>=3) else "U")

    def test_original_live_checks_cancel_unknown_and_release_failure(self):
        # Literal number: initial/preflight2; pre/post each read-only step4;
        # pre/post-intent/post each seven write steps21; final pre/post-intent2.
        for nth in range(1,30):
            for value in (0,2):
                r,e=self.parse_result(self.run_mode("execute",faults=("L",nth,value,"-",0,1)))
                self.assertNotEqual(r[0],4)
                self.assertEqual(e.count("L"),nth)
                self.assertEqual(r[0],3 if value==2 else (2 if r[1] else 1))
                self.assertEqual(e[-1],"R" if (value==2 or r[1]) else "U")
                # Every observed operation is retained even if the following
                # original boundary expires; no later operation can appear.
                self.assertEqual(r[3],sum(x.startswith("S") for x in e))
        r,e=self.parse_result(self.run_mode("execute",faults=("L",1,0,"U",1,2)))
        self.assertEqual(r[0],3);self.assertEqual(e[-2:],["U","R"])

    def test_unreviewable_full_plan_never_reaches_admission(self):
        p=copy.deepcopy(BASE)
        for i in range(1,64):
            member=f"module{i}/"+"a"*(192-len(f"module{i}/"))
            stem=f"/usr/local/share/viewer/module{i}/"
            target=stem+"b"*(192-len(stem))
            p["files"].append(dict(BASE["files"][0],member=member,target=target))
        self.assertTrue(self.run_mode("parse",p).startswith("PLAN "))
        self.assertEqual(self.run_mode("render",p),"OUTPUT-UNCHANGED\n")
        result,events=self.parse_result(self.run_mode("execute",p))
        self.assertEqual(result,[1,0,0,0,0,0,0]);self.assertEqual(events,[])
        result,events=self.parse_result(self.run_mode("invalid-entry",raw=b"{}"))
        self.assertEqual(result,[1,0,0,0,0,0,0]);self.assertEqual(events,[])

    def test_source_accounting_and_unchanged_leaf_limits(self):
        groups={16:["install_profile.c","install_profile.h"],8:["install_plan.c","install_profile.h"],
                32:["install_executor.c","install_platform.c","private_install.h","install_profile.h"]}
        for cap,names in groups.items():self.assertLessEqual(sum((ROOT/"src"/n).stat().st_size for n in names),cap*1024)
        entry=["trusted_start.c","trusted_start.h","entry_platform.c","entry_native.c","entry_openbsd.c","entry_cli.c",
               "private_entry.h","private_cli.h","private_limits.h","private_start_material.h","main.c",
               "install_entry.c","install_platform.c","private_install.h","install_profile.h"]
        self.assertLessEqual(sum((ROOT/"src"/n).stat().st_size for n in entry),24*1024)
        self.assertLessEqual(sum(p.stat().st_size for p in (ROOT/"src").rglob("*") if p.suffix in (".c",".h")),256*1024)
        self.assertIn("PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c",(ROOT/"Makefile").read_text())
