/* SPDX-License-Identifier: BSD-3-Clause */
/* Qualification build witness only. No caller command, retry or removal API. */
#ifdef __OpenBSD__
#define _BSD_SOURCE
#endif
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if defined(__OpenBSD__) || defined(BW_NQ_SOURCE_CHECK)
#include "BUILD-JOBS.h"
static volatile sig_atomic_t stopped;

static void stop(int value) { (void)value; stopped = 1; }

static int consume(int *fd)
{
    int number = *fd;
    *fd = -1;
    return number >= 0 && close(number) ? -1 : 0;
}

static long long stamp(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t) || t.tv_sec < 0 ||
        t.tv_nsec < 0 || t.tv_nsec >= 1000000000 ||
        (uintmax_t)t.tv_sec > ((uintmax_t)LLONG_MAX - 999) / 1000) return -1;
    return (long long)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

static int private_file(int fd)
{
    struct stat s;
    int flags = fcntl(fd, F_GETFL);
    int descriptor = fcntl(fd, F_GETFD);
    return flags >= 0 && descriptor >= 0 && !(descriptor & FD_CLOEXEC) &&
        (flags & O_ACCMODE) == O_WRONLY &&
        !(flags & O_APPEND) && fstat(fd, &s) == 0 && S_ISREG(s.st_mode) &&
        s.st_uid == 0 && (s.st_mode & 07777) == 0600 && s.st_nlink == 1 &&
        s.st_size == 0 && lseek(fd, 0, SEEK_CUR) == 0;
}

static int ceiling(void)
{
    static const int kind[] = {RLIMIT_DATA, RLIMIT_STACK, RLIMIT_NOFILE,
        RLIMIT_CORE, RLIMIT_FSIZE, RLIMIT_CPU};
    /* Stricter output ceiling than the proposed16MiB compiler maximum. */
    static const rlim_t cap[] = {536870912, 8388608, 128, 0, 262144, 120};
    struct rlimit seen, limit;
    unsigned i;
    for (i = 0; i < sizeof(kind) / sizeof(kind[0]); i++) {
        if (getrlimit(kind[i], &seen) || seen.rlim_cur < cap[i] ||
            seen.rlim_max < cap[i]) return 0;
        limit.rlim_cur = limit.rlim_max = cap[i];
        if (setrlimit(kind[i], &limit) || getrlimit(kind[i], &seen) ||
            seen.rlim_cur != cap[i] || seen.rlim_max != cap[i]) return 0;
    }
    return 1;
}

