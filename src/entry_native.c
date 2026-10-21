/* SPDX-License-Identifier: BSD-3-Clause */
#define _POSIX_C_SOURCE 200809L
#include "private_entry.h"
#include "private_start_material.h"
#include <string.h>

static int
reserved(const struct bw_verifier_material *m)
{
    static const char prefix[] = "/tmp/bootweave-verify.";
    size_t i;
    if (!m || m->state != BW_VM_RESERVED || m->cause != BW_VM_NONE ||
        m->directory < 5 || m->directory_known != 1 || m->directory_lost ||
        m->created || m->known || m->uncertain || m->close_errors ||
        m->unknown_closes || m->capture_taken || m->capture_close_errors ||
        m->capture_unknown_closes || !m->cancelled || m->started.tv_sec < 0 ||
        m->started.tv_nsec < 0 || m->started.tv_nsec >= 1000000000L ||
        !S_ISDIR(m->identity.st_mode) || (m->identity.st_mode & 07777) != 0700 ||
        !m->identity.st_nlink) return 0;
    for (i = 0; i < 3; i++)
        if (m->writer[i] != -1 || m->fd[i] != -1) return 0;
    if (memcmp(m->path, prefix, sizeof(prefix) - 1)) return 0;
    return m->path[sizeof(prefix) + 9] == 0 &&
        strspn(m->path + sizeof(prefix) - 1,
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") == 10;
}

static int
same_directory(const struct stat *seen, const struct stat *record, uid_t owner)
{
    return seen->st_dev == record->st_dev && seen->st_ino == record->st_ino &&
        seen->st_mode == record->st_mode && seen->st_uid == record->st_uid &&
        seen->st_gid == record->st_gid && seen->st_uid == owner && seen->st_nlink > 0;
}

int
bw_entry_native(const struct bw_verifier_material *m)
{
    /* Provisional7.9/amd64 profile: source/model only, never the normal boundary. */
    static const struct { const char *path, *permissions; } paths[] = {
        {"/usr/bin/signify", "x"}, {"/usr/libexec/ld.so", "r"},
        {"/usr/lib/libc.so.103.0", "r"}, {"/usr/lib/libutil.so.21.0", "r"},
        {"/dev/null", "r"}, {"/dev/fd/3", "r"}, {"/dev/fd/4", "r"}, {"/dev/fd/5", "r"}
    };
    struct stat held, named;
    uid_t owner;
    size_t i;
    if (!reserved(m) || bw_entry_live() != 1 || bw_en_identity(&owner) != 1 ||
        !owner || m->identity.st_uid != owner) return -1;
    if (bw_entry_live() != 1 || fstat(m->directory, &held) != 0 ||
        bw_entry_live() != 1 || lstat(m->path, &named) != 0 ||
        !same_directory(&held, &m->identity, owner) ||
        !same_directory(&named, &m->identity, owner)) return -1;
    if (bw_entry_live() != 1 || bw_en_unveil(m->path, "rwc") != 0) return -1;
    for (i = 0; i < sizeof(paths) / sizeof(paths[0]); i++)
        if (bw_entry_live() != 1 || bw_en_unveil(paths[i].path, paths[i].permissions) != 0)
            return -1;
    if (bw_entry_live() != 1 || bw_en_unveil(NULL, NULL) != 0 ||
        bw_entry_live() != 1 || bw_en_pledge() != 0 || bw_entry_live() != 1) return -1;
    return 0;
}
