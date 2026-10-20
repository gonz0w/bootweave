"""Finite owned portable checks; keep the born leader until the last signal."""
import datetime
import hashlib
import json
import os
from pathlib import Path
import resource
import signal
import subprocess
import sys
import time

BASE = Path(__file__).resolve().parent
SOURCE = BASE / 'bootweave-signature-verifier'
JOB = BASE / 'bootweave-signature-full-check'
COMMAND = ['make', 'verify']
JOB.mkdir(mode=0o700)
for name in ('home', 'tmp'):
    (JOB / name).mkdir(mode=0o700)
ENV = {'PATH': '/usr/bin:/bin:/usr/sbin:/sbin:/opt/homebrew/bin',
       'HOME': str(JOB / 'home'), 'TMPDIR': str(JOB / 'tmp'),
       'LC_ALL': 'C', 'LANG': 'C', 'PYTHONDONTWRITEBYTECODE': '1',
       'PYTHONNOUSERSITE': '1', 'GIT_CONFIG_NOSYSTEM': '1',
       'GIT_CONFIG_GLOBAL': '/dev/null', 'GIT_CONFIG_COUNT': '2',
       'GIT_CONFIG_KEY_0': 'maintenance.auto', 'GIT_CONFIG_VALUE_0': 'false',
       'GIT_CONFIG_KEY_1': 'gc.auto', 'GIT_CONFIG_VALUE_1': '0'}


def snapshot():
    return {str(p.relative_to(SOURCE)): hashlib.sha256(p.read_bytes()).hexdigest()
            for p in SOURCE.rglob('*') if p.is_file()
            and not any(part in ('.git', '.tools', '__pycache__')
                        for part in p.relative_to(SOURCE).parts)}


def limits():
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    resource.setrlimit(resource.RLIMIT_CPU, (90, 90))
    resource.setrlimit(resource.RLIMIT_NOFILE, (128, 128))
    resource.setrlimit(resource.RLIMIT_FSIZE, (134217728, 134217728))


def group_rows(pid):
    result = subprocess.run(['/bin/ps', '-axo', 'pid=,pgid=,lstart=,stat=,command='],
                            capture_output=True, text=True, timeout=5,
                            env={'PATH': '/usr/bin:/bin', 'LC_ALL': 'C', 'LANG': 'C'})
    if result.returncode or result.stderr or not result.stdout:
        raise RuntimeError('Process observation unavailable')
    rows = []
    for line in result.stdout.splitlines():
        parts = line.split(None, 8)
        if len(parts) != 9:
            raise RuntimeError('Malformed process row')
        if int(parts[1]) == pid:
            rows.append(line)
    return rows


assert signal.getsignal(signal.SIGCHLD) == signal.SIG_DFL
assert hasattr(os, 'WNOWAIT') and hasattr(os, 'waitid')
record = {'source_before': snapshot(), 'command': COMMAND,
          'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
          'native': False, 'work_seconds': 270, 'total_seconds': 300,
          'network_requested': False, 'credentials_inherited': False,
          'hard_isolation_claimed': False, 'events': []}
prior_builds = {str(p) for p in (SOURCE / '.tools').glob('signature-build-*')}
start = time.monotonic()
child = None
consumed = lost = False


def interrupt(*_):
    raise RuntimeError('Finite check interrupted')


for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGHUP):
    signal.signal(sig, interrupt)
try:
    with (JOB / 'verify.log').open('xb') as log:
        child = subprocess.Popen(COMMAND, cwd=SOURCE, env=ENV,
                                 stdin=subprocess.DEVNULL, stdout=log,
                                 stderr=subprocess.STDOUT, start_new_session=True,
                                 close_fds=True, preexec_fn=limits)
        record.update(pid=child.pid, pgid=child.pid,
                      group_at_start=group_rows(child.pid))
        while True:
            try:
                observed = os.waitid(os.P_PID, child.pid,
                                     os.WEXITED | os.WNOHANG | os.WNOWAIT)
            except InterruptedError:
                continue
            except BaseException:
                lost = True
                raise
            if observed is not None and observed.si_pid:
                if observed.si_pid != child.pid:
                    lost = True
                    raise RuntimeError('Lost wait identity')
                break
            if time.monotonic() - start >= 270:
                raise RuntimeError('Finite work deadline')
            total = 0
            for root in (JOB / 'home', JOB / 'tmp', (SOURCE / '.tools').resolve()):
                for p in root.rglob('*'):
                    try:
                        total += p.lstat().st_size
                    except FileNotFoundError:
                        pass
            if total > 536870912 or log.tell() > 8388608:
                raise RuntimeError('Owned output bound')
            time.sleep(.25)
except BaseException as error:
    record['failure'] = {'type': type(error).__name__, 'error': str(error)}
