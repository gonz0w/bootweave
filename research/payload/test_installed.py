# SPDX-License-Identifier: BSD-3-Clause
"""Host tooling only: pure comparisons, native refusal, and public byte packing."""
import base64
import ctypes as c
import hashlib
import importlib.util
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
SOURCES = ROOT / 'research/installed'


class Part(c.Structure):
    _fields_ = [('offset', c.c_uint64), ('size', c.c_uint64)]


class Label(c.Structure):
    _fields_ = [('magic', c.c_uint32), ('magic2', c.c_uint32), ('sector_bytes', c.c_uint32),
               ('version', c.c_uint16), ('count', c.c_uint16), ('sectors', c.c_uint64),
               ('partition', Part * 16)]


class Files(c.Structure):
    _fields_ = [(v, c.c_uint64) for v in ('root_device', 'root_inode', 'parent_inode', 'case_inode', 'mount_time')] + [
        ('flags', c.c_uint32), ('fsid', c.c_uint32 * 2), ('spec', c.c_char * 90)] + [
        (v, c.c_int) for v in ('rootfd', 'rawfd', 'blockfd', 'parentfd', 'casefd')]


class Binding(c.Structure):
    _fields_ = [('label', Label), ('uid', c.c_ubyte * 8), ('types', c.c_ubyte * 16),
               ('label_flags', c.c_uint32), ('checksum', c.c_uint32), ('held', Files)]


for _name, _type in Files._fields_:
    setattr(Binding, _name, property(lambda s, n=_name: getattr(s.held, n),
                                    lambda s, v, n=_name: setattr(s.held, n, v)))


