# SPDX-License-Identifier: BSD-3-Clause
"""Host research observer only; no input, guest, socket or disk operations."""


class Rejected(ValueError):
    """Refused observation; private screen text is never echoed."""


def handoff_page(lines, marker, interactive=True):
    if (not isinstance(lines, list) or not 1 <= len(lines) <= 25 or
            not isinstance(marker, str) or not 1 <= len(marker) <= 80 or
            any(not 32 <= ord(c) <= 126 for c in marker)):
        raise Rejected('shape')
    for row in lines:
        if (not isinstance(row, str) or len(row) > 80 or
                any(not 32 <= ord(c) <= 126 and c != '\uFFFD' for c in row)):
            raise Rejected('glyph')
        if 'Research handoff exit:' in row:
            raise Rejected('native refusal')
    # A decoder's unknown cursor cell may follow an otherwise complete prompt.
    rows = [row.rstrip('_\uFFFD ') for row in lines]
    active = next((row for row in reversed(rows) if row), '')
    return marker in rows and (not interactive or active == marker)
