/* SPDX-License-Identifier: BSD-3-Clause */
#ifdef __OpenBSD__
#define _BSD_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "signature_verify.h"
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>

enum { NULL_FD, OUT_R, OUT_W, ERR_R, ERR_W, GATE_R, GATE_W, FD_COUNT };
struct operation {
    struct bw_verifier_material *material;
    struct bw_signature_observation *result;
    struct timespec started, cleanup_started;
    int fd[FD_COUNT];
    unsigned eof, uncertain, supported, cleanup_clock;
};
typedef char signature_workspace[(sizeof(struct operation) + sizeof(struct bw_signature_observation) +
    3*sizeof(struct stat) + 512 <= 8192) ? 1 : -1];

static void fail(struct operation *o, enum bw_signature_cause c) {
    if (o->result->cause == BW_SV_NONE) o->result->cause = c;
}

static int stamp(struct timespec *t) {
    return clock_gettime(CLOCK_MONOTONIC, t) == 0 && t->tv_sec >= 0 &&
        t->tv_nsec >= 0 && t->tv_nsec < 1000000000L;
}

/* Both callers supply one already-normalized clock sample. */
static int bounded(struct operation *o, const struct timespec *now,
    const struct timespec *start, int seconds, int total) {
    const struct timespec *original = &o->material->started;
    time_t elapsed, overall;
    if (original->tv_sec < 0 || original->tv_nsec < 0 ||
        original->tv_nsec >= 1000000000L || now->tv_sec < original->tv_sec ||
        (now->tv_sec == original->tv_sec && now->tv_nsec < original->tv_nsec) ||
        now->tv_sec < start->tv_sec || (now->tv_sec == start->tv_sec && now->tv_nsec < start->tv_nsec)) {
        fail(o, BW_SV_CLOCK); return 0;
    }
    elapsed = now->tv_sec - start->tv_sec;
    overall = now->tv_sec - original->tv_sec;
    return (elapsed < seconds || (elapsed == seconds && now->tv_nsec < start->tv_nsec)) &&
        (overall < total || (overall == total && now->tv_nsec < original->tv_nsec));
}

static int within(struct operation *o, const struct timespec *start, int seconds, int total) {
    struct timespec now;
    if (!stamp(&now)) { fail(o, BW_SV_CLOCK); return 0; }
    return bounded(o, &now, start, seconds, total);
}

static int live(struct operation *o) {
    if (*o->material->cancelled) { fail(o, BW_SV_CANCEL); return 0; }
    if (!within(o, &o->started, 5, 11)) { fail(o, BW_SV_DEADLINE); return 0; }
    return 1;
}

static int terminal_check(struct operation *o) {
    if (o->material->cancelled && *o->material->cancelled) fail(o, BW_SV_CANCEL);
    if (!o->cleanup_clock || !within(o, &o->cleanup_started, 4, 15)) {
        o->result->cleanup_budget_failed = 1;
        fail(o, o->cleanup_clock ? BW_SV_DEADLINE : BW_SV_CLOCK);
        return 0;
    }
    return 1;
}

static void consume(struct operation *o, unsigned i) {
    int fd = o->fd[i];
    o->fd[i] = -1;
    if (fd >= 0 && close(fd) == -1) {
        fail(o, BW_SV_CLOSE); o->result->close_errors |= 1U << i;
        if (errno != EBADF) o->result->unknown_closes |= 1U << i;
    }
}

static void release(struct operation *o) {
    enum bw_material_result result;
    if (o->material->state != BW_VM_VERIFYING) return;
    result = bw_verifier_release(o->material);
    o->result->material_close_errors = o->material->close_errors;
    o->result->material_unknown_closes = o->material->unknown_closes;
    if (result != BW_MATERIAL_OK) fail(o, BW_SV_CLOSE);
    if (result == BW_MATERIAL_UNCERTAIN) o->uncertain = 1;
}

