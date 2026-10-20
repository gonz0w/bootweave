/* SPDX-License-Identifier: BSD-3-Clause */
#ifdef __OpenBSD__
#define _BSD_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "auth_material.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *const names[3] = {"key.pub", "signature.sig", "definition"};
static const size_t caps[3] = {1024, 16384, 16384};
typedef char workspace_bound[(sizeof(struct bw_verifier_material) + 4096 +
    3 * sizeof(struct stat) <= 8192) ? 1 : -1];

static int
value(unsigned char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    return c == '+' ? 62 : c == '/' ? 63 : -1;
}

static int
record(const unsigned char *b, size_t n, size_t decoded, size_t cap)
{
    static const char prefix[] = "untrusted comment: ";
    size_t line = sizeof(prefix) - 1, i, encoded = ((decoded + 2) / 3) * 4;
    size_t pad = (3 - decoded % 3) % 3;
    if (!b || n > cap || n <= line || memcmp(b, prefix, line)) return 0;
    while (line < n && b[line] != '\n') {
        if (b[line] < 32 || b[line] > 126) return 0;
        line++;
    }
    if (line <= sizeof(prefix) - 1 || line - (sizeof(prefix) - 1) > 255 ||
        n != line + 1 + encoded + 1 || b[n - 1] != '\n') return 0;
    b += line + 1;
    for (i = 0; i < encoded - pad; i++) if (value(b[i]) < 0) return 0;
    for (; i < encoded; i++) if (b[i] != '=') return 0;
    if (pad && (value(b[encoded - pad - 1]) & (pad == 1 ? 3 : 15))) return 0;
    return ((value(b[0]) << 2) | (value(b[1]) >> 4)) == 'E' &&
        (((value(b[1]) & 15) << 4) | (value(b[2]) >> 2)) == 'd';
}

int
bw_signify_records(const unsigned char *key, size_t key_length,
    const unsigned char *signature, size_t signature_length)
{
    return record(key, key_length, 42, 1024) && record(signature, signature_length, 74, 16384);
}

static void
fail(struct bw_verifier_material *m, enum bw_material_cause cause)
{
    if (m->cause == BW_VM_NONE) m->cause = cause;
}

static int
stamp(struct timespec *t)
{
    return clock_gettime(CLOCK_MONOTONIC, t) == 0 && t->tv_sec >= 0 &&
        t->tv_nsec >= 0 && t->tv_nsec < 1000000000L;
}

static int
live(struct bw_verifier_material *m)
{
    struct timespec t;
    time_t seconds;
    if (*m->cancelled) { fail(m, BW_VM_CANCEL); return 0; }
    if (!stamp(&t) || t.tv_sec < m->started.tv_sec ||
        (t.tv_sec == m->started.tv_sec && t.tv_nsec < m->started.tv_nsec)) goto expired;
    seconds = t.tv_sec - m->started.tv_sec;
    if (seconds < 2 || (seconds == 2 && t.tv_nsec < m->started.tv_nsec)) return 1;
expired:
    fail(m, BW_VM_DEADLINE);
    return 0;
}

static int
identity(const struct stat *a, const struct stat *b)
{
    return a->st_dev == b->st_dev && a->st_ino == b->st_ino;
}

static int
owned(const struct stat *a, const struct stat *b, int directory)
{
    return identity(a, b) && a->st_mode == b->st_mode && a->st_uid == b->st_uid &&
        a->st_gid == b->st_gid && a->st_uid == geteuid() &&
        (a->st_mode & 07777) == (directory ? 0700 : 0600) &&
        (directory ? S_ISDIR(a->st_mode) : S_ISREG(a->st_mode)) &&
        (directory ? a->st_nlink > 0 && b->st_nlink > 0 : a->st_nlink == 1);
}

static int
same_file(const struct stat *a, const struct stat *b)
{
    if (!owned(a, b, 0) || a->st_size != b->st_size) return 0;
#ifdef __APPLE__
    return a->st_mtimespec.tv_sec == b->st_mtimespec.tv_sec &&
        a->st_mtimespec.tv_nsec == b->st_mtimespec.tv_nsec &&
        a->st_ctimespec.tv_sec == b->st_ctimespec.tv_sec &&
        a->st_ctimespec.tv_nsec == b->st_ctimespec.tv_nsec;
#else
    return a->st_mtim.tv_sec == b->st_mtim.tv_sec && a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
        a->st_ctim.tv_sec == b->st_ctim.tv_sec && a->st_ctim.tv_nsec == b->st_ctim.tv_nsec;
#endif
}

static int
directory_matches(struct bw_verifier_material *m, int work)
{
    struct stat held, named;
    if (m->directory_lost || !m->directory_known || m->directory < 0) goto lost;
    if (work && !live(m)) return 0;
    if (fstat(m->directory, &held) == -1) goto lost;
    if (work && !live(m)) return 0;
    if (lstat(m->path, &named) == -1) goto lost;
    if (work && !live(m)) return 0;
    if (owned(&held, &m->identity, 1) && owned(&named, &m->identity, 1)) return 1;
lost:
    m->directory_lost = 1;
    return 0;
}

