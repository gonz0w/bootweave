/* SPDX-License-Identifier: BSD-3-Clause */
#include "plan.h"
#include <unistd.h>
#ifdef __OpenBSD__
#include <sys/resource.h>
#include <stdlib.h>

static int
limit(int resource, rlim_t ceiling)
{
    struct rlimit bounds;
    if (getrlimit(resource, &bounds) == -1)
        return 0;
    if (bounds.rlim_max > ceiling)
        bounds.rlim_max = ceiling;
    if (bounds.rlim_cur > bounds.rlim_max)
        bounds.rlim_cur = bounds.rlim_max;
    return setrlimit(resource, &bounds) == 0;
}

int
bw_confine(void)
{
    if (getuid() == 0 || getuid() != geteuid() || getgid() != getegid() ||
        issetugid())
        return 0;
    closefrom(3);
    if (!limit(RLIMIT_DATA, 16 * 1024 * 1024) ||
        !limit(RLIMIT_STACK, 2 * 1024 * 1024) || !limit(RLIMIT_NOFILE, 8) ||
        !limit(RLIMIT_CORE, 0) || !limit(RLIMIT_FSIZE, 8192) ||
        !limit(RLIMIT_CPU, 2))
        return 0;
    return unveil("/", "") == 0 && unveil(NULL, NULL) == 0 &&
        pledge("stdio", NULL) == 0;
}
#else
int
bw_confine(void)
{
    return 0;
}
#endif
