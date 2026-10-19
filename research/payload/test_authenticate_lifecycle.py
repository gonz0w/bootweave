# SPDX-License-Identifier: BSD-3-Clause
"""Issue #78: every process, selector, wait and signal effect is injected."""
from contextlib import ExitStack
from dataclasses import FrozenInstanceError
from pathlib import Path
from types import SimpleNamespace
import signal
import time
import unittest
from unittest.mock import patch

import authenticate as gate


class Stream:
    def __init__(self, name, events, fail_at=0):
        self.name, self.events, self.fail_at, self.closes = name, events, fail_at, 0

    def fileno(self):
        return {'stdin': 17, 'stdout': 18, 'stderr': 19}[self.name]

    def close(self):
        self.closes += 1
        self.events.append(('close', self.name))
        if self.closes == self.fail_at:
            raise OSError('injected stream closure')


class Selector:
    def __init__(self, events, fail=False):
        self.events, self.fail = events, fail

    def register(self, *args):
        pass

    def get_map(self):
        return {}

    def select(self, timeout):
        return []

    def close(self):
        self.events.append(('selector-close',))
        if self.fail:
            raise OSError('injected selector closure')


class Process:
    pid = 4242

    def __init__(self, events, bad_stream=False):
        self.returncode = None
        self.stdin = Stream('stdin', events, 2 if bad_stream else 0)
        self.stdout = Stream('stdout', events)
        self.stderr = Stream('stderr', events)


