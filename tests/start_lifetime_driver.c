/* SPDX-License-Identifier: BSD-3-Clause */
/* Test-only preambles redirect calls AFTER SDK headers, including Darwin's
 * signal-set macros. No production implementation is copied or rewritten. */
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "trusted_start.h"
#include "auth_input.h"
#include "signature_verify.h"
#include <sys/time.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(LS_START_PREAMBLE)
int lt_supported(void);
uid_t lt_uid(void); uid_t lt_euid(void); gid_t lt_gid(void); gid_t lt_egid(void);
int lt_empty(sigset_t *); int lt_add(sigset_t *, int);
int lt_action(int, const struct sigaction *, struct sigaction *);
int lt_mask(int, const sigset_t *, sigset_t *);
int lt_getlimit(int, struct rlimit *); int lt_setlimit(int, const struct rlimit *);
int lt_gettimer(int, struct itimerval *); int lt_settimer(int, const struct itimerval *, struct itimerval *);
int lt_origin(clockid_t, struct timespec *); void lt_forbidden_exit(int);
#undef sigemptyset
#undef sigaddset
#define bw_sv_supported() lt_supported()
#define getuid() lt_uid()
#define geteuid() lt_euid()
#define getgid() lt_gid()
#define getegid() lt_egid()
#define sigemptyset(s) lt_empty(s)
#define sigaddset(s,n) lt_add(s,n)
#define sigaction(n,a,o) lt_action(n,a,o)
#define sigprocmask(h,s,o) lt_mask(h,s,o)
#define getrlimit(k,s) lt_getlimit(k,s)
#define setrlimit(k,s) lt_setlimit(k,s)
#define getitimer(k,s) lt_gettimer(k,s)
#define setitimer(k,s,o) lt_settimer(k,s,o)
#define clock_gettime(k,s) lt_origin(k,s)
#define _exit(s) lt_forbidden_exit(s)

#elif defined(LS_SIGNATURE_PREAMBLE)
int lt_signature_clock(clockid_t, struct timespec *);
int lt_signature_action(int, const struct sigaction *, struct sigaction *);
int lt_child_mask(int, const sigset_t *, sigset_t *);
pid_t lt_fork(void); int lt_kill(pid_t, int);
int lt_waitid(idtype_t, id_t, siginfo_t *, int);
pid_t lt_wait4(pid_t, int *, int, struct rusage *);
ssize_t lt_gate_write(int, const void *, size_t);
int bw_v_open(const char *, int, ...); int bw_v_pipe(int *);
int bw_v_fcntl(int, int, ...); off_t bw_v_lseek(int, off_t, int);
int bw_v_fstat(int, struct stat *); int bw_v_close(int);
int bw_v_setpgid(pid_t, pid_t); int bw_v_dup2(int, int);
ssize_t bw_v_read(int, void *, size_t); int bw_v_poll(struct pollfd *, nfds_t, int);
uid_t bw_v_getuid(void); int bw_v_execve(const char *, char *const[], char *const[]);
void lt_forbidden_exit(int);
#define clock_gettime(k,s) lt_signature_clock(k,s)
#define sigaction(n,a,o) lt_signature_action(n,a,o)
#define sigprocmask(h,s,o) lt_child_mask(h,s,o)
#define open(...) bw_v_open(__VA_ARGS__)
#define pipe(p) bw_v_pipe(p)
#define fcntl(...) bw_v_fcntl(__VA_ARGS__)
#define lseek(f,o,w) bw_v_lseek(f,o,w)
#define fstat(f,s) bw_v_fstat(f,s)
#define close(f) bw_v_close(f)
#define fork() lt_fork()
#define setpgid(p,g) bw_v_setpgid(p,g)
#define dup2(f,n) bw_v_dup2(f,n)
#define write(f,b,n) lt_gate_write(f,b,n)
#define read(f,b,n) bw_v_read(f,b,n)
#define poll(p,n,t) bw_v_poll(p,n,t)
#define waitid(t,p,s,o) lt_waitid(t,p,s,o)
#define wait4(p,s,o,u) lt_wait4(p,s,o,u)
#define kill(p,s) lt_kill(p,s)
#define getuid() bw_v_getuid()
#define execve(p,a,e) bw_v_execve(p,a,e)
#define _exit(s) lt_forbidden_exit(s)

#else
#include <CommonCrypto/CommonDigest.h>
/* Reuse the existing inert FD/pipe/close/EOF adapters. Their fork, kill and wait
 * functions are NOT used: the bindings above select pure synthetic models. */
