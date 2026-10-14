#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-3-Clause
"""Development-only public fixture packing; no guest/backend or secret input."""
import hashlib
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'payload'))
from authenticate import _record


def header(key, signature, other, digest):
    if (not _record(key, 42) or not _record(signature, 74)
            or not _record(other, 74) or type(digest) is not bytes or len(digest) != 32):
        raise ValueError('invalid fixed public fixture')
    records = [b'untrusted comment: fixture\n' + v.split(b'\n')[1] + b'\n'
               for v in (key, signature, other)]
    records.append(digest)
    text = b''.join(b'static const unsigned char ' + name + b'[]={'
                    + b','.join(str(v).encode('ascii') for v in data) + b'};\n'
                    for name, data in zip(
                        (b'ip_key', b'ip_sig', b'ip_other_sig', b'ip_data_hash'), records))
    if len(text) > 2048:
        raise ValueError('fixed header exceeds budget')
    return text


if __name__ == '__main__':
    if len(sys.argv) != 1:
        raise SystemExit(2)
    root = Path(__file__).resolve().parents[2] / '.tools/installed-payload-flow/exported'
    values = [(root / name).read_bytes() for name in ('selected.pub', 'data.sig', 'other.sig')]
    data = (root / 'data.tar').read_bytes()
    if not 0 < len(data) <= 4096:
        raise SystemExit('fixed payload bounds')
    packed = header(*values, hashlib.sha256(data).digest())
    with (root / 'fixture_keys.h').open('xb') as output:
        output.write(packed)
    print('Fixed public numeric fixture header prepared; freeze its digest before compilation.')
