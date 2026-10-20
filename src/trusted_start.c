/* SPDX-License-Identifier: BSD-3-Clause */
#define _POSIX_C_SOURCE 200809L
#include "trusted_start.h"
#include "private_start_material.h"
#include "signature_verify.h"
#include "private_limits.h"
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static int entered;
static struct timespec started;
static volatile sig_atomic_t cancelled;
typedef char start_workspace[(sizeof(struct sigaction) + sizeof(struct itimerval) +
    2 * sizeof(sigset_t) + sizeof(struct timespec) + 256 <= 8192) ? 1 : -1];

static void
cancel(int signal_number)
{
    (void)signal_number;
    cancelled = 1;
}

static void
expired(int signal_number)
{
    _exit(signal_number == SIGXFSZ ? 5 : 4);
}

static int
start_stamp(struct timespec *value)
{
    return clock_gettime(CLOCK_MONOTONIC, value) == 0 && value->tv_sec >= 0 &&
        value->tv_nsec >= 0 && value->tv_nsec < 1000000000L;
}

static int
timer_state(int kind, int ceiling)
{
    struct itimerval seen;
    struct timeval *value = &seen.it_value;
    return getitimer(kind, &seen) == 0 && !seen.it_interval.tv_sec &&
        !seen.it_interval.tv_usec && value->tv_sec >= 0 &&
        value->tv_usec >= 0 && value->tv_usec < 1000000 &&
        value->tv_sec <= ceiling && (value->tv_sec != ceiling || !value->tv_usec) && (!ceiling || value->tv_sec || value->tv_usec);
}

static int
guard(int lifetime)
{
    static const rlim_t caps[] = {4, 64*1024*1024, 2*1024*1024, 32, 0, 65536};
    static const int handled[] = {SIGALRM, SIGPROF, SIGXCPU, SIGXFSZ, SIGPIPE, SIGCHLD};
    static const int timers[] = {ITIMER_REAL, ITIMER_PROF};
    static const int seconds[] = {15, 4};
    static const int cancellation[] = {SIGINT, SIGTERM, SIGHUP};
    struct sigaction action;
    struct itimerval timer;
    sigset_t mask, blocked;
    size_t i;

    if (!bw_sv_supported() || getuid() == 0 || getuid() != geteuid() ||
        getgid() != getegid() || entered) return 0;
    entered = 1;
    for (i = 0; i < 2; i++)
        if (!timer_state(timers[i], 0)) return 0;
    memset(&action, 0, sizeof(action));
    if (sigemptyset(&action.sa_mask) == -1 || sigemptyset(&mask) == -1) return 0;
    if (lifetime) {
        if (sigemptyset(&blocked) == -1) return 0;
        for (i = 0; i < 3; i++)
            if (sigaddset(&blocked, cancellation[i]) == -1 ||
                sigaddset(&mask, cancellation[i]) == -1) return 0;
        if (sigprocmask(SIG_BLOCK, &blocked, NULL) == -1) return 0;
        action.sa_handler = cancel;
        action.sa_mask = blocked;
        for (i = 0; i < 3; i++)
            if (sigaction(cancellation[i], &action, NULL) == -1) return 0;
        if (sigemptyset(&action.sa_mask) == -1) return 0;
    }
    for (i = 0; i < 6; i++) {
        action.sa_handler = i < 4 ? expired : i == 4 ? SIG_IGN : SIG_DFL;
        if (sigaction(handled[i], &action, NULL) == -1 ||
            sigaddset(&mask, handled[i]) == -1) return 0;
    }
    if (fixed_limits(caps) == -1 ||
        sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) return 0;
    for (i = 0; i < 2; i++) {
        if (lifetime && cancelled) return 0;
        if (lifetime && i == 0 && !start_stamp(&started)) return 0;
        if (lifetime && cancelled) return 0;
        memset(&timer, 0, sizeof(timer));
        timer.it_value.tv_sec = seconds[i];
        if (setitimer(timers[i], &timer, NULL) == -1 ||
            !timer_state(timers[i], seconds[i])) return 0;
    }
    return !lifetime || !cancelled;
}

int
bw_process_guard(void)
{
    return guard(0);
}

enum bw_material_result
bw_start_reserve(struct bw_verifier_material *m)
{
    if (!m || m->state != BW_VM_EMPTY || !guard(1)) return BW_MATERIAL_REJECTED;
    return bw_material_reserve_started(&started, &cancelled, m);
}

