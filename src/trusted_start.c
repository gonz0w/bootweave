/* SPDX-License-Identifier: BSD-3-Clause */
#define _POSIX_C_SOURCE 200809L
#include "trusted_start.h"
#include "signature_verify.h"
#include "private_limits.h"
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

static int entered;

static void
expired(int signal_number)
{
    _exit(signal_number == SIGXFSZ ? 5 : 4);
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

int
bw_process_guard(void)
{
    static const rlim_t caps[] = {4, 64*1024*1024, 2*1024*1024, 32, 0, 65536};
    static const int handled[] = {SIGALRM, SIGPROF, SIGXCPU, SIGXFSZ, SIGPIPE, SIGCHLD};
    static const int timers[] = {ITIMER_REAL, ITIMER_PROF};
    static const int seconds[] = {15, 4};
    struct sigaction action;
    struct itimerval timer;
    sigset_t mask;
    size_t i;

    if (!bw_sv_supported() || getuid() == 0 || getuid() != geteuid() ||
        getgid() != getegid() || entered) return 0;
    entered = 1;
    for (i = 0; i < 2; i++)
        if (!timer_state(timers[i], 0)) return 0;
    memset(&action, 0, sizeof(action));
    if (sigemptyset(&action.sa_mask) == -1 || sigemptyset(&mask) == -1) return 0;
    for (i = 0; i < 6; i++) {
        action.sa_handler = i < 4 ? expired : i == 4 ? SIG_IGN : SIG_DFL;
        if (sigaction(handled[i], &action, NULL) == -1 ||
            sigaddset(&mask, handled[i]) == -1) return 0;
    }
    if (fixed_limits(caps) == -1 ||
        sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) return 0;
    for (i = 0; i < 2; i++) {
        memset(&timer, 0, sizeof(timer));
        timer.it_value.tv_sec = seconds[i];
        if (setitimer(timers[i], &timer, NULL) == -1 ||
            !timer_state(timers[i], seconds[i])) return 0;
    }
    return 1;
}
