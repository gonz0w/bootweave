/* SPDX-License-Identifier: BSD-3-Clause */
#include "plan.h"

#include <string.h>
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

static int
confine(void)
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
#endif

static int
message(int fd, const char *text, int status)
{
    return bw_write(fd, text, strlen(text)) ? status : 5;
}

int
main(int argc, char **argv)
{
    if (!bw_start())
        return 4;
    if (argc == 2 && strcmp(argv[1], "--help") == 0)
        return message(1, "Usage: bootweave plan < project.json\n"
            "       bootweave --help | --version\n"
            "Read-only unverified planner; OpenBSD only.\n", 0);
    if (argc == 2 && strcmp(argv[1], "--version") == 0)
        return message(1, "bootweave planner 0.1 (definition version 1)\n", 0);
    if (argc != 2 || strcmp(argv[1], "plan") != 0)
        return message(2, "bootweave: invalid invocation; use --help\n", 2);
#ifndef __OpenBSD__
    return message(2, "bootweave: planning requires OpenBSD\n", 77);
#else
    if (!confine())
        return message(2, "bootweave: runtime refused\n", 4);
    return bw_run();
#endif
}
