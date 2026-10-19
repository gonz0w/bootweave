# SPDX-License-Identifier: BSD-3-Clause
import unittest

from trace_evidence import check_trace


def evidence():
    lines = []
    receipts = []
    for index, pid in enumerate((101, 102)):
        lines.extend([f"1 trace-check CALL fork()", f"1 trace-check RET fork {pid}",
                      f"{pid} trace-bin RET execve JUSTRETURN"])
        calls = [("sigaction", "SIGALRM,0x1,0"), ("setitimer", "ITIMER_REAL,0x1,0"), ("setitimer", "ITIMER_PROF,0x1,0"),
                 ("closefrom", "3")]
        calls += [("setrlimit", f"RLIMIT_{name},0x1") for name in
                  ("DATA", "STACK", "NOFILE", "CORE", "FSIZE", "CPU")]
        calls += [("unveil", "0x1,0x2"), ("unveil", "0,0"), ("pledge", "0x1,0")]
        for name, args in calls:
            lines += [f"{pid} trace-bin CALL {name}({args})", f"{pid} trace-bin RET {name} 0"]
        lines += [f"{pid} trace-bin CALL read(0,0x1,16)", f"{pid} trace-bin RET read 16",
                  f"{pid} trace-bin CALL exit(0)"]
        receipts.append([index, pid, 0, 1, 0, 0, 100, 0, 0])
    return "\n".join(lines) + "\n", receipts


class TraceEvidenceTests(unittest.TestCase):
    def test_complete_sequences_and_injected_missing_evidence(self):
        decoded, receipts = evidence()
        self.assertEqual(len(check_trace(decoded, receipts, 1, positive=True)), 2)
        for changed in [decoded.replace("101 trace-bin RET pledge 0\n", ""),
                        decoded.replace("101 trace-bin CALL exit(0)\n", ""),
                        decoded.replace("101 trace-bin CALL exit(0)", "101 trace-bin CALL exit(3)"),
                        decoded.replace("101 trace-bin CALL read", "999 trace-bin CALL read"),
                        decoded.replace("101 trace-bin CALL exit(0)",
                                        "101 trace-bin CALL open(0x1,0)\n101 trace-bin CALL exit(0)"),
                        decoded + "malformed record\n"]:
            with self.assertRaises(ValueError):
                check_trace(changed, receipts, 1, positive=True)

    def test_all_edge_pids_and_disallowed_input_failure(self):
        positive, _ = evidence()
        sample = [line for line in positive.splitlines() if line.startswith("101 ")]
        lines, receipts = [], []
        for index in range(46):
            pid = 200 + index
            edge = index % 23
            lines += ["1 trace-check CALL fork()", f"1 trace-check RET fork {pid}"]
            runtime = sample if edge < 15 else ["101 trace-bin RET execve JUSTRETURN",
                                               "101 trace-bin CALL exit(0)"]
            runtime = [line.replace("101 ", f"{pid} ", 1) for line in runtime]
            if edge < 10:
                runtime = [line for line in runtime if " read" not in line]
            if edge == 14:
                runtime[-1] = f"{pid} trace-bin PSIG SIGTERM SIG_DFL code=SI_USER"
            if edge >= 20:
                runtime[1:1] = [f"{pid} trace-bin CALL sigaction(SIGALRM,0x1,0)",
                                f"{pid} trace-bin RET sigaction 0"]
            if edge == 7:
                runtime[1:1] = [f"{pid} trace-bin CALL open(0x1,0)",
                                f"{pid} trace-bin RET open 0",
                                f"{pid} trace-bin CALL read(0,0x1,16)",
                                f"{pid} trace-bin RET read 16",
                                f"{pid} trace-bin CALL close(0)",
                                f"{pid} trace-bin RET close 0"]
                runtime[-1:-1] = [f"{pid} trace-bin CALL fstat(0,0x1)",
                                  f"{pid} trace-bin RET fstat -1 errno 9 Bad file descriptor"]
            lines.extend(runtime)
            receipts.append([index, pid, 15 if edge == 14 else 0, 1, 0, 0, 100, 0, 0])
        decoded = "\n".join(lines) + "\n"
        self.assertEqual(len(check_trace(decoded, receipts, 1)), 46)
        changed = decoded.replace("200 trace-bin CALL exit(0)",
                                  "200 trace-bin CALL read(0,0x1,1)\n200 trace-bin CALL exit(0)")
        with self.assertRaises(ValueError):
            check_trace(changed, receipts, 1)
        for changed in [decoded.replace("207 trace-bin RET close 0\n", ""),
                        decoded.replace("207 trace-bin RET open 0", "207 trace-bin RET open 3"),
                        decoded.replace("207 trace-bin RET fstat -1 errno 9 Bad file descriptor",
                                        "207 trace-bin RET fstat 0")]:
            with self.assertRaises(ValueError):
                check_trace(changed, receipts, 1)

    def test_disallowed_input_and_pid_correlation(self):
        decoded, receipts = evidence()
        with self.assertRaises(ValueError):
            check_trace(decoded, receipts, 3, positive=True)
        with self.assertRaises(ValueError):
            check_trace(decoded, receipts + receipts, 1, positive=True)
        with self.assertRaises(ValueError):
            check_trace(decoded + "x" * 1048576, receipts, 1, positive=True)


if __name__ == "__main__":
    unittest.main()
