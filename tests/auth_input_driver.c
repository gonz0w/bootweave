/* SPDX-License-Identifier: BSD-3-Clause */
/* Test-only syscall witnesses; never linked into the product. */
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "auth_input.h"
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static const char *fault;
static unsigned int reads, stats[5], seeks[5], closes[5], ticks;
static struct bw_auth_snapshot snapshot, first;
static int mode(const char *s) { return strcmp(fault, s) == 0; }

int
bw_io_clock_gettime(clockid_t id, struct timespec *t)
{
    int result = clock_gettime(id, t);
    ticks++;
    if (closes[4] && mode("close-clock-error")) return -1;
    if (closes[4] && (mode("close-deadline") || mode("close-eintr-deadline"))) t->tv_sec += 3;
    if (mode("clock-error")) return -1;
    if (mode("clock-invalid")) t->tv_nsec = 1000000000L;
    if (mode("clock-backwards")) t->tv_sec -= ticks;
    if (mode("eintr-forever")) t->tv_sec += reads / 3;
    if (mode("deadline")) t->tv_sec += ticks; /* strict two-second boundary */
    return result;
}

ssize_t
bw_io_read(int fd, void *b, size_t n)
{
    reads++;
    if (mode("read-error")) { errno = EIO; return -1; }
    if (mode("eintr-forever")) { errno = EINTR; return -1; }
    if (mode("short")) {
        if (reads == 1) { errno = EINTR; return -1; }
        if (n > 7) n = 7;
    }
    if (mode("shrink") && reads == 2) return 0;
    if (mode("grow") && fd == 4) { memset(b, 'x', n); return (ssize_t)n; }
    return read(fd, b, n);
}

int
bw_io_fstat(int fd, struct stat *s)
{
    int result = fstat(fd, s);
    if (fd < 0 || fd > 4 || result) return result;
    stats[fd]++;
    if (mode("stat-error")) { errno = EIO; return -1; }
    if (fd != 0 || stats[fd] != 2) return result;
    if (mode("changed-device")) s->st_dev++;
    if (mode("changed-inode")) s->st_ino++;
    if (mode("changed-mode")) s->st_mode ^= 1;
    if (mode("changed-uid")) s->st_uid++;
    if (mode("changed-gid")) s->st_gid++;
    if (mode("changed-link")) s->st_nlink++;
    if (mode("changed-size")) s->st_size++;
#ifdef __APPLE__
    if (mode("changed-mtime")) s->st_mtimespec.tv_nsec ^= 1;
    if (mode("changed-ctime")) s->st_ctimespec.tv_nsec ^= 1;
    if (mode("changed-atime")) s->st_atimespec.tv_nsec ^= 1;
#else
    if (mode("changed-mtime")) s->st_mtim.tv_nsec ^= 1;
    if (mode("changed-ctime")) s->st_ctim.tv_nsec ^= 1;
    if (mode("changed-atime")) s->st_atim.tv_nsec ^= 1;
#endif
    return result;
}

int
bw_io_fcntl(int fd, int command, ...)
{
    if (mode("flags-error")) { errno = EIO; return -1; }
    return fcntl(fd, command);
}

off_t
bw_io_lseek(int fd, off_t offset, int whence)
{
    off_t result = lseek(fd, offset, whence);
    if (fd >= 0 && fd <= 4 && ++seeks[fd] == 2 && mode("changed-offset")) return result + 1;
    return result;
}

int
bw_io_close(int fd)
{
    int result = close(fd);
    if (fd >= 0 && fd <= 4) closes[fd]++;
    if ((fd == 0 && (mode("close-eintr") || mode("close-eintr-deadline"))) || (fd == 3 && mode("close-eintr-3")) ||
        (fd == 4 && mode("close-eintr-4"))) { errno = EINTR; return -1; }
    if ((fd == 0 && mode("close-ebadf")) || (fd == 3 && mode("close-ebadf-3")) ||
        (fd == 4 && mode("close-ebadf-4"))) { errno = EBADF; return -1; }
    return result;
}

