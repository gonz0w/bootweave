/* SPDX-License-Identifier: BSD-3-Clause */
/* Source preparation only. Collection is not qualification or run authority. */
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
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <inttypes.h>
#include <poll.h>
#include <pwd.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if defined(__OpenBSD__) || defined(BW_NQ_SOURCE_CHECK)
#include "ORACLE-GATES.h"
/* These declarations only expose otherwise inactive source to host syntax
 * checks. They cannot enable the OpenBSD main or supply host implementations. */
#ifndef __OpenBSD__
extern int setresuid(uid_t, uid_t, uid_t);
extern int setresgid(gid_t, gid_t, gid_t);
extern int nq_openbsd_closefrom(int);
#define closefrom nq_openbsd_closefrom
#endif

#define ROOT "/bwq-92166f07"
#define ORDINARY 32767
#define STREAM_CAP 1024
#define EVENT_CAP 32768
#define EVENT_COUNT 256

struct writer {
    int fd, failed;
    size_t bytes;
    unsigned records;
};

struct spec {
    const char *directory, *mode;
    int entry, root_identity, missing, file;
};

static const struct spec cases[] = {
    {"00-parent-readback", "parent-readback", 0, 0, -1, 0},
    {"01-child-readback", "child-readback", 0, 0, -1, 0},
    {"02-parent-file-size", "parent-file-size", 0, 0, -1, 1},
    {"03-child-file-size", "child-file-size", 0, 0, -1, 1},
    {"04-entry-normal-refusal", NULL, 1, 0, -1, 0},
    {"05-entry-root-refusal", NULL, 1, 1, -1, 0},
    {"06-entry-missing-0", NULL, 1, 0, 0, 0},
    {"07-entry-missing-3", NULL, 1, 0, 3, 0},
    {"08-entry-missing-4", NULL, 1, 0, 4, 0}
};
static const char *input_names[] = {"definition.bin", "payload.bin", "signature.bin"};
static const off_t input_caps[] = {16384, 1048576, 16384};
static const int input_roles[] = {0, 3, 4};
static const char *stream_names[] = {"stdout", "stderr", "setup"};
static char *const environment[] = {
    "PATH=/usr/bin:/bin", "HOME=/nonexistent", "TMPDIR=/tmp",
    "LC_ALL=C", "LANG=C", NULL
};

struct collection {
    struct writer events;
    int directory, executable, inputs[3], file;
    int pipes[3][2], outputs[3], gate[2];
    struct stat input_before[3], executable_before, file_before, output_before[3];
    size_t observed[3], written[3], setup_length;
    unsigned char setup[STREAM_CAP];
    int eof[3], failed, completed, lost, stopped, consumed, group_owned, granted;
    int status;
    pid_t pid;
    struct timespec start;
};

static struct timespec suite_start, last_stamp;
static struct writer journal;
static volatile sig_atomic_t cancelled;

static int
emit(struct writer *w, const char *format, ...)
{
    char line[512];
    va_list args;
    int count;
    ssize_t written;
    if (w->failed || w->fd < 0) return -1;
    va_start(args, format);
    count = vsnprintf(line, sizeof(line), format, args);
    va_end(args);
    if (count < 0 || (size_t)count >= sizeof(line) ||
        w->records >= EVENT_COUNT || w->bytes + (size_t)count > EVENT_CAP) {
        w->failed = 1;
        return -1;
    }
    w->records++;
    written = write(w->fd, line, (size_t)count);
    if (written > 0) w->bytes += (size_t)written;
    if (written != count) {
        w->failed = 1;
        return -1; /* Never resend an uncertain or partial receipt. */
    }
    return 0;
}

static int
stamp(struct timespec *value)
{
    if (clock_gettime(CLOCK_MONOTONIC, value) == -1 || value->tv_sec < 0 ||
        (uintmax_t)value->tv_sec > INT64_MAX - 1000 ||
        value->tv_nsec < 0 || value->tv_nsec >= 1000000000L ||
        value->tv_sec < last_stamp.tv_sec ||
        (value->tv_sec == last_stamp.tv_sec && value->tv_nsec < last_stamp.tv_nsec))
        return -1;
    last_stamp = *value;
    return 0;
}

static int
before(const struct timespec *now, const struct timespec *start, int seconds)
{
    return now->tv_sec < start->tv_sec + seconds ||
        (now->tv_sec == start->tv_sec + seconds && now->tv_nsec < start->tv_nsec);
}

static int
budget(struct collection *c, int seconds, int work)
{
    struct timespec now;
    if (cancelled) c->failed = 1;
    if ((work && cancelled) || stamp(&now) == -1 || !before(&now, &suite_start, 900) ||
        !before(&now, &c->start, seconds)) {
        c->failed = 1;
        return 0;
    }
    return 1;
}

static int
live(struct collection *c, int seconds)
{
    return budget(c, seconds, 1);
}

static int
timer_until(const struct timespec *origin, int seconds, struct itimerval *timer)
{
    struct timespec now;
    time_t remaining;
    long nanos;
    if (stamp(&now) == -1 || !before(&now, origin, seconds)) return -1;
    remaining = origin->tv_sec + seconds - now.tv_sec;
    nanos = origin->tv_nsec - now.tv_nsec;
    if (nanos < 0) { remaining--; nanos += 1000000000L; }
    memset(timer, 0, sizeof(*timer));
    timer->it_value.tv_sec = remaining;
    timer->it_value.tv_usec = nanos / 1000; /* Round down; never renew the origin. */
    return remaining || timer->it_value.tv_usec ? 0 : -1;
}

static int
bad(const struct collection *c)
{
    return c->failed || c->events.failed || journal.failed;
}

