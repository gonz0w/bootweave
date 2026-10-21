/* SPDX-License-Identifier: BSD-3-Clause */
/* Disclosed companion-only enforcement AND readback substitution. */
#define _POSIX_C_SOURCE 200809L
#include <sys/resource.h>
#include "trace_limits.h"
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

enum trace_phase {
    PARENT_BEFORE, PARENT_SET, PARENT_AFTER, PARENT_DONE,
    CHILD_BEFORE, CHILD_SET, CHILD_AFTER, CHILD_DONE, FAILED
};
enum { PARENT_BYTES = 65536, TRACE_BYTES = 4 * 1024 * 1024 };

static enum trace_phase phase;
static pid_t owner;
static struct rlimit before;

static int
refuse(int error)
{
    phase = FAILED;
    errno = error;
    return -1;
}

static int
process(void)
{
    pid_t current = getpid();

    if (phase == FAILED || current <= 0) return refuse(EINVAL);
    if (!owner) {
        if (phase != PARENT_BEFORE) return refuse(EINVAL);
        owner = current;
    } else if (owner != current) {
        /* A fork copies completed parent state. This checks that relationship;
         * it grants no custody and cannot count sibling forks globally. The
         * real signature stage and independent observer own the one-child proof.
         */
        if (phase != PARENT_DONE || getppid() != owner) return refuse(EINVAL);
        owner = current;
        phase = CHILD_BEFORE;
    }
    return 0;
}

static int
same(const struct rlimit *a, const struct rlimit *b)
{
    return a->rlim_cur == b->rlim_cur && a->rlim_max == b->rlim_max;
}

static int
fixed(const struct rlimit *limit)
{
    return limit->rlim_cur == TRACE_BYTES && limit->rlim_max == TRACE_BYTES;
}

static int
inherited(const struct rlimit *limit)
{
    return (limit->rlim_cur == RLIM_INFINITY || limit->rlim_cur >= TRACE_BYTES) &&
        (limit->rlim_max == RLIM_INFINITY || limit->rlim_max >= TRACE_BYTES) &&
        (limit->rlim_max == RLIM_INFINITY ||
         (limit->rlim_cur != RLIM_INFINITY && limit->rlim_cur <= limit->rlim_max));
}

int
bw_nq_trace_getrlimit(int resource, struct rlimit *limit)
{
    struct rlimit seen;
    enum trace_phase next;
    rlim_t reported;
    int saved = errno;

    if (resource != RLIMIT_FSIZE) return getrlimit(resource, limit);
    if (!limit || process() == -1) return refuse(EINVAL);
    switch (phase) {
    case PARENT_BEFORE:
        next = PARENT_SET;
        reported = 0; /* Initial parent observation is real, not virtual. */
        break;
    case PARENT_AFTER:
        next = PARENT_DONE;
        reported = PARENT_BYTES;
        break;
    case CHILD_BEFORE:
        next = CHILD_SET;
        reported = PARENT_BYTES; /* Copied parent's original requested cap. */
        break;
    case CHILD_AFTER:
        next = CHILD_DONE;
        reported = 0;
        break;
    default:
        return refuse(EINVAL);
    }
    if (getrlimit(resource, &seen) != 0) return refuse(errno);
    if (phase == PARENT_BEFORE) {
        if (!inherited(&seen)) return refuse(EINVAL);
        before = seen;
        *limit = seen;
    } else {
        if (!fixed(&seen)) return refuse(EINVAL);
        if (phase == CHILD_BEFORE) before = seen;
        limit->rlim_cur = limit->rlim_max = reported;
    }
    phase = next;
    errno = saved;
    return 0;
}

int
bw_nq_trace_setrlimit(int resource, const struct rlimit *requested)
{
    const struct rlimit actual = {TRACE_BYTES, TRACE_BYTES};
    struct rlimit seen;
    enum trace_phase next;
    rlim_t expected;
    int saved = errno;

    if (resource != RLIMIT_FSIZE) return setrlimit(resource, requested);
    if (!requested || process() == -1) return refuse(EINVAL);
    if (phase == PARENT_SET) {
        expected = PARENT_BYTES;
        next = PARENT_AFTER;
    } else if (phase == CHILD_SET) {
        expected = 0;
        next = CHILD_AFTER;
    } else return refuse(EINVAL);
    if (requested->rlim_cur != expected || requested->rlim_max != expected)
        return refuse(EINVAL);
    /* Recheck immediately before the real setter: do not raise a limit that
     * changed after the initial observation, even within this test process.
     * Any setter/readback failure is terminal; never restore or retry it.
     */
    phase = FAILED;
    if (getrlimit(resource, &seen) != 0) return refuse(errno);
    if (!same(&seen, &before)) return refuse(EINVAL);
    if (setrlimit(resource, &actual) != 0) return refuse(errno);
    if (getrlimit(resource, &seen) != 0) return refuse(errno);
    if (!fixed(&seen)) return refuse(EINVAL);
    phase = next;
    errno = saved;
    return 0;
}
