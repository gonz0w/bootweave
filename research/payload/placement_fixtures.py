# SPDX-License-Identifier: BSD-3-Clause
"""Trusted host-only synthetic fixtures and independent expected inventories.

No extraction, guest launch, operator-selected path or public payload schema.
Expected records are private test media, read only from fixed harness paths.
"""
import hashlib
import io
import tarfile
from native_fixtures import make_cases, case_manifest
from test_payload import archive, member, wire_directory


def aliases():
    result = []
    for fields in (((b'a', b''), (b'a/', b'')),
                   ((b'viewer/docs', b''), (b'viewer/docs/', b''),
                    (b'docs', b'viewer'), (b'docs/', b'viewer'))):
        headers = [wire_directory(*pair) for pair in fields]
        result.extend(archive(member('readme.txt', b'hello'), raw) for raw in headers)
        result.extend(archive(member('readme.txt', b'hello'), a, b)
                      for a in headers for b in headers)
    assert len(result) == 26
    return result


def extra_cases():
    return (
        archive(member('viewer/docs/readme.txt', b'hello'), member('viewer/empty')),
        archive(member('/'.join(['d'] * 15 + ['f']), b'hello')),
        archive(member('/'.join(['d'] * 16 + ['f']), b'hello')),
        archive(*(member(f'd{i}/f') for i in range(64))),
        archive(*(member(f'd{i}/f') for i in range(65))),
    )


def expected(data):
    # Python's independent reader is used only on known admitted synthetic bytes.
    # It never writes to a filesystem. Fixed effects/hashes are separate from C.
    objects = {}
    with tarfile.open(fileobj=io.BytesIO(data), mode='r:') as reader:
        for item in reader.getmembers():
            parts = item.name.rstrip('/').split('/')
            for depth in range(1, len(parts)):
                objects['/'.join(parts[:depth])] = (1, 0, '0' * 64)
            if item.isdir():
                objects['/'.join(parts)] = (1, 0, '0' * 64)
            else:
                content = reader.extractfile(item).read()
                assert len(content) == item.size
                objects['/'.join(parts)] = (0, len(content), hashlib.sha256(content).hexdigest())
    return ''.join(f'{path} {kind} {size} {digest}\n'
                   for path, (kind, size, digest) in sorted(objects.items())).encode()


def files():
    historical = make_cases()
    result = {'cases.json': case_manifest(historical)}
    for kind, cases in (('cases/case', historical), ('aliases/alias', aliases()),
                        ('placement/new', extra_cases())):
        for i, data in enumerate(cases):
            stem = f'{kind}-{i:02d}'
            result[f'{stem}.tar'] = data
            admitted = (i < 8 if kind.startswith('cases') else
                        i < 2 or 6 <= i < 10 if kind.startswith('aliases') else i in (0, 1, 3))
            if admitted:
                result[f'{stem}.expected'] = expected(data)
    return result