#define main lt_unused_legacy_main
#include "signature_driver.c"
#undef main

static const char *lt_kind, *lt_mode, *lt_phase;
static int lt_selected_signal;
static const int lt_signals[] = {SIGINT, SIGTERM, SIGHUP, SIGALRM, SIGPROF, SIGXCPU, SIGXFSZ, SIGPIPE, SIGCHLD};
static const char *const lt_signal_names[] = {"INT", "TERM", "HUP", "ALRM", "PROF", "XCPU", "XFSZ", "PIPE", "CHLD"};
static const int lt_resources[] = {RLIMIT_CPU, RLIMIT_DATA, RLIMIT_STACK, RLIMIT_NOFILE, RLIMIT_CORE, RLIMIT_FSIZE};
static const char *const lt_resource_names[] = {"CPU", "DATA", "STACK", "NOFILE", "CORE", "FSIZE"};
static struct sigaction lt_actions[9];
static unsigned lt_installed[9], lt_empty_count, lt_mutations, lt_origin_count, lt_callback_count, lt_callbacks_clean = 1;
static sigset_t *lt_unblock_set, *lt_cancel_set;
static struct rlimit lt_limits[6];
static struct itimerval lt_timers[2];
static unsigned lt_timer_reads[2], lt_unblocked, lt_blocked, lt_mkdirs, lt_members, lt_member_writes, lt_material_closes;
static unsigned lt_capture_closes[3], lt_capture_reads, lt_material_ticks, lt_forks, lt_stops, lt_consumes, lt_sig_ticks;
static struct bw_verifier_material lt_material;
static struct bw_auth_snapshot lt_snapshot;
static long long lt_material_ns = 250000000LL;
struct lt_moment { long long ns; int fault; };
static struct lt_moment lt_start_time, lt_work_time, lt_cleanup_time, lt_now;
struct lt_event { char call[56]; int outcome; long long values[4]; };
static struct lt_event lt_events[100];
static unsigned lt_event_count;
static long long lt_samples[128];

