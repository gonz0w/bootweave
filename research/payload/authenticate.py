# SPDX-License-Identifier: BSD-3-Clause
"""Host-only authenticated inspection research; trusted test context, no CLI API."""
import base64
from dataclasses import dataclass
import fcntl
import hashlib
import os
from pathlib import Path
import re
import resource
import select
import selectors
import signal
import stat
import subprocess
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parents[2]
VERIFIER = ROOT / '.tools/signify-review/signify-osx-1.4/signify'
VERIFIER_SHA256 = '4b9b2069c5e5acda9d35ca898ecd5afc050094170cf3c73fa7cf9eb615530946'
INPUT_CAP = 1048576
RECORD_CAP = 2048
OUTPUT_CAP = 4096
CHILD_SECONDS = 15
INPUT_SECONDS = 10
WORKFLOW_SECONDS = 45
CHILD_RSS_CAP = 32 * 1024 * 1024
ENVIRONMENT = {'LC_ALL': 'C', 'LANG': 'C', 'PATH': ''}
CATEGORIES = frozenset(('context', 'input', 'identity', 'signature', 'inspection',
                        'tools', 'io', 'deadline', 'output', 'resource',
                        'interrupted', 'cleanup'))
METRICS = {'child_peak_rss_bytes': 0, 'reaped_children': 0}


class Failure(Exception):
    def __init__(self, category):
        if category not in CATEGORIES:
            category = 'io'
        self.category = category
        super().__init__(f'Authenticated inspection stopped: {category}.')


@dataclass(frozen=True)
class Context:
    expected_length: int
    expected_sha256: str
    public_key: bytes


@dataclass(frozen=True)
class Inspector:
    # Supplied by trusted development tooling, never operator/payload input.
    executable: Path
    sha256: str


@dataclass(frozen=True)
class Result:
    completed: bool
    verified: bool
    category: str
    summary: bytes = b''


@dataclass(frozen=True)
class ChildResult:
    status: int
    stdout: bytes
    stderr: bytes
    rss_bytes: int


def _check(deadline, cancelled):
    if cancelled[0]:
        raise Failure('interrupted')
    if time.monotonic() >= deadline:
        raise Failure('deadline')


def _record(data, decoded_size):
    if type(data) is not bytes or not 0 < len(data) <= RECORD_CAP:
        return False
    lines = data.split(b'\n')
    if (len(lines) != 3 or lines[2] or not lines[0].startswith(b'untrusted comment: ')
            or not 19 < len(lines[0]) < 1024 or b'\0' in lines[0]):
        return False
    try:
        decoded = base64.b64decode(lines[1], validate=True)
    except ValueError:
        return False
    return (len(decoded) == decoded_size and decoded[:2] == b'Ed'
            and base64.b64encode(decoded) == lines[1])


def _digest(value):
    return type(value) is str and re.fullmatch(r'[0-9a-f]{64}', value) is not None


def _pin(executable, expected):
    if not isinstance(executable, Path) or not executable.is_absolute() or not _digest(expected):
        raise Failure('tools')
    flags = os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK
    fd = os.open(executable, flags)
    try:
        observed = os.fstat(fd)
        if (not stat.S_ISREG(observed.st_mode) or observed.st_uid != os.getuid()
                or observed.st_mode & 0o022 or not observed.st_mode & 0o100
                or not 0 < observed.st_size <= INPUT_CAP):
            raise Failure('tools')
        data = bytearray()
        while len(data) <= INPUT_CAP:
            part = os.read(fd, min(8192, INPUT_CAP + 1 - len(data)))
            if not part:
                break
            data.extend(part)
        if hashlib.sha256(data).hexdigest() != expected or len(data) != observed.st_size:
            raise Failure('tools')
    finally:
        os.close(fd)


def _capture(fd, deadline, cancelled):
    if type(fd) is not int or fd < 0 or os.isatty(fd):
        raise Failure('input')
    original = fcntl.fcntl(fd, fcntl.F_GETFL)
    parts, length = [], 0
    try:
        fcntl.fcntl(fd, fcntl.F_SETFL, original | os.O_NONBLOCK)
        while True:
            _check(deadline, cancelled)
            if not select.select([fd], [], [], min(.1, max(0, deadline - time.monotonic())))[0]:
                continue
            try:
                part = os.read(fd, min(8192, INPUT_CAP + 1 - length))
            except BlockingIOError:
                continue
            if not part:
                if not length:
                    raise Failure('input')
                return b''.join(parts)
            length += len(part)
            if length > INPUT_CAP:
                raise Failure('input')
            parts.append(part)
    finally:
        try:
            fcntl.fcntl(fd, fcntl.F_SETFL, original)
        except OSError:
            raise Failure('cleanup') from None


