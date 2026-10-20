/* SPDX-License-Identifier: BSD-3-Clause */
/* Syscall-model fixture: never changes host timers, dispositions or limits. */
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "trusted_start.h"
#include "signature_verify.h"
#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const int resources[] = {RLIMIT_CPU, RLIMIT_DATA, RLIMIT_STACK, RLIMIT_NOFILE, RLIMIT_CORE, RLIMIT_FSIZE};
static const int signals[] = {SIGALRM, SIGPROF, SIGXCPU, SIGXFSZ, SIGPIPE, SIGCHLD};
static const char *const resource_names[] = {"CPU", "DATA", "STACK", "NOFILE", "CORE", "FSIZE"};
static const char *const signal_names[] = {"ALRM", "PROF", "XCPU", "XFSZ", "PIPE", "CHLD"};
static const char *const timer_names[] = {"REAL", "PROF"};
struct event { char call[64]; int outcome; long long value[4]; };
static struct event events[128];
static unsigned event_count, mutations, empty_calls, limit_reads[6], timer_reads[2];
static struct rlimit limits[6];
static struct itimerval timers[2];
static struct sigaction actions[6];
static int installed[6], unblocked[6];
static const char *mode, *field, *handler;
static unsigned selected;
static long long value;
static jmp_buf fatal_target;
static volatile sig_atomic_t fatal_status = -1;
#ifdef TS_NORMAL
struct host_state {
    struct rlimit limits[6];
    struct itimerval timers[2];
    struct sigaction actions[6];
    sigset_t mask;
};
static unsigned readonly_checks;
static int host_read(struct host_state *out) {
    unsigned i;
    for (i = 0; i < 6; i++) {
        readonly_checks++;
        if (getrlimit(resources[i], &out->limits[i])) return -1;
        readonly_checks++;
        if (sigaction(signals[i], NULL, &out->actions[i])) return -1;
    }
    readonly_checks++;
    if (getitimer(ITIMER_REAL, &out->timers[0])) return -1;
    readonly_checks++;
    if (getitimer(ITIMER_PROF, &out->timers[1])) return -1;
    readonly_checks++;
    return sigprocmask(SIG_BLOCK, NULL, &out->mask);
}
static int host_equal(const struct host_state *a, const struct host_state *b) {
    unsigned i, j;
    for (i = 0; i < 6; i++) {
        if (a->limits[i].rlim_cur != b->limits[i].rlim_cur || a->limits[i].rlim_max != b->limits[i].rlim_max ||
            a->actions[i].sa_handler != b->actions[i].sa_handler || a->actions[i].sa_flags != b->actions[i].sa_flags) return 0;
        for (j = 1; j < NSIG; j++) if (sigismember(&a->actions[i].sa_mask, (int)j) !=
            sigismember(&b->actions[i].sa_mask, (int)j)) return 0;
    }
    for (i = 0; i < 2; i++) if (a->timers[i].it_value.tv_sec != b->timers[i].it_value.tv_sec ||
        a->timers[i].it_value.tv_usec != b->timers[i].it_value.tv_usec ||
        a->timers[i].it_interval.tv_sec != b->timers[i].it_interval.tv_sec ||
        a->timers[i].it_interval.tv_usec != b->timers[i].it_interval.tv_usec) return 0;
    for (j = 1; j < NSIG; j++) if (sigismember(&a->mask, (int)j) != sigismember(&b->mask, (int)j)) return 0;
    return 1;
}
#endif

