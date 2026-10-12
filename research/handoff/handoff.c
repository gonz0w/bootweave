/* SPDX-License-Identifier: BSD-3-Clause */
#include "snapshot.h"
#include <stdio.h>
#ifdef __OpenBSD__
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>
int
main(int argc, char **argv)
{
    struct handoff_snapshot reviewed;
    struct rusage usage;
    char *const arguments[] = { "install", NULL };
    char *const environment[] = {
        "PATH=/sbin:/bin:/usr/bin:/usr/sbin:/", "VNAME=7.9", "VERSION=79",
        "ARCH=amd64", "TERM=vt220", "LANG=C", NULL
    };
    int result;
    (void)argv;
    if (argc != 1) return 2;
    alarm(10);
    result = handoff_observe(&reviewed);
    alarm(0);
    if (result != STORAGE_VALID) {
        puts("No qualified research target; no handoff.");
        return 2;
    }
    result = handoff_console(&reviewed, handoff_observe);
    if (result != 0) return result;
    if (getrusage(RUSAGE_SELF, &usage) || usage.ru_maxrss > 32768 ||
        chdir("/") || handoff_interrupted()) return 3;
    closefrom(3);
    alarm(0);
    if (signal(SIGPIPE, SIG_DFL) == SIG_ERR || handoff_interrupted()) return 3;
    execve("/install", arguments, environment);
    puts("Stock entry failed; no automatic retry.");
    return 3;
}
#else
int
main(int argc, char **argv)
{
    (void)argv;
    if (argc != 1) return 2;
    puts("Native OpenBSD research required; no handoff.");
    return 77;
}
#endif