finally:
    for sig in (signal.SIGINT, signal.SIGTERM, signal.SIGHUP):
        signal.signal(sig, signal.SIG_IGN)
    failures = []
    if child is not None:
        if not lost and not consumed:
            record['events'].append('final-group-kill-before-wait')
            try:
                os.killpg(child.pid, signal.SIGKILL)
            except ProcessLookupError:
                pass
            except PermissionError as error:
                record['stop_errno'] = error.errno
                try:
                    observed = group_rows(child.pid)
                    record['permission_stop_observation'] = observed
                    leaders = [line for line in observed if int(line.split()[0]) == child.pid]
                    if sys.platform != 'darwin' or error.errno != 1 or len(leaders) != 1 or any(not line.split(None, 8)[7].startswith('Z') for line in observed):
                        failures.append('group-stop:' + repr(error))
                    else:
                        record['stop_result'] = 'signal refused; only zombies observed; wait pending'
                except BaseException as observation_error:
                    failures.append('group-stop-observation:' + repr(observation_error))
            except BaseException as error:
                failures.append('group-stop:' + repr(error))
        else:
            failures.append('wait-custody-lost; no signal')
        if not lost:
            try:
                while time.monotonic() - start < 290:
                    try:
                        pid, status, usage = os.wait4(child.pid, os.WNOHANG)
                    except InterruptedError:
                        continue
                    if pid:
                        consumed = True
                        record['events'].append('actual-consuming-wait')
                        assert pid == child.pid
                        child.returncode = os.waitstatus_to_exitcode(status)
                        record['exit_code'] = child.returncode
                        record['child_peak_rss_bytes'] = usage.ru_maxrss
                        break
                    time.sleep(.05)
                if not consumed:
                    failures.append('actual-wait-not-completed')
            except BaseException as error:
                lost = not consumed
                failures.append('wait:' + repr(error))
        try:
            end = min(start + 298, time.monotonic() + 5)
            remaining = group_rows(child.pid)
            while remaining and time.monotonic() < end:
                time.sleep(.1)
                remaining = group_rows(child.pid)
            record['remaining_group'] = remaining
            if remaining:
                failures.append('group-still-observed; no post-wait signal')
        except BaseException as error:
            failures.append('terminal-observation:' + repr(error))
    try:
        inner, groups, setgroups = [], [], []
        builds = [p for p in (SOURCE / '.tools').glob('signature-build-*') if str(p) not in prior_builds]
        record['prior_build_references'] = sorted(prior_builds)
        record['current_builds'] = [str(p) for p in builds]
        record['inner_observation_complete'] = bool(builds)
        for build in builds:
            ledger = build / 'observations.jsonl'
            if not ledger.is_file():
                record['inner_observation_complete'] = False
                failures.append('missing-current-inner-ledger:' + str(build))
                continue
            for line in ledger.read_text().splitlines():
                row = json.loads(line)
                if row.get('parse_error') or row.get('error_type') or row.get('witness_errors') or row.get('result') is None:
                    record['inner_observation_complete'] = False
                    failures.append('incomplete-inner-case:' + row['case'])
                for group in row.get('groups', []):
                    groups.append({'fault': row['fault'], 'case': row['case'], 'pid': group[0], 'pgid': group[1], 'errno': group[2]})
                for operation in row.get('setgroups', []):
                    setgroups.append({'fault': row['fault'], 'case': row['case'], 'pid': operation[0], 'requested_pgid': operation[1], 'syscall_result': 0})
                for born in row.get('born', []):
                    inner.append({'fault': row['fault'], 'case': row['case'], 'born': born})
        record['inner_born_observations'] = inner
        record['inner_group_observations'] = groups
        record['inner_setgroup_operations'] = setgroups
        ids = {row['born'][0] for row in inner}
        actual_groups = {row['born'][1] for row in inner if row['born'][1] > 0} | {row['pgid'] for row in groups if row['pgid'] > 0} | {row['requested_pgid'] for row in setgroups}
        # Initial PGID may be the still-owned outer group; it is observation only.
        record['inner_observed_groups'] = sorted(actual_groups)
        if not record['inner_observation_complete']:
            failures.append('inner-observation-incomplete; absence-is-not-custody-proof')
        result = subprocess.run(['/bin/ps', '-axo', 'pid=,pgid=,lstart=,stat=,command='],
                                capture_output=True, text=True, timeout=5,
                                env={'PATH': '/usr/bin:/bin', 'LC_ALL': 'C', 'LANG': 'C'})
        if result.returncode or result.stderr or not result.stdout:
            raise RuntimeError('Inner terminal process observation unavailable')
        rows = []
        for line in result.stdout.splitlines():
            parts = line.split(None, 8)
            if len(parts) != 9:
                raise RuntimeError('Malformed inner process row')
            if int(parts[0]) in ids or int(parts[1]) in actual_groups:
                rows.append(line)
        record['inner_terminal_rows'] = rows
        if rows:
            failures.append('inner-process-still-observed; no recovered signal authority')
    except BaseException as error:
        failures.append('inner-terminal-observation:' + repr(error))
    record.update(wait_consumed=consumed, wait_lost=lost, cleanup_errors=failures,
                  source_after=snapshot(), elapsed_seconds=time.monotonic() - start,
                  finished_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  log_sha256=hashlib.sha256((JOB / 'verify.log').read_bytes()).hexdigest())
    record['source_unchanged'] = record['source_before'] == record['source_after']
    (JOB / 'receipt.json').write_text(json.dumps(record, indent=2) + '\n')
    print(json.dumps({k: v for k, v in record.items()
                      if k not in ('source_before', 'source_after')}, indent=2))
    raise SystemExit(0 if record.get('exit_code') == 0 and not record.get('failure')
                     and not failures and consumed and record['source_unchanged'] else 1)