static int is(const char *name) { return !strcmp(mode, name); }
static long long bound(rlim_t n) { return n == RLIM_INFINITY ? -1 : (long long)n; }
static int failure(void) { errno = EIO; return -1; }
static unsigned resource_index(int kind) {
    unsigned i; for (i = 0; i < 6; i++) if (kind == resources[i]) return i;
    exit(90);
}
static unsigned signal_index(int signal_number) {
    unsigned i; for (i = 0; i < 6; i++) if (signal_number == signals[i]) return i;
    exit(90);
}
static unsigned timer_index(int kind) {
    if (kind == ITIMER_REAL) return 0;
    if (kind == ITIMER_PROF) return 1;
    exit(90);
}
static void event(const char *operation, const char *name, const char *phase,
    int outcome, long long a, long long b, long long c, long long d) {
    struct event *out;
    int n;
    if (event_count >= sizeof(events) / sizeof(events[0])) exit(91);
    out = &events[event_count++];
    n = snprintf(out->call, sizeof(out->call), "%s%s%s%s%s", operation,
        name ? ":" : "", name ? name : "", phase ? ":" : "", phase ? phase : "");
    if (n < 0 || (size_t)n >= sizeof(out->call)) exit(91);
    out->outcome = outcome;
    out->value[0] = a; out->value[1] = b; out->value[2] = c; out->value[3] = d;
}
static void change_timer(struct itimerval *timer) {
    if (!strcmp(field, "value_sec")) timer->it_value.tv_sec = (time_t)value;
    else if (!strcmp(field, "value_usec")) timer->it_value.tv_usec = (suseconds_t)value;
    else if (!strcmp(field, "interval_sec")) timer->it_interval.tv_sec = (time_t)value;
    else if (!strcmp(field, "interval_usec")) timer->it_interval.tv_usec = (suseconds_t)value;
    else exit(92);
}

int bw_ts_supported(void) {
    int result = !(is("unsupported") || is("setid"));
    event("supported", NULL, NULL, result, 0, 0, 0, 0); return result;
}
uid_t bw_ts_getuid(void) {
    uid_t result = is("root") ? 0 : 1000;
    event("getuid", NULL, NULL, 0, result, 0, 0, 0); return result;
}
uid_t bw_ts_geteuid(void) {
    uid_t result = is("uid-mismatch") ? 1001 : 1000;
    event("geteuid", NULL, NULL, 0, result, 0, 0, 0); return result;
}
gid_t bw_ts_getgid(void) {
    event("getgid", NULL, NULL, 0, 1000, 0, 0, 0); return 1000;
}
gid_t bw_ts_getegid(void) {
    gid_t result = is("gid-mismatch") ? 1001 : 1000;
    event("getegid", NULL, NULL, 0, result, 0, 0, 0); return result;
}
int bw_ts_getrlimit(int kind, struct rlimit *out) {
    unsigned i = resource_index(kind), readback = limit_reads[i]++;
    int fail = selected == i && (readback ? is("limit-final-error") : is("limit-initial-error"));
    *out = limits[i];
    if (selected == i && readback && is("limit-final-mismatch")) {
        if (!strcmp(field, "soft") || !strcmp(field, "both")) out->rlim_cur++;
        if (!strcmp(field, "hard") || !strcmp(field, "both")) out->rlim_max++;
    }
    event("limit-get", resource_names[i], readback ? "final" : "initial", fail ? -1 : 0,
        bound(out->rlim_cur), bound(out->rlim_max), 0, 0);
    return fail ? failure() : 0;
}
int bw_ts_setrlimit(int kind, const struct rlimit *requested) {
    unsigned i = resource_index(kind);
    int fail = selected == i && is("limit-set-error");
    mutations++;
    if (!fail && requested->rlim_cur > requested->rlim_max) fail = 1;
    event("limit-set", resource_names[i], NULL, fail ? -1 : 0,
        bound(requested->rlim_cur), bound(requested->rlim_max), 0, 0);
    if (fail) return failure();
    memcpy(&limits[i], requested, sizeof(limits[i])); return 0;
}
int bw_ts_sigemptyset(sigset_t *set) {
    unsigned i = empty_calls++;
    int fail = is("signal-empty-error") && selected == i;
    int result = fail ? failure() : sigemptyset(set);
    event("empty", i ? "unblock" : "action", NULL, result, 0, 0, 0, 0);
    return result;
}
int bw_ts_sigaddset(sigset_t *set, int signal_number) {
    unsigned i = signal_index(signal_number);
    int fail = is("signal-add-error") && selected == i;
    int result = fail ? failure() : sigaddset(set, signal_number);
    event("add", signal_names[i], NULL, result, 0, 0, 0, 0); return result;
}
int bw_ts_sigaction(int signal_number, const struct sigaction *action, struct sigaction *old) {
    unsigned i = signal_index(signal_number), j;
    int fail = is("signal-action-error") && selected == i, mask = 0;
    int role = action->sa_handler == SIG_DFL ? 3 : action->sa_handler == SIG_IGN ? 2 : 1;
    mutations++;
    if (old) exit(93);
    for (j = 1; j < NSIG; j++) if (sigismember(&action->sa_mask, (int)j) == 1) mask++;
    event("action", signal_names[i], NULL, fail ? -1 : 0, role, action->sa_flags, mask, 0);
    if (fail) return failure();
    memcpy(&actions[i], action, sizeof(actions[i])); installed[i] = 1; return 0;
}
int bw_ts_sigprocmask(int how, const sigset_t *set, sigset_t *old) {
    unsigned i, members = 0, unexpected = 0;
    int fail = is("signal-unblock-error");
    mutations++;
    if (old) exit(93);
    for (i = 0; i < 6; i++) if (sigismember(set, signals[i]) == 1) members |= 1U << i;
    for (i = 1; i < NSIG; i++) if (sigismember(set, (int)i) == 1) {
        unsigned j; for (j = 0; j < 6 && signals[j] != (int)i; j++) {}
        if (j == 6) unexpected++;
    }
    event("unblock", NULL, NULL, fail ? -1 : 0, how == SIG_UNBLOCK, members, unexpected, 0);
    if (fail) return failure();
    for (i = 0; i < 6; i++) unblocked[i] = (members >> i) & 1U;
    return 0;
}
int bw_ts_getitimer(int kind, struct itimerval *out) {
    unsigned i = timer_index(kind), readback = timer_reads[i]++;
    int fail = selected == i && (readback ? is("timer-final-error") : is("timer-initial-error"));
    memcpy(out, &timers[i], sizeof(*out));
    if (selected == i && readback && is("timer-final-field")) change_timer(out);
    event("timer-get", timer_names[i], readback ? "final" : "initial", fail ? -1 : 0,
        (long long)out->it_value.tv_sec, out->it_value.tv_usec,
        (long long)out->it_interval.tv_sec, out->it_interval.tv_usec);
    return fail ? failure() : 0;
}
int bw_ts_setitimer(int kind, const struct itimerval *requested, struct itimerval *old) {
    unsigned i = timer_index(kind);
    int fail = selected == i && is("timer-arm-error");
    mutations++;
    if (old) exit(93);
    event("timer-set", timer_names[i], NULL, fail ? -1 : 0,
        (long long)requested->it_value.tv_sec, requested->it_value.tv_usec,
        (long long)requested->it_interval.tv_sec, requested->it_interval.tv_usec);
    if (fail) return failure();
    memcpy(&timers[i], requested, sizeof(timers[i])); return 0;
}
void bw_ts_exit(int status) {
    fatal_status = status;
    event("exit", NULL, NULL, 0, status, 0, 0, 0);
    longjmp(fatal_target, 1);
}