def _limits():
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    resource.setrlimit(resource.RLIMIT_CPU, (CHILD_SECONDS, CHILD_SECONDS))
    resource.setrlimit(resource.RLIMIT_FSIZE, (1048576, 1048576))
    # Darwin rejects these calls even in an unsandboxed owned process. wait4
    # still enforces an observed-completion ceiling, not kernel containment.
    if sys.platform != 'darwin':
        resource.setrlimit(resource.RLIMIT_DATA, (CHILD_RSS_CAP, CHILD_RSS_CAP))
        resource.setrlimit(resource.RLIMIT_RSS, (CHILD_RSS_CAP, CHILD_RSS_CAP))
    os.umask(0o077)


def _reap(process, block=False):
    while True:
        try:
            pid, status, usage = os.wait4(process.pid, 0 if block else os.WNOHANG)
            break
        except InterruptedError:
            continue
    if not pid:
        return None
    process.returncode = os.waitstatus_to_exitcode(status)
    rss = usage.ru_maxrss * (1 if sys.platform == 'darwin' else 1024)
    METRICS['child_peak_rss_bytes'] = max(METRICS['child_peak_rss_bytes'], rss)
    METRICS['reaped_children'] += 1
    return rss


def _run(argv, data, directory, deadline, cancelled):
    """Trusted-only child supervision shared by the experiment and fixture tests."""
    _check(deadline, cancelled)
    deadline = min(deadline, time.monotonic() + CHILD_SECONDS)
    process = None
    selector = selectors.DefaultSelector()
    output, errors = bytearray(), bytearray()
    sent, rss, finished = 0, None, False
    try:
        process = subprocess.Popen(argv, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE, cwd=directory, env=ENVIRONMENT,
                                   close_fds=True, start_new_session=True, preexec_fn=_limits)
        for stream in (process.stdin, process.stdout, process.stderr):
            os.set_blocking(stream.fileno(), False)
        if data:
            selector.register(process.stdin, selectors.EVENT_WRITE, 'input')
        else:
            process.stdin.close()
        selector.register(process.stdout, selectors.EVENT_READ, 'output')
        selector.register(process.stderr, selectors.EVENT_READ, 'error')
        while selector.get_map() or process.returncode is None:
            _check(deadline, cancelled)
            for key, _ in selector.select(min(.1, max(0, deadline - time.monotonic()))):
                if key.data == 'input':
                    try:
                        sent += os.write(key.fd, data[sent:sent + 8192])
                    except BlockingIOError:
                        continue
                    except BrokenPipeError:
                        raise Failure('io') from None
                    if sent == len(data):
                        selector.unregister(key.fileobj)
                        key.fileobj.close()
                else:
                    try:
                        part = os.read(key.fd, min(8192, OUTPUT_CAP + 1 - len(output) - len(errors)))
                    except BlockingIOError:
                        continue
                    if not part:
                        selector.unregister(key.fileobj)
                        key.fileobj.close()
                    else:
                        (output if key.data == 'output' else errors).extend(part)
                        if len(output) + len(errors) > OUTPUT_CAP:
                            raise Failure('output')
            if process.returncode is None:
                measured = _reap(process)
                if measured is not None:
                    rss = measured
        if rss is None or rss > CHILD_RSS_CAP:
            raise Failure('resource')
        if sent != len(data):
            raise Failure('io')
        finished = True
        return ChildResult(process.returncode, bytes(output), bytes(errors), rss)
    finally:
        selector.close()
        if process is not None:
            if not finished:
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
            if process.returncode is None:
                _reap(process, block=True)
            for stream in (process.stdin, process.stdout, process.stderr):
                stream.close()


def _write(directory, name, data):
    fd = os.open(directory / name, os.O_WRONLY | os.O_CREAT | os.O_EXCL | os.O_NOFOLLOW, 0o600)
    with os.fdopen(fd, 'wb') as stream:
        stream.write(data)


