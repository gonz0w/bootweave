# SPDX-License-Identifier: BSD-3-Clause
"""Finite syscall-modeled guard checks; no host guard/native dispatch."""
from collections import Counter
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest

import test_policy
import trusted_start_vectors as vectors

ROOT = Path(__file__).resolve().parents[1]
IDENTITY = {"supported", "getuid", "geteuid", "getgid", "getegid"}
FIELDS = {"success", "events", "repeat_result", "repeat_events", "mutations", "repeat_mutations",
          "fatal_status", "limits", "handlers", "unblocked", "timers", "host_unchanged", "readonly_checks"}


class TrustedStartTests(unittest.TestCase):
    execute = classmethod(test_policy.PolicyTests.execute.__func__)

    @classmethod
    def setUpClass(cls):
        if sys.platform != "darwin":
            raise RuntimeError("Mac modeled guard fixture only; no native dispatch")
        cls.directory = Path(tempfile.mkdtemp(prefix="trusted-start-build-", dir=ROOT / ".tools"))
        cls.ledger = cls.directory / "observations.jsonl"
        cls.env = {"PATH": "/usr/bin:/bin", "HOME": str(cls.directory), "TMPDIR": str(cls.directory),
                   "LC_ALL": "C", "LANG": "C", "PYTHONDONTWRITEBYTECODE": "1",
                   "ASAN_OPTIONS": "detect_leaks=0:abort_on_error=1"}
        cls.calls = cls.builds = 0
        cls.cache, cls.quarantine = {}, []
        cls.flags = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-pedantic",
                     "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g", "-I", str(ROOT / "src")]
        paths = sorted([*ROOT.joinpath("src").glob("*.[ch]"),
            *(ROOT / "tests" / n for n in ("trusted_start_driver.c", "test_trusted_start.py",
                                            "trusted_start_vectors.py", "test_policy.py", "entry_ownership_driver.c"))])
        manifest = {str(p.relative_to(ROOT)): {"bytes": p.stat().st_size,
                    "sha256": hashlib.sha256(p.read_bytes()).hexdigest()} for p in paths}
        (cls.directory / "source-manifest.json").write_text(json.dumps(manifest, sort_keys=True) + "\n")
        cls.plain, platform, material = (cls.directory / n for n in ("guard.o", "platform.o", "material.o"))
        for target, source, extra in ((cls.plain, "trusted_start.c", []),
                (platform, "signature_platform.c", []), (material, "auth_material.c", [])):
            cls.execute(cls.flags + extra + ["-c", str(ROOT / "src" / source), "-o", str(target)], compile_job=True)
        dependencies = []
        for source in ("auth_input", "authenticated_review", "content_review", "policy_binding", "plan",
                       "payload_review", "signature_verify", "entry_platform"):
            target = cls.directory / (source + ".o")
            cls.execute(cls.flags + ["-c", str(ROOT / "src" / (source + ".c")), "-o", str(target)], compile_job=True)
            dependencies.append(target)
        support = cls.directory / "unused-entry-policy.o"
        cls.execute(cls.flags + ["-DEO_UNUSED_DEPENDENCIES", "-c", str(ROOT / "tests/entry_ownership_driver.c"),
                                "-o", str(support)], compile_job=True)
        dependencies += [support, platform]
        cls.modeled, cls.normal = cls.directory / "modeled", cls.directory / "normal-unsupported"
        for target, objects, extra in ((cls.modeled, [material, *dependencies], []),
                (cls.normal, [cls.plain, material, *dependencies], ["-DTS_NORMAL"])):
            cls.execute(cls.flags + extra + list(map(str, objects)) +
                        [str(ROOT / "tests/trusted_start_driver.c"), "-o", str(target)], compile_job=True)

    def run_case(self, case):
        stimulus = case["stimulus"]
        binary = self.normal if stimulus["adapter"] == "normal-unsupported" else self.modeled
        self.assertIn(stimulus["adapter"], ("modeled", "normal-unsupported"))
        command = [str(binary), stimulus["mode"], str(stimulus["index"]), stimulus["field"],
                   str(stimulus["value"]), stimulus["handler"], str(int(stimulus["repeat"]))]
        # The existing launcher retains command, streams, hashes, EOF, publication,
        # exact born direct-child identity and its consuming wait on every path.
        data = self.execute(command)
        self.assertTrue(data.startswith(b"TS_RESULT "), case["name"])
        self.assertEqual(data.count(b"\n"), 1, case["name"])
        result = json.loads(data[len(b"TS_RESULT "):])
        self.assertEqual(set(result), FIELDS, case["name"])
        for key in ("events", "repeat_events"):
            self.assertLessEqual(len(result[key]), 128)
            for event in result[key]:
                self.assertEqual(set(event), {"call", "outcome", "values"})
                self.assertIs(type(event["outcome"]), int)
                self.assertEqual(len(event["values"]), 4)
                self.assertTrue(all(type(n) is int for n in event["values"]))
        self.assertEqual(len(result["limits"]), 6)
        self.assertTrue(all(len(bounds) == 2 for bounds in result["limits"]))
        self.assertEqual(len(result["timers"]), 2)
        self.assertTrue(all(len(timer) == 4 for timer in result["timers"]))
        return result

    def check(self, case):
        observed, expected, stimulus = self.run_case(case), case["expected"], case["stimulus"]
        events = observed["events"]
        calls = [event["call"] for event in events]
        self.assertEqual(observed["success"], expected["success"], case["name"])
        self.assertEqual(observed["fatal_status"], -1 if expected["fatal_status"] is None else expected["fatal_status"])
        self.assertEqual(observed["repeat_result"], -1 if expected["repeat_result"] is None else expected["repeat_result"])
        self.assertEqual(observed["repeat_mutations"], expected["repeat_mutations"] or 0)
        if expected["repeat_observations"] == "identity-only":
            self.assertTrue(all(e["call"] in IDENTITY for e in observed["repeat_events"]))
        else:
            self.assertFalse(observed["repeat_events"])
        if stimulus["adapter"] == "normal-unsupported":
            self.assertEqual(observed["host_unchanged"], 1)
            self.assertEqual(observed["readonly_checks"], 30)
            self.assertFalse(events)
        else:
            self.assertIsNone(observed["host_unchanged"])
            self.assertEqual(observed["readonly_checks"], 0)

        # Constraints come from literal independent cases. No observed success is
        # used as a baseline or to generate expected failure/prefix data.
        sets = [e for e in events if e["call"].startswith("limit-set:")]
        self.assertEqual(tuple(e["call"].split(":")[1] for e in sets), expected["set_resource_prefix"])
        self.assertEqual(tuple((e["call"].split(":")[1], *e["values"][:2]) for e in sets), expected["set_bounds"])
        set_values = {e["call"].split(":")[1]: e["values"][:2] for e in sets}
        verified = tuple(e["call"].split(":")[1] for e in events
            if e["call"].startswith("limit-get:") and e["call"].endswith(":final") and
            e["outcome"] == 0 and e["values"][:2] == set_values.get(e["call"].split(":")[1]))
        self.assertEqual(verified, expected["verified_resource_prefix"])
        initial = [e for e in events if e["call"].startswith("timer-get:") and e["call"].endswith(":initial")]
        self.assertEqual(len(initial), expected["initial_timer_checks"])
        self.assertEqual(tuple(vectors.ORACLE["signal_order"][i] for i, flag in enumerate(observed["unblocked"]) if flag),
                         expected["unblocked"])
        if expected["signal_roles"] is not None:
            self.assertEqual(tuple(observed["handlers"]), expected["signal_roles"])
            actions = [e for e in events if e["call"].startswith("action:")]
            self.assertEqual(tuple(e["call"].split(":")[1] for e in actions), vectors.ORACLE["signal_order"])
            self.assertEqual(tuple(e["values"][1] for e in actions), expected["signal_flags"])
            self.assertEqual(tuple(() if not e["values"][2] else ("nonempty",) for e in actions),
                             expected["signal_handler_masks"])
            self.assertTrue(all(e["values"][0] == role for e, role in zip(actions, (1, 1, 1, 1, 2, 3))))
        for event in events:
            if event["call"] == "unblock":
                self.assertEqual(event["values"], [1, 63, 0, 0])
        timer_sets = [e for e in events if e["call"].startswith("timer-set:")]
        targets = {name: list(values) for name, *values in expected["timer_caps"]}
        for event in timer_sets:
            self.assertEqual(event["values"], targets[event["call"].split(":")[1]])
        if expected.get("timer_readbacks"):
            final = [e for e in events if e["call"].startswith("timer-get:") and e["call"].endswith(":final")]
            self.assertEqual(len(final), 2)
            for event in final:
                seconds, micros, interval_s, interval_us = event["values"]
                cap = targets[event["call"].split(":")[1]][0]
                self.assertEqual(event["outcome"], 0)
                self.assertGreater(seconds * 1000000 + micros, 0)
                self.assertGreaterEqual(seconds, 0)
                self.assertTrue(0 <= micros < 1000000)
                self.assertLessEqual(seconds * 1000000 + micros, cap * 1000000)
                self.assertEqual((interval_s, interval_us), (0, 0))
        if expected.get("mutations") is not None:
            self.assertEqual(observed["mutations"], expected["mutations"])
        self.assertEqual(observed["mutations"], sum(e["call"].startswith(("action:", "limit-set:", "timer-set:")) or
                                                  e["call"] == "unblock" for e in events))
        if expected.get("timers_not_armed"):
            self.assertFalse(timer_sets)
        if expected.get("limits_not_observed"):
            self.assertFalse(any(e["call"].startswith("limit-") for e in events))
        if expected.get("no_actions_after_signal_index") is not None:
            limit = expected["signal_failure_index"]
            self.assertTrue(all(vectors.ORACLE["signal_order"].index(e["call"].split(":")[1]) <= limit
                                for e in events if e["call"].startswith(("action:", "add:"))))
        self.check_order_and_terminal(case, observed)

    def check_order_and_terminal(self, case, observed):
        expected, stimulus, events = case["expected"], case["stimulus"], observed["events"]
        calls = [e["call"] for e in events]
        ranks = []
        for call in calls:
            if call in IDENTITY:
                ranks.append(0)
            elif call.startswith("timer-get:") and call.endswith(":initial"):
                ranks.append(1)
            elif call.startswith(("empty:", "action:", "add:")):
                ranks.append(2)
            elif call.startswith(("limit-get:", "limit-set:")):
                ranks.append(3)
            elif call == "unblock":
                ranks.append(4)
            elif call.startswith(("timer-set:", "timer-get:")):
                ranks.append(5)
            elif call == "exit":
                ranks.append(6)
            else:
                self.fail("unexpected modeled call " + call)
        self.assertEqual(ranks, sorted(ranks), case["name"])
        if expected["no_restoration_or_retry"]:
            self.assertTrue(all(count == 1 for call, count in Counter(calls).items() if call not in IDENTITY))
        failure = expected["failure_call"]
        if failure is not None:
            if failure.startswith("limit-readback-mismatch:"):
                terminal = "limit-get:" + failure.split(":")[1] + ":final"
            elif failure.startswith("limit-invalid-zero:"):
                terminal = "limit-get:" + failure.split(":")[1] + ":initial"
            elif failure.startswith("timer-inherited-refusal:"):
                terminal = "timer-get:" + failure.split(":")[1] + ":initial"
            elif failure.startswith("timer-readback-refusal:"):
                terminal = "timer-get:" + failure.split(":")[1] + ":final"
            else:
                terminal = failure
            self.assertTrue(calls)
            self.assertEqual(calls[-1], terminal, case["name"])
            if failure == terminal:
                self.assertEqual(events[-1]["outcome"], -1)
            else:
                self.assertEqual(events[-1]["outcome"], 0)
            if failure.startswith("limit-readback-mismatch:"):
                key = "limit-set:" + failure.split(":")[1]
                wanted = next(e["values"][:2] for e in events if e["call"] == key)
                self.assertNotEqual(events[-1]["values"][:2], wanted)
            if failure.startswith("limit-invalid-zero:"):
                self.assertEqual(events[-1]["values"][0], 0)
        # Every successfully completed disposition stage installs/adds all six;
        # failed signal cases retain only their independently specified prefix.
        signal_order = vectors.ORACLE["signal_order"]
        actions = [e for e in events if e["call"].startswith("action:")]
        additions = [e for e in events if e["call"].startswith("add:")]
        if expected["signal_roles"] is not None:
            self.assertEqual(tuple(e["call"].split(":")[1] for e in additions), signal_order)
        elif expected["stage"] == "signal-setup":
            if failure.startswith("empty:"):
                self.assertFalse(actions)
                self.assertFalse(additions)
            else:
                index = expected["signal_failure_index"]
                self.assertEqual(tuple(e["call"].split(":")[1] for e in actions), signal_order[:index + 1])
                self.assertEqual(tuple(e["call"].split(":")[1] for e in additions),
                                 signal_order[:index + (1 if failure.startswith("add:") else 0)])
        role_numbers = (1, 1, 1, 1, 2, 3)
        for action in actions:
            self.assertEqual(action["values"], [role_numbers[signal_order.index(action["call"].split(":")[1])], 0, 0, 0])
        if expected["stage"] == "identity":
            self.assertTrue(all(call in IDENTITY for call in calls))
        if expected.get("handler_has_no_cleanup_or_output"):
            self.assertEqual(calls[-1], "exit")
            self.assertEqual(events[-1]["values"], [expected["fatal_status"], 0, 0, 0])
        # Every successful set is reflected in the modeled process; failed sets
        # preserve inherited/previous state. Readback faults alter only the sample.
        resources = vectors.ORACLE["resource_order"]
        state = [[-1, -1] for _ in resources]
        if stimulus["mode"] == "limit-inherited":
            index, value = stimulus["index"], stimulus["value"]
            if stimulus["field"] in ("soft", "both"):
                state[index][0] = value
            if stimulus["field"] in ("hard", "both"):
                state[index][1] = value
        for event in events:
            if event["call"].startswith("limit-set:") and event["outcome"] == 0:
                state[resources.index(event["call"].split(":")[1])] = event["values"][:2]
        self.assertEqual(observed["limits"], state)
        timer_state = [[0, 0, 0, 0], [0, 0, 0, 0]]
        if stimulus["mode"] == "timer-initial-field":
            timer_state[stimulus["index"]][("value_sec", "value_usec", "interval_sec", "interval_usec").index(stimulus["field"])] = stimulus["value"]
        armed = set()
        timer_order = vectors.ORACLE["timer_order"]
        for event in events:
            if event["call"].startswith("timer-set:") and event["outcome"] == 0:
                name = event["call"].split(":")[1]
                timer_state[timer_order.index(name)] = event["values"]
                armed.add(name)
            if event["call"].startswith("timer-get:") and event["call"].endswith(":final"):
                self.assertIn(event["call"].split(":")[1], armed)
        self.assertEqual(observed["timers"], timer_state)

    def test_independent_finite_guard_corpus(self):
        self.assertEqual(len(vectors.CASES), vectors.ORACLE["case_count"])
        self.assertLessEqual(len(vectors.CASES), 180)
        for case in vectors.CASES:
            with self.subTest(case=case["name"]):
                self.check(case)

    def test_closed_production_symbols_cli_and_fully_charged_source(self):
        undefined = self.execute(["nm", "-u", str(self.plain)]).decode()
        names = {row.split()[-1] for row in undefined.splitlines()}
        self.assertLessEqual({n for n in names if "asan" not in n and "ubsan" not in n}, {
            "_bw_sv_supported", "_bw_material_reserve_started", "_clock_gettime", "_getuid", "_geteuid", "_getgid", "_getegid", "_getitimer", "_setitimer",
            "_bw_entry_boundary", "_bw_auth_capture_reserved", "_bw_authenticated_output", "_bw_verifier_release",
            "_bw_sv_closefrom", "_bw_sv_parent_boundary", "_fcntl", "_close", "___error",
            "_getrlimit", "_setrlimit", "_sigemptyset", "_sigaddset", "_sigaction", "_sigprocmask",
            "__exit", "_memset", "_bzero", "_memcpy", "___stack_chk_fail", "___stack_chk_guard"})
        exported = self.execute(["nm", "-gU", str(self.plain)]).decode()
        self.assertEqual({row.split()[-1] for row in exported.splitlines()
                          if "asan" not in row and "ubsan" not in row}, {"_bw_process_guard", "_bw_start_reserve", "_bw_entry_output", "_bw_entry_observe"})
        makefile = (ROOT / "Makefile").read_text()
        self.assertEqual(next(row for row in makefile.splitlines() if row.startswith("PLAN_SOURCES =")),
                         "PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c")
        self.assertLessEqual(sum(p.stat().st_size for p in (ROOT / "src").rglob("*")
                                 if p.suffix in (".c", ".h")), 96 * 1024)
        self.assertLessEqual(sum((ROOT / "src" / n).stat().st_size for n in
            ("trusted_start.c", "trusted_start.h", "entry_platform.c", "private_limits.h", "private_start_material.h")), 10 * 1024)
        self.assertLessEqual(sum((ROOT / "src" / n).stat().st_size for n in
            ("signature_verify.c", "signature_verify.h", "signature_platform.c", "private_limits.h")), 18 * 1024)
        self.assertLessEqual(sum((ROOT / "src" / n).stat().st_size for n in
            ("authenticated_review.c", "authenticated_review.h", "private_span.h", "private_content_format.h")), 16 * 1024)