static int lt_is(const char *name) { return !strcmp(lt_mode, name); }
static int lt_at(const char *name) { return !strcmp(lt_phase, name); }
static int lt_failure(void) { errno = EIO; return -1; }
static long long lt_bound(rlim_t value) { return value == RLIM_INFINITY ? -1 : (long long)value; }
static void lt_event(const char *call, int outcome, long long a, long long b, long long c, long long d) {
    struct lt_event *e;
    if (lt_event_count >= 100 || strlen(call) >= sizeof(lt_events[0].call)) exit(90);
    e = &lt_events[lt_event_count++]; memcpy(e->call, call, strlen(call) + 1);
    e->outcome = outcome; e->values[0] = a; e->values[1] = b; e->values[2] = c; e->values[3] = d;
}
static unsigned lt_signal_index(int number) {
    unsigned i; for (i = 0; i < 9; i++) if (lt_signals[i] == number) return i;
    exit(90);
}
static unsigned lt_limit_index(int kind) {
    unsigned i; for (i = 0; i < 6; i++) if (lt_resources[i] == kind) return i;
    exit(90);
}
static unsigned lt_timer_index(int kind) {
    if (kind == ITIMER_REAL) return 0;
    if (kind == ITIMER_PROF) return 1;
    exit(90);
}
static unsigned lt_mask_bits(const sigset_t *set) {
    unsigned i, bits = 0, count = 0;
    for (i = 0; i < 9; i++) if (sigismember(set, lt_signals[i]) == 1) bits |= 1U << i;
    for (i = 1; i < NSIG; i++) if (sigismember(set, (int)i) == 1) count++;
    if (count != (unsigned)__builtin_popcount(bits)) exit(90);
    return bits;
}
static void lt_fire(void) {
    unsigned before = lt_event_count, mutations = lt_mutations, dirs = lt_mkdirs;
    struct bw_verifier_material copy;
    memcpy(&copy, &lt_material, sizeof(copy));
    if (lt_selected_signal < 0 || lt_selected_signal > 2 || !lt_installed[lt_selected_signal] ||
        lt_actions[lt_selected_signal].sa_handler == SIG_DFL || lt_actions[lt_selected_signal].sa_handler == SIG_IGN) exit(91);
    lt_callback_count++;
    lt_actions[lt_selected_signal].sa_handler(lt_signals[lt_selected_signal]);
    if (before != lt_event_count || mutations != lt_mutations || dirs != lt_mkdirs || memcmp(&copy, &lt_material, sizeof(copy)))
        lt_callbacks_clean = 0;
}
static void lt_time(struct timespec *out, long long ns) {
    out->tv_sec = 1000 + (time_t)(ns / 1000000000LL);
    out->tv_nsec = 1234 + (long)(ns % 1000000000LL);
    if (out->tv_nsec >= 1000000000L) { out->tv_sec++; out->tv_nsec -= 1000000000L; }
    if (out->tv_nsec < 0) { out->tv_sec--; out->tv_nsec += 1000000000L; }
}
static struct lt_moment lt_parse(const char *text) {
    struct lt_moment m = {0, 0};
    if (!strcmp(text, "error")) m.fault = 1;
    else if (!strcmp(text, "invalid")) m.fault = 2;
    else m.ns = strtoll(text, NULL, 10);
    if (m.ns < -500000000LL || m.ns > 15000000000LL) exit(90);
    return m;
}
int lt_supported(void) { int ok = !lt_is("unsupported"); lt_event("supported", ok, 0,0,0,0); return ok; }
uid_t lt_uid(void) { uid_t n = lt_is("root") ? 0 : getuid(); lt_event("getuid",0,n,0,0,0); return n; }
uid_t lt_euid(void) { uid_t n = geteuid(); lt_event("geteuid",0,n,0,0,0); return n; }
gid_t lt_gid(void) { gid_t n = getgid(); lt_event("getgid",0,n,0,0,0); return n; }
gid_t lt_egid(void) { gid_t n = getegid(); lt_event("getegid",0,n,0,0,0); return n; }
int lt_empty(sigset_t *set) {
    unsigned i = lt_empty_count++; char token[56], bad[56]; int result;
    if (i == 1) lt_unblock_set = set;
    if (i == 2) lt_cancel_set = set;
    snprintf(token, sizeof(token), "empty:%u", i);
    snprintf(bad, sizeof(bad), "empty-%s", i == 0 ? "action" : i == 1 ? "unblock" : i == 2 ? "cancel" : "reset");
    result = lt_is(bad) ? lt_failure() : sigemptyset(set);
    lt_event(token,result,0,0,0,0); return result;
}
int lt_add(sigset_t *set, int number) {
    unsigned i = lt_signal_index(number); const char *which = set == lt_cancel_set ? "cancel" : set == lt_unblock_set ? "unblock" : "other";
    char token[56]; int result;
    snprintf(token, sizeof(token), "add:%s:%s", lt_signal_names[i], which);
    result = i == (unsigned)lt_selected_signal && ((lt_is("add-cancel") && set == lt_cancel_set) ||
        (lt_is("add-unblock") && set == lt_unblock_set)) ? lt_failure() : sigaddset(set, number);
    lt_event(token,result,0,0,0,0); return result;
}
int lt_action(int number, const struct sigaction *action, struct sigaction *old) {
    unsigned i = lt_signal_index(number); char token[56]; int fail = lt_is("action-cancel") && i == (unsigned)lt_selected_signal;
    int role = action->sa_handler == SIG_DFL ? 4 : action->sa_handler == SIG_IGN ? 3 : i < 3 ? 1 : 2;
    if (old) exit(90);
    lt_mutations++; snprintf(token, sizeof(token), "action:%s", lt_signal_names[i]);
    lt_event(token,fail ? -1 : 0,role,action->sa_flags,lt_mask_bits(&action->sa_mask),0);
    if (fail) return lt_failure();
    memcpy(&lt_actions[i], action, sizeof(*action)); lt_installed[i] = 1; return 0;
}
int lt_mask(int how, const sigset_t *set, sigset_t *old) {
    unsigned bits = lt_mask_bits(set); int block = how == SIG_BLOCK, fail = lt_is(block ? "block" : "unblock");
    if (old || (how != SIG_BLOCK && how != SIG_UNBLOCK)) exit(90);
    lt_mutations++; lt_event(block ? "block" : "unblock",fail ? -1 : 0,bits,0,0,0);
    if (fail) return lt_failure();
    if (block) lt_blocked = bits; else lt_unblocked = bits;
    if (!block && lt_is("pending-unblock")) lt_fire();
    return 0;
}
int lt_getlimit(int kind, struct rlimit *out) {
    unsigned i = lt_limit_index(kind); char token[56];
    memcpy(out, &lt_limits[i], sizeof(*out)); snprintf(token,sizeof(token),"limit-get:%s",lt_resource_names[i]);
    lt_event(token,0,lt_bound(out->rlim_cur),lt_bound(out->rlim_max),0,0); return 0;
}
int lt_setlimit(int kind, const struct rlimit *requested) {
    unsigned i = lt_limit_index(kind); char token[56];
    lt_mutations++; snprintf(token,sizeof(token),"limit-set:%s",lt_resource_names[i]);
    lt_event(token,0,lt_bound(requested->rlim_cur),lt_bound(requested->rlim_max),0,0);
    memcpy(&lt_limits[i], requested, sizeof(*requested)); return 0;
}
int lt_gettimer(int kind, struct itimerval *out) {
    unsigned i = lt_timer_index(kind); char token[56];
    memcpy(out, &lt_timers[i], sizeof(*out));
    snprintf(token,sizeof(token),"timer-get:%s:%s",i ? "PROF" : "REAL",lt_timer_reads[i]++ ? "final" : "initial");
    lt_event(token,0,out->it_value.tv_sec,out->it_value.tv_usec,out->it_interval.tv_sec,out->it_interval.tv_usec); return 0;
}
int lt_settimer(int kind, const struct itimerval *requested, struct itimerval *old) {
    unsigned i = lt_timer_index(kind); if (old) exit(90);
    lt_mutations++; lt_event(i ? "timer-set:PROF" : "timer-set:REAL",0,
        requested->it_value.tv_sec,requested->it_value.tv_usec,requested->it_interval.tv_sec,requested->it_interval.tv_usec);
    memcpy(&lt_timers[i], requested, sizeof(*requested));
    if ((!i && lt_is("cancel-after-real")) || (i && lt_is("cancel-after-prof"))) lt_fire();
    if (!i && !strcmp(lt_kind,"spent") && lt_at("reserve")) lt_material_ns = 2000000000LL;
    return 0;
}
int lt_origin(clockid_t kind, struct timespec *out) {
    int failed = lt_is("origin-error"); if (kind != CLOCK_MONOTONIC) exit(90);
    lt_origin_count++; lt_time(out,0);
    if (lt_is("origin-negative")) out->tv_sec = -1;
    if (lt_is("origin-invalid")) out->tv_nsec = 1000000000L;
    if (lt_is("origin-negative-nsec")) out->tv_nsec = -1;
    lt_event("origin",failed ? -1 : 0,out->tv_sec,out->tv_nsec,0,0);
    if (lt_is("cancel-at-origin")) lt_fire();
    return failed ? -1 : 0;
}
void lt_forbidden_exit(int status) { (void)status; exit(97); }
int lt_child_mask(int how, const sigset_t *set, sigset_t *old) { (void)how; (void)set; (void)old; exit(97); }
int lt_signature_action(int number, const struct sigaction *action, struct sigaction *old) {
    unsigned i = lt_signal_index(number);
    if (action || !old || !lt_installed[i]) return lt_failure();
    memcpy(old,&lt_actions[i],sizeof(*old)); return 0;
}
int lt_material_clock(clockid_t kind, struct timespec *out) {
    if (kind != CLOCK_MONOTONIC) exit(90);
    lt_material_ticks++; lt_time(out,lt_material_ns); return 0;
}
char *lt_mkdtemp(char *template) {
    int n; if (strcmp(template,"/tmp/bootweave-verify.XXXXXXXXXX")) exit(90);
    lt_mkdirs++; n = snprintf(template,256,"%s/material.XXXXXXXXXX",root);
    return n < 0 || n >= 256 ? NULL : mkdtemp(template);
}
int lt_material_openat(int fd, const char *name, int flags, ...) {
    va_list args; mode_t permissions = 0; lt_members++;
    if (flags & O_CREAT) { va_start(args,flags); permissions = (mode_t)va_arg(args,int); va_end(args); }
    return flags & O_CREAT ? openat(fd,name,flags,permissions) : openat(fd,name,flags);
}
ssize_t lt_material_write(int fd, const void *bytes, size_t length) { lt_member_writes++; return write(fd,bytes,length); }
int lt_material_close(int fd) { lt_material_closes++; return active ? bw_v_close(fd) : close(fd); }
ssize_t lt_capture_read(int fd, void *bytes, size_t length) { lt_capture_reads++; return read(fd,bytes,length); }
int lt_capture_close(int fd) {
    if (fd == 0) lt_capture_closes[0]++; else if (fd == 3) lt_capture_closes[1]++; else if (fd == 4) lt_capture_closes[2]++; else exit(90);
    return close(fd);
}
int lt_signature_clock(clockid_t kind, struct timespec *out) {
    if (kind != CLOCK_MONOTONIC || lt_sig_ticks >= 128) exit(90);
    if (!lt_sig_ticks) lt_now = lt_start_time;
    lt_samples[lt_sig_ticks++] = lt_now.fault ? -100 - lt_now.fault : lt_now.ns;
    lt_time(out,lt_now.ns); if (lt_now.fault == 2) out->tv_nsec = 1000000000L;
    return lt_now.fault == 1 ? -1 : 0;
}
pid_t lt_fork(void) { lt_forks++; born_child = 12345; return born_child; }
ssize_t lt_gate_write(int fd, const void *bytes, size_t length) {
    if (fd != fds[6] || length != 1 || *(const char *)bytes != 'V') exit(90);
    if (!lt_is("signature-lost-wait-late")) lt_now = lt_work_time;
    if (!strcmp(lt_kind,"flag") && lt_at("signature-gate")) lt_fire();
    return (ssize_t)length; /* No actual pipe write or signal delivery. */
}
int lt_waitid(idtype_t type, id_t pid, siginfo_t *out, int options) {
    if (type != P_PID || options != (WEXITED | WNOHANG | WNOWAIT)) exit(90);
    if (pid == (id_t)getpid()) { errno = ECHILD; return -1; }
    if (pid != 12345 || reaped_seen || lost_seen) { bad_order++; return lt_failure(); }
    if (lt_is("signature-lost-wait-late")) { lost_seen = 1; lt_now = lt_work_time; errno = ECHILD; return -1; }
    memset(out,0,sizeof(*out)); out->si_pid = 12345; out->si_code = CLD_EXITED;
    if (lt_is("signature-original-fifteen-cold-cleanup-just-below")) lt_now.ns = 10999999999LL;
    if (lt_is("signature-work-completed-at-own-five")) lt_now.ns = 6000000000LL;
    if (lt_is("signature-work-completed-at-original-eleven")) lt_now.ns = 11000000000LL;
    return 0;
}
int lt_kill(pid_t pid, int number) {
    lt_stops++; signals_sent++;
    if ((pid != 12345 && pid != -12345) || number != SIGKILL || reaped_seen || lost_seen) { bad_order++; return lt_failure(); }
    lt_now = lt_cleanup_time;
    if (lt_is("signature-failed-stop-late")) { errno = EPERM; return -1; }
    return 0; /* Synthetic custody only, NEVER kill(2). */
}
pid_t lt_wait4(pid_t pid, int *status, int options, struct rusage *usage) {
    if (pid != 12345 || options != WNOHANG || reaped_seen || lost_seen || !signals_sent) { bad_order++; errno = ECHILD; return -1; }
    lt_consumes++; reaped_seen = 1; *status = 0; memset(usage,0,sizeof(*usage)); return pid;
}

