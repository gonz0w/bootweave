# SPDX-License-Identifier: BSD-3-Clause
"""Confirmation observer boundaries; no guest or input backend."""
from pathlib import Path
import runpy
import unittest

MODEL = runpy.run_path(str(Path(__file__).resolve().parents[1] / 'check-native-prompt.py'))
PAGE, REJECTED = MODEL['handoff_page'], MODEL['Rejected']
OBSERVATION = MODEL['handoff_observation']
LOGIN = MODEL['installed_login_page']
REVIEW = MODEL['installed_review_pages']
SCROLLBACK = MODEL['installed_scrollback_probe']

HASHES = ('38e5e458bc7cb0d72bc8d2b6fd90f5363eaad309680d3aef18fbe43343c7398a',
          '73eac4a9de4571f73d185e2dc07f8afc82ef236965e9daaa2876eb0b6c21c45d',
          'f29045c0813293c5d3e6af36729eca0fb427c13b7e70d7201644ba1362b9ab2a',
          '7f1c86f89cf61bc48a732259cec0aaae00fe95f12e0f74d199872bccd06785c8')
# Sanitized fixed fixture: literal native review text with a synthetic root inode.
REVIEW_ROWS = ['Bootweave file placement research', 'Only this fresh case root is writable.',
               'viewer-case', 'Root dev=1024 inode=1',
               'Directories: 32767/32767 mode 0700', 'Files: 32767/32767 mode 0600',
               'Payload SHA256:', HASHES[0][:40], HASHES[0][40:],
               'Explicit key SHA256:', HASHES[1][:40], HASHES[1][40:],
               'Verifier SHA256:', HASHES[2][:40], HASHES[2][40:],
               'Inspector SHA256:', HASHES[3][:40], HASHES[3][40:],
               'Planned=2 files=1 bytes=5 input=2560',
               'Limits: 16 objects, depth 16, path 255',
               'Input 1 MiB; content 512 KiB total', 'Each file 256 KiB; writer 15 seconds',
               'Controller 64 MiB; child 32 MiB', 'No overwrite, links or executable data',
               'Directory viewer bytes=0', 'File viewer/readme.txt bytes=5',
               'h help | b back | q cancel', 'Type review to confirm these effects.']


def views(rows, offset=12):
    full = ['synthetic preceding output'] * 25 + rows + ['']
    return full[-25:], full[-25-offset:-offset], full[-25:]