static int build(int argc, char **argv)
{
    char cwd[64], marker[128], receipt[128], line[512];
    char *const env[] = {"PATH=/bin:/usr/bin", "LC_ALL=C", "LANG=C", "TZ=UTC",
        "TMPDIR=/bwq-92166f07/compiler-tmp", NULL};
    struct utsname platform;
    struct sigaction action;
    struct sigaction inherited;
    struct itimerval timer;
    sigset_t unblocked;
    struct stat root, a, b;
    struct rusage usage;
    siginfo_t peek;
    struct timespec pause = {0, 10000000};
    pid_t child, waited;
    unsigned job, grouped = 0, consumed = 0, lost = 0, failed = 0;
    int ready[2], grant[2], flags, fd, input, status = 0, size;
    long long origin, now, total, work;
    char byte;

    if (argc != 2 || getuid() != 0 || geteuid() != 0 || getgid() != getegid() ||
        uname(&platform) || strcmp(platform.sysname, "OpenBSD") ||
        strcmp(platform.release, "7.9") || strcmp(platform.machine, "amd64") ||
        !getcwd(cwd, sizeof(cwd)) || strcmp(cwd, "/bwq-92166f07") ||
        lstat(cwd, &root) || !S_ISDIR(root.st_mode) || root.st_uid != 0 ||
        (root.st_mode & 07777) != 0755 || !private_file(1) || !private_file(2) ||
        fstat(1, &a) || fstat(2, &b) ||
        (a.st_dev == b.st_dev && a.st_ino == b.st_ino)) return 2;
    for (job = 0; job < NQ_BUILD_COUNT; job++)
        if (!strcmp(argv[1], nq_build_names[job])) break;
    origin = stamp();
    if (origin < 0 || job == NQ_BUILD_COUNT || !ceiling() ||
        getitimer(ITIMER_REAL, &timer) || timer.it_value.tv_sec || timer.it_value.tv_usec ||
        timer.it_interval.tv_sec || timer.it_interval.tv_usec ||
        getitimer(ITIMER_PROF, &timer) || timer.it_value.tv_sec || timer.it_value.tv_usec ||
        timer.it_interval.tv_sec || timer.it_interval.tv_usec ||
        sigaction(SIGCHLD, NULL, &inherited) || inherited.sa_handler != SIG_DFL ||
        (inherited.sa_flags & SA_NOCLDWAIT) || sigemptyset(&unblocked) ||
        sigaddset(&unblocked, SIGALRM) || sigaddset(&unblocked, SIGINT) ||
        sigaddset(&unblocked, SIGTERM) || sigaddset(&unblocked, SIGHUP) ||
        sigprocmask(SIG_UNBLOCK, &unblocked, NULL)) return 2;
    total = nq_build_ms[job];
    if (total < 6000 || total > 180000) return 2;
    work = total - 5000;
    memset(&action, 0, sizeof(action));
    action.sa_handler = stop;
    if (sigemptyset(&action.sa_mask) || sigaction(SIGALRM, &action, NULL) ||
        sigaction(SIGINT, &action, NULL) || sigaction(SIGTERM, &action, NULL) ||
        sigaction(SIGHUP, &action, NULL)) return 4;
    action.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &action, NULL)) return 4;
    alarm((unsigned)((total + 999) / 1000));
    size = snprintf(marker, sizeof(marker), "artifacts/%s.started", nq_build_names[job]);
    if (size < 0 || (size_t)size >= sizeof(marker)) return 4;
    fd = open(marker, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (fd < 0) return 4;
    size = snprintf(line, sizeof(line), "NQ_COMPILE_START %u %lld\n", job, origin);
    if (size < 0 || (size_t)size >= sizeof(line) || write(fd, line, (size_t)size) != size) {
        (void)close(fd); return 4;
    }
    if (consume(&fd) || pipe(ready) || pipe(grant)) return 4;
    input = open("/dev/null", O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
    if (input < 3 || fstat(input, &a) || !S_ISCHR(a.st_mode)) return 4;
    flags = fcntl(ready[0], F_GETFL);
    if (flags < 0 || fcntl(ready[0], F_SETFL, flags | O_NONBLOCK)) return 4;
    now = stamp();
    if (stopped || now < origin || now - origin >= work) return 4;
    child = fork();
    if (child < 0) return 4;
    if (!child) {
        if (close(ready[0]) || close(grant[1]) || setpgid(0, 0) ||
            write(ready[1], "R", 1) != 1 || close(ready[1]) ||
            read(grant[0], &byte, 1) != 1 || byte != 'G' || close(grant[0]) ||
            dup2(input, 0) != 0) _exit(125);
#ifdef __OpenBSD__
        if (closefrom(3)) _exit(125);
#endif
        execve("/usr/bin/cc", nq_build_argv[job], env);
        _exit(125);
    }
    if (consume(&ready[1])) failed = 1;
    if (consume(&grant[0])) failed = 1;
    if (consume(&input)) failed = 1;
    while (!failed && !stopped) {
        ssize_t count = read(ready[0], &byte, 1);
        if (count == 1) { grouped = byte == 'R'; failed = !grouped; break; }
        if (!count || (count < 0 && errno != EINTR && errno != EAGAIN)) { failed = 1; break; }
        now = stamp();
        if (now < origin || now - origin >= work) { failed = 1; break; }
        (void)nanosleep(&pause, NULL);
    }
    now = stamp();
    if (stopped || now < origin || now - origin >= work || !grouped) failed = 1;
    if (!failed && write(grant[1], "G", 1) != 1) failed = 1;
    if (consume(&ready[0])) failed = 1;
    if (consume(&grant[1])) failed = 1;
    while (!failed && !stopped) {
        memset(&peek, 0, sizeof(peek));
        if (waitid(P_PID, (id_t)child, &peek, WEXITED | WNOHANG | WNOWAIT)) {
            if (errno == EINTR) continue;
            lost = failed = 1; break;
        }
        if (peek.si_pid) { if (peek.si_pid != child) lost = failed = 1; break; }
        now = stamp();
        if (now < origin || now - origin >= work) { failed = 1; break; }
        (void)nanosleep(&pause, NULL);
    }
    now = stamp();
    if (stopped || now < origin || now - origin >= work) failed = 1;
    /* The original unreaped child reserves identity through the LAST signal. */
    if (!lost && kill(grouped ? -child : child, SIGKILL) && errno != ESRCH) failed = 1;
    while (!lost) {
        waited = wait4(child, &status, WNOHANG, &usage);
        if (waited == child) { consumed = 1; break; }
        if (waited < 0 && errno != EINTR) { lost = 1; break; }
        now = stamp();
        if (now < origin || now - origin >= total) break;
        (void)nanosleep(&pause, NULL);
    }
    now = stamp();
    if (now < origin || now - origin >= total) return 4;
    size = snprintf(receipt, sizeof(receipt), "artifacts/%s.receipt", nq_build_names[job]);
    if (size < 0 || (size_t)size >= sizeof(receipt)) return 4;
    fd = open(receipt, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (fd < 0) return 4;
    now = stamp();
    if (now < origin || now - origin >= total) { (void)consume(&fd); return 4; }
    size = snprintf(line, sizeof(line), "NQ_COMPILE_END %u %jd %u %u %u %u %d %lld %ld\n",
        job, (intmax_t)child, grouped, consumed, lost, failed, status,
        now < origin ? -1 : now - origin, consumed ? usage.ru_maxrss : -1L);
    if (size < 0 || (size_t)size >= sizeof(line) || write(fd, line, (size_t)size) != size) {
        (void)close(fd); return 4;
    }
    if (consume(&fd)) return 4;
    now = stamp();
    if (stopped || !consumed || lost || failed || now < origin || now - origin >= total ||
        !WIFEXITED(status) || WEXITSTATUS(status) || usage.ru_maxrss > 524288) return 4;
    alarm(0);
    return 0;
}
#endif

int main(int argc, char **argv)
{
#ifdef __OpenBSD__
    return build(argc, argv);
#else
    (void)argc; (void)argv;
#ifdef BW_NQ_SOURCE_CHECK
    (void)&build;
#endif
    return 77;
#endif
}
