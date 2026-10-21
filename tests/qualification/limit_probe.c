/* SPDX-License-Identifier: BSD-3-Clause */
/* Unmodified resource objects only. No trace wrapper or native authority. */
#define _POSIX_C_SOURCE 200809L
#include "trusted_start.h"
#include "signature_verify.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#if defined(__OpenBSD__) || defined(BW_NQ_SOURCE_CHECK)
static int
file_ready(void)
{
    struct stat seen;
    int flags = fcntl(3, F_GETFL);
    return flags >= 0 && (flags & O_ACCMODE) != O_RDONLY &&
        !(flags & O_APPEND) && fstat(3, &seen) == 0 &&
        S_ISREG(seen.st_mode) && seen.st_uid == getuid() &&
        (seen.st_mode & 07777) == 0600 && seen.st_nlink == 1 &&
        seen.st_size == 0 && lseek(3, 0, SEEK_CUR) == 0;
}

static int
readback(int child)
{
    static const int kinds[] = {RLIMIT_CPU, RLIMIT_DATA, RLIMIT_STACK,
        RLIMIT_NOFILE, RLIMIT_CORE, RLIMIT_FSIZE};
    static const rlim_t parent_caps[] = {4, 67108864, 2097152, 32, 0, 65536};
    static const rlim_t child_caps[] = {2, 33554432, 2097152, 16, 0, 0};
    const rlim_t *caps = child ? child_caps : parent_caps;
    struct rlimit seen;
    struct itimerval real, prof;
    char line[512];
    size_t used;
    unsigned i;
    int count;

    count = snprintf(line, sizeof(line), "NQ_LIMITS %s", child ? "child" : "parent");
    if (count < 0 || (size_t)count >= sizeof(line)) return 70;
    used = (size_t)count;
    for (i = 0; i < sizeof(kinds) / sizeof(kinds[0]); i++) {
        if (getrlimit(kinds[i], &seen) == -1 ||
            seen.rlim_cur != caps[i] || seen.rlim_max != caps[i]) return 6;
        count = snprintf(line + used, sizeof(line) - used, " %ju:%ju",
            (uintmax_t)seen.rlim_cur, (uintmax_t)seen.rlim_max);
        if (count < 0 || (size_t)count >= sizeof(line) - used) return 70;
        used += (size_t)count;
    }
    if (getitimer(ITIMER_REAL, &real) == -1 ||
        getitimer(ITIMER_PROF, &prof) == -1) return 6;
    count = snprintf(line + used, sizeof(line) - used,
        " REAL %jd %jd %jd %jd PROF %jd %jd %jd %jd\n",
        (intmax_t)real.it_value.tv_sec, (intmax_t)real.it_value.tv_usec,
        (intmax_t)real.it_interval.tv_sec, (intmax_t)real.it_interval.tv_usec,
        (intmax_t)prof.it_value.tv_sec, (intmax_t)prof.it_value.tv_usec,
        (intmax_t)prof.it_interval.tv_sec, (intmax_t)prof.it_interval.tv_usec);
    if (count < 0 || (size_t)count >= sizeof(line) - used) return 70;
    used += (size_t)count;
    /* Publish actual timer fields; the independent observer validates their
     * normalization, positivity, cap and nonperiodicity, never guessed times. */
    return write(1, line, used) == (ssize_t)used ? 0 : 70;
}

static int
probe(const char *mode)
{
    int child, file;
    unsigned i;
    char block[4096];
    struct stat output;
    int flags;

    if (strcmp(mode, "parent-readback") == 0) child = file = 0;
    else if (strcmp(mode, "child-readback") == 0) { child = 1; file = 0; }
    else if (strcmp(mode, "parent-file-size") == 0) { child = 0; file = 1; }
    else if (strcmp(mode, "child-file-size") == 0) child = file = 1;
    else return 2;
    if (file) {
        if (!file_ready()) return 2;
    } else {
        flags = fcntl(1, F_GETFL);
        if (flags < 0 || (flags & O_ACCMODE) == O_RDONLY ||
            !(flags & O_NONBLOCK) || fstat(1, &output) == -1 ||
            !S_ISFIFO(output.st_mode)) return 2;
    }
    if (!bw_process_guard() || (child && bw_sv_child_limits() != 0)) return 4;
    if (!file) return readback(child);
    memset(block, 'X', sizeof(block));
    if (!child) for (i = 0; i < 16; i++)
        if (write(3, block, sizeof(block)) != (ssize_t)sizeof(block)) return 6;
    /* Expected native SIGXFSZ handler exit5. Never fake the signal, retry a
     * partial write, close/remove the caller's file or manufacture a pass. */
    (void)write(3, block, 1);
    return 6;
}
#endif

int
main(int argc, char **argv)
{
#ifdef __OpenBSD__
    if (argc != 2) return 2;
    return probe(argv[1]);
#else
    (void)argc; (void)argv;
#ifdef BW_NQ_SOURCE_CHECK
    /* Syntax checking the native body never makes it executable on this host. */
    (void)&probe;
#endif
    return 77;
#endif
}
