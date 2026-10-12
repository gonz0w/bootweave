# SPDX-License-Identifier: BSD-3-Clause
"""Reproduce the 35 source-bound synthetic native stimuli; never extract them.

Host development tooling only. Running this creates one owned private directory
under .tools, not a disk or guest. Native use still needs accepted exact-target
scope. No caller-selected paths, archive input or commands are accepted.
"""
import hashlib
import json
import os
from pathlib import Path
import shutil
import sys
import tempfile
from test_payload import ZERO, archive, change, header, member

ROOT = Path(__file__).resolve().parents[2]
LABELS = (
    'viewer', 'reordered', 'empty-file', '128-members', '512K-content',
    '255-path', '1M-input', 'full-prefix', 'traversal', 'prefix-escape',
    'duplicate', 'alias-dir', 'ancestor', 'ancestor-reverse', 'symlink',
    'device', 'extension', 'unknown-type', 'binary-number', 'bad-checksum',
    'ownership', 'privilege-mode', 'hidden-name', 'directory-content',
    'truncated-header', 'truncated-body', 'one-end-block', 'trailing-data',
    'concatenated', 'empty', 'directory-only', 'file-limit', 'input-limit',
    'member-limit', 'path-limit',
)
DIGESTS = (
    '38e5e458bc7cb0d72bc8d2b6fd90f5363eaad309680d3aef18fbe43343c7398a',
    'f49fbc90c3e9084a99de50d8a7d8dd4fd092238182cd3076b346340c0bb16de2',
    '7ff5ed51d6cbdb73bc018b40621de7f668ab1d054ef3d79b53cfb1544845bffe',
    'b2b1773030a37ec82393e50b9911d72f6a75a03ec1a2b910b68c8984fc38eb78',
    'ab5c5b1246f8fdd4d0745e20dd1b2cb40ce21be1aaa2107c4fe0e58ea6a9df70',
    '00ab91f21285cdacd5dcc3b6f6f13f42b52ec7bf574976abfed1c73b174ea229',
    'b89b9b2ebf5a46caad693139656a67069217d7f800d65a84a3c340faabf4eb9b',
    '74f53efe1e88ca0f8061355aca1e1d68a8d9d3646271963f2c13dba659b1f420',
    '9f2a069901ebc6f50fe2b2cc39965291f24014d4eb241f322d35866488bf3c7c',
    '6958ea61ee9b4fb5935630178e46fdd954ea883549f1fd5ec8f4e13803c64b29',
    'd0e3074297f5b02d3d826155d1fe2d9de32d43aa939b4a683a751e33f2e9305b',
    '79bd5a03541c707b42da32f0b4c1ee0906013c2e73ad3c14a63fa7a43f148483',
    '3aefc6f45602067ca84da4868456254c204c68f5a9582d819f4322cc343f9c94',
    'bbd4d5befcc0997c025f2f7bbc99ce06feb1885b5b3ba9db777b57139dee933f',
    '83790a4fe78b24f6f432b755bd950b3dced570f91abeccb488f74530b17ccf3f',
    '5ffedd4f1690def31994f30a133b0853308a8a43c58ca5f2134eb924ca6af54f',
    'a05483818896cda9d5a8b3d8d7f71cc67906fef2e930daa80469c8280737750d',
    '517673ff148d196443ff70e50307a6d5c700bc2afdfd3e21a179e997d18f8450',
    '1a6e9d2e9c1d5d9c17acb4bbacf03fbd6283f7f280ab0ca2a157b3823db3b654',
    '05d9e4e19192e86a402d505b8329505ea4bc41af65b92c76260ea0e71d9f185e',
    '51457b4d6d04fdf7ae63c90ae85312f8a9fd2bdaed9aa31d2b9634a614eb292e',
    '52e73cb1b6c8e472f0e2d22b6482a956f46651b4b004cad0e546c211cd14072b',
    'bef8ed4263098621003bb6abd1978d34e0f6693f04fb17d05dce75f04a8b1a34',
    '012bed7ea474d935a740ae58d0902ecc258b175e4dee92ff9ebf4fa2f24b2b94',
    '8f00f04d7c2738a9c675b445c458000e47be095c5710729edb1362d6c8464021',
    '89ca13b0c04995e4f7740adcbb2ed5498823b65078ab8721191f64c4ab28d2fc',
    'c5213804bb3ed497d2590165e6647970829c8ac3a2779504e667046985c91191',
    'f0f5348af627be7138c79d1af3643bfcbaacc1542fced9dda9c86298a8f826d3',
    '82ef2d9eb4e6bd05928a073fe16c1a2a1a66a37b90a933435015b18298689ecd',
    '5f70bf18a086007016e948b04aed3b82103a36bea41755b6cddfaf10ace3c6ef',
    '79758958e3c399d36cffccf13b10faa3966a377878e7dc56531bc7108d9a93ed',
    'ccce74d775433abfad649fc44fc85bd282f3ffd8dcdfd62a79c87b306d237080',
    '5a21a49df26fc12e3602a4548e815be0c579deeb07c84157fe804de05df3fe58',
    '4d94a02437cb4eb4977a3161d3cf633e2ac1e5535503fcd7cfd28f11436613ba',
    '7812a43db6fa3799af5dd444b2d79e6108b3f5a83574aba82f4af1b677dd34ef',
)
COUNTS = ((2, 1, 5), (2, 1, 5), (1, 1, 0), (128, 128, 0),
          (2, 2, 524288), (1, 1, 0), (1, 1, 0), (1, 1, 0))