static int descriptors(struct operation *o) {
    struct stat s[3], output;
    const struct stat *expected;
    size_t i, j;
    int flags;
    for (i = 0; i < 3; i++) {
        expected = &o->material->file[i];
        if (!live(o) || fstat(o->material->fd[i], &s[i]) == -1 ||
            !S_ISREG(s[i].st_mode) || s[i].st_nlink != 0 || s[i].st_size <= 0 ||
            s[i].st_dev != expected->st_dev || s[i].st_ino != expected->st_ino ||
            s[i].st_mode != expected->st_mode || s[i].st_uid != expected->st_uid ||
            s[i].st_gid != expected->st_gid || s[i].st_size != expected->st_size ||
            s[i].st_uid != geteuid() || !live(o) ||
            (flags = fcntl(o->material->fd[i], F_GETFL)) == -1 || (flags & O_ACCMODE) != O_RDONLY ||
            !live(o) || (flags = fcntl(o->material->fd[i], F_GETFD)) == -1 || !(flags & FD_CLOEXEC) ||
            !live(o) || lseek(o->material->fd[i], 0, SEEK_CUR) != 0) return 0;
        for (j = 0; j < i; j++)
            if (s[i].st_dev == s[j].st_dev && s[i].st_ino == s[j].st_ino) return 0;
    }
    for (i = 1; i <= 2; i++) {
        if (!live(o) || fstat((int)i, &output) == -1) return 0;
        for (j = 0; j < 3; j++)
            if (output.st_dev == s[j].st_dev && output.st_ino == s[j].st_ino) return 0;
    }
    return live(o);
}

static int channels(struct operation *o) {
    struct stat s;
    int pair[2], flags;
    unsigned i;
    if (!live(o)) return 0;
    o->fd[NULL_FD] = open("/dev/null", O_RDONLY | O_NOFOLLOW | O_CLOEXEC);
    if (o->fd[NULL_FD] < 0 || !live(o) || fstat(o->fd[NULL_FD], &s) == -1 || !S_ISCHR(s.st_mode)) return 0;
    for (i = OUT_R; i < FD_COUNT; i += 2) {
        if (!live(o) || pipe(pair) == -1) return 0;
        o->fd[i] = pair[0]; o->fd[i+1] = pair[1];
        if (!live(o) || fcntl(pair[0], F_SETFD, FD_CLOEXEC) == -1 || !live(o) ||
            fcntl(pair[1], F_SETFD, FD_CLOEXEC) == -1) return 0;
        if (i == GATE_R) continue;
        if (!live(o) || (flags = fcntl(pair[0], F_GETFL)) == -1 ||
            !live(o) || fcntl(pair[0], F_SETFL, flags | O_NONBLOCK) == -1) return 0;
    }
    return live(o);
}

static void child(struct operation *o) {
    struct sigaction action;
    sigset_t signals;
    int sources[6], copies[6];
    unsigned i;
    char grant;
    memset(&action, 0, sizeof(action)); action.sa_handler = SIG_DFL;
    if (sigemptyset(&action.sa_mask) == -1 || sigaction(SIGALRM, &action, NULL) == -1 ||
        sigaction(SIGPIPE, &action, NULL) == -1 || sigemptyset(&signals) == -1 ||
        sigaddset(&signals, SIGALRM) == -1 || sigprocmask(SIG_UNBLOCK, &signals, NULL) == -1) _exit(125);
    (void)alarm(5);
    if (close(o->fd[GATE_W]) == -1 || close(o->fd[OUT_R]) == -1 ||
        close(o->fd[ERR_R]) == -1 || setpgid(0, 0) == -1 ||
        read(o->fd[GATE_R], &grant, 1) != 1 || grant != 'V' ||
        close(o->fd[GATE_R]) == -1) _exit(125);
    sources[0] = o->fd[NULL_FD]; sources[1] = o->fd[OUT_W]; sources[2] = o->fd[ERR_W];
    for (i = 0; i < 3; i++) sources[i+3] = o->material->fd[i];
    for (i = 0; i < 6; i++)
        if ((copies[i] = fcntl(sources[i], F_DUPFD_CLOEXEC, 6)) == -1) _exit(125);
    for (i = 0; i < 6; i++) if (dup2(copies[i], (int)i) == -1) _exit(125);
    if (bw_sv_closefrom(6) == -1 || bw_sv_child_limits() == -1 ||
        bw_sv_child_boundary() == -1) _exit(125);
    {
        char *const args[] = {"signify", "-V", "-q", "-p", "/dev/fd/3", "-x", "/dev/fd/4", "-m", "/dev/fd/5", NULL};
        char *const env[] = {"LC_ALL=C", "LANG=C", "TZ=UTC", "PATH=", NULL};
        execve("/usr/bin/signify", args, env);
    }
    _exit(125);
}