static void
close_owned(struct collection *c, int *number, const char *role)
{
    int fd, result, saved;
    if (*number < 0) return;
    fd = *number;
    *number = -1; /* Consume the numeric role before the only close attempt. */
    errno = 0;
    result = close(fd);
    saved = result == -1 ? errno : 0;
    if (result == -1) c->failed = 1;
    (void)emit(&c->events, "CLOSE %s %d %d %d\n", role, fd, result, saved);
}

static int
promote(struct collection *c, int *fd)
{
    int old, moved;
    if (*fd < 0) return -1;
    if (*fd >= 10) return 0;
    moved = fcntl(*fd, F_DUPFD_CLOEXEC, 10);
    if (moved == -1) return -1;
    old = *fd;
    *fd = moved;
    close_owned(c, &old, "promotion-original");
    return bad(c) ? -1 : 0;
}

static int
same(const struct stat *a, const struct stat *b)
{
    return a->st_dev == b->st_dev && a->st_ino == b->st_ino &&
        a->st_mode == b->st_mode && a->st_uid == b->st_uid &&
        a->st_gid == b->st_gid && a->st_nlink == b->st_nlink &&
        a->st_size == b->st_size && a->st_mtime == b->st_mtime &&
        a->st_ctime == b->st_ctime;
}

static int
object(struct writer *w, const char *role, int fd, const struct stat *s)
{
    off_t offset = lseek(fd, 0, SEEK_CUR);
    int flags = fcntl(fd, F_GETFL);
    int result = emit(w, "OBJECT %s fd=%d dev=%ju ino=%ju uid=%ju gid=%ju mode=%jo "
        "links=%ju size=%jd mtime=%jd ctime=%jd offset=%jd flags=%d\n",
        role, fd, (uintmax_t)s->st_dev, (uintmax_t)s->st_ino,
        (uintmax_t)s->st_uid, (uintmax_t)s->st_gid, (uintmax_t)s->st_mode,
        (uintmax_t)s->st_nlink, (intmax_t)s->st_size, (intmax_t)s->st_mtime,
        (intmax_t)s->st_ctime, (intmax_t)offset, flags);
    return offset < 0 || flags < 0 ? -1 : result;
}