class AuthenticationLifecycleTests(unittest.TestCase):
    def run_injected(self, *, rss=1, selector_failure=False, conversion_failure=False,
                     lost_wait=False, interrupted=False, bad_stream=False):
        events = []
        process = Process(events, bad_stream)
        selector = Selector(events, selector_failure)
        old_metrics = dict(gate.METRICS)
        unresolved = getattr(gate, '_UNRESOLVED_CHILDREN', None)
        old_unresolved = None if unresolved is None else list(unresolved)

        def wait4(pid, options):
            events.append(('wait-consumed', pid))
            return pid, 0, SimpleNamespace(ru_maxrss=rss)

        def waitid(*args):
            events.append(('nonreaping-observation',))
            if lost_wait:
                raise ChildProcessError('injected stolen wait')
            return SimpleNamespace(si_pid=process.pid)

        checks = 0

        def check(*args):
            nonlocal checks
            checks += 1
            if interrupted and checks > 1:
                raise gate.Failure('interrupted')

        with ExitStack() as stack:
            stack.enter_context(patch.object(gate.subprocess, 'Popen', return_value=process))
            stack.enter_context(patch.object(gate.selectors, 'DefaultSelector', return_value=selector))
            stack.enter_context(patch.object(gate.os, 'set_blocking'))
            stack.enter_context(patch.object(gate.os, 'wait4', side_effect=wait4))
            stack.enter_context(patch.object(gate.os, 'waitid', side_effect=waitid))
            stack.enter_context(patch.object(gate.os, 'killpg', side_effect=lambda pid, sig: events.append(('signal', pid, sig))))
            stack.enter_context(patch.object(gate.signal, 'getsignal', return_value=signal.SIG_DFL))
            stack.enter_context(patch.object(gate, '_check', side_effect=check))
            if conversion_failure:
                stack.enter_context(patch.object(gate.os, 'waitstatus_to_exitcode', side_effect=ValueError('injected status conversion')))
            try:
                result = gate._run(['not-executed'], b'', Path('/not-opened'), time.monotonic() + 5, [False])
                failure = None
            except BaseException as error:
                result, failure = None, error
            finally:
                self.last_metrics = dict(gate.METRICS)
                gate.METRICS.clear(); gate.METRICS.update(old_metrics)
                if unresolved is not None:
                    unresolved[:] = old_unresolved
        return events, result, failure

    def assert_no_signal_after_wait(self, events):
        positions = [i for i, item in enumerate(events) if item[0] == 'wait-consumed']
        self.assertEqual(len(positions), 1)
        self.assertFalse(any(item[0] == 'signal' for item in events[positions[0] + 1:]))

    def test_PAV_LC01_post_reap_resource_refusal_has_no_later_signal(self):
        events, result, error = self.run_injected(rss=gate.CHILD_RSS_CAP + 1)
        self.assertIsNone(result)
        self.assertEqual(error.category, 'resource')
        self.assert_no_signal_after_wait(events)

    def test_PAV_LC02_conversion_failure_keeps_consumed_wait(self):
        events, result, error = self.run_injected(conversion_failure=True)
        self.assertIsInstance(error, ValueError)
        self.assert_no_signal_after_wait(events)

    def test_PAV_LC03_selector_close_cannot_skip_terminal_stages(self):
        events, result, error = self.run_injected(selector_failure=True, interrupted=True)
        self.assertEqual(error.category, 'interrupted')
        self.assertTrue(any(item[0] == 'signal' for item in events))
        self.assert_no_signal_after_wait(events)
        self.assertTrue(all(('close', name) in events for name in ['stdin', 'stdout', 'stderr']))
        self.assertIn('selector-close:OSError', ' '.join(error.__notes__))

    def test_PAV_LC03_success_is_withheld_on_selector_failure(self):
        events, result, error = self.run_injected(selector_failure=True)
        self.assertIsNone(result)
        self.assertEqual(error.category, 'cleanup')
        self.assert_no_signal_after_wait(events)

    def test_PAV_LC04_lost_wait_never_renews_group_authority(self):
        events, result, error = self.run_injected(lost_wait=True)
        self.assertIsInstance(error, ChildProcessError)
        self.assertFalse(any(item[0] in ['signal', 'wait-consumed'] for item in events))
        self.assertTrue(all(('close', name) in events for name in ['stdin', 'stdout', 'stderr']))

    def test_PAV_LC05_multiple_cleanup_failures_preserve_primary(self):
        events, result, error = self.run_injected(selector_failure=True, interrupted=True, bad_stream=True)
        self.assertEqual(error.category, 'interrupted')
        notes = ' '.join(error.__notes__)
        self.assertIn('selector-close:OSError', notes)
        self.assertIn('stdin-close:OSError', notes)
        self.assertIn(('close', 'stderr'), events)
        self.assert_no_signal_after_wait(events)

    def test_PAV_LC06_normal_completion_has_one_terminal_wait(self):
        events, result, error = self.run_injected()
        self.assertIsNone(error)
        self.assertEqual(result.status, 0)
        self.assertGreater(result.rss_bytes, 0)
        self.assert_no_signal_after_wait(events)
        wait_at = next(i for i, item in enumerate(events) if item[0] == 'wait-consumed')
        self.assertTrue(any(item[0] == 'signal' for item in events[:wait_at]))

    def test_PAV_LC06_born_pid_is_immutable_and_consumed_wait_is_not_repeated(self):
        process = Process([])
        owner = gate._BornChild(process, process.pid, gate._WaitState())
        with self.assertRaises(FrozenInstanceError):
            owner.pid = 9
        with patch.object(gate.os, 'wait4', return_value=(4242, 0, SimpleNamespace(ru_maxrss=1))) as waited, \
                patch.object(gate.os, 'killpg') as signalled:
            gate._reap(owner)
            gate._reap(owner)
            gate._stop_owned_group(owner)
        self.assertEqual(waited.call_count, 1)
        signalled.assert_not_called()

    def test_nondefault_SIGCHLD_refuses_before_spawn(self):
        with patch.object(gate.signal, 'getsignal', return_value=signal.SIG_IGN), \
                patch.object(gate.subprocess, 'Popen') as spawned:
            with self.assertRaises(gate.Failure) as error:
                gate._run(['not-executed'], b'', Path('/not-opened'), time.monotonic() + 5, [False])
        self.assertEqual(error.exception.category, 'tools')
        spawned.assert_not_called()


if __name__ == '__main__':
    unittest.main()
