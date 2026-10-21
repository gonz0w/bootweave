/* SPDX-License-Identifier: BSD-3-Clause */
#ifdef __OpenBSD__
#define _BSD_SOURCE
#endif
#include "private_entry.h"
#include <unistd.h>

int
bw_en_identity(uid_t *owner)
{
#ifdef __OpenBSD__
    uid_t uid;
    gid_t gid;
    if (!owner || issetugid()) return 0;
    uid = getuid(); gid = getgid();
    if (!uid || uid != geteuid() || gid != getegid()) return 0;
    *owner = uid;
    return 1;
#else
    (void)owner;
    return 0;
#endif
}

int
bw_en_unveil(const char *path, const char *permissions)
{
#ifdef __OpenBSD__
    return unveil(path, permissions);
#else
    (void)path; (void)permissions;
    return -1;
#endif
}

int
bw_en_pledge(void)
{
#ifdef __OpenBSD__
    return pledge("stdio rpath wpath cpath proc exec", NULL);
#else
    return -1;
#endif
}
