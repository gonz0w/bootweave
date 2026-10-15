# SPDX-License-Identifier: BSD-3-Clause
"""Confirmation observer boundaries; no guest or input backend."""
from pathlib import Path
import runpy
import unittest

MODEL = runpy.run_path(str(Path(__file__).resolve().parents[1] / 'check-native-prompt.py'))
PAGE, REJECTED = MODEL['handoff_page'], MODEL['Rejected']


class NativePromptTests(unittest.TestCase):
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