static void
consume(int *fd, unsigned bit, struct bw_verifier_material *m)
{
    int owned_fd = *fd;
    *fd = -1; /* Latch before a fallible close; never reuse this number. */
    if (owned_fd >= 0 && close(owned_fd) == -1) {
        fail(m, BW_VM_CLOSE);
        m->close_errors |= 1U << bit;
        if (errno != EBADF) m->unknown_closes |= 1U << bit;
    }
}

static void
remove_names(struct bw_verifier_material *m)
{
    struct stat s;
    size_t i;
    if (!m->path[0]) return;
    for (i = 0; i < 3; i++) {
        if (!(m->created & (1U << i))) continue;
        if (!directory_matches(m, 0) || !(m->known & (1U << i)) ||
            fstatat(m->directory, names[i], &s, AT_SYMLINK_NOFOLLOW) == -1 ||
            !owned(&s, &m->file[i], 0) || s.st_size < 0 || s.st_size > (off_t)caps[i]) {
            fail(m, BW_VM_OWNERSHIP); m->uncertain = 1; continue;
        }
        if (unlinkat(m->directory, names[i], 0) == -1) {
            fail(m, BW_VM_IO); m->uncertain = 1; continue;
        }
        m->created &= ~(1U << i);
        if (fstatat(m->directory, names[i], &s, AT_SYMLINK_NOFOLLOW) != -1 || errno != ENOENT) {
            fail(m, BW_VM_OWNERSHIP); m->uncertain = 1;
        }
    }
    if (m->created) { m->uncertain = 1; return; }
    if (!directory_matches(m, 0)) { fail(m, BW_VM_OWNERSHIP); m->uncertain = 1; return; }
    if (rmdir(m->path) == -1) { fail(m, BW_VM_IO); m->uncertain = 1; return; }
    if (lstat(m->path, &s) != -1 || errno != ENOENT) {
        fail(m, BW_VM_OWNERSHIP); m->uncertain = 1; return;
    }
    m->path[0] = 0;
}

static enum bw_material_result
finish(struct bw_verifier_material *m, int ready)
{
    struct stat s;
    size_t i;
    int flags;
    for (i = 0; i < 3; i++) consume(&m->writer[i], (unsigned)i, m);
    remove_names(m);
    consume(&m->directory, 6, m);
    if (ready && m->cause == BW_VM_NONE && !m->uncertain && !m->unknown_closes) {
        for (i = 0; i < 3; i++) {
            if (!live(m) || fstat(m->fd[i], &s) == -1 || !identity(&s, &m->file[i]) ||
                !S_ISREG(s.st_mode) || s.st_uid != m->file[i].st_uid ||
                s.st_gid != m->file[i].st_gid || s.st_mode != m->file[i].st_mode ||
                s.st_size != m->file[i].st_size || s.st_nlink != 0 ||
                !live(m) || (flags = fcntl(m->fd[i], F_GETFL)) == -1 || (flags & O_ACCMODE) != O_RDONLY ||
                !live(m) || (flags = fcntl(m->fd[i], F_GETFD)) == -1 || !(flags & FD_CLOEXEC) ||
                !live(m) || lseek(m->fd[i], 0, SEEK_CUR) != 0) { fail(m, BW_VM_OWNERSHIP); break; }
        }
        if (m->cause == BW_VM_NONE && live(m)) { m->state = BW_VM_READY; return BW_MATERIAL_OK; }
    }
    for (i = 0; i < 3; i++) consume(&m->fd[i], (unsigned)i + 3, m);
    if (m->uncertain || m->unknown_closes) {
        m->state = BW_VM_UNCERTAIN; return BW_MATERIAL_UNCERTAIN;
    }
    if (m->cause != BW_VM_NONE) { m->state = BW_VM_REJECTED; return BW_MATERIAL_REJECTED; }
    m->state = BW_VM_CLOSED;
    return BW_MATERIAL_OK;
}

