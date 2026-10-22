/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_QUALIFICATION_TRACE_LIMITS_H
#define BOOTWEAVE_QUALIFICATION_TRACE_LIMITS_H
struct rlimit;

/* Test-only paired FSIZE interposer; not production limits or exact-binary
 * evidence. Rename calls only in the two companion limit objects. Compile
 * trace_limits.c separately WITHOUT those renames so its calls reach the OS.
 * A dedicated single-threaded process gets one parent get/set/get sequence;
 * its direct fork copy gets one child get/set/get sequence. No reset/selector,
 * signal-handler calls or extra virtual readbacks. Other resources pass through.
 * FSIZE policy/state refusals use EINVAL and are terminal. OS errors retain
 * errno; successful virtual calls restore incoming errno. Every virtual get
 * checks real limits. Lower inherited actual ceilings are never raised.
 * Host execution requires separately reviewed complete syscall/PID mocks.
 */
int bw_nq_trace_getrlimit(int, struct rlimit *);
int bw_nq_trace_setrlimit(int, const struct rlimit *);

#endif
