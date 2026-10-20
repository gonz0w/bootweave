/* SPDX-License-Identifier: BSD-3-Clause */
#include "plan.h"
#include "private_cli.h"

#include <string.h>
#include <unistd.h>

static int
message(int fd, const char *text, int status)
{
    return bw_write(fd, text, strlen(text)) ? status : 5;
}

int
bw_legacy_cli(int argc, char **argv)
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
    if (!bw_confine())
        return message(2, "bootweave: runtime refused\n", 4);
    return bw_run();
#endif
}

int
main(int argc, char **argv)
{
    return bw_legacy_cli(argc, argv);
}