static int drain(struct operation *o, unsigned i, unsigned bit) {
    unsigned char bytes[256];
    ssize_t n;
    if (o->eof & bit) return 1;
    n = read(o->fd[i], bytes, sizeof(bytes));
    if (!n) { o->eof |= bit; return 1; }
    if (n < 0) {
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) return 1;
        fail(o, BW_SV_IO); return 0;
    }
    if ((size_t)n > 4096 - o->result->diagnostic_bytes) { fail(o, BW_SV_OUTPUT); return 0; }
    o->result->diagnostic_bytes += (unsigned)n;
    return 1;
}

static void pause_pipes(struct operation *o) {
    struct pollfd fd[2];
    fd[0].fd = o->eof & 1U ? -1 : o->fd[OUT_R];
    fd[1].fd = o->eof & 2U ? -1 : o->fd[ERR_R];
    fd[0].events = fd[1].events = POLLIN;
    fd[0].revents = fd[1].revents = 0;
    if (poll(fd, 2, 20) == -1 && errno != EINTR) fail(o, BW_SV_IO);
}

static void observe(struct operation *o) {
    siginfo_t status;
    for (;;) {
        if (!live(o) || !drain(o, OUT_R, 1) || !live(o) || !drain(o, ERR_R, 2)) return;
        memset(&status, 0, sizeof(status));
        if (!live(o)) return;
        if (waitid(P_PID, (id_t)o->result->pid, &status, WEXITED | WNOHANG | WNOWAIT) == -1) {
            if (errno == EINTR) continue;
            o->result->wait_lost = 1; fail(o, BW_SV_WAIT); return;
        }
        if (status.si_pid) {
            if (status.si_pid != o->result->pid) { o->result->wait_lost = 1; fail(o, BW_SV_WAIT); }
            return;
        }
        pause_pipes(o);
        if (o->result->cause != BW_SV_NONE) return;
    }
}

static void custody(struct operation *o) {
    pid_t got;
    unsigned attempt;
    if (!o->result->wait_lost && !o->result->wait_consumed) {
        o->result->stop_attempted = 1;
        if (kill(o->result->group_owned ? -o->result->pid : o->result->pid, SIGKILL) == -1 && errno != ESRCH) {
            fail(o, BW_SV_STOP); o->uncertain = 1;
        }
    }
    for (attempt = 0; attempt < 200; attempt++) {
        if (!(o->eof & 1U)) (void)drain(o, OUT_R, 1);
        if (!(o->eof & 2U)) (void)drain(o, ERR_R, 2);
        if (!o->result->wait_consumed && !o->result->wait_lost) {
            got = wait4(o->result->pid, &o->result->wait_status, WNOHANG, &o->result->usage);
            if (got == o->result->pid) o->result->wait_consumed = 1;
            else if (got < 0 && errno != EINTR) { o->result->wait_lost = 1; fail(o, BW_SV_WAIT); }
            else if (got > 0) { o->result->wait_lost = 1; fail(o, BW_SV_WAIT); }
        }
        if (!terminal_check(o)) break;
        if ((o->result->wait_consumed || o->result->wait_lost) && o->eof == 3) break;
        pause_pipes(o);
    }
    if (!o->result->wait_consumed || o->result->wait_lost || o->eof != 3) {
        fail(o, BW_SV_WAIT); o->uncertain = 1;
    } else if (!WIFEXITED(o->result->wait_status) || WEXITSTATUS(o->result->wait_status) != 0)
        fail(o, BW_SV_CHILD);
}