def _summary(data):
    found = re.fullmatch(rb'Inspection passed: ([0-9]{1,3}) members, ([0-9]{1,3}) files, '
                         rb'([0-9]{1,6}) content bytes\.\n', data)
    if found is None:
        return False
    members, files, content = map(int, found.groups())
    return 1 <= files <= members <= 128 and content <= 524288


def _postconditions(directory, public_key, signature):
    if sorted(p.name for p in directory.iterdir()) != ['selected.pub', 'supplied.sig']:
        raise Failure('cleanup')
    for name, expected in (('selected.pub', public_key), ('supplied.sig', signature)):
        fd = os.open(directory / name, os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK)
        try:
            item = os.fstat(fd)
            if (not stat.S_ISREG(item.st_mode) or item.st_uid != os.getuid()
                    or stat.S_IMODE(item.st_mode) != 0o600 or item.st_size != len(expected)
                    or os.read(fd, RECORD_CAP + 1) != expected):
                raise Failure('cleanup')
        finally:
            os.close(fd)


def inspect_authenticated(source_fd, signature, context, inspector):
    """Called only by the trusted host harness; not a public/runtime interface."""
    verified = False
    handlers, cancelled = {}, [False]
    result = Result(False, False, 'context')
    deadline = time.monotonic() + WORKFLOW_SECONDS
    try:
        if (sys.platform != 'darwin' or type(context) is not Context or type(inspector) is not Inspector
                or type(context.expected_length) is not int
                or not 0 < context.expected_length <= INPUT_CAP
                or not _digest(context.expected_sha256) or not _record(context.public_key, 42)):
            raise Failure('context')
        if not _record(signature, 74):
            raise Failure('signature')
        for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGHUP, signal.SIGPIPE):
            previous = signal.getsignal(sig)
            signal.signal(sig, signal.SIG_IGN if sig == signal.SIGPIPE
                          else lambda *_: cancelled.__setitem__(0, True))
            handlers[sig] = previous
        _pin(VERIFIER, VERIFIER_SHA256)
        _pin(inspector.executable, inspector.sha256)
        snapshot = _capture(source_fd, min(deadline, time.monotonic() + INPUT_SECONDS), cancelled)
        if (len(snapshot) != context.expected_length
                or hashlib.sha256(snapshot).hexdigest() != context.expected_sha256):
            raise Failure('identity')
        # Only public-key/signature snapshots go to files. Payload bytes remain
        # in one retained immutable object used for both child input pipes.
        with tempfile.TemporaryDirectory(prefix='payload-auth-', dir=ROOT / '.tools') as owned:
            directory = Path(owned)
            _write(directory, 'selected.pub', context.public_key)
            _write(directory, 'supplied.sig', signature)
            checked = _run((str(VERIFIER), '-V', '-q', '-p', 'selected.pub',
                            '-x', 'supplied.sig', '-m', '-'), snapshot, directory, deadline, cancelled)
            if checked.status != 0 or checked.stdout or checked.stderr:
                raise Failure('signature')
            _postconditions(directory, context.public_key, signature)
            verified = True
            inspected = _run((str(inspector.executable),), snapshot, directory, deadline, cancelled)
            if inspected.status != 0 or inspected.stderr or not _summary(inspected.stdout):
                raise Failure('inspection')
            _postconditions(directory, context.public_key, signature)
            _check(deadline, cancelled)
            result = Result(True, True, 'passed', inspected.stdout)
    except Failure as failure:
        result = Result(False, verified, failure.category)
    except (OSError, ValueError, subprocess.SubprocessError):
        result = Result(False, verified, 'io')
    finally:
        for sig, handler in handlers.items():
            signal.signal(sig, handler)
    if result.completed:
        try:
            _check(deadline, cancelled)
            rss = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
            if rss > 64 * 1024 * 1024:
                raise Failure('resource')
        except Failure as failure:
            return Result(False, verified, failure.category)
    return result


if __name__ == '__main__':
    # A path/key/expectation supplied by an operator is not trusted test context.
    sys.stderr.write('Host authentication research requires trusted test context.\n')
    raise SystemExit(2 if len(sys.argv) != 1 else 77)