class NativePromptTests(unittest.TestCase):
    def test_full_native_review_exceeds_current_VGA_page(self):
        self.assertEqual(len(REVIEW_ROWS), 28)
        current, earlier, restored = views(REVIEW_ROWS)
        self.assertNotIn(REVIEW_ROWS[0], current)
        self.assertFalse(REVIEW(current, current, current, HASHES))
        for offset in (4, 12, 24):
            with self.subTest(offset=offset):
                self.assertTrue(REVIEW(*views(REVIEW_ROWS, offset), HASHES))

    def test_review_requires_observed_overlap_and_exact_restoration(self):
        current, earlier, restored = views(REVIEW_ROWS)
        gap = ['synthetic preceding output'] * 21 + REVIEW_ROWS[:4]
        self.assertFalse(REVIEW(current, gap, restored, HASHES))
        changed = restored.copy()
        changed[-2] = 'bootweave#'
        self.assertFalse(REVIEW(current, earlier, changed, HASHES))
        self.assertFalse(REVIEW(current, earlier, restored, ('0' * 64,) * 4))
        cursor = restored.copy()
        cursor[-1] = '\uFFFD'
        self.assertTrue(REVIEW(current, earlier, cursor, HASHES))

    def test_review_uses_the_frozen_inspector_for_each_variant(self):
        static = '5fc1918f6a98bdae2b01b210110d5e2b5ea80a809e2389bee6d667788005aa8f'
        rows = REVIEW_ROWS.copy()
        rows[16:18] = [static[:40], static[40:]]
        self.assertTrue(REVIEW(*views(rows), (*HASHES[:3], static)))
        self.assertFalse(REVIEW(*views(rows), HASHES))

    def test_review_refuses_changed_identity_effects_and_digest_rows(self):
        for index in range(len(REVIEW_ROWS)):
            changed = REVIEW_ROWS.copy()
            changed[index] = 'changed row'
            with self.subTest(index=index):
                self.assertFalse(REVIEW(*views(changed), HASHES))
        for root in ('Root dev=1025 inode=1', 'Root dev=1024 inode=0'):
            changed = REVIEW_ROWS.copy()
            changed[3] = root
            self.assertFalse(REVIEW(*views(changed), HASHES))

    def test_scrollback_probe_needs_both_observed_pages_and_return(self):
        rows = [f'IPF_SCROLLBACK_{i:02d}' for i in range(1, 30)] + ['bootweave#']
        current, earlier, restored = views(rows)
        self.assertTrue(SCROLLBACK(current, earlier, restored))
        self.assertFalse(SCROLLBACK(current, current, restored))
        changed = earlier.copy()
        changed[-1] = 'unobserved gap'
        self.assertFalse(SCROLLBACK(current, changed, restored))

    def test_scrollback_rejects_malformed_pages_and_hash_inputs(self):
        current, earlier, restored = views(REVIEW_ROWS)
        for bad in ([], None, ['x'] * 26, ['\x1b[2J'] * 25, ['x' * 81] * 25):
            for position in range(3):
                pages = [current, earlier, restored]
                pages[position] = bad
                with self.subTest(position=position, bad=bad), self.assertRaises(REJECTED):
                    REVIEW(*pages, HASHES)
        for bad in (None, (), ['0' * 64] * 4, ('x' * 64,) * 4):
            with self.subTest(bad=bad), self.assertRaises(REJECTED):
                REVIEW(current, earlier, restored, bad)

    def test_installed_VGA_login_requires_banner_and_fresh_prompt(self):
        banner = 'OpenBSD/amd64 (bootweave.example.invalid) (ttyC0)'
        self.assertTrue(LOGIN([banner, '', 'login: \uFFFD']))
        for rows in ([], ['login:'], [banner, 'bootweave login:'],
                     [banner, 'Password:'], [banner, 'login:', '#'],
                     [banner.replace('ttyC0', 'tty00'), 'login:'],
                     [banner.replace('bootweave', 'other'), 'login:']):
            with self.subTest(rows=rows):
                self.assertFalse(LOGIN(rows))
        for rows in (None, [banner] * 26, [banner, '\x1b[2Jlogin:']):
            with self.subTest(rows=rows), self.assertRaises(REJECTED):
                LOGIN(rows)

    def test_preboot_observation_never_authorizes_input(self):
        marker = 'Type next to review effects.'
        self.assertFalse(OBSERVATION([], marker))
        self.assertFalse(OBSERVATION(['BIOS startup'], marker))
        self.assertTrue(OBSERVATION([marker, '\uFFFD'], marker))
        self.assertFalse(OBSERVATION([marker, '#'], marker))
        with self.assertRaises(REJECTED):
            PAGE([], marker)
        for rows, expected in (([], '\x1b'), (None, marker),
                               ([marker] * 26, marker),
                               ([marker, '\x1b[2J#'], marker),
                               ([marker, 'Research handoff exit: 2'], marker)):
            with self.subTest(rows=rows, marker=expected), self.assertRaises(REJECTED):
                OBSERVATION(rows, expected)

    def test_current_prompt_and_refusal_boundaries(self):
        marker = 'Type next to review effects.'
        self.assertTrue(PAGE(['boot glyph: \uFFFD', marker, '\uFFFD'], marker))
        self.assertFalse(PAGE([marker, '#'], marker))
        self.assertFalse(PAGE(['Type next to confirm.'], marker))
        with self.assertRaises(REJECTED):
            PAGE([marker, 'Research handoff exit: 2', '#'], marker)
        self.assertFalse(PAGE([marker.replace('effects', 'eff\uFFFDefs')], marker))
        result = 'Confirmation checked; handoff requested.'
        self.assertTrue(PAGE([result, 'Stock question?'], result, interactive=False))
        for rows in ([marker, '\x1b[2J#'], [marker, '\n'], [marker] * 26, ['x' * 81], []):
            with self.subTest(rows=rows), self.assertRaises(REJECTED):
                PAGE(rows, marker)
