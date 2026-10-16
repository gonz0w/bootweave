/* SPDX-License-Identifier: BSD-3-Clause */
/* Trusted, fixed-environment build guard; never part of the runtime. */
#ifndef __OpenBSD__
int main(void) { return 77; }
#else
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t compiler_pid;

static void
expired(int signal_number)
{
    int status;
    pid_t observed;
    (void)signal_number;
    if (compiler_pid > 0) {
        do {
            observed = waitpid((pid_t)compiler_pid, &status, WNOHANG);
        } while (observed == -1 && errno == EINTR);
        /* An unreaped live child keeps its PID/group identity reserved. */
        if (observed == 0) {
            (void)kill(-(pid_t)compiler_pid, SIGKILL);
            (void)kill((pid_t)compiler_pid, SIGKILL);
            while (waitpid((pid_t)compiler_pid, &status, 0) == -1 && errno == EINTR)
                ;
        }
    }
    _exit(124);
}

int
main(int argc, char **argv)
{
    const int resources[] = {RLIMIT_DATA, RLIMIT_STACK, RLIMIT_NOFILE,
        RLIMIT_CORE, RLIMIT_FSIZE, RLIMIT_CPU};
    const rlim_t ceilings[] = {536870912, 8388608, 128, 0, 16777216, 120};
    char *arguments[97];
    char *const environment[] = {"PATH=/bin:/usr/bin", "LC_ALL=C", "TZ=UTC",
        "TMPDIR=/planner/compiler-tmp", NULL};
    char directory[64];
    struct rlimit bounds;
    struct rusage usage;
    struct sigaction action;
    struct timespec start, finish;
    sigset_t mask;
    int i, status;
    pid_t child;

    if (argc < 2 || argc > 96 || getuid() != 0 ||
        getcwd(directory, sizeof(directory)) == NULL || strcmp(directory, "/planner"))
        return 2;
    memset(&action, 0, sizeof(action));
    sigemptyset(&action.sa_mask);
    action.sa_handler = expired;
    if (sigaction(SIGALRM, &action, NULL))
        return 4;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) || clock_gettime(CLOCK_MONOTONIC, &start))
        return 4;
    arguments[0] = "/usr/bin/cc";
    for (i = 1; i < argc; i++)
        arguments[i] = argv[i];
    arguments[argc] = NULL;
    child = fork();
    if (child < 0)
        return 4;
    if (child == 0) {
        if (setpgid(0, 0))
            _exit(125);
        for (i = 0; i < (int)(sizeof(resources) / sizeof(resources[0])); i++) {
            bounds.rlim_cur = bounds.rlim_max = ceilings[i];
            if (setrlimit(resources[i], &bounds))
                _exit(125);
        }
        closefrom(3);
        execve("/usr/bin/cc", arguments, environment);
        _exit(125);
    }
    compiler_pid = child;
    alarm(180);
    while (wait4(child, &status, 0, &usage) == -1)
        if (errno != EINTR)
            expired(0);
    compiler_pid = 0;
    alarm(0);
    if (clock_gettime(CLOCK_MONOTONIC, &finish))
        return 4;
    printf("Compiler wait=%d rss_kib=%ld user_us=%lld system_us=%lld wall_ms=%lld\n",
        status, usage.ru_maxrss,
        (long long)usage.ru_utime.tv_sec * 1000000 + usage.ru_utime.tv_usec,
        (long long)usage.ru_stime.tv_sec * 1000000 + usage.ru_stime.tv_usec,
        (long long)(finish.tv_sec - start.tv_sec) * 1000 +
        (finish.tv_nsec - start.tv_nsec) / 1000000);
    if (fflush(stdout) || usage.ru_maxrss > 524288 ||
        !WIFEXITED(status) || WEXITSTATUS(status) != 0)
        return 1;
    return 0;
}
#endif
