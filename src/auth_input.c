/* SPDX-License-Identifier: BSD-3-Clause */
#ifdef __OpenBSD__
#define _BSD_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "auth_input.h"
#include "auth_material.h"
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int
stamp(struct timespec *t)
{
    return clock_gettime(CLOCK_MONOTONIC, t) == 0 && t->tv_sec >= 0 &&
        t->tv_nsec >= 0 && t->tv_nsec < 1000000000L;
}

static void
fail(struct bw_verifier_material *m, enum bw_material_cause cause)
{
    if (m && m->cause == BW_VM_NONE) m->cause = cause;
}

static int
live(const struct timespec *start, struct bw_verifier_material *m)
{
    struct timespec now;
    time_t seconds;
    if (m && *m->cancelled) { fail(m, BW_VM_CANCEL); return 0; }
    if (!stamp(&now) || now.tv_sec < start->tv_sec ||
        (now.tv_sec == start->tv_sec && now.tv_nsec < start->tv_nsec)) goto expired;
    seconds = now.tv_sec - start->tv_sec;
    if (seconds < 2 || (seconds == 2 && now.tv_nsec < start->tv_nsec)) return 1;
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
disjoint(const volatile void *a, size_t an,
    const volatile void *b, size_t bn)
{
    uintptr_t x = (uintptr_t)a, y = (uintptr_t)b;
    if (!a || !b || an > UINTPTR_MAX - x || bn > UINTPTR_MAX - y)
        return 0;
    return x + an <= y || y + bn <= x;
}

static int
unchanged(const struct stat *a, const struct stat *b)
{
    if (!identity(a, b) || a->st_mode != b->st_mode || a->st_uid != b->st_uid ||
        a->st_gid != b->st_gid || a->st_nlink != b->st_nlink || a->st_size != b->st_size)
        return 0;
#ifdef __APPLE__
    return a->st_mtimespec.tv_sec == b->st_mtimespec.tv_sec &&
        a->st_mtimespec.tv_nsec == b->st_mtimespec.tv_nsec &&
        a->st_ctimespec.tv_sec == b->st_ctimespec.tv_sec &&
        a->st_ctimespec.tv_nsec == b->st_ctimespec.tv_nsec;
#else
    return a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
        a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
        a->st_ctim.tv_sec == b->st_ctim.tv_sec &&
        a->st_ctim.tv_nsec == b->st_ctim.tv_nsec;
#endif
}

static enum bw_capture
capture(struct bw_auth_snapshot *snapshot, const struct timespec *start,
    struct bw_verifier_material *m)
{
    static const int fd[3] = {0, 3, 4};
    static const size_t cap[3] = {BW_AUTH_DEFINITION_MAX, BW_AUTH_PAYLOAD_MAX,
        BW_AUTH_SIGNATURE_MAX};
    struct stat before[3], sink[2], after;
    struct timespec origin;
    unsigned char *bytes[3];
    size_t length[3] = {0, 0, 0}, i, j, amount;
    int flags;
    ssize_t n;
    enum bw_capture result = BW_CAPTURE_REJECTED;

    if (!snapshot) goto cleanup;
    memset(snapshot, 0, sizeof(*snapshot));
    bytes[0] = snapshot->definition;
    bytes[1] = snapshot->payload;
    bytes[2] = snapshot->signature;
    if (!start) {
        if (!stamp(&origin)) goto cleanup;
        start = &origin;
    }
    if (m && (!live(start, m) || fstat(m->directory, &after) == -1 ||
        !unchanged(&after, &m->identity))) {
        fail(m, BW_VM_OWNERSHIP); goto cleanup;
    }
    for (i = 0; i < 2; i++)
        if (!live(start, m) || fstat((int)i + 1, &sink[i]) == -1) goto cleanup;
    /* Complete preflight precedes all content reads. */
    for (i = 0; i < 3; i++) {
        if (!live(start, m) || fstat(fd[i], &before[i]) == -1 ||
            !S_ISREG(before[i].st_mode) || before[i].st_size <= 0 ||
            before[i].st_size > (off_t)cap[i]) goto cleanup;
        if (!live(start, m) || (flags = fcntl(fd[i], F_GETFL)) == -1 ||
            (flags & O_ACCMODE) != O_RDONLY || !live(start, m) ||
            lseek(fd[i], 0, SEEK_CUR) != 0) goto cleanup;
        for (j = 0; j < i; j++)
            if (identity(&before[i], &before[j])) goto cleanup;
        for (j = 0; j < 2; j++)
            if (S_ISREG(sink[j].st_mode) && identity(&before[i], &sink[j])) goto cleanup;
    }
    for (i = 0; i < 3; i++) {
        for (;;) {
            if (!live(start, m)) goto cleanup;
            amount = cap[i] + 1 - length[i];
            if (amount > 4096) amount = 4096;
            n = read(fd[i], bytes[i] + length[i], amount);
            if (n < 0 && errno == EINTR) continue;
            if (n < 0) goto cleanup;
            if (!n) break;
            length[i] += (size_t)n;
            if (length[i] > cap[i]) goto cleanup;
        }
        if (length[i] != (size_t)before[i].st_size) goto cleanup;
    }
    for (i = 0; i < 3; i++) {
        if (!live(start, m) || fstat(fd[i], &after) == -1 ||
            !unchanged(&before[i], &after) || !live(start, m) ||
            lseek(fd[i], 0, SEEK_CUR) != (off_t)length[i]) goto cleanup;
    }
    if (!live(start, m)) goto cleanup;
    snapshot->definition_length = length[0];
    snapshot->payload_length = length[1];
    snapshot->signature_length = length[2];
    result = BW_CAPTURED;
cleanup:
    if (result != BW_CAPTURED) fail(m, BW_VM_IO);
    for (i = 0; i < 3; i++) {
        if (close(fd[i]) == -1) {
            if (m) {
                m->capture_close_errors |= 1U << i;
                fail(m, BW_VM_CLOSE);
                if (errno != EBADF) {
                    m->capture_unknown_closes |= 1U << i;
                    m->uncertain = 1;
                }
            }
            if (errno != EBADF) result = BW_CAPTURE_UNCERTAIN;
            else if (result == BW_CAPTURED) result = BW_CAPTURE_REJECTED;
        }
    }
    if (result == BW_CAPTURED && !live(start, m)) result = BW_CAPTURE_REJECTED;
    if (snapshot && result != BW_CAPTURED) memset(snapshot, 0, sizeof(*snapshot));
    return result;
}

enum bw_capture
bw_auth_capture(struct bw_auth_snapshot *snapshot)
{
    return capture(snapshot, NULL, NULL);
}

enum bw_capture
bw_auth_capture_reserved(struct bw_verifier_material *m, struct bw_auth_snapshot *snapshot)
{
    size_t i;
    if (!disjoint(m, sizeof(*m), snapshot, sizeof(*snapshot)) ||
        !disjoint(m, sizeof(*m), m->cancelled, sizeof(*m->cancelled)) ||
        !disjoint(snapshot, sizeof(*snapshot), m->cancelled, sizeof(*m->cancelled)))
        return BW_CAPTURE_NOT_TAKEN;
    if (m->state != BW_VM_RESERVED || m->capture_taken || m->cause != BW_VM_NONE ||
        m->uncertain || m->close_errors || m->unknown_closes || m->capture_close_errors ||
        m->capture_unknown_closes || m->directory < 5 || m->directory_known != 1 ||
        m->directory_lost || m->created || m->known || !m->path[0] ||
        m->started.tv_sec < 0 || m->started.tv_nsec < 0 || m->started.tv_nsec >= 1000000000L ||
        !S_ISDIR(m->identity.st_mode) || (m->identity.st_mode & 07777) != 0700 ||
        !m->identity.st_nlink) return BW_CAPTURE_NOT_TAKEN;
    for (i = 0; i < 3; i++)
        if (m->writer[i] != -1 || m->fd[i] != -1) return BW_CAPTURE_NOT_TAKEN;
    m->capture_taken = 1; /* Latch before clearing storage or taking fixed inputs. */
    return capture(snapshot, &m->started, m);
}