static int
directory_at(int parent, const char *name, mode_t mode)
{
    struct stat held, named;
    int fd = openat(parent, name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (fd == -1) return -1;
    if (fstat(fd, &held) == -1 || fstatat(parent, name, &named, AT_SYMLINK_NOFOLLOW) == -1 ||
        !same(&held, &named) || !S_ISDIR(held.st_mode) ||
        held.st_uid != 0 || held.st_gid != 0 || (held.st_mode & 07777) != mode ||
        held.st_nlink == 0) {
        /* A failed close is still consumed; the caller aborts without retry. */
        (void)close(fd);
        return -1;
    }
    return fd;
}

static int
regular_at(struct collection *c, int parent, const char *name, int executable,
    off_t cap, struct stat *before_stat)
{
    struct stat held, named;
    int fd, flags;
    fd = openat(parent, name, O_RDONLY | O_NONBLOCK | O_NOFOLLOW | O_CLOEXEC);
    if (fd == -1) return -1;
    if (promote(c, &fd) == -1 || fstat(fd, &held) == -1 ||
        fstatat(parent, name, &named, AT_SYMLINK_NOFOLLOW) == -1 ||
        !same(&held, &named) || !S_ISREG(held.st_mode) || held.st_nlink != 1 ||
        held.st_uid != 0 || held.st_gid != 0 ||
        (held.st_mode & 07777) != (executable ? 0555 : 0444) ||
        held.st_size <= 0 || held.st_size > cap || lseek(fd, 0, SEEK_CUR) != 0 ||
        (flags = fcntl(fd, F_GETFL)) == -1 || (flags & O_ACCMODE) != O_RDONLY) {
        close_owned(c, &fd, "invalid-source");
        return -1;
    }
    *before_stat = held;
    if (object(&c->events, name, fd, &held) == -1) c->failed = 1;
    return fd;
}

static int
new_file(struct collection *c, const char *name, struct stat *seen)
{
    int fd = openat(c->directory, name,
        O_RDWR | O_NONBLOCK | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (fd == -1) return -1;
    if (fstat(fd, seen) == -1 || !S_ISREG(seen->st_mode) || seen->st_uid != 0 ||
        seen->st_gid != 0 || (seen->st_mode & 07777) != 0600 ||
        seen->st_nlink != 1 || seen->st_size != 0 || lseek(fd, 0, SEEK_CUR) != 0) {
        close_owned(c, &fd, "invalid-output");
        return -1;
    }
    return fd;
}

static int
new_pipe(struct collection *c, int ends[2])
{
    int i, flags;
    struct stat seen;
    if (pipe(ends) == -1) return -1;
    for (i = 0; i < 2; i++) {
        if (promote(c, &ends[i]) == -1 ||
            (flags = fcntl(ends[i], F_GETFL)) == -1 ||
            fcntl(ends[i], F_SETFL, flags | O_NONBLOCK) == -1 ||
            fcntl(ends[i], F_SETFD, FD_CLOEXEC) == -1 ||
            fstat(ends[i], &seen) == -1 || !S_ISFIFO(seen.st_mode)) return -1;
    }
    return 0;
}

static void
child_failure(int stage)
{
    char line[96];
    int saved = errno, n = snprintf(line, sizeof(line), "ERROR %d %d\n", stage, saved);
    if (n > 0 && (size_t)n < sizeof(line)) (void)write(5, line, (size_t)n);
    _exit(76);
}

static void
child_run(const struct collection *c, const struct spec *spec, const char *path)
{
    static const int signals[] = {SIGALRM, SIGPROF, SIGXCPU, SIGXFSZ,
        SIGPIPE, SIGCHLD, SIGINT, SIGTERM, SIGHUP};
    struct sigaction action;
    struct itimerval timer, seen;
    struct pollfd gate;
    sigset_t empty;
    gid_t group = ORDINARY;
    char grant, ready[128];
    char *argv[3];
    int i, n;

    /* Sources have all been promoted above the complete fixed mapping. */
    for (i = 0; i < 3; i++)
        if (dup2(c->inputs[i], input_roles[i]) == -1) _exit(76);
    if (dup2(c->pipes[0][1], 1) == -1 || dup2(c->pipes[1][1], 2) == -1 ||
        dup2(c->pipes[2][1], 5) == -1 || dup2(c->gate[0], 6) == -1 ||
        fcntl(5, F_SETFD, FD_CLOEXEC) == -1) _exit(76);
    if (spec->file && dup2(c->file, 3) == -1) child_failure(1);
    if (spec->missing >= 0 && close(spec->missing) == -1) child_failure(2);
    if (closefrom(7) == -1) child_failure(3);
    memset(&action, 0, sizeof(action));
    action.sa_handler = SIG_DFL;
    if (sigemptyset(&action.sa_mask) == -1 || sigemptyset(&empty) == -1)
        child_failure(4);
    for (i = 0; i < (int)(sizeof(signals) / sizeof(signals[0])); i++)
        if (sigaction(signals[i], &action, NULL) == -1) child_failure(5);
    if (sigprocmask(SIG_SETMASK, &empty, NULL) == -1) child_failure(6);
    memset(&timer, 0, sizeof(timer));
    if (setitimer(ITIMER_PROF, &timer, NULL) == -1) child_failure(7);
    /* Same original case deadline, not twenty new seconds after child setup. */
    if (timer_until(&c->start, 20, &timer) == -1 ||
        setitimer(ITIMER_REAL, &timer, NULL) == -1) child_failure(8);
    if (!spec->root_identity && (setgroups(1, &group) == -1 ||
        setresgid(ORDINARY, ORDINARY, ORDINARY) == -1 ||
        setresuid(ORDINARY, ORDINARY, ORDINARY) == -1)) child_failure(9);
    if (getuid() != (uid_t)(spec->root_identity ? 0 : ORDINARY) ||
        geteuid() != getuid() || getgid() != (gid_t)(spec->root_identity ? 0 : ORDINARY) ||
        getegid() != getgid() || (!spec->root_identity &&
        (getgroups(1, &group) != 1 || group != ORDINARY))) child_failure(10);
    gate.fd = 6;
    gate.events = POLLIN;
    gate.revents = 0;
    if (poll(&gate, 1, 15000) <= 0 || !(gate.revents & POLLIN) ||
        read(6, &grant, 1) != 1 || grant != 'G') child_failure(11);
    if (close(6) == -1 || getpgrp() != getpid()) child_failure(12);
    memset(&timer, 0, sizeof(timer));
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1 ||
        getitimer(ITIMER_REAL, &seen) == -1 || seen.it_value.tv_sec ||
        seen.it_value.tv_usec || seen.it_interval.tv_sec || seen.it_interval.tv_usec ||
        getitimer(ITIMER_PROF, &seen) == -1 || seen.it_value.tv_sec ||
        seen.it_value.tv_usec || seen.it_interval.tv_sec || seen.it_interval.tv_usec)
        child_failure(13);
    n = snprintf(ready, sizeof(ready), "READY %ju %ju %ju %ju %jd\n",
        (uintmax_t)getuid(), (uintmax_t)geteuid(), (uintmax_t)getgid(),
        (uintmax_t)getegid(), (intmax_t)getpgrp());
    if (n < 0 || (size_t)n >= sizeof(ready) || write(5, ready, (size_t)n) != n)
        child_failure(14);
    argv[0] = (char *)path;
    argv[1] = (char *)spec->mode;
    argv[2] = NULL;
    execve(path, argv, environment);
    child_failure(15);
}

static void
observe(struct collection *c)
{
    siginfo_t info;
    int result, saved;
    if (c->pid <= 0 || c->completed || c->lost || c->consumed) return;
    memset(&info, 0, sizeof(info));
    errno = 0;
    result = waitid(P_PID, (id_t)c->pid, &info, WEXITED | WNOHANG | WNOWAIT);
    saved = result == -1 ? errno : 0;
    if (result == -1 && saved == EINTR) return;
    if (result == -1 || (info.si_pid && info.si_pid != c->pid)) {
        c->lost = c->failed = 1;
        (void)emit(&c->events, "OBSERVE_ERROR %d %d pid=%jd\n",
            result, saved, (intmax_t)info.si_pid);
    } else if (info.si_pid == c->pid) {
        c->completed = 1;
        (void)emit(&c->events, "WNOWAIT pid=%jd code=%d status=%d\n",
            (intmax_t)info.si_pid, info.si_code, info.si_status);
    }
}

static void
stop_owned(struct collection *c)
{
    int result, saved;
    pid_t target;
    if (c->pid <= 0 || c->stopped || c->lost || c->consumed) return;
    c->stopped = 1;
    target = c->group_owned ? -c->pid : c->pid;
    errno = 0;
    result = kill(target, SIGKILL);
    saved = result == -1 ? errno : 0;
    if (result == -1) c->failed = 1;
    (void)emit(&c->events, "LAST_STOP target=%jd group_owned=%d result=%d errno=%d\n",
        (intmax_t)target, c->group_owned, result, saved);
}

static void
drain(struct collection *c, unsigned index)
{
    unsigned char buffer[256];
    ssize_t count, written;
    size_t take;
    int saved;
    if (c->pipes[index][0] < 0) return;
    take = STREAM_CAP - c->written[index] + 1;
    if (take > sizeof(buffer)) take = sizeof(buffer);
    errno = 0;
    count = read(c->pipes[index][0], buffer, take);
    saved = count == -1 ? errno : 0;
    if (count == -1 && (saved == EAGAIN || saved == EINTR)) return;
    (void)emit(&c->events, "READ %s result=%jd errno=%d\n",
        stream_names[index], (intmax_t)count, saved);
    if (count <= 0) {
        if (count == 0) c->eof[index] = 1;
        else c->failed = 1;
        close_owned(c, &c->pipes[index][0], stream_names[index]);
        return;
    }
    c->observed[index] += (size_t)count;
    take = (size_t)count;
    if (take > STREAM_CAP - c->written[index]) {
        take = STREAM_CAP - c->written[index];
        c->failed = 1;
    }
    if (index == 2 && take) {
        memcpy(c->setup + c->setup_length, buffer, take);
        c->setup_length += take;
    }
    errno = 0;
    written = take ? write(c->outputs[index], buffer, take) : 0;
    saved = written == -1 ? errno : 0;
    (void)emit(&c->events, "PUBLISH %s requested=%zu result=%jd errno=%d\n",
        stream_names[index], take, (intmax_t)written, saved);
    if (written > 0) c->written[index] += (size_t)written;
    if (written != (ssize_t)take) c->failed = 1;
    if (bad(c)) close_owned(c, &c->pipes[index][0], stream_names[index]);
}

static void
collect(struct collection *c)
{
    struct pollfd ready[3];
    struct rusage usage;
    struct timespec now;
    unsigned i, attempts;
    pid_t waited;
    int result, saved;
    for (attempts = 0; attempts < 10000; attempts++) {
        observe(c);
        if (c->lost || !budget(c, 20, 0)) break;
        if (stamp(&now) == -1 || !before(&now, &c->start, 15)) c->failed = 1;
        if (bad(c) || (c->completed && c->eof[0] && c->eof[1] && c->eof[2]))
            stop_owned(c);
        if (c->completed && c->eof[0] && c->eof[1] && c->eof[2]) break;
        for (i = 0; i < 3; i++) {
            ready[i].fd = c->pipes[i][0];
            ready[i].events = POLLIN;
            ready[i].revents = 0;
        }
        result = poll(ready, 3, 20);
        saved = result == -1 ? errno : 0;
        if (result == -1 && saved != EINTR) {
            c->failed = 1;
            (void)emit(&c->events, "POLL_ERROR %d\n", saved);
        }
        for (i = 0; i < 3; i++)
            if (ready[i].fd >= 0 && ready[i].revents) drain(c, i);
    }
    if (attempts == 10000) c->failed = 1;
    /* No reaping, recovery or further signals after either custody latch. */
    if (!c->lost && !c->consumed && c->completed) {
        stop_owned(c);
        memset(&usage, 0, sizeof(usage));
        errno = 0;
        waited = wait4(c->pid, &c->status, WNOHANG, &usage);
        saved = waited == -1 ? errno : 0;
        if (waited > 0) c->consumed = 1;
        if (waited != c->pid) c->lost = c->failed = 1;
        (void)emit(&c->events, "WAIT4 result=%jd errno=%d consumed=%d lost=%d "
            "raw_status=%d exited=%d exit=%d signaled=%d signal=%d "
            "maxrss_native=%jd user=%jd:%jd system=%jd:%jd\n",
            (intmax_t)waited, saved, c->consumed, c->lost, c->status,
            waited == c->pid && WIFEXITED(c->status),
            waited == c->pid && WIFEXITED(c->status) ? WEXITSTATUS(c->status) : -1,
            waited == c->pid && WIFSIGNALED(c->status),
            waited == c->pid && WIFSIGNALED(c->status) ? WTERMSIG(c->status) : -1,
            (intmax_t)usage.ru_maxrss, (intmax_t)usage.ru_utime.tv_sec,
            (intmax_t)usage.ru_utime.tv_usec, (intmax_t)usage.ru_stime.tv_sec,
            (intmax_t)usage.ru_stime.tv_usec);
        if (!c->lost && !WIFEXITED(c->status)) c->failed = 1;
    } else if (!c->consumed) {
        c->failed = 1;
        stop_owned(c);
        (void)emit(&c->events, "CUSTODY_UNRESOLVED lost=%d completed=%d\n", c->lost, c->completed);
    }
}

static void
initialize(struct collection *c)
{
    unsigned i;
    memset(c, 0, sizeof(*c));
    c->events.fd = c->directory = c->executable = c->file = -1;
    c->gate[0] = c->gate[1] = -1;
    for (i = 0; i < 3; i++) {
        c->inputs[i] = c->outputs[i] = -1;
        c->pipes[i][0] = c->pipes[i][1] = -1;
    }
}

static void
final_objects(struct collection *c, int inputs, int artifacts, const char *binary)
{
    struct stat held, named;
    unsigned char data[4096];
    unsigned i, j;
    off_t offset;
    ssize_t count;
    for (i = 0; i < 3; i++) {
        if (c->inputs[i] < 0) continue;
        if (fstat(c->inputs[i], &held) == -1 ||
            fstatat(inputs, input_names[i], &named, AT_SYMLINK_NOFOLLOW) == -1 ||
            !same(&held, &c->input_before[i]) || !same(&held, &named) ||
            lseek(c->inputs[i], 0, SEEK_CUR) != 0) c->failed = 1;
        else if (object(&c->events, input_names[i], c->inputs[i], &held) == -1) c->failed = 1;
    }
    if (c->executable >= 0) {
        if (fstat(c->executable, &held) == -1 ||
            fstatat(artifacts, binary, &named, AT_SYMLINK_NOFOLLOW) == -1 ||
            !same(&held, &c->executable_before) || !same(&held, &named)) c->failed = 1;
        else if (object(&c->events, binary, c->executable, &held) == -1) c->failed = 1;
    }
    for (i = 0; i < 3; i++) {
        if (c->outputs[i] < 0) continue;
        if (fstat(c->outputs[i], &held) == -1 ||
            fstatat(c->directory, stream_names[i], &named, AT_SYMLINK_NOFOLLOW) == -1 ||
            held.st_dev != c->output_before[i].st_dev || held.st_ino != c->output_before[i].st_ino ||
            held.st_mode != c->output_before[i].st_mode || held.st_uid != 0 || held.st_gid != 0 ||
            held.st_nlink != 1 || held.st_size != (off_t)c->written[i] || !same(&held, &named) ||
            lseek(c->outputs[i], 0, SEEK_CUR) != (off_t)c->written[i]) c->failed = 1;
        else if (object(&c->events, stream_names[i], c->outputs[i], &held) == -1) c->failed = 1;
    }
    if (c->file >= 0) {
        if (fstat(c->file, &held) == -1 ||
            fstatat(c->directory, "fsize", &named, AT_SYMLINK_NOFOLLOW) == -1 ||
            !same(&held, &named) || held.st_dev != c->file_before.st_dev ||
            held.st_ino != c->file_before.st_ino || !S_ISREG(held.st_mode) ||
            held.st_uid != ORDINARY || held.st_gid != ORDINARY ||
            (held.st_mode & 07777) != 0600 || held.st_nlink != 1 ||
            held.st_size < 0 || held.st_size > 65536) c->failed = 1;
        else {
            if (object(&c->events, "fsize-final", c->file, &held) == -1) c->failed = 1;
            for (offset = 0; offset < held.st_size; offset += count) {
                if (!budget(c, 20, 0)) break;
                count = pread(c->file, data, sizeof(data), offset);
                if (count <= 0 || count > held.st_size - offset) { c->failed = 1; break; }
                for (j = 0; j < (unsigned)count; j++) if (data[j] != 'X') c->failed = 1;
            }
            if (fstat(c->file, &named) == -1 || !same(&held, &named)) c->failed = 1;
        }
    }
    (void)budget(c, 20, 0);
}

static int
held_stream(struct collection *c, unsigned index, unsigned char *bytes)
{
    struct stat seen;
    ssize_t count;
    size_t length = c->written[index];
    if (length > STREAM_CAP || !c->eof[index] || c->outputs[index] < 0 ||
        fstat(c->outputs[index], &seen) == -1 || seen.st_size != (off_t)length)
        return -1;
    /* One bounded positional read of the original held object, including the
     * expected EOF position. A short/error/extra read is not retried. */
    count = pread(c->outputs[index], bytes, length + 1, 0);
    if (count != (ssize_t)length || memchr(bytes, 0, length)) return -1;
    bytes[length] = 0;
    return 0;
}

static int
literal(const unsigned char **cursor, const unsigned char *end, const char *text)
{
    size_t length = strlen(text);
    if ((size_t)(end - *cursor) < length || memcmp(*cursor, text, length)) return -1;
    *cursor += length;
    return 0;
}

static int
decimal(const unsigned char **cursor, const unsigned char *end, unsigned cap, unsigned *value)
{
    const unsigned char *first = *cursor;
    unsigned result = 0, digit;
    if (first == end || *first < '0' || *first > '9') return -1;
    while (*cursor < end && **cursor >= '0' && **cursor <= '9') {
        digit = (unsigned)(**cursor - '0');
        if (result > cap / 10 || (result == cap / 10 && digit > cap % 10)) return -1;
        result = result * 10 + digit;
        (*cursor)++;
    }
    if (*first == '0' && *cursor - first != 1) return -1;
    *value = result;
    return 0;
}

static int
limit_record(const unsigned char *bytes, size_t length, const char *prefix)
{
    static const unsigned caps[] = {15, 999999, 0, 0, 4, 999999, 0, 0};
    const unsigned char *cursor = bytes, *end = bytes + length;
    unsigned values[8], i;
    if (!prefix || literal(&cursor, end, prefix) == -1 ||
        literal(&cursor, end, " REAL ") == -1) return -1;
    for (i = 0; i < 8; i++) {
        if (decimal(&cursor, end, caps[i], &values[i]) == -1 ||
            literal(&cursor, end, i == 3 ? " PROF " : i == 7 ? "\n" : " ") == -1)
            return -1;
    }
    return cursor == end && (values[0] || values[1]) && (values[4] || values[5]) &&
        (values[0] != 15 || values[1] == 0) && (values[4] != 4 || values[5] == 0) ? 0 : -1;
}

static int
oracle_gate(struct collection *c, unsigned index)
{
    unsigned char output[STREAM_CAP + 1], error[STREAM_CAP + 1];
    struct stat file;
    const char *expected;
    size_t length;
    if (index >= sizeof(cases) / sizeof(cases[0]) || !c->consumed || c->lost ||
        !WIFEXITED(c->status) || WEXITSTATUS(c->status) != nq_expected_exit[index] ||
        !budget(c, 20, 0) || held_stream(c, 0, output) == -1 ||
        held_stream(c, 1, error) == -1) return -1;
    if (index < 2)
        return c->written[1] == 0 ? limit_record(output, c->written[0], nq_expected_limits[index]) : -1;
    if (nq_expected_fsize[index] >= 0)
        return !c->written[0] && !c->written[1] && c->file >= 0 &&
            fstat(c->file, &file) == 0 && file.st_size == nq_expected_fsize[index] ? 0 : -1;
    expected = nq_expected_entry[index];
    if (!expected || c->written[0]) return -1;
    length = strlen(expected);
    return length == c->written[1] && !memcmp(error, expected, length) ? 0 : -1;
}

static int
one_case(unsigned index, int run, int inputs, int artifacts)
{
    const struct spec *spec = &cases[index];
    const char *binary = spec->entry ? "nq-closed-dynamic" : "nq-limits-dynamic";
    const char *path = spec->entry ? ROOT "/artifacts/nq-closed-dynamic" : ROOT "/artifacts/nq-limits-dynamic";
    struct collection c;
    struct stat seen;
    struct itimerval timer;
    char expected[128];
    int marker = -1, group_result, group_errno, flags, n, result;
    unsigned i;
    initialize(&c);
    /* Never start a slot without its complete work-plus-finalization allowance. */
    if (stamp(&c.start) == -1 || !before(&c.start, &suite_start, 880) || cancelled) return -1;
    /* Tighten the original suite watchdog to this complete original slot.
     * Every accepted slot ends before suite900, so this cannot extend hard910.
     * Keep it armed through file publication, closes and return to the caller. */
    if (timer_until(&c.start, 20, &timer) == -1 ||
        setitimer(ITIMER_REAL, &timer, NULL) == -1) return -1;
    if (mkdirat(run, spec->directory, 0700) == -1) return -1;
    c.directory = directory_at(run, spec->directory, 0700);
    if (c.directory < 0) return -1;
    c.events.fd = new_file(&c, "events", &seen);
    if (c.events.fd < 0) goto done;
    (void)emit(&journal, "CASE %u %s start=%jd:%ld\n", index, spec->directory,
        (intmax_t)c.start.tv_sec, c.start.tv_nsec);
    marker = new_file(&c, "started", &seen);
    if (marker < 0) goto done;
    close_owned(&c, &marker, "started-marker");
    for (i = 0; i < 3; i++) {
        c.inputs[i] = regular_at(&c, inputs, input_names[i], 0, input_caps[i], &c.input_before[i]);
        c.outputs[i] = new_file(&c, stream_names[i], &c.output_before[i]);
        if (c.inputs[i] < 0 || c.outputs[i] < 0 || new_pipe(&c, c.pipes[i]) == -1) goto done;
    }
    c.executable = regular_at(&c, artifacts, binary, 1, 8 * 1024 * 1024, &c.executable_before);
    if (c.executable < 0 || new_pipe(&c, c.gate) == -1) goto done;
    if (spec->file) {
        c.file = new_file(&c, "fsize", &seen);
        if (c.file < 0 || promote(&c, &c.file) == -1 ||
            fchown(c.file, ORDINARY, ORDINARY) == -1 ||
            fstat(c.file, &c.file_before) == -1 ||
            c.file_before.st_uid != ORDINARY || c.file_before.st_gid != ORDINARY ||
            (c.file_before.st_mode & 07777) != 0600 || c.file_before.st_size != 0 ||
            c.file_before.st_nlink != 1 || (flags = fcntl(c.file, F_GETFL)) == -1 ||
            (flags & O_ACCMODE) != O_RDWR || (flags & O_APPEND) || lseek(c.file, 0, SEEK_CUR) != 0)
            goto done;
        if (object(&c.events, "fsize-initial", c.file, &c.file_before) == -1) c.failed = 1;
    }
    if (bad(&c) || !live(&c, 15)) goto done;
    c.pid = fork();
    if (c.pid == 0) child_run(&c, spec, path);
    if (c.pid == -1) goto done;
    (void)emit(&c.events, "BORN pid=%jd original_case_start=%jd:%ld\n",
        (intmax_t)c.pid, (intmax_t)c.start.tv_sec, c.start.tv_nsec);
    for (i = 0; i < 3; i++) close_owned(&c, &c.pipes[i][1], "parent-unused-writer");
    close_owned(&c, &c.gate[0], "parent-unused-gate-reader");
    errno = 0;
    group_result = setpgid(c.pid, c.pid);
    group_errno = group_result == -1 ? errno : 0;
    if (group_result == 0) {
        c.group_owned = 1; /* Actual successful operation, never a sampled guess. */
        if (getpgid(c.pid) != c.pid) c.failed = 1;
    } else c.failed = 1;
    (void)emit(&c.events, "SETGROUP pid=%jd requested=%jd result=%d errno=%d owned=%d\n",
        (intmax_t)c.pid, (intmax_t)c.pid, group_result, group_errno, c.group_owned);
    if (!bad(&c) && live(&c, 15)) {
        errno = 0;
        result = (int)write(c.gate[1], "G", 1);
        n = result == -1 ? errno : 0;
        c.granted = result == 1;
        if (!c.granted) c.failed = 1;
        (void)emit(&c.events, "GRANT requested=1 result=%d errno=%d\n", result, n);
    }
    close_owned(&c, &c.gate[1], "parent-gate-writer");
    collect(&c);
    n = snprintf(expected, sizeof(expected), "READY %d %d %d %d %jd\n",
        spec->root_identity ? 0 : ORDINARY, spec->root_identity ? 0 : ORDINARY,
        spec->root_identity ? 0 : ORDINARY, spec->root_identity ? 0 : ORDINARY, (intmax_t)c.pid);
    if (n < 0 || (size_t)n >= sizeof(expected) || c.setup_length != (size_t)n ||
        memcmp(c.setup, expected, (size_t)n) || !c.granted) c.failed = 1;
    /* Never inspect or seek a shared candidate OFD after custody was lost or
     * while its original child is unresolved. Retention is the only outcome. */
    if (c.consumed && !c.lost) {
        final_objects(&c, inputs, artifacts, binary);
        /* These source-bound necessary gates only stop dependent cases. They
         * do not replace the independent byte/hash/identity/terminal auditor. */
        result = bad(&c) ? -1 : oracle_gate(&c, index);
        if (result == -1) c.failed = 1;
        (void)emit(&c.events, "ORACLE_GATE result=%d independent_qualification=0\n", result);
    }
done:
    if (c.pid <= 0 || !c.consumed || c.lost) c.failed = 1;
    for (i = 0; i < 3; i++) {
        if (c.pid > 0 && !c.eof[i]) c.failed = 1;
        close_owned(&c, &c.pipes[i][0], "final-reader");
        close_owned(&c, &c.pipes[i][1], "final-writer");
        close_owned(&c, &c.inputs[i], "input-original");
        close_owned(&c, &c.outputs[i], "retained-stream-file");
    }
    close_owned(&c, &c.gate[0], "final-gate-reader");
    close_owned(&c, &c.gate[1], "final-gate-writer");
    close_owned(&c, &c.executable, "held-executable");
    close_owned(&c, &c.file, "fsize-original");
    close_owned(&c, &marker, "started-marker-final");
    (void)budget(&c, 20, 0);
    (void)emit(&c.events, "END failed=%d completed=%d consumed=%d lost=%d status=%d "
        "stdout=%zu:%zu:%d stderr=%zu:%zu:%d setup=%zu:%zu:%d\n", c.failed,
        c.completed, c.consumed, c.lost, c.status,
        c.observed[0], c.written[0], c.eof[0], c.observed[1], c.written[1], c.eof[1],
        c.observed[2], c.written[2], c.eof[2]);
    result = bad(&c) ? -1 : 0;
    if (c.events.fd >= 0) {
        int fd = c.events.fd;
        c.events.fd = -1;
        errno = 0;
        n = close(fd);
        if (n == -1) result = -1;
        (void)emit(&journal, "CASE_EVENTS_CLOSE %u result=%d errno=%d\n", index, n, n == -1 ? errno : 0);
    }
    if (c.directory >= 0) {
        errno = 0;
        n = close(c.directory);
        c.directory = -1;
        if (n == -1) result = -1;
        (void)emit(&journal, "CASE_DIRECTORY_CLOSE %u result=%d errno=%d\n", index, n, n == -1 ? errno : 0);
    }
    if (!budget(&c, 20, 0)) result = -1;
    (void)emit(&journal, "CASE_END %u collection_result=%d consumed=%d lost=%d\n",
        index, result, c.consumed, c.lost);
    if (!budget(&c, 20, 0)) result = -1;
    return result || journal.failed ? -1 : 0;
}

static void
interrupt(int number)
{
    (void)number;
    cancelled = 1;
}

static void
hard_expiry(int number)
{
    (void)number;
    /* External original-supervisor custody remains mandatory for this failure.
     * No signal handler fabricates a receipt, wait or complete cleanup. */
    _exit(78);
}

static int
arm_watchdog(void)
{
    struct sigaction action;
    struct itimerval timer;
    sigset_t mask, pending;
    unsigned i;
    if (sigprocmask(SIG_SETMASK, NULL, &mask) == -1 || sigpending(&pending) == -1 ||
        sigismember(&mask, SIGALRM) != 0 || sigismember(&pending, SIGALRM) != 0)
        return -1;
    for (i = 0; i < 2; i++)
        if (getitimer(i ? ITIMER_PROF : ITIMER_REAL, &timer) == -1 ||
            timer.it_value.tv_sec || timer.it_value.tv_usec ||
            timer.it_interval.tv_sec || timer.it_interval.tv_usec) return -1;
    memset(&action, 0, sizeof(action));
    action.sa_handler = hard_expiry;
    if (sigemptyset(&action.sa_mask) == -1 || sigaction(SIGALRM, &action, NULL) == -1 ||
        timer_until(&suite_start, 910, &timer) == -1) return -1;
    /* Arm before account/database/filesystem work, from the original sample. */
    return setitimer(ITIMER_REAL, &timer, NULL);
}

static int
preflight(void)
{
    static const int kinds[] = {RLIMIT_CPU, RLIMIT_DATA, RLIMIT_STACK,
        RLIMIT_NOFILE, RLIMIT_CORE, RLIMIT_FSIZE};
    static const rlim_t caps[] = {180, 67108864, 8388608, 64, 0, 8388608};
    static const int cancellation[] = {SIGINT, SIGTERM, SIGHUP};
    struct utsname system;
    struct passwd *account;
    struct sigaction action;
    struct rlimit limit, readback;
    siginfo_t child;
    sigset_t mask, pending;
    unsigned i;
    if (getuid() != 0 || geteuid() != 0 || getgid() != 0 || getegid() != 0 ||
        uname(&system) == -1 || strcmp(system.sysname, "OpenBSD") ||
        strcmp(system.release, "7.9") || strcmp(system.machine, "amd64")) return -1;
    if (sigaction(SIGCHLD, NULL, &action) == -1 || action.sa_handler != SIG_DFL ||
        (action.sa_flags & SA_NOCLDWAIT) || sigprocmask(SIG_SETMASK, NULL, &mask) == -1 ||
        sigpending(&pending) == -1 || sigismember(&mask, SIGCHLD) != 0 ||
        sigismember(&mask, SIGALRM) != 0 || sigismember(&pending, SIGCHLD) != 0 ||
        sigismember(&pending, SIGALRM) != 0) return -1;
    /* Refuse inherited masking/pending cancellation or a previous disposition
     * owner. With default actions, an earlier arrival terminates this fresh
     * process; it cannot be silently discarded before the flag handler. */
    for (i = 0; i < 3; i++)
        if (sigismember(&mask, cancellation[i]) != 0 ||
            sigismember(&pending, cancellation[i]) != 0 ||
            sigaction(cancellation[i], NULL, &action) == -1 || action.sa_handler != SIG_DFL)
            return -1;
    memset(&action, 0, sizeof(action));
    action.sa_handler = interrupt;
    if (sigemptyset(&action.sa_mask) == -1) return -1;
    for (i = 0; i < 3; i++)
        if (sigaction(cancellation[i], &action, NULL) == -1) return -1;
    if (sigprocmask(SIG_SETMASK, NULL, &mask) == -1 || sigpending(&pending) == -1 || cancelled)
        return -1;
    for (i = 0; i < 3; i++)
        if (sigismember(&mask, cancellation[i]) != 0 ||
            sigismember(&pending, cancellation[i]) != 0) return -1;
    memset(&child, 0, sizeof(child));
    errno = 0;
    if (waitid(P_ALL, 0, &child, WEXITED | WNOHANG | WNOWAIT) != -1 || errno != ECHILD) return -1;
    for (i = 0; i < 3; i++) if (fcntl((int)i, F_GETFD) == -1) return -1;
    if (closefrom(3) == -1) return -1;
    for (i = 0; i < 6; i++) {
        if (getrlimit(kinds[i], &limit) == -1 || limit.rlim_cur < caps[i] || limit.rlim_max < caps[i]) return -1;
        limit.rlim_cur = limit.rlim_max = caps[i];
        if (setrlimit(kinds[i], &limit) == -1 || getrlimit(kinds[i], &readback) == -1 ||
            readback.rlim_cur != caps[i] || readback.rlim_max != caps[i]) return -1;
    }
    account = getpwnam("nobody");
    if (!account || account->pw_uid != ORDINARY || account->pw_gid != ORDINARY) return -1;
    memset(&action, 0, sizeof(action));
    if (sigemptyset(&action.sa_mask) == -1) return -1;
    action.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &action, NULL) == -1) return -1;
    return cancelled ? -1 : 0;
}

