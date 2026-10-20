/* SPDX-License-Identifier: BSD-3-Clause */
#ifdef __OpenBSD__
#define _BSD_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#include "signature_verify.h"
#include <unistd.h>
#ifdef __OpenBSD__
#include "private_limits.h"
#endif

int bw_sv_supported(void) {
#ifdef __OpenBSD__
    return !issetugid();
#else
    return 0;
#endif
}

int bw_sv_child_limits(void) {
#ifdef __OpenBSD__
    static const rlim_t caps[] = {2, 32*1024*1024, 2*1024*1024, 16, 0, 0};
    return fixed_limits(caps);
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
