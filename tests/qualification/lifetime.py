# SPDX-License-Identifier: BSD-3-Clause
"""Shared original clock/cancellation for dormant qualification host integration.

No origin/deadline seed, reset, effect callback or native authority. The owning
Coordinator is once-only. Threads may observe this lifetime, but only its serial
owner consumes phase slots. A child-stop watchdog cannot interrupt a blocked
filesystem operation in that owner; an external coordinator watchdog is still
required before native execution.
"""
from dataclasses import dataclass
import math
import threading
import time


class Refusal(RuntimeError):
    pass


@dataclass(frozen=True)
class Window:
    phase: str
    origin: float
    work_end: float
    end: float


class Lifetime:
    def __init__(self):
        self._lock = threading.RLock()
        self._cancel = threading.Event()
        self._origin = self._last = self._stamp()
        self._end = self._origin + 10800
        self._phases, self._window, self._host_claimed = (), None, False
        self._clock_failed = False

    @staticmethod
    def _stamp():
        value = time.monotonic()
        if type(value) not in (int, float) or not math.isfinite(value) or value < 0:
            raise Refusal("CLOCK")
        return value

    @property
    def origin(self):
        return self._origin

    @property
    def end(self):
        return self._end

    @property
    def cancelled(self):
        return self._cancel.is_set()

    def cancel(self):
        self._cancel.set()

    def now(self):
        with self._lock:
            try:
                value = self._stamp()
                if self._clock_failed or value < self._last:
                    raise Refusal("CLOCK")
                self._last = value
                return value
            except BaseException:
                self._clock_failed = True
                self.cancel()
                raise

    def begin(self, phase):
        with self._lock:
            expected = ("install", "installed")
            if self._phases != expected[:len(self._phases)] or len(self._phases) >= 2 or phase != expected[len(self._phases)]:
                raise Refusal("PHASE_ORDER")
            self._phases += (phase,)  # A failed window is consumed, never renewed.
            now = self.now()
            end = min(now + 3600, self._end)
            work = min(now + 2700, end - 900)
            if self.cancelled or now >= work:
                raise Refusal("NO_WORK_WINDOW")
            self._window = Window(phase, now, work, end)
            return self._window

    def window(self, phase):
        with self._lock:
            if self._window is None or self._window.phase != phase:
                raise Refusal("NO_SHARED_PHASE")
            return self._window

    def require_work(self, window):
        with self._lock:
            if window is not self._window or self.cancelled or self.now() >= window.work_end:
                raise Refusal("WORK_CLOSED")

    def claim_host(self):
        with self._lock:
            if self._host_claimed:
                raise Refusal("HOST_ONCE")
            self._host_claimed = True