#ifndef TS_NORMAL
/* Darwin supplies signal-set macros. Substitute calls after system headers so
 * those local operations are measured too, without renaming struct sigaction.
 * This includes the real core/helper once; no implementation is copied here. */
#undef sigemptyset
#undef sigaddset
#define bw_sv_supported() bw_ts_supported()
#define getuid() bw_ts_getuid()
#define geteuid() bw_ts_geteuid()
#define getgid() bw_ts_getgid()
#define getegid() bw_ts_getegid()
#define sigemptyset(set) bw_ts_sigemptyset(set)
#define sigaddset(set, number) bw_ts_sigaddset(set, number)
#define sigaction(number, action, old) bw_ts_sigaction(number, action, old)
#define sigprocmask(how, set, old) bw_ts_sigprocmask(how, set, old)
#define getrlimit(kind, out) bw_ts_getrlimit(kind, out)
#define setrlimit(kind, in) bw_ts_setrlimit(kind, in)
#define getitimer(kind, out) bw_ts_getitimer(kind, out)
#define setitimer(kind, in, old) bw_ts_setitimer(kind, in, old)
#define _exit(status) bw_ts_exit(status)
#include "../src/trusted_start.c"
#undef bw_sv_supported
#undef getuid
#undef geteuid
#undef getgid
#undef getegid
#undef sigemptyset
#undef sigaddset
#undef sigaction
#undef sigprocmask
#undef getrlimit
#undef setrlimit
#undef getitimer
#undef setitimer
#undef _exit
#endif