static int
supervise(int argc)
{
    struct stat seen, named;
    struct timespec now;
    int root = -1, inputs = -1, artifacts = -1, run = -1;
    int result = 74, fd, closed, saved;
    unsigned i;
    journal.fd = -1;
    if (argc != 1 || stamp(&suite_start) == -1 || arm_watchdog() == -1 || preflight() == -1)
        return 74;
    root = open(ROOT, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (root < 0 || fstat(root, &seen) == -1 || lstat(ROOT, &named) == -1 ||
        !same(&seen, &named) || seen.st_uid != 0 || seen.st_gid != 0 ||
        !S_ISDIR(seen.st_mode) || (seen.st_mode & 07777) != 0755 || seen.st_nlink == 0) goto done;
    inputs = directory_at(root, "inputs", 0755);
    artifacts = directory_at(root, "artifacts", 0755);
    if (inputs < 0 || artifacts < 0 || mkdirat(root, "native-nine-run", 0700) == -1) goto done;
    run = directory_at(root, "native-nine-run", 0700);
    if (run < 0) goto done;
    journal.fd = openat(run, "supervisor.receipt",
        O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK, 0600);
    if (journal.fd < 0 || fstat(journal.fd, &seen) == -1 || !S_ISREG(seen.st_mode) ||
        seen.st_uid != 0 || seen.st_gid != 0 || (seen.st_mode & 07777) != 0600 ||
        seen.st_nlink != 1 || seen.st_size != 0) goto done;
    (void)emit(&journal, "NQ_SUPERVISOR_V1 pid=%jd pgid=%jd start=%jd:%ld suite=900 hard=910 "
        "case=20 final_reserve=5 cases=9 collection_only=1\n", (intmax_t)getpid(),
        (intmax_t)getpgrp(), (intmax_t)suite_start.tv_sec, suite_start.tv_nsec);
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
        if (stamp(&now) == -1 || !before(&now, &suite_start, 880) || cancelled ||
            journal.failed || one_case(i, run, inputs, artifacts) == -1) goto done;
    result = 0;
done:
    /* No removal or automatic disposal. Even incomplete namespaces are retained. */
    fd = artifacts; artifacts = -1;
    if (fd >= 0) { errno = 0; closed = close(fd); saved = closed == -1 ? errno : 0;
        if (closed == -1) result = 74;
        (void)emit(&journal, "ARTIFACTS_CLOSE %d %d\n", closed, saved); }
    fd = inputs; inputs = -1;
    if (fd >= 0) { errno = 0; closed = close(fd); saved = closed == -1 ? errno : 0;
        if (closed == -1) result = 74;
        (void)emit(&journal, "INPUTS_CLOSE %d %d\n", closed, saved); }
    fd = root; root = -1;
    if (fd >= 0) { errno = 0; closed = close(fd); saved = closed == -1 ? errno : 0;
        if (closed == -1) result = 74;
        (void)emit(&journal, "ROOT_CLOSE %d %d\n", closed, saved); }
    fd = run; run = -1;
    if (fd >= 0) { errno = 0; closed = close(fd); saved = closed == -1 ? errno : 0;
        if (closed == -1) result = 74;
        (void)emit(&journal, "RUN_CLOSE %d %d\n", closed, saved); }
    if (stamp(&now) == -1 || !before(&now, &suite_start, 900) || cancelled) result = 74;
    (void)emit(&journal, "END collection_result=%d final_journal_close_pending=1 native_pass_claim=0\n", result);
    if (journal.failed) result = 74;
    if (journal.fd >= 0) {
        fd = journal.fd;
        journal.fd = -1;
        if (close(fd) == -1) result = 74;
    }
    /* The END line has a pending close, not final completion authority. The
     * external original supervisor wait must also observe this final status. */
    if (stamp(&now) == -1 || !before(&now, &suite_start, 900) || cancelled) result = 74;
    return result;
}
#endif

int
main(int argc, char **argv)
{
    (void)argv;
#ifdef __OpenBSD__
    return supervise(argc);
#else
    (void)argc;
#ifdef BW_NQ_SOURCE_CHECK
    (void)&supervise;
#endif
    return 77;
#endif
}
