# SPDX-License-Identifier: BSD-3-Clause
"""Host research observer only; no input, guest, socket or disk operations."""
import re


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


def handoff_observation(lines, marker, interactive=True):
    """An empty preboot observation cannot authorize input; pages stay strict."""
    if isinstance(lines, list) and not lines:
        # The framebuffer decoder can report no text page during BIOS startup.
        # Still validate the expected marker, without manufacturing a prompt.
        handoff_page([''], marker, interactive)
        return False
    return handoff_page(lines, marker, interactive)


def installed_login_page(lines):
    """Recognize the observed synthetic guest's VGA getty page; no input."""
    ready = handoff_observation(lines, 'login:')
    return ready and 'OpenBSD/amd64 (bootweave.example.invalid) (ttyC0)' in lines


def _scrollback_rows(page):
    handoff_page(page, 'shape check', interactive=False)
    if len(page) != 25:
        raise Rejected('scrollback shape')
    rows = [row.rstrip('_\uFFFD ') for row in page]
    while rows and not rows[-1]:
        rows.pop()
    return rows


def _scrollback_complete(current, earlier, restored, expected):
    """Check observed overlap; callers must establish one owned navigation cycle."""
    tail, head, back = map(_scrollback_rows, (current, earlier, restored))
    if tail != back or not tail or tail[-1] != expected[-1]:
        return False
    if head.count(expected[0]) != 1:
        return False
    prefix = head[head.index(expected[0]):]
    start = len(expected) - len(tail)
    # At least one actual row must overlap. Never fill an unseen gap from source.
    return (0 < start < len(prefix) < len(expected) and
            prefix == expected[:len(prefix)] and tail == expected[start:])


def installed_scrollback_probe(current, earlier, restored):
    """Fixed inert native display probe; no console input or support claim."""
    expected = [f'IPF_SCROLLBACK_{i:02d}' for i in range(1, 30)] + ['bootweave#']
    return _scrollback_complete(current, earlier, restored, expected)


def installed_review_pages(current, earlier, restored, expected_hashes):
    """Frozen two-member research fixture only; not a generic review protocol."""
    if (not isinstance(expected_hashes, tuple) or len(expected_hashes) != 4 or
            any(not isinstance(h, str) or not re.fullmatch('[0-9a-f]{64}', h)
                for h in expected_hashes)):
        raise Rejected('review hashes')
    head = _scrollback_rows(earlier)
    title = 'Bootweave file placement research'
    if head.count(title) != 1:
        return False
    prefix = head[head.index(title):]
    if len(prefix) < 5 or not re.fullmatch(r'Root dev=1024 inode=[1-9][0-9]{0,19}', prefix[3]):
        return False
    expected = [title, 'Only this fresh case root is writable.', 'viewer-case', prefix[3],
                'Directories: 32767/32767 mode 0700', 'Files: 32767/32767 mode 0600']
    for label, value in zip(('Payload', 'Explicit key', 'Verifier', 'Inspector'), expected_hashes):
        expected.extend([label + ' SHA256:', value[:40], value[40:]])
    expected.extend(['Planned=2 files=1 bytes=5 input=2560',
                     'Limits: 16 objects, depth 16, path 255',
                     'Input 1 MiB; content 512 KiB total',
                     'Each file 256 KiB; writer 15 seconds',
                     'Controller 64 MiB; child 32 MiB',
                     'No overwrite, links or executable data',
                     'Directory viewer bytes=0', 'File viewer/readme.txt bytes=5',
                     'h help | b back | q cancel', 'Type review to confirm these effects.'])
    return _scrollback_complete(current, earlier, restored, expected)