enum bw_signature_result
bw_signature_verify(struct bw_verifier_material *m, struct bw_signature_observation *r) {
    struct operation o;
    struct sigaction action;
    siginfo_t status;
    unsigned i;
    ssize_t n;
    if (!m || !r || m->state != BW_VM_READY || r->state != BW_SV_EMPTY) return BW_SIGNATURE_REJECTED;
    memset(&o, 0, sizeof(o)); o.material = m; o.result = r;
    memset(r, 0, sizeof(*r)); r->state = BW_SV_WORKING; m->state = BW_VM_VERIFYING;
    for (i = 0; i < FD_COUNT; i++) o.fd[i] = -1;
    if (!m->cancelled || !bw_sv_supported()) { fail(&o, BW_SV_UNSUPPORTED); goto finish; }
    o.supported = 1;
    if (getuid() == 0 || getuid() != geteuid() || getgid() != getegid()) { fail(&o, BW_SV_IDENTITY); goto finish; }
    if (!stamp(&o.started)) { fail(&o, BW_SV_CLOCK); goto finish; }
    if (!live(&o)) goto finish;
    if (sigaction(SIGCHLD, NULL, &action) == -1 || action.sa_handler != SIG_DFL || (action.sa_flags & SA_NOCLDWAIT) ||
        sigaction(SIGPIPE, NULL, &action) == -1 || action.sa_handler != SIG_IGN) { fail(&o, BW_SV_SIGNALS); goto finish; }
    memset(&status, 0, sizeof(status));
    if (!live(&o)) goto finish;
    if (waitid(P_PID, (id_t)getpid(), &status, WEXITED | WNOHANG | WNOWAIT) != -1 || errno != ECHILD) {
        fail(&o, BW_SV_WAIT); goto finish;
    }
    if (!descriptors(&o)) { fail(&o, BW_SV_DESCRIPTOR); goto finish; }
    if (!channels(&o)) { fail(&o, BW_SV_IO); goto finish; }
    if (!live(&o)) goto finish;
    r->pid = fork();
    if (r->pid < 0) { r->pid = 0; fail(&o, BW_SV_FORK); goto finish; }
    if (!r->pid) child(&o);
    if (setpgid(r->pid, r->pid) == -1) { fail(&o, BW_SV_GROUP); goto finish; }
    r->group_owned = 1;
    consume(&o, NULL_FD); consume(&o, OUT_W); consume(&o, ERR_W); consume(&o, GATE_R);
    release(&o);
    if (r->cause != BW_SV_NONE || !live(&o)) goto finish;
    if (bw_sv_parent_boundary(0) == -1) { fail(&o, BW_SV_BOUNDARY); goto finish; }
    if (!live(&o)) goto finish;
    n = write(o.fd[GATE_W], "V", 1);
    if (n != 1) { fail(&o, BW_SV_GATE); goto finish; }
    r->launch_granted = 1;
    consume(&o, GATE_W);
    if (r->cause == BW_SV_NONE) observe(&o);
finish:
    o.cleanup_clock = stamp(&o.cleanup_started);
    if (!o.cleanup_clock) fail(&o, BW_SV_CLOCK);
    else if (r->cause == BW_SV_NONE) {
        if (*m->cancelled) fail(&o, BW_SV_CANCEL);
        else if (!bounded(&o, &o.cleanup_started, &o.started, 5, 11)) fail(&o, BW_SV_DEADLINE);
    }
    consume(&o, GATE_W);
    (void)terminal_check(&o);
    if (r->pid > 0) {
        consume(&o, NULL_FD); (void)terminal_check(&o);
        consume(&o, OUT_W); (void)terminal_check(&o);
        consume(&o, ERR_W); (void)terminal_check(&o);
        consume(&o, GATE_R); (void)terminal_check(&o);
        custody(&o);
        (void)terminal_check(&o);
    }
    release(&o);
    (void)terminal_check(&o);
    for (i = 0; i < FD_COUNT; i++) {
        consume(&o, i);
        (void)terminal_check(&o);
    }
    if (o.supported && bw_sv_parent_boundary(1) == -1) {
        r->final_boundary_failed = 1; fail(&o, BW_SV_BOUNDARY);
    }
    (void)terminal_check(&o);
    r->state = BW_SV_FINISHED;
    if (o.uncertain || r->unknown_closes || r->material_unknown_closes) return BW_SIGNATURE_UNCERTAIN;
    return r->cause == BW_SV_NONE ? BW_SIGNATURE_ACCEPTED : BW_SIGNATURE_REJECTED;
}