static void print_events(unsigned begin, unsigned end) {
    unsigned i;
    putchar('[');
    for (i = begin; i < end; i++) {
        struct event *e = &events[i];
        printf("%s{\"call\":\"%s\",\"outcome\":%d,\"values\":[%lld,%lld,%lld,%lld]}",
            i == begin ? "" : ",", e->call, e->outcome, e->value[0], e->value[1], e->value[2], e->value[3]);
    }
    putchar(']');
}
int main(int argc, char **argv) {
    unsigned i, before_repeat, first_count, first_mutations, repeat_mutations = 0;
    int result, repeat_result = -1, repeat;
#ifdef TS_NORMAL
    struct host_state host_before, host_after;
#endif
    if (argc != 7) return 90;
    mode = argv[1]; selected = (unsigned)strtoul(argv[2], NULL, 10); field = argv[3];
    value = strtoll(argv[4], NULL, 10); handler = argv[5]; repeat = atoi(argv[6]);
    if (selected > 5 || value < -1 || value > 1000000000LL || (repeat != 0 && repeat != 1)) return 90;
    for (i = 0; i < 6; i++) limits[i].rlim_cur = limits[i].rlim_max = RLIM_INFINITY;
    if (is("limit-inherited")) {
        if (!strcmp(field, "soft") || !strcmp(field, "both")) limits[selected].rlim_cur = value == -1 ? RLIM_INFINITY : (rlim_t)value;
        if (!strcmp(field, "hard") || !strcmp(field, "both")) limits[selected].rlim_max = value == -1 ? RLIM_INFINITY : (rlim_t)value;
    }
    if (is("timer-initial-field")) {
        if (selected > 1) return 90;
        change_timer(&timers[selected]);
    }
#ifdef TS_NORMAL
    if (host_read(&host_before)) return 96;
#endif
    result = bw_process_guard();
    if (strcmp(handler, "none")) {
        for (i = 0; i < 4 && strcmp(handler, signal_names[i]); i++) {}
        if (!result || i == 4 || !installed[i] || actions[i].sa_handler == SIG_IGN || actions[i].sa_handler == SIG_DFL) return 94;
        if (!setjmp(fatal_target)) {
            actions[i].sa_handler(signals[i]);
            return 95; /* A fatal callback must not return. */
        }
    }
    first_count = event_count; first_mutations = mutations;
    before_repeat = event_count;
    if (repeat) {
        repeat_result = bw_process_guard();
        repeat_mutations = mutations - first_mutations;
    }
#ifdef TS_NORMAL
    if (host_read(&host_after) || !host_equal(&host_before, &host_after)) return 97;
#endif
    printf("TS_RESULT {\"success\":");
    if (fatal_status >= 0) printf("null"); else printf("%d", result);
    printf(",\"events\":");
    print_events(0, first_count);
    printf(",\"repeat_result\":%d,\"repeat_events\":", repeat_result);
    print_events(before_repeat, event_count);
    printf(",\"mutations\":%u,\"repeat_mutations\":%u,\"fatal_status\":%d,\"limits\":[",
        first_mutations, repeat_mutations, (int)fatal_status);
    for (i = 0; i < 6; i++) printf("%s[%lld,%lld]", i ? "," : "", bound(limits[i].rlim_cur), bound(limits[i].rlim_max));
    printf("],\"handlers\":[");
    for (i = 0; i < 6; i++) {
        const char *role = !installed[i] ? "absent" : actions[i].sa_handler == SIG_DFL ? "default" : actions[i].sa_handler == SIG_IGN ? "ignore" : "fatal";
        printf("%s\"%s\"", i ? "," : "", role);
    }
    printf("],\"unblocked\":[");
    for (i = 0; i < 6; i++) printf("%s%d", i ? "," : "", unblocked[i]);
    printf("],\"timers\":[");
    for (i = 0; i < 2; i++) printf("%s[%lld,%ld,%lld,%ld]", i ? "," : "", (long long)timers[i].it_value.tv_sec,
        (long)timers[i].it_value.tv_usec, (long long)timers[i].it_interval.tv_sec, (long)timers[i].it_interval.tv_usec);
#ifdef TS_NORMAL
    printf("],\"host_unchanged\":1,\"readonly_checks\":%u}\n", readonly_checks);
#else
    printf("],\"host_unchanged\":null,\"readonly_checks\":0}\n");
#endif
    return 0;
}
