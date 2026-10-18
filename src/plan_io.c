/* SPDX-License-Identifier: BSD-3-Clause */
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "plan.h"

#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void
deadline(int signal_number)
{
    _exit(signal_number == SIGXFSZ ? 5 : 4);
}

int
bw_start(void)
{
    struct sigaction action;
    struct itimerval timer;
    sigset_t signals;

    memset(&action, 0, sizeof(action));
    sigemptyset(&action.sa_mask);
    action.sa_handler = deadline;
    if (sigaction(SIGALRM, &action, NULL) == -1 ||
        sigaction(SIGPROF, &action, NULL) == -1 ||
        sigaction(SIGXCPU, &action, NULL) == -1 ||
        sigaction(SIGXFSZ, &action, NULL) == -1)
        return 0;
    action.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &action, NULL) == -1)
        return 0;
    sigemptyset(&signals);
    sigaddset(&signals, SIGALRM);
    sigaddset(&signals, SIGPROF);
    sigaddset(&signals, SIGXCPU);
    sigaddset(&signals, SIGXFSZ);
    sigaddset(&signals, SIGPIPE);
    if (sigprocmask(SIG_UNBLOCK, &signals, NULL) == -1)
        return 0;
    memset(&timer, 0, sizeof(timer));
    timer.it_value.tv_sec = 5;
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
        return 0;
    /* Keep CPU enforcement independent of the kernel's coarse rlimit checks. */
    timer.it_value.tv_sec = 2;
    return setitimer(ITIMER_PROF, &timer, NULL) == 0;
}

int
bw_write(int fd, const char *text, size_t length)
{
    ssize_t n;

    while (length > 0) {
        n = write(fd, text, length);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0)
            return 0;
        text += n;
        length -= (size_t)n;
    }
    return 1;
}

static int
failure(int status, const char *message)
{
    return bw_write(STDERR_FILENO, message, strlen(message)) ? status : 5;
}

static int
same_input(const struct stat *a, const struct stat *b)
{
#ifdef __APPLE__
    return a->st_size == b->st_size &&
        a->st_mtimespec.tv_sec == b->st_mtimespec.tv_sec &&
        a->st_mtimespec.tv_nsec == b->st_mtimespec.tv_nsec &&
        a->st_ctimespec.tv_sec == b->st_ctimespec.tv_sec &&
        a->st_ctimespec.tv_nsec == b->st_ctimespec.tv_nsec;
#else
    return a->st_size == b->st_size &&
        a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
        a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
        a->st_ctim.tv_sec == b->st_ctim.tv_sec &&
        a->st_ctim.tv_nsec == b->st_ctim.tv_nsec;
#endif
}

int
bw_run(void)
{
    unsigned char input[BW_INPUT_MAX + 1];
    char output[BW_OUTPUT_MAX], diagnostic[96];
    struct bw_plan plan;
    struct stat before, after, sink;
    size_t used = 0, offset = 0, length;
    ssize_t n;
    int flags, fd, rejected = 0, quiet = 0;

    if (fstat(STDIN_FILENO, &before) == -1)
        rejected = 1;
    else {
        /* Refuse output aliases before any diagnostic can alter the input. */
        for (fd = 1; fd <= 2; fd++) {
            if (fstat(fd, &sink) == 0 && S_ISREG(sink.st_mode) &&
                sink.st_dev == before.st_dev && sink.st_ino == before.st_ino) {
                rejected = 1;
                quiet = 1;
            }
        }
        flags = fcntl(STDIN_FILENO, F_GETFL);
        if (!S_ISREG(before.st_mode) || flags == -1 ||
            (flags & O_ACCMODE) != O_RDONLY ||
            lseek(STDIN_FILENO, 0, SEEK_CUR) != 0 || before.st_size < 0 ||
            before.st_size > BW_INPUT_MAX)
            rejected = 1;
    }
    if (!rejected) {
        while (used < sizeof(input)) {
            n = read(STDIN_FILENO, input + used, sizeof(input) - used);
            if (n < 0 && errno == EINTR)
                continue;
            if (n < 0) {
                rejected = 1;
                break;
            }
            if (n == 0)
                break;
            used += (size_t)n;
        }
        if (used > BW_INPUT_MAX || used != (size_t)before.st_size ||
            fstat(STDIN_FILENO, &after) == -1 || !same_input(&before, &after))
            rejected = 1;
    }
    if (close(STDIN_FILENO) == -1)
        rejected = 1;
    if (rejected)
        return quiet ? 3 : failure(3, "bootweave: input rejected\n");
    if (!bw_parse(input, used, &plan, &offset)) {
        n = snprintf(diagnostic, sizeof(diagnostic),
            "bootweave: definition rejected at byte %zu\n", offset);
        if (n < 0 || (size_t)n >= sizeof(diagnostic))
            return 4;
        return failure(3, diagnostic);
    }
    if (!bw_render(&plan, output, sizeof(output), &length))
        return failure(4, "bootweave: rendering failed\n");
    return bw_write(STDOUT_FILENO, output, length) ? 0 : 5;
}
