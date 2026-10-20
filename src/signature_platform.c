/* SPDX-License-Identifier: BSD-3-Clause */
#ifdef __OpenBSD__
#define _BSD_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#include "signature_verify.h"
#include <unistd.h>

int bw_sv_supported(void) {
#ifdef __OpenBSD__
    return !issetugid();
#else
    return 0;
#endif
}

int bw_sv_child_limits(void) {
#ifdef __OpenBSD__
    static const int kinds[] = {RLIMIT_CPU, RLIMIT_DATA, RLIMIT_STACK, RLIMIT_NOFILE, RLIMIT_CORE, RLIMIT_FSIZE};
    static const rlim_t caps[] = {2, 32*1024*1024, 2*1024*1024, 16, 0, 0};
    struct rlimit limit, seen;
    size_t i;
    for (i = 0; i < sizeof(kinds)/sizeof(kinds[0]); i++) {
        if (getrlimit(kinds[i], &limit) == -1) return -1;
        if (limit.rlim_cur == RLIM_INFINITY || limit.rlim_cur > caps[i]) limit.rlim_cur = caps[i];
        if (limit.rlim_max == RLIM_INFINITY || limit.rlim_max > caps[i]) limit.rlim_max = caps[i];
        if ((i < 4 && !limit.rlim_cur) || setrlimit(kinds[i], &limit) == -1 ||
            getrlimit(kinds[i], &seen) == -1 || seen.rlim_cur != limit.rlim_cur ||
            seen.rlim_max != limit.rlim_max) return -1;
    }
    return 0;
#else
    return -1;
#endif
}

int bw_sv_child_boundary(void) {
#ifdef __OpenBSD__
    return pledge("stdio rpath exec", "stdio rpath");
#else
    return -1;
#endif
}

int bw_sv_parent_boundary(int final) {
#ifdef __OpenBSD__
    return pledge(final ? "stdio" : "stdio proc", NULL);
#else
    (void)final;
    return -1;
#endif
}

int bw_sv_closefrom(int low) {
#ifdef __OpenBSD__
    return closefrom(low);
#else
    (void)low;
    return -1;
#endif
}