#ifdef LS_NORMAL
struct lt_host { struct rlimit limits[6]; struct sigaction actions[9]; struct itimerval timers[2]; sigset_t mask; };
static unsigned lt_readonly;
static int lt_host_read(struct lt_host *out) {
    unsigned i;
    for (i = 0; i < 6; i++) { lt_readonly++; if (getrlimit(lt_resources[i],&out->limits[i])) return -1; }
    for (i = 0; i < 9; i++) { lt_readonly++; if (sigaction(lt_signals[i],NULL,&out->actions[i])) return -1; }
    lt_readonly++; if (getitimer(ITIMER_REAL,&out->timers[0])) return -1;
    lt_readonly++; if (getitimer(ITIMER_PROF,&out->timers[1])) return -1;
    lt_readonly++; return sigprocmask(SIG_BLOCK,NULL,&out->mask);
}
static int lt_host_same(const struct lt_host *a, const struct lt_host *b) {
    unsigned i,j;
    for (i=0;i<6;i++) if (a->limits[i].rlim_cur!=b->limits[i].rlim_cur || a->limits[i].rlim_max!=b->limits[i].rlim_max) return 0;
    for (i=0;i<9;i++) {
        if (a->actions[i].sa_handler!=b->actions[i].sa_handler || a->actions[i].sa_flags!=b->actions[i].sa_flags) return 0;
        for(j=1;j<NSIG;j++) if(sigismember(&a->actions[i].sa_mask,(int)j)!=sigismember(&b->actions[i].sa_mask,(int)j)) return 0;
    }
    for(i=0;i<2;i++) if(a->timers[i].it_value.tv_sec!=b->timers[i].it_value.tv_sec ||
        a->timers[i].it_value.tv_usec!=b->timers[i].it_value.tv_usec || a->timers[i].it_interval.tv_sec!=b->timers[i].it_interval.tv_sec ||
        a->timers[i].it_interval.tv_usec!=b->timers[i].it_interval.tv_usec) return 0;
    for(j=1;j<NSIG;j++) if(sigismember(&a->mask,(int)j)!=sigismember(&b->mask,(int)j)) return 0;
    return 1;
}
#endif
static size_t lt_load(const char *name, unsigned char *bytes, size_t cap) {
    char path[512]; FILE *file; size_t n;
    if (snprintf(path,sizeof(path),"%s/%s",root,name) >= (int)sizeof(path) || !(file=fopen(path,"rb"))) exit(92);
    n=fread(bytes,1,cap,file); if(ferror(file)||fclose(file)) exit(92); return n;
}
static int lt_capture_exact(void) {
    unsigned char bytes[17000]; size_t n;
    n=lt_load("definition",bytes,sizeof(bytes));
    if(n!=lt_snapshot.definition_length||memcmp(bytes,lt_snapshot.definition,n)) return 0;
    n=lt_load("signature",bytes,sizeof(bytes));
    if(n!=lt_snapshot.signature_length||memcmp(bytes,lt_snapshot.signature,n)) return 0;
    n=lt_load("payload",bytes,sizeof(bytes));
    return n==lt_snapshot.payload_length&&!memcmp(bytes,lt_snapshot.payload,n);
}
static int lt_signature_reuse(struct bw_signature_observation *sv) {
    struct bw_verifier_material saved_material;
    struct bw_signature_observation saved_observation;
    unsigned ticks=lt_sig_ticks, forks=lt_forks, stops=lt_stops, waits=lt_consumes;
    unsigned adapter_calls=calls, boundary_calls=final_calls;
    char path[512], bytes[32]; int fd, ok, closed; ssize_t n;
    memcpy(&saved_material,&lt_material,sizeof(saved_material));
    memcpy(&saved_observation,sv,sizeof(saved_observation));
    if(snprintf(path,sizeof(path),"%s/canary",root)>=(int)sizeof(path)||(fd=open(path,O_RDONLY|O_NONBLOCK))<0) exit(92);
    active=1; ok=bw_signature_verify(&lt_material,sv)==BW_SIGNATURE_REJECTED; active=0;
    n=read(fd,bytes,sizeof(bytes));
    closed=close(fd);
    ok=ok&&n==7&&!memcmp(bytes,"canary\n",7)&&!closed&&
        !memcmp(&saved_material,&lt_material,sizeof(saved_material))&&!memcmp(&saved_observation,sv,sizeof(saved_observation))&&
        ticks==lt_sig_ticks&&forks==lt_forks&&stops==lt_stops&&waits==lt_consumes&&adapter_calls==calls&&boundary_calls==final_calls;
    return ok;
}
static int lt_inputs(int held[3]) {
    static const char *const file_names[]={"definition","payload","signature"};
    static const int role[]={0,3,4}; unsigned i; char path[512]; int fd;
    for(i=0;i<3;i++) {
        if(snprintf(path,sizeof(path),"%s/%s",root,file_names[i]) >= (int)sizeof(path) || (fd=open(path,O_RDONLY|O_NONBLOCK))<0) return -1;
        held[i]=fcntl(fd,F_DUPFD_CLOEXEC,10); if(held[i]<0||close(fd)||dup2(held[i],role[i])!=role[i]) return -1;
    }
    return 0;
}
static void lt_print_events(void) {
    unsigned i; printf("[");
    for(i=0;i<lt_event_count;i++) { struct lt_event *e=&lt_events[i];
        printf("%s{\"call\":\"%s\",\"outcome\":%d,\"values\":[%lld,%lld,%lld,%lld]}",i?",":"",e->call,e->outcome,e->values[0],e->values[1],e->values[2],e->values[3]); }
    printf("]");
}
int main(int argc, char **argv) {
    struct bw_verifier_material before, probe, probe_before;
    struct bw_signature_observation sv={0};
    struct bw_verifier_request request;
    const volatile sig_atomic_t *flag=NULL;
    unsigned i, first_events, first_dirs, repeat_at, before_mutations, before_origin;
    int first, retry=-9, legacy_first=-9, repeated, legacy_repeat, first_unchanged, repeat_clean=1;
    int captured=-9, prepared=-9, verified=-9, held[3]={-1,-1,-1}, host_unchanged=-1;
    int origin_ok=1, flag_ok=1, released=-9, captures_exact=0, final_owned=1, sv_reuse_clean=1;
#ifdef LS_NORMAL
    struct lt_host host_before, host_after;
#endif
    if(argc!=9) return 90;
    root=argv[1]; lt_kind=argv[2]; lt_mode=argv[3]; lt_phase=argv[5]; owner=getpid(); fake=1;
    lt_selected_signal=0; for(i=0;i<3;i++) if(!strcmp(argv[4],lt_signal_names[i])) lt_selected_signal=(int)i;
    lt_start_time=lt_parse(argv[6]); lt_work_time=lt_parse(argv[7]); lt_cleanup_time=lt_parse(argv[8]);
    fault=lt_is("signature-unknown-close-late")?"material-close-0":"modeled-lifetime";
    for(i=0;i<7;i++) fds[i]=-1;
    for(i=0;i<3;i++) material[i]=-1;
    for(i=0;i<6;i++) lt_limits[i].rlim_cur=lt_limits[i].rlim_max=RLIM_INFINITY;
#ifdef LS_NORMAL
    if(lt_host_read(&host_before)) return 93;
#endif
    if(!strcmp(lt_kind,"nonempty")) { memset(&lt_material,0xa5,sizeof(lt_material)); lt_material.state=BW_VM_READY; }
    memcpy(&before,&lt_material,sizeof(before));
    if(!strcmp(lt_kind,"legacy-first")) legacy_first=bw_process_guard();
    first=bw_start_reserve(!strcmp(lt_kind,"null")?NULL:&lt_material);
    first_events=lt_event_count; first_dirs=lt_mkdirs; first_unchanged=!memcmp(&before,&lt_material,sizeof(before));
    if(!strcmp(lt_kind,"null")||!strcmp(lt_kind,"nonempty")) { memset(&lt_material,0,sizeof(lt_material)); retry=bw_start_reserve(&lt_material); }
    if(lt_material.state==BW_VM_RESERVED||(!strcmp(lt_kind,"spent")&&lt_at("reserve"))) {
        flag=lt_material.cancelled;
        origin_ok=lt_material.started.tv_sec==1000&&lt_material.started.tv_nsec==1234&&flag!=NULL&&!*flag;
    }
    memset(&probe,0,sizeof(probe)); memcpy(&probe_before,&probe,sizeof(probe));
    repeat_at=lt_event_count; before_mutations=lt_mutations; before_origin=lt_origin_count;
    repeated=bw_start_reserve(&probe); legacy_repeat=bw_process_guard();
    if(repeated!=0||legacy_repeat!=0||memcmp(&probe,&probe_before,sizeof(probe))||before_mutations!=lt_mutations||before_origin!=lt_origin_count)
        repeat_clean=0;
    for(i=repeat_at;i<lt_event_count;i++) if(strcmp(lt_events[i].call,"supported")&&strcmp(lt_events[i].call,"getuid")&&
        strcmp(lt_events[i].call,"geteuid")&&strcmp(lt_events[i].call,"getgid")&&strcmp(lt_events[i].call,"getegid")) repeat_clean=0;
    if(lt_material.state==BW_VM_RESERVED) {
        origin_ok=lt_material.started.tv_sec==1000&&lt_material.started.tv_nsec==1234&&flag!=NULL&&!*flag;
        if(!strcmp(lt_kind,"flag")&&lt_at("after-reserve")) lt_fire();
        if(strcmp(lt_kind,"null")&&strcmp(lt_kind,"nonempty")) {
            if(lt_inputs(held)) return 94;
            if(!strcmp(lt_kind,"spent")&&lt_at("capture")) lt_material_ns=2000000000LL;
            captured=bw_auth_capture_reserved(&lt_material,&lt_snapshot);
            if(captured==BW_CAPTURED) captures_exact=lt_capture_exact();
            if(!strcmp(lt_kind,"flag")&&lt_at("after-capture")) lt_fire();
            if(!strcmp(lt_kind,"spent")&&lt_at("prepare")) lt_material_ns=2000000000LL;
            lengths[0]=lt_load("key",inputs[0],sizeof(inputs[0]));
            lengths[1]=lt_load("signature",inputs[1],sizeof(inputs[1]));
            lengths[2]=lt_load("definition",inputs[2],sizeof(inputs[2]));
            request=(struct bw_verifier_request){inputs[0],captured==BW_CAPTURED?lt_snapshot.signature:inputs[1],
                captured==BW_CAPTURED?lt_snapshot.definition:inputs[2],lengths[0],
                captured==BW_CAPTURED?lt_snapshot.signature_length:lengths[1],captured==BW_CAPTURED?lt_snapshot.definition_length:lengths[2]};
            prepared=bw_verifier_prepare(&request,&lt_material);
            if(prepared==BW_MATERIAL_OK) {
                for(i=0;i<3;i++) { material[i]=lt_material.fd[i]; track(material[i]); }
                if(!strcmp(lt_kind,"flag")&&lt_at("after-prepare")) lt_fire();
                active=1; verified=bw_signature_verify(&lt_material,&sv); active=0;
                if(!strcmp(lt_kind,"flag")&&lt_at("after-signature")) lt_fire();
                sv_reuse_clean=lt_signature_reuse(&sv);
            }
        }
        if(lt_material.state==BW_VM_RESERVED||lt_material.state==BW_VM_READY) released=bw_verifier_release(&lt_material);
        flag_ok=lt_material.cancelled==flag;
        origin_ok=origin_ok&&lt_material.started.tv_sec==1000&&lt_material.started.tv_nsec==1234;
    }
    for(i=0;i<3;i++) if(held[i]>=0&&close(held[i])) return 95;
    for(i=3;i<32;i++) if(fcntl((int)i,F_GETFD)!=-1) final_owned=0;
    if(lt_material.state==BW_VM_RESERVED||lt_material.state==BW_VM_READY||lt_material.state==BW_VM_VERIFYING) final_owned=0;
#ifdef LS_NORMAL
    if(lt_host_read(&host_after)) return 93;
    host_unchanged=lt_host_same(&host_before,&host_after);
#endif
    printf("LS_RESULT {\"first\":%d,\"first_unchanged\":%d,\"first_events\":%u,\"first_dirs\":%u,"
        "\"retry\":%d,\"legacy_first\":%d,\"repeat_clean\":%d,\"repeat_at\":%u,\"origin_count\":%u,"
        "\"origin_ok\":%d,\"flag_ok\":%d,\"flag_value\":%d,\"callbacks\":%u,\"callbacks_clean\":%u,"
        "\"mkdirs\":%u,\"members\":%u,\"member_writes\":%u,\"capture\":%d,\"capture_exact\":%d,"
        "\"capture_reads\":%u,\"capture_closes\":[%u,%u,%u],\"prepare\":%d,\"signature\":%d,"
        "\"sv_cause\":%d,\"sv_budget\":%u,\"sv_launch\":%u,\"sv_consumed\":%u,\"sv_lost\":%u,"
        "\"sv_bytes\":%u,\"sv_final_failed\":%u,\"sv_material_unknown\":%u,\"forks\":%u,"
        "\"stops\":%u,\"consumes\":%u,\"synthetic_pid\":%d,\"bad_order\":%u,\"bad_close\":%u,\"sv_reuse_clean\":%d,"
        "\"final_calls\":%u,\"material_state\":%d,\"material_cause\":%d,\"material_uncertain\":%u,"
        "\"released\":%d,\"final_owned\":%d,\"mutations\":%u,\"blocked\":%u,\"unblocked\":%u,"
        "\"host_unchanged\":%d,\"readonly_checks\":%u,\"samples\":[",
        first,first_unchanged,first_events,first_dirs,retry,legacy_first,repeat_clean,repeat_at,lt_origin_count,
        origin_ok,flag_ok,flag?*flag:-1,lt_callback_count,lt_callbacks_clean,lt_mkdirs,lt_members,lt_member_writes,
        captured,captures_exact,lt_capture_reads,lt_capture_closes[0],lt_capture_closes[1],lt_capture_closes[2],prepared,verified,
        sv.cause,sv.cleanup_budget_failed,sv.launch_granted,sv.wait_consumed,sv.wait_lost,sv.diagnostic_bytes,sv.final_boundary_failed,
        sv.material_unknown_closes,lt_forks,lt_stops,lt_consumes,sv.pid?sv.pid==12345:1,bad_order,bad_close,sv_reuse_clean,final_calls,
        lt_material.state,lt_material.cause,lt_material.uncertain,released,final_owned,lt_mutations,lt_blocked,lt_unblocked,host_unchanged,
#ifdef LS_NORMAL
        lt_readonly
#else
        0U
#endif
    );
    for(i=0;i<lt_sig_ticks;i++) printf("%s%lld",i?",":"",lt_samples[i]);
    printf("],\"events\":"); lt_print_events(); printf("}\n"); return 0;
}
#endif