class InstalledTooling(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.owned = tempfile.TemporaryDirectory(prefix='bootweave-installed-host-')
        path = Path(cls.owned.name)
        flags = ['cc', '-std=c99', '-Os', '-Wall', '-Wextra', '-Werror', '-pedantic',
                 '-I' + str(ROOT / 'research/storage'), '-I' + str(ROOT / 'research/payload'),
                 '-I' + str(SOURCES)]
        subprocess.run(flags + ['-shared', '-fPIC', str(SOURCES / 'root.c'),
                       str(ROOT / 'research/storage/validate.c'), '-o', str(path / 'model.so')], check=True)
        cls.lib = c.CDLL(str(path / 'model.so'))
        cls.lib.ip_valid.argtypes = [c.POINTER(Binding)]
        cls.lib.ip_same.argtypes = [c.POINTER(Binding), c.POINTER(Binding)]
        cls.stub = path / 'native-stub'
        subprocess.run(flags + ['-DAUTH_INSTALLED', '-DAUTH_PLACEMENT', '-DAUTH_REVIEW',
                       str(SOURCES / 'check.c'), '-o', str(cls.stub)], check=True)

    @classmethod
    def tearDownClass(cls):
        cls.owned.cleanup()

    def binding(self):
        a = Binding()
        a.label.magic = a.label.magic2 = 0x82564557
        a.label.sector_bytes, a.label.version, a.label.count, a.label.sectors = 512, 1, 16, 16777216
        a.label.partition[0].offset, a.label.partition[0].size = 64, 2097152
        a.label.partition[2].size = 16777216
        a.uid[0], a.types[0] = 1, 7
        a.root_device = a.fsid[0] = 1024
        a.fsid[1], a.root_inode, a.parent_inode, a.case_inode, a.flags = 77, 2, 100, 101, 0x5000
        a.rootfd, a.rawfd, a.blockfd, a.parentfd, a.casefd = 9, 10, 11, 4, 3
        a.spec = b'/dev/sd0a'
        return a

    def test_IPFH01_each_authority_field_and_unsafe_root(self):
        a = self.binding()
        self.assertEqual(self.lib.ip_valid(a), 1)
        self.assertEqual(self.lib.ip_same(a, a), 1)
        for name in ('root_device', 'root_inode', 'parent_inode', 'case_inode', 'mount_time',
                     'flags', 'label_flags', 'checksum', 'rootfd', 'rawfd', 'blockfd', 'parentfd', 'casefd'):
            with self.subTest(field=name):
                b = Binding.from_buffer_copy(a)
                setattr(b, name, getattr(b, name) + 1)
                self.assertEqual(self.lib.ip_same(a, b), 0)
        for name in ('magic', 'magic2', 'sector_bytes', 'version', 'count', 'sectors'):
            b = Binding.from_buffer_copy(a)
            setattr(b.label, name, getattr(b.label, name) + 1)
            self.assertEqual(self.lib.ip_same(a, b), 0)
        for name, length in (('uid', 8), ('fsid', 2), ('types', 16)):
            for i in range(length):
                b = Binding.from_buffer_copy(a)
                getattr(b, name)[i] += 1
                self.assertEqual(self.lib.ip_same(a, b), 0)
        for i in range(16):
            for name in ('offset', 'size'):
                b = Binding.from_buffer_copy(a)
                setattr(b.label.partition[i], name, getattr(b.label.partition[i], name) + 1)
                self.assertEqual(self.lib.ip_same(a, b), 0)
        b = Binding.from_buffer_copy(a)
        b.spec = b'0100000000000000.a'
        self.assertEqual(self.lib.ip_valid(b), 1)
        self.assertEqual(self.lib.ip_same(a, b), 0)
        b.spec = b'X' * 90
        self.assertEqual(self.lib.ip_valid(b), 0)
        for flag in (1, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400):
            b = Binding.from_buffer_copy(a)
            b.flags |= flag
            self.assertEqual(self.lib.ip_valid(b), 0)
        for field in ('root_inode', 'parent_inode', 'case_inode'):
            b = Binding.from_buffer_copy(a)
            setattr(b, field, 0)
            self.assertEqual(self.lib.ip_valid(b), 0)

    def test_IPFH02_host_refuses_and_original_source_budgets(self):
        for args, code in (([], 77), (['arbitrary-command'], 2)):
            result = subprocess.run([str(self.stub)] + args, capture_output=True)
            self.assertEqual(result.returncode, code)
            self.assertNotIn(b'Installed checks passed', result.stdout)
        baseline = 'bc989d022f97d25fda9702c97b531aa51da12cf2'
        originals = [p for directory in ('payload', 'handoff', 'storage')
                     for p in (ROOT / 'research' / directory).glob('*') if p.suffix in ('.c', '.h')]
        added = sum(p.stat().st_size for p in SOURCES.glob('*') if p.suffix in ('.c', '.h'))
        for p in originals:
            prior = subprocess.check_output(['git', 'show', baseline + ':' + str(p.relative_to(ROOT))], cwd=ROOT)
            added += p.stat().st_size - len(prior)
        self.assertLessEqual(added, 16384)
        self.assertLessEqual(sum(p.stat().st_size for p in originals) + sum(
            p.stat().st_size for p in SOURCES.glob('*') if p.suffix in ('.c', '.h')) + 2048, 200 * 1024)

    def test_IPFH03_public_records_become_only_numeric_data(self):
        spec = importlib.util.spec_from_file_location('ipf_fixture_header', SOURCES / 'fixture_header.py')
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        def record(size):
            return b'untrusted comment: \"); system(\"injected\")\n' + base64.b64encode(b'Ed' + bytes(size - 2)) + b'\n'
        packed = module.header(record(42), record(74), record(74), hashlib.sha256(b'hello').digest())
        self.assertLessEqual(len(packed), 2048)
        self.assertNotIn(b'injected', packed)
        self.assertNotIn(b'system', packed)
        for line in packed.splitlines():
            self.assertRegex(line, rb'^static const unsigned char ip_[a-z_]+\[\]=\{[0-9,]+\};$')
        for malformed in (b'', b'untrusted comment: bad\nAAAA\n', b'\x00' * 4000):
            with self.assertRaises(ValueError):
                module.header(malformed, record(74), record(74), bytes(32))
        with self.assertRaises(ValueError):
            module.header(record(42), record(74), record(74), bytes(31))