def make_cases():
    empty = archive(member())
    path255 = change(change(header(), 0, b'n' * 100), 345, b'p' * 154 + b'\0')
    full = change(change(header(), 0, b'n' * 99 + b'\0'), 345, b'p' * 155)
    bad = bytearray(header()); bad[0] = ord('X')  # Deliberately stale checksum.
    cases = (
        archive(member('viewer/', directory=True), member(data=b'hello')),
        archive(member(data=b'hello'), member('viewer/', directory=True)),
        empty, archive(*(member(f'f{i}') for i in range(128))),
        archive(member('a', b'x' * 262144), member('b', b'y' * 262144)),
        archive(path255), empty + bytes(1048576 - len(empty)), archive(full),
        archive(header('../escape')),
        archive(change(header(), 345, b'../escape' + bytes(146))),
        archive(member('a'), member('a')),
        # tarfile encodes both directory spellings with a slash: this is an
        # identical canonical-path duplicate, not distinct wire encodings.
        archive(member('a', directory=True), member('a/', directory=True), member('b')),
        archive(member('a'), member('a/b')), archive(member('a/b'), member('a')),
        archive(change(header(), 156, b'2')), archive(change(header(), 156, b'3')),
        archive(change(header(), 156, b'x')), archive(change(header(), 156, b'?')),
        archive(change(header(), 124, b'\x80' + bytes(11))), archive(bytes(bad)),
        archive(change(header(), 108, b'0000001\0')),
        archive(change(header(), 100, b'0004755\0')),
        archive(change(header(), 0, b'a\0hidden')),
        archive(header(directory=True, size=1)), header()[:-1],
        header(size=513) + ZERO, member() + ZERO, empty + b'bad',
        empty * 2, ZERO * 2, archive(member('d/', directory=True)),
        archive(header(size=262145)), empty + bytes(1048577 - len(empty)),
        archive(*(member(f'f{i}') for i in range(129))),
        archive(change(change(header(), 0, b'n' * 100), 345, b'p' * 155)),
    )
    if len(cases) != 35 or sum(map(len, cases)) != 2806787:
        raise RuntimeError('Native fixture count/byte binding changed')
    if tuple(hashlib.sha256(x).hexdigest() for x in cases) != DIGESTS:
        raise RuntimeError('Native fixture bytes differ from the observed run')
    return cases


def case_manifest(cases):
    result = {}
    for index, data in enumerate(cases):
        item = {'label': LABELS[index], 'accepted': index < 8,
                'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest()}
        if index < 8:
            members, files, content = COUNTS[index]
            item['expected_stdout'] = (f'Inspection passed: {members} members, '
                                       f'{files} files, {content} content bytes.\n')
        result[f'case-{index:02d}.tar'] = item
    return (json.dumps(result, indent=2) + '\n').encode()


def prepare():
    cases = make_cases()
    manifest = case_manifest(cases)
    if hashlib.sha256(manifest).hexdigest() != '052f68eeb8c267e6efe4c44ba6c233efc94b9574fed97a6dcaf1b5985b7f2b23':
        raise RuntimeError('Native case-manifest binding changed')
    directory = Path(tempfile.mkdtemp(prefix='payload-native-fixtures-', dir=ROOT / '.tools'))
    try:
        (directory / 'cases').mkdir(mode=0o700)
        files = {f'cases/case-{i:02d}.tar': data for i, data in enumerate(cases)}
        files['cases.json'] = manifest
        for name in ('inspect.c', 'native_check.c', 'native_build.sh', 'native_run.sh'):
            files[name] = (ROOT / 'research/payload' / name).read_bytes()
        source_names = [f'cases/case-{i:02d}.tar' for i in range(35)] + ['cases.json', 'inspect.c']
        tool_names = ('native_check.c', 'native_build.sh', 'native_run.sh')
        for name, names in (('SHA256', source_names), ('TOOLS.SHA256', tool_names)):
            files[name] = ''.join(f'SHA256 ({n}) = {hashlib.sha256(files[n]).hexdigest()}\n'
                                  for n in names).encode()
        for name, data in files.items():
            fd = os.open(directory / name, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
            with os.fdopen(fd, 'wb') as stream: stream.write(data)
        return directory
    except BaseException:
        shutil.rmtree(directory)
        raise


if __name__ == '__main__':
    if len(sys.argv) != 1:
        raise SystemExit('Native fixture preparation accepts no arguments.')
    print(prepare())
