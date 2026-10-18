/* SPDX-License-Identifier: BSD-3-Clause */
/* Independent test of the trace-only substitution and real kernel forwarding. */
#include <sys/resource.h>
#include <errno.h>
#include <stdio.h>

int bw_trace_setrlimit(int, const struct rlimit *);

int
main(void)
{
    struct rlimit before, after, requested;
    if (getrlimit(RLIMIT_FSIZE, &before))
        return 1;
    requested.rlim_cur = 8191;
    requested.rlim_max = 8192;
    errno = 0;
    if (bw_trace_setrlimit(RLIMIT_FSIZE, &requested) != -1 || errno != EINVAL ||
        getrlimit(RLIMIT_FSIZE, &after) || before.rlim_cur != after.rlim_cur ||
        before.rlim_max != after.rlim_max)
        return 2;
    requested.rlim_cur = 8192;
    requested.rlim_max = 8193;
    if (bw_trace_setrlimit(RLIMIT_FSIZE, &requested) != -1 ||
        bw_trace_setrlimit(RLIMIT_FSIZE, NULL) != -1)
        return 3;
    requested.rlim_cur = requested.rlim_max = 8192;
    if (bw_trace_setrlimit(RLIMIT_FSIZE, &requested) || getrlimit(RLIMIT_FSIZE, &after) ||
        after.rlim_cur != 4 * 1024 * 1024 || after.rlim_max != 4 * 1024 * 1024)
        return 4;
    requested.rlim_cur = requested.rlim_max = 0;
    if (bw_trace_setrlimit(RLIMIT_CORE, &requested) || getrlimit(RLIMIT_CORE, &after) ||
        after.rlim_cur != 0 || after.rlim_max != 0)
        return 5;
    requested.rlim_cur = 1;
    if (bw_trace_setrlimit(RLIMIT_CORE, &requested) != -1)
        return 6;
    puts("Trace-only limits checked; production FSIZE remains separate.");
    return 0;
}
