/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef __OpenBSD__
int bw_native_faults_host_placeholder;
#else
#include <sys/resource.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Only a separately compiled confinement test object calls these wrappers. */
static int
selected(const char *mode)
{
    const char *value = getenv("BW_TEST_GATE");
    return value != NULL && strcmp(value, mode) == 0;
}

int
bw_test_unveil(const char *path, const char *permissions)
{
    if (selected(path == NULL ? "lock" : "unveil")) {
        errno = EPERM;
        return -1;
    }
    return unveil(path, permissions);
}

int
bw_test_pledge(const char *promises, const char *execpromises)
{
    if (selected("pledge")) {
        errno = EPERM;
        return -1;
    }
    return pledge(promises, execpromises);
}

int
bw_test_setrlimit(int resource, const struct rlimit *bounds)
{
    const int resources[] = {RLIMIT_DATA, RLIMIT_STACK, RLIMIT_NOFILE,
        RLIMIT_CORE, RLIMIT_FSIZE, RLIMIT_CPU};
    const char *const modes[] = {"data", "stack", "nofile", "core", "fsize", "cpu"};
    size_t i;
    for (i = 0; i < sizeof(resources) / sizeof(resources[0]); i++)
        if (resource == resources[i] && selected(modes[i])) {
            errno = EPERM;
            return -1;
        }
    return setrlimit(resource, bounds);
}

int
bw_test_issetugid(void)
{
    return selected("ugid") ? 1 : issetugid();
}
#endif