enum bw_material_result
bw_verifier_reserve(const volatile sig_atomic_t *cancelled, struct bw_verifier_material *m)
{
    struct stat s;
    int original, moved;
    size_t i;
    if (!m || m->state != BW_VM_EMPTY) return BW_MATERIAL_REJECTED;
    memset(m, 0, sizeof(*m)); m->state = BW_VM_WORKING; m->directory = -1;
    for (i = 0; i < 3; i++) m->writer[i] = m->fd[i] = -1;
    m->cancelled = cancelled;
    if (!cancelled || getuid() == 0 || getuid() != geteuid() || getgid() != getegid()) {
        fail(m, BW_VM_ARGUMENT); return finish(m, 0);
    }
#if defined(__APPLE__) || defined(__OpenBSD__)
    if (issetugid()) { fail(m, BW_VM_ARGUMENT); return finish(m, 0); }
#endif
    if (!stamp(&m->started)) { fail(m, BW_VM_DEADLINE); return finish(m, 0); }
    if (!live(m)) return finish(m, 0);
    memcpy(m->path, "/tmp/bootweave-verify.XXXXXXXXXX", sizeof("/tmp/bootweave-verify.XXXXXXXXXX"));
    if (!live(m)) { m->path[0] = 0; return finish(m, 0); }
    if (!mkdtemp(m->path)) { m->path[0] = 0; fail(m, BW_VM_IO); return finish(m, 0); }
    if (!live(m) || (m->directory = open(m->path, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC)) == -1 ||
        !live(m) || fstat(m->directory, &m->identity) == -1 || !live(m) || lstat(m->path, &s) == -1 ||
        !owned(&s, &m->identity, 1)) { fail(m, BW_VM_OWNERSHIP); return finish(m, 0); }
    m->directory_known = 1;
    if (m->directory < 5) {
        if (!live(m)) return finish(m, 0);
        moved = fcntl(m->directory, F_DUPFD_CLOEXEC, 5);
        if (moved == -1) { fail(m, BW_VM_IO); return finish(m, 0); }
        original = m->directory; m->directory = moved;
        consume(&original, 7, m);
    }
    if (m->cause != BW_VM_NONE || !live(m)) return finish(m, 0);
    m->state = BW_VM_RESERVED;
    return BW_MATERIAL_OK;
}

static int
materialize(struct bw_verifier_material *m, size_t i, const unsigned char *bytes, size_t length)
{
    unsigned char buffer[4096];
    struct stat s, after;
    size_t at = 0, amount;
    ssize_t n;
    if (!directory_matches(m, 1)) { fail(m, BW_VM_OWNERSHIP); return 0; }
    if (!live(m)) return 0;
    m->writer[i] = openat(m->directory, names[i], O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (m->writer[i] == -1) { fail(m, BW_VM_IO); return 0; }
    m->created |= 1U << i;
    if (!live(m) || fstat(m->writer[i], &s) == -1 || !owned(&s, &s, 0) || s.st_size != 0) {
        fail(m, BW_VM_OWNERSHIP); return 0;
    }
    m->file[i] = s; m->known |= 1U << i;
    while (at < length) {
        if (!live(m)) return 0;
        amount = length - at; if (amount > sizeof(buffer)) amount = sizeof(buffer);
        n = write(m->writer[i], bytes + at, amount);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) { fail(m, BW_VM_IO); return 0; }
        at += (size_t)n;
    }
    if (!live(m) || fstat(m->writer[i], &s) == -1 || !owned(&s, &m->file[i], 0) ||
        s.st_size != (off_t)length) { fail(m, BW_VM_OWNERSHIP); return 0; }
    m->file[i] = s;
    if (!live(m) || (m->fd[i] = openat(m->directory, names[i], O_RDONLY | O_NOFOLLOW | O_CLOEXEC)) == -1 ||
        !live(m) || fstat(m->fd[i], &after) == -1 || !same_file(&after, &s)) {
        fail(m, BW_VM_OWNERSHIP); return 0;
    }
    at = 0;
    for (;;) {
        if (!live(m)) return 0;
        amount = length - at; if (amount > sizeof(buffer)) amount = sizeof(buffer);
        n = read(m->fd[i], buffer, amount ? amount : 1);
        if (n < 0 && errno == EINTR) continue;
        if (n < 0 || (size_t)n > amount || (n && memcmp(buffer, bytes + at, (size_t)n))) {
            fail(m, BW_VM_IO); return 0;
        }
        if (!n) break;
        at += (size_t)n;
    }
    if (at != length || !live(m) || fstat(m->fd[i], &after) == -1 || !same_file(&after, &s) ||
        !live(m) || lseek(m->fd[i], 0, SEEK_SET) != 0) { fail(m, BW_VM_OWNERSHIP); return 0; }
    return 1;
}

enum bw_material_result
bw_verifier_prepare(const struct bw_verifier_request *r, struct bw_verifier_material *m)
{
    if (!m || m->state != BW_VM_RESERVED) return BW_MATERIAL_REJECTED;
    m->state = BW_VM_WORKING;
    if (!r || !r->key || !r->signature || !r->definition || !r->definition_length ||
        r->definition_length > 16384 || !r->key_length || r->key_length > 1024 ||
        !r->signature_length || r->signature_length > 16384) fail(m, BW_VM_ARGUMENT);
    else if (!bw_signify_records(r->key, r->key_length, r->signature, r->signature_length)) fail(m, BW_VM_RECORD);
    else if (live(m) && materialize(m, 0, r->key, r->key_length) &&
        materialize(m, 1, r->signature, r->signature_length))
        (void)materialize(m, 2, r->definition, r->definition_length);
    return finish(m, 1);
}

enum bw_material_result
bw_verifier_release(struct bw_verifier_material *m)
{
    if (!m || (m->state != BW_VM_RESERVED && m->state != BW_VM_READY &&
        m->state != BW_VM_VERIFYING)) return BW_MATERIAL_REJECTED;
    m->state = BW_VM_WORKING; /* Ownership consumed before any close/removal. */
    return finish(m, 0);
}