static struct bw_entry_observation entry;
static struct bw_auth_snapshot snapshot;
static struct bw_verifier_material material;
typedef char entry_storage[(sizeof(entry) + sizeof(snapshot) + sizeof(material) +
    131072 <= 2 * 1024 * 1024) ? 1 : -1];

int
bw_entry_live(void)
{
    struct timespec now;
    if (cancelled) entry.cause = BW_ENTRY_CANCEL;
    else if (!start_stamp(&now) ||
        now.tv_sec < started.tv_sec ||
        (now.tv_sec == started.tv_sec && now.tv_nsec < started.tv_nsec))
        entry.cause = BW_ENTRY_CLOCK;
    else if (now.tv_sec - started.tv_sec > 2 ||
        (now.tv_sec - started.tv_sec == 2 && now.tv_nsec >= started.tv_nsec))
        entry.cause = BW_ENTRY_DEADLINE;
    else return 1;
    return 0;
}

const struct bw_entry_observation *
bw_entry_observe(void)
{
    return &entry;
}

enum bw_authenticated_result
bw_entry_output(void)
{
    static const int inputs[] = {0, 3, 4};
    int inputs_owned = 1, material_owned = 0, boundary_owned = 0, uncertain = 0;
    enum bw_authenticated_result result;
    size_t i;
    if (entry.state != BW_ENTRY_EMPTY || entered) return BW_AUTH_NOT_TAKEN;
    entry.state = BW_ENTRY_WORKING;
    entry.capture = BW_CAPTURE_NOT_TAKEN;
    entry.cause = BW_ENTRY_START;
    if (!guard(1)) { entered = 1; goto finish; }
    boundary_owned = 1;
    entry.cause = BW_ENTRY_INPUT;
    for (i = 0; i < 5; i++)
        if (!bw_entry_live() || fcntl((int)i, F_GETFD) == -1) goto finish;
    entry.cause = BW_ENTRY_UNRELATED;
    if (!bw_entry_live()) goto finish;
    if (bw_sv_closefrom(5) == -1) {
        uncertain = 1; goto finish;
    }
    if (!bw_entry_live()) goto finish;
    entry.cause = BW_ENTRY_MATERIAL;
    if (bw_material_reserve_started(&started, &cancelled, &material) != BW_MATERIAL_OK)
        goto finish;
    material_owned = 1;
    entry.cause = BW_ENTRY_CONFINEMENT;
    if (!bw_entry_live() || bw_entry_boundary(&material) == -1) goto finish;
    entry.cause = BW_ENTRY_CAPTURE;
    entry.capture = bw_auth_capture_reserved(&material, &snapshot);
    if (entry.capture != BW_CAPTURE_NOT_TAKEN) inputs_owned = 0;
    uncertain = entry.capture == BW_CAPTURE_UNCERTAIN;
    if (entry.capture != BW_CAPTURED) goto finish;
    entry.cause = BW_ENTRY_AUTHENTICATED;
    if (!bw_entry_live()) goto finish;
    result = bw_authenticated_output(&material, &snapshot, &entry.authenticated);
    if (result != BW_AUTH_NOT_TAKEN) material_owned = boundary_owned = 0;
    uncertain = result == BW_AUTH_UNCERTAIN;
    if (result == BW_AUTH_COMPLETE) entry.cause = BW_ENTRY_NONE;
finish:
    if (inputs_owned) for (i = 0; i < 3; i++) {
        /* Every role is consumed by this attempt, including uncertain closes. */
        if (close(inputs[i]) == -1) {
            entry.input_close_errors |= 1U << i;
            if (errno != EBADF) entry.input_unknown_closes |= 1U << i;
        }
    }
    else {
        entry.input_close_errors = material.capture_close_errors;
        entry.input_unknown_closes = material.capture_unknown_closes;
    }
    if (material_owned && bw_verifier_release(&material) == BW_MATERIAL_UNCERTAIN) uncertain = 1;
    if (boundary_owned && bw_sv_parent_boundary(1) == -1) entry.final_boundary_failed = 1;
    entry.result = uncertain || entry.input_unknown_closes || material.uncertain ||
        material.unknown_closes || material.state == BW_VM_UNCERTAIN ? BW_AUTH_UNCERTAIN :
        entry.cause == BW_ENTRY_NONE ? BW_AUTH_COMPLETE : BW_AUTH_REJECTED;
    memset(&snapshot, 0, sizeof(snapshot));
    entry.state = BW_ENTRY_FINISHED;
    return entry.result;
}
