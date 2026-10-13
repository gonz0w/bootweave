# SPDX-License-Identifier: BSD-3-Clause
"""Host planner/tooling checks; no native confinement or write qualification."""
import ctypes
import hashlib
from pathlib import Path
import subprocess
import tempfile
import unittest
import placement_fixtures
import native_fixtures
from test_payload import archive, member

ROOT = Path(__file__).resolve().parents[2]


class Node(ctypes.Structure):
    _fields_ = [('path', ctypes.c_char * 256), ('offset', ctypes.c_size_t),
               ('size', ctypes.c_size_t), ('directory', ctypes.c_int),
               ('explicit_member', ctypes.c_int), ('parent', ctypes.c_int)]


class Plan(ctypes.Structure):
    _fields_ = [('nodes', Node * 128), ('count', ctypes.c_size_t),
               ('members', ctypes.c_size_t), ('files', ctypes.c_size_t),
               ('bytes', ctypes.c_size_t)]


class PlacementTooling(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory(prefix='bootweave-placement-host-')
        cls.directory = Path(cls.tmp.name)
        lib = cls.directory / 'planner.dylib'
        subprocess.run(['cc', '-std=c99', '-Os', '-Wall', '-Wextra', '-Werror',
                        '-pedantic', '-shared', '-fPIC', str(ROOT / 'research/payload/placement_plan.c'),
                        '-o', str(lib)], check=True, capture_output=True)
        cls.lib = ctypes.CDLL(str(lib))
        cls.lib.pl_plan.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(Plan)]
        cls.lib.pl_plan.restype = ctypes.c_int

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def plan(self, data, accepted=True):
        plan = Plan()
        rc = self.lib.pl_plan(data, len(data), ctypes.byref(plan))
        self.assertEqual(rc, 0 if accepted else -1)
        return plan

    def test_PPLH01_frozen_structural_and_alias_rejections(self):
        for i, data in enumerate(native_fixtures.make_cases()):
            with self.subTest(historical=i): self.plan(data, i < 8)
        for i, data in enumerate(placement_fixtures.aliases()):
            with self.subTest(alias=i): self.plan(data, i < 2 or 6 <= i < 10)
        self.assertEqual(hashlib.sha256((ROOT / 'research/payload/inspect.c').read_bytes()).hexdigest(),
                         '8009aed82f67ae60c70538579fa493995a87fa6ff1ea324dc23ee79af0847698')

    def test_PPLH02_realized_objects_depth_and_independent_content(self):
        for i, data in enumerate(placement_fixtures.extra_cases()):
            with self.subTest(extra=i): self.plan(data, i in (0, 1, 3))
        data = placement_fixtures.extra_cases()[0]
        plan = self.plan(data)
        actual = {bytes(n.path).decode(): (n.directory, data[n.offset:n.offset+n.size])
                  for n in plan.nodes[:plan.count]}
        self.assertEqual(actual, {'viewer': (1, b''), 'viewer/docs': (1, b''),
                                 'viewer/docs/readme.txt': (0, b'hello'), 'viewer/empty': (0, b'')})
        for i, node in enumerate(plan.nodes[:plan.count]):
            if node.parent >= 0:
                self.assertLess(node.parent, i)
                self.assertEqual(plan.nodes[node.parent].directory, 1)
        # File/implicit parent conflict and explicit directory aliases reject.
        self.plan(archive(member('x/y'), member('x')), False)
        self.plan(archive(member('x/y'), member('x/', directory=True),
                          member('x/', directory=True)), False)

    def test_PPLH03_native_entry_refusals_and_source_budgets(self):
        binary = self.directory / 'refusal'
        before = sorted(p.name for p in self.directory.iterdir())
        try:
            subprocess.run(['cc', '-std=c99', '-Os', '-Wall', '-Wextra', '-Werror', '-pedantic',
                            '-DAUTH_TEST', '-DAUTH_PLACEMENT',
                            *[str(ROOT / 'research/payload' / p) for p in
                              ('auth_native.c', 'placement_plan.c', 'placement_native.c', 'placement_check.c')],
                            '-o', str(binary)], check=True, capture_output=True)
            for args, status in (([], 77), (['$(touch forbidden)'], 2)):
                r = subprocess.run([str(binary), *args], capture_output=True, timeout=3)
                self.assertEqual(r.returncode, status)
                self.assertEqual(r.stdout, b'')
                self.assertEqual(r.stderr, b'Native placement checks require OpenBSD.\n')
            for script in ('placement_build.sh', 'placement_run.sh'):
                r = subprocess.run(['sh', str(ROOT / 'research/payload' / script)], capture_output=True, timeout=3)
                self.assertEqual(r.returncode, 77)
                self.assertEqual(r.stdout, b'')
            additional = sum((ROOT / 'research/payload' / p).stat().st_size for p in
                             ('placement.h', 'placement_plan.c', 'placement_native.c', 'placement_check.c'))
            self.assertLessEqual(additional, 65536)
            existing = sum((ROOT / 'research/payload' / p).stat().st_size for p in
                           ('auth_native.h', 'auth_native.c', 'auth_native_check.c', 'inspect.c', 'native_check.c'))
            self.assertLessEqual(additional + existing, 131072)
        finally:
            binary.unlink(missing_ok=True)
        self.assertEqual(sorted(p.name for p in self.directory.iterdir()), before)
