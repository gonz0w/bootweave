/* SPDX-License-Identifier: BSD-3-Clause */
/* Test evidence only. Never link either object into the production planner. */
#include <sys/resource.h>
#include <errno.h>
#include <stddef.h>

#ifdef BW_TRACE_POLICY
int
bw_trace_setrlimit(int resource, const struct rlimit *requested)
{
    struct rlimit bounds;
    if (resource != RLIMIT_FSIZE)
        return setrlimit(resource, requested);
    if (requested == NULL || requested->rlim_cur != 8192 || requested->rlim_max != 8192) {
        errno = EINVAL;
        return -1;
    }
    bounds.rlim_cur = bounds.rlim_max = 4 * 1024 * 1024;
    return setrlimit(resource, &bounds);
}
#else
#ifdef __OpenBSD__
#include <sys/types.h>
#include <sys/ktrace.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static const char *trace_path;
static int trace_fd = -1;
static struct stat trace_identity;
static const int facilities = KTRFAC_SYSCALL | KTRFAC_SYSRET | KTRFAC_PSIG | KTRFAC_INHERIT;

static int
trace_bounds(void)
{
    struct stat held, named;
    if (fstat(trace_fd, &held) || lstat(trace_path, &named))
        return 0;
    return S_ISREG(held.st_mode) && held.st_mode == trace_identity.st_mode &&
        held.st_uid == 0 && held.st_gid == 0 && held.st_nlink == 1 &&
        held.st_dev == trace_identity.st_dev && held.st_ino == trace_identity.st_ino &&
        named.st_dev == held.st_dev && named.st_ino == held.st_ino &&
        named.st_mode == held.st_mode && named.st_nlink == 1 &&
        named.st_uid == 0 && named.st_gid == 0 &&
        held.st_size >= 0 && held.st_size <= 4 * 1024 * 1024 - 4096;
}

int
bw_trace_prepare(const char *path)
{
    if (trace_fd != -1 || getuid() != 0)
        return 0;
    trace_path = path;
    trace_fd = open(path, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
    if (trace_fd < 0 || fstat(trace_fd, &trace_identity) ||
        (trace_identity.st_mode & 07777) != 0600 || !trace_bounds())
        return 0;
    return 1;
}

int
bw_trace_before(void)
{
    return trace_bounds() && ktrace(trace_path, KTROP_SET, facilities, getpid()) == 0;
}

int
bw_trace_after_fork(void)
{
    /* Clear this supervisor only. The child's inherited trace must remain. */
    return ktrace(NULL, KTROP_CLEAR, facilities, getpid()) == 0;
}

int
bw_trace_after(void)
{
    return trace_bounds();
}

int
bw_trace_finish(void)
{
    int ok = trace_bounds();
    if (close(trace_fd))
        ok = 0;
    trace_fd = -1;
    return ok;
}
#endif
#endif
