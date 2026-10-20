/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_PRIVATE_LIMITS_H
#define BOOTWEAVE_PRIVATE_LIMITS_H
#include <sys/resource.h>

/* Clamp six fixed limits; verify readback. */
static int
fixed_limits(const rlim_t *caps)
{
    static const int kinds[] = {RLIMIT_CPU, RLIMIT_DATA, RLIMIT_STACK,
        RLIMIT_NOFILE, RLIMIT_CORE, RLIMIT_FSIZE};
    struct rlimit limit, seen;
    unsigned i;
    for (i = 0; i < 6; i++) {
        if (getrlimit(kinds[i], &limit) == -1) return -1;
        if (limit.rlim_cur == RLIM_INFINITY || limit.rlim_cur > caps[i]) limit.rlim_cur = caps[i];
        if (limit.rlim_max == RLIM_INFINITY || limit.rlim_max > caps[i]) limit.rlim_max = caps[i];
        if ((i < 4 && !limit.rlim_cur) || setrlimit(kinds[i], &limit) == -1 ||
            getrlimit(kinds[i], &seen) == -1 || seen.rlim_cur != limit.rlim_cur ||
            seen.rlim_max != limit.rlim_max) return -1;
    }
    return 0;
}
#endif
