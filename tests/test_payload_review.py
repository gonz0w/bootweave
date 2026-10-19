# SPDX-License-Identifier: BSD-3-Clause
"""Independent portable structure corpus; no verifier, key or native effects."""
import io
from pathlib import Path
import subprocess
import tarfile
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[1]

def entry(name='viewer/readme.txt', data=b'', kind=tarfile.REGTYPE):
    item=tarfile.TarInfo(name);item.type=kind;item.mode=0o755 if kind==tarfile.DIRTYPE else 0o644;item.size=len(data)
    return item.tobuf(format=tarfile.USTAR_FORMAT)+data+bytes((-len(data))%512)
def archive(*entries):return b''.join(entries)+bytes(1024)
def modify(header,at,data):
    out=bytearray(header);out[at:at+len(data)]=data;out[148:156]=b' '*8;out[148:156]=f'{sum(out[:512]):06o}'.encode()+b'\0 ';return bytes(out)

class PayloadReviewTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp=tempfile.TemporaryDirectory(prefix='bootweave-review-',dir=ROOT/'.tools');cls.addClassCleanup(cls.tmp.cleanup)
        cls.binary=Path(cls.tmp.name)/'review'
        subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-pedantic','-I'+str(ROOT/'src'),str(ROOT/'src/payload_review.c'),str(ROOT/'tests/payload_review_driver.c'),'-o',str(cls.binary)],check=True,capture_output=True,timeout=15)
    def run_archive(self,data):return subprocess.run([self.binary],input=data,capture_output=True,timeout=3)
    def test_null_alias_failure_reset_and_repeatability(self):
        self.assertEqual(subprocess.run([self.binary,'--selftest'],input=archive(entry(data=b'unchanged')),timeout=2).returncode,0)
        valid=archive(entry(data=b'unchanged'))
        for _ in range(3):
            self.assertEqual(self.run_archive(valid).returncode,0)
            self.assertEqual(self.run_archive(b'broken').returncode,2)
            self.assertEqual(self.run_archive(valid).stdout,b'structure-only 1 1 9\n')

    def test_valid_contents_and_exact_summary(self):
        for data,expected in [(archive(entry(data=b'hello')),b'structure-only 1 1 5\n'),(archive(entry('viewer',kind=tarfile.DIRTYPE),entry(data=b'abc')),b'structure-only 2 1 3\n'),(archive(entry(data=b'x'*262144)),b'structure-only 1 1 262144\n')]:
            result=self.run_archive(data);self.assertEqual((result.returncode,result.stdout),(0,expected))
    def test_paths_types_aliases_and_conflicts(self):
        for name in ['../bad','/absolute','a/../b','a//b','./bad','bad\x1b','/'.join(['a']*17)]:
            self.assertEqual(self.run_archive(archive(entry(name))).returncode,2)
        for kind in [tarfile.SYMTYPE,tarfile.LNKTYPE,tarfile.CHRTYPE,tarfile.FIFOTYPE]:self.assertEqual(self.run_archive(archive(entry(kind=kind))).returncode,2)
        for parts in [(entry('a'),entry('a')), (entry('a'),entry('a/b')),(entry('a/b'),entry('a')),(entry('a',kind=tarfile.DIRTYPE),entry('a/',kind=tarfile.DIRTYPE),entry('b'))]:self.assertEqual(self.run_archive(archive(*parts)).returncode,2)
    def test_ownership_modes_checksum_padding_and_trailer(self):
        base=entry(data=b'x')
        self.assertEqual(self.run_archive(archive(modify(base,100,b'0000644\0'))).returncode,0)
        for offset,value in [(100,b'0004755\0'),(108,b'0000001\0'),(116,b'0000001\0'),(157,b'link'),(265,b'owner')]:
            self.assertEqual(self.run_archive(archive(modify(base,offset,value))).returncode,2)
        corrupt=bytearray(base);corrupt[0]^=1
        pad=bytearray(base);pad[-1]=1
        for data in [bytes(corrupt)+bytes(1024),bytes(pad)+bytes(1024),base+bytes(512),archive(entry())+entry(),archive(entry())+b'x',bytes(1024),b'']:
            self.assertEqual(self.run_archive(data).returncode,2)
    def test_exact_depth_path_content_and_input_ceiling(self):
        for data in [archive(entry('/'.join(['a']*16))),archive(entry('a'*78+'/'+ 'b'*76+'/'+ 'c'*99)),archive(entry('a',b'x'*262144),entry('b',b'x'*262144))]:
            self.assertEqual(self.run_archive(data).returncode,0)
        data=archive(entry());data+=bytes(1048576-len(data))
        self.assertEqual(self.run_archive(data).returncode,0)

    def test_count_content_and_input_bounds(self):
        self.assertEqual(self.run_archive(archive(*(entry(str(i)) for i in range(128)))).returncode,0)
        for data in [archive(*(entry(str(i)) for i in range(129))),archive(entry(data=b'x'*262145)),archive(entry('a',b'x'*262144),entry('b',b'x'*262144),entry('c',b'x')),archive(entry())+bytes(1048576)]:self.assertEqual(self.run_archive(data).returncode,2)