static int
high(int fd)
{
    int result;
    if (fd < 0) return -1;
    result = fcntl(fd, F_DUPFD, 10);
    close(fd);
    return result;
}

static int
all_zero(const void *b, size_t length)
{
    const unsigned char *p = b;
    while (length--) if (*p++) return 0;
    return 1;
}

int
main(int argc, char **argv)
{
    const int role[3] = {0, 3, 4};
    int held[3], extra[2], i, repeat, run, result;
    const char *behavior;
    if (argc != 6) return 90;
    behavior = argv[4]; fault = argv[5]; repeat = strcmp(behavior, "repeat") == 0;
    for (i = 0; i < 3; i++) {
        held[i] = high(open(argv[i + 1],
            (strcmp(behavior, "rw") == 0 && i == 1 ? O_RDWR : O_RDONLY) | O_NONBLOCK));
        if (held[i] < 0) return 91;
    }
    if (!strcmp(behavior, "input-alias")) {
        close(held[1]); held[1] = dup(held[0]);
    }
    for (run = 0; run <= repeat; run++) {
        for (i = 0; i < 3; i++) {
            (void)lseek(held[i], 0, SEEK_SET);
            if (dup2(held[i], role[i]) != role[i]) return 92;
        }
        if (!strcmp(behavior, "offset")) (void)lseek(3, 1, SEEK_SET);
        if (!strcmp(behavior, "missing")) close(4);
        if (!strcmp(behavior, "no-output")) close(2);
        if (!strcmp(behavior, "stdout-alias") || !strcmp(behavior, "stderr-alias")) {
            int output = !strcmp(behavior, "stdout-alias") ? 1 : 2;
            int alias = open(argv[output == 1 ? 3 : 1], O_WRONLY);
            if (alias < 0 || dup2(alias, output) != output) return 93;
            close(alias);
        }
        if (!strcmp(behavior, "pipe")) {
            if (pipe(extra) || dup2(extra[0], 4) != 4) return 94;
            close(extra[0]); close(extra[1]);
        }
        if (!strcmp(behavior, "socket")) {
            if (socketpair(AF_UNIX, SOCK_STREAM, 0, extra) || dup2(extra[0], 4) != 4) return 94;
            close(extra[0]); close(extra[1]);
        }
        reads = ticks = 0;
        memset(stats, 0, sizeof(stats)); memset(seeks, 0, sizeof(seeks));
        memset(closes, 0, sizeof(closes)); memset(&snapshot, 0xa5, sizeof(snapshot));
        result = bw_auth_capture(!strcmp(behavior, "null") ? NULL : &snapshot);
        for (i = 0; i < 3; i++) {
            if (closes[role[i]] != 1 || fcntl(role[i], F_GETFD) != -1 || errno != EBADF)
                return 95;
        }
        if (result != BW_CAPTURED) {
            if (strcmp(behavior, "null") && !all_zero(&snapshot, sizeof(snapshot))) return 96;
            if (strcmp(behavior, "ok") && strcmp(behavior, "repeat") && reads) return 97;
            if (mode("eintr-forever") && reads < 3) return 97;
            return result == BW_CAPTURE_UNCERTAIN ? 3 : 2;
        }
        if (run == 0 && repeat) first = snapshot;
        if (run && memcmp(&first, &snapshot, sizeof(first))) return 98;
    }
    if (printf("captured-only %zu %zu %zu\n", snapshot.definition_length,
        snapshot.payload_length, snapshot.signature_length) < 0 ||
        fwrite(snapshot.definition, 1, snapshot.definition_length, stdout) != snapshot.definition_length ||
        fwrite(snapshot.payload, 1, snapshot.payload_length, stdout) != snapshot.payload_length ||
        fwrite(snapshot.signature, 1, snapshot.signature_length, stdout) != snapshot.signature_length)
        return 99;
    return 0;
}
