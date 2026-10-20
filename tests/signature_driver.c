/* SPDX-License-Identifier: BSD-3-Clause */
/* Fixed inert execution double; no native verifier, secret key or project code. */
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
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *root, *fault;
static pid_t owner, born_child;
static volatile sig_atomic_t cancelled;
static unsigned char inputs[3][17000];
static size_t lengths[3];
static int active, fake, channels, fds[7], material[3], stop_errno, modeled_stop;
static int death_gate[2]={-1,-1};
static unsigned calls, waits, signals_sent, lost_seen, reaped_seen, final_calls, bad_order, bad_close;
static unsigned generations[128], consumed[128];
static int jumped, final_seen, interrupted, poll_failed, witness_error;
static struct timespec baseline;
static int is(const char *s) { return !strcmp(fault,s); }
static int parent(void) { return getpid()==owner; }
static int named(const char *prefix,int i) { char b[80];snprintf(b,sizeof(b),"%s-%d",prefix,i);return is(b); }
static void track(int fd) { if(fd>=0 && fd<128) generations[fd]++; }
static void death_consume(unsigned i) {
    int fd=death_gate[i];
    death_gate[i]=-1;
    if(fd>=0 && close(fd)) witness_error=1;
}
static void group_observation(pid_t pid) {
    pid_t group=getpgid(pid);
    int saved=group<0?errno:0;
    printf("SV_GROUP %ld %ld %d\n",(long)pid,(long)group,saved);
    if(fflush(stdout)) witness_error=1;
}

char *bw_sv_test_mkdtemp(char *p) {
    int n=snprintf(p,256,"%s/material.XXXXXXXXXX",root);
    return n<0 || n>=256 ? NULL : mkdtemp(p);
}

uid_t bw_v_getuid(void) { return is("root") ? 0 : getuid(); }
int bw_v_clock_gettime(clockid_t id,struct timespec *t) {
    int r=clock_gettime(id,t);
    calls++;
    if(!active) return r;
    if(is("clock-error") || (final_seen && is("clock-final-error"))) return -1;
    if(is("clock-invalid")) t->tv_nsec=1000000000L;
    if(is("child-alarm")) *t=baseline;
    if((born_child && is("deadline-work")) || jumped) t->tv_sec+=10;
    return r;
}
int bw_v_open(const char *p,int flags,...) {
    int fd;
    calls++;
    if(is("null-error")) {errno=EIO;return -1;}
    fd=open(p,flags);track(fd);fds[0]=fd;return fd;
}
int bw_v_pipe(int pair[2]) {
    int n=channels++,r;
    calls++;
    if(named("pipe",n)) {errno=EIO;return -1;}
    r=pipe(pair);
    if(!r) {fds[1+2*n]=pair[0];fds[2+2*n]=pair[1];track(pair[0]);track(pair[1]);}
    return r;
}
int bw_v_fcntl(int fd,int command,...) {
    va_list ap;
    int arg=0,i;
    calls++;
    if(command==F_SETFD || command==F_SETFL || command==F_DUPFD_CLOEXEC) {
        va_start(ap,command);arg=va_arg(ap,int);va_end(ap);
    }
    if(parent() && command==F_SETFD)
        for(i=0;i<7;i++) if(fd==fds[i] && named("cloexec",i)) {errno=EIO;return -1;}
    if(!parent() && command==F_DUPFD_CLOEXEC && is("child-dup")) {errno=EIO;return -1;}
    if(parent() && fd==material[0] && command==F_GETFL && is("access")) return O_RDWR;
    if(parent() && fd==material[0] && command==F_GETFD && is("cloexec-missing")) return 0;
    if(command==F_SETFD || command==F_SETFL || command==F_DUPFD_CLOEXEC) return fcntl(fd,command,arg);
    return fcntl(fd,command);
}
off_t bw_v_lseek(int fd,off_t at,int whence) {
    calls++;
    if(parent() && is("offset") && fd==material[0]) return 1;
    return lseek(fd,at,whence);
}
int bw_v_fstat(int fd,struct stat *s) {
    int r=fstat(fd,s);
    calls++;
    if(parent() && !r && fd==material[0]) {
        if(is("metadata")) s->st_ino++;
        if(is("links")) s->st_nlink=1;
        if(is("owner")) s->st_uid++;
    }
    if(parent() && !r && ((fd==1 && is("output-alias")) ||
        (fd==material[1] && is("input-alias")))) return fstat(material[0],s);
    return r;
}
int bw_v_close(int fd) {
    int r=close(fd),i,fail=0,absent=0;
    if(!active || !parent()) return r;
    calls++;
    if(fd>=0 && fd<128) {
        if(consumed[fd]==generations[fd]) bad_close++;
        consumed[fd]=generations[fd];
    }
    for(i=0;i<7;i++) if(fd==fds[i]) {
        if(named("close",i)) fail=1;
        if(named("ebadf",i)) absent=1;
    }
    if(fd==fds[1] && is("cancel-work-close")) fail=1;
    for(i=0;i<3;i++) if(fd==material[i] && named("material-close",i)) fail=1;
    if(fd==fds[1] && is("deadline-close")) jumped=1;
    if(fd==fds[3] && is("cancel-close")) cancelled=1;
    if(fail) {errno=EINTR;return -1;}
    if(absent) {errno=EBADF;return -1;}
    return r;
}
pid_t bw_v_fork(void) {
    pid_t p;
    calls++;
    if(is("fork-error")) {errno=EAGAIN;return -1;}
    if(fake) {born_child=12345;return born_child;}
    if(is("gate-reader-death")) {
        if(pipe(death_gate)) return -1;
        if(fcntl(death_gate[0],F_SETFD,FD_CLOEXEC) || fcntl(death_gate[1],F_SETFD,FD_CLOEXEC)) {
            death_consume(0);death_consume(1);return -1;
        }
    }
    p=fork();
    if(p<0) {death_consume(0);death_consume(1);}
    if(p>0) {
        struct timespec born={0,0};
        pid_t observed_group;
        int group_errno;
        born_child=p;
        death_consume(0);
        if(clock_gettime(CLOCK_MONOTONIC,&born)) witness_error=1;
        observed_group=getpgid(p);group_errno=observed_group<0?errno:0;
        printf("SV_BORN %ld %ld %lld %ld %d\n",(long)p,(long)observed_group,(long long)born.tv_sec,born.tv_nsec,group_errno);
        if(fflush(stdout)) witness_error=1;
    }
    if(!p) death_consume(1);
    return p;
}
int bw_v_setpgid(pid_t pid,pid_t group) {
    int r;
    calls++;
    if(parent() && is("group-error")) {errno=EPERM;return -1;}
    r=fake ? 0 : setpgid(pid,group);
    if(parent() && !fake && !r) {
        /* Acknowledged owned syscall, separate from a fallible later sample. */
        printf("SV_SETGROUP %ld %ld\n",(long)pid,(long)group);
        if(fflush(stdout)) witness_error=1;
        group_observation(pid);
        if(is("gate-reader-death")) {
            if(write(death_gate[1],"D",1)!=1) witness_error=1;
            death_consume(1);
        }
    }
    if(parent() && r<0) {death_consume(0);death_consume(1);}
    return r;
}
int bw_v_dup2(int old,int target) {
    if(is("child-map")) {errno=EIO;return -1;}
    return dup2(old,target);
}
ssize_t bw_v_write(int fd,const void *p,size_t n) {
    ssize_t r;
    calls++;
    if(parent() && fd==fds[6]) {
        if(is("gate-error")) {errno=EPIPE;return -1;}
        if(is("gate-short")) return 0;
        if(fake) return (ssize_t)n;
        r=write(fd,p,n);
        if(is("cancel-work") || is("cancel-work-close")) cancelled=1;
        return r;
    }
    return write(fd,p,n);
}
ssize_t bw_v_read(int fd,void *p,size_t n) {
    calls++;
    if(!parent() && is("gate-reader-death") && fd==fds[5]) {
        char grant;
        /* Common child() has already installed/unblocked its ALRM5 here. */
        if(read(death_gate[0],&grant,1)!=1 || grant!='D') _exit(125);
        death_consume(0);
        _exit(125);
    }
    if(parent()) {
        if(is("read-error")) {errno=EIO;return -1;}
        if(is("read-eintr") && !interrupted++) {errno=EINTR;return -1;}
        if(is("short") && n>1) n=1;
    }
    return read(fd,p,n);
}
int bw_v_poll(struct pollfd *p,nfds_t n,int ms) {
    calls++;
    /* The work boundary fails once; terminal custody keeps its independent
     * real polling opportunity rather than deliberately losing a real child. */
    if(is("poll-error") && !poll_failed++) {errno=EIO;return -1;}
    return poll(p,n,ms);
}
int bw_v_waitid(idtype_t type,id_t pid,siginfo_t *s,int options) {
    int r;
    calls++;
    if(pid==(id_t)getpid()) {
        if(is("unsupported-wait")) {errno=EINVAL;return -1;}
        return waitid(type,pid,s,options);
    }
    if(options!=(WEXITED|WNOHANG|WNOWAIT) || reaped_seen) bad_order++;
    /* Force the erroring poll boundary even if this tiny child exited already. */
    if(is("poll-error")) {memset(s,0,sizeof(*s));return 0;}
    if(fake) {
        if(is("modeled-observer-loss")) {lost_seen=1;errno=ECHILD;return -1;}
        memset(s,0,sizeof(*s));s->si_pid=born_child;s->si_code=CLD_EXITED;return 0;
    }
    r=waitid(type,pid,s,options);
    return r;
}
pid_t bw_v_wait4(pid_t pid,int *status,int options,struct rusage *usage) {
    pid_t r;
    calls++;waits++;
    if(lost_seen || reaped_seen || !signals_sent || pid!=born_child || options!=WNOHANG) bad_order++;
    if(fake) {
        if(is("modeled-wait-error")) {lost_seen=1;errno=ECHILD;return -1;}
        *status=is("modeled-status")?127:0;
        memset(usage,0,sizeof(*usage));reaped_seen=1;return pid;
    }
    r=wait4(pid,status,options,usage);
    if(r==pid) {
        reaped_seen=1;
        if(is("cancel-wait")) cancelled=1;
        if(is("deadline-wait")) jumped=1;
    }
    return r;
}
int bw_v_kill(pid_t pid,int sig) {
    int r,saved;
    siginfo_t observed;
    calls++;signals_sent++;
    if(lost_seen || reaped_seen || sig!=SIGKILL || (pid!=born_child && pid!=-born_child)) {
        bad_order++;errno=EPERM;return -1;
    }
    if(fake) r=0;
    else {
        /* Observation while custody is still held; it grants no new authority. */
        group_observation(born_child);
        r=kill(pid,sig);saved=errno;stop_errno=r<0?saved:0;
        if(r<0 && saved==EPERM) {
            memset(&observed,0,sizeof(observed));
            /* Fixed test child never forks. Model native stop separately from
             * Darwin's actual zombie-only refusal; actual consuming wait follows. */
            if(!waitid(P_PID,(id_t)born_child,&observed,WEXITED|WNOHANG|WNOWAIT) && observed.si_pid==born_child) {
                modeled_stop=1;r=0;
            }
        }
    }
    if(is("stop-error")) {errno=EPERM;return -1;}
    return r;
}

#ifndef TEST_REAL_PLATFORM
int bw_sv_supported(void) { return !is("unsupported"); }
int bw_sv_closefrom(int low) {
    int i;
    if(is("child-closefrom")) {errno=EINTR;return -1;}
    for(i=low;i<128;i++) (void)close(i);
    return 0;
}
int bw_sv_child_limits(void) {
    const int kinds[4]={RLIMIT_CPU,RLIMIT_NOFILE,RLIMIT_CORE,RLIMIT_FSIZE};
    const rlim_t caps[4]={2,16,0,0};
    struct rlimit r,seen;
    unsigned i;
    if(is("child-limit")) return -1;
    /* DATA/STACK and native pledge are modeled, never host qualification. */
    for(i=0;i<4;i++) {
        if(getrlimit(kinds[i],&r)) return -1;
        if(r.rlim_cur==RLIM_INFINITY || r.rlim_cur>caps[i]) r.rlim_cur=caps[i];
        if(r.rlim_max==RLIM_INFINITY || r.rlim_max>caps[i]) r.rlim_max=caps[i];
        if(setrlimit(kinds[i],&r) || getrlimit(kinds[i],&seen) ||
            seen.rlim_cur!=r.rlim_cur || seen.rlim_max!=r.rlim_max) return -1;
    }
    return 0;
}
int bw_sv_child_boundary(void) { return is("child-boundary") ? -1 : 0; }
int bw_sv_parent_boundary(int final) {
    siginfo_t s;
    unsigned i;
    if(final) {
        final_calls++;final_seen=1;
        if(is("cancel-final")) cancelled=1;
        if(is("deadline-final")) jumped=1;
        return is("final-boundary") ? -1 : 0;
    }
    if(is("gate-reader-death")) {
        for(i=0;i<100;i++) {
            memset(&s,0,sizeof(s));
            if(!waitid(P_PID,(id_t)born_child,&s,WEXITED|WNOHANG|WNOWAIT) && s.si_pid==born_child) return 0;
            (void)poll(NULL,0,10);
        }
        return -1;
    }
    return is("parent-boundary") ? -1 : 0;
}
#endif

int bw_v_execve(const char *path,char *const args[],char *const env[]) {
    static const char *const expected[]={"signify","-V","-q","-p","/dev/fd/3","-x","/dev/fd/4","-m","/dev/fd/5",NULL};
    static const char *const environment[]={"LC_ALL=C","LANG=C","TZ=UTC","PATH=",NULL};
    struct stat s;
    unsigned i;
    unsigned char b[256];
    size_t at;
    ssize_t n;
    if(is("exec-error")) {errno=ENOENT;return -1;}
    if(strcmp(path,"/usr/bin/signify")) _exit(91);
    for(i=0;expected[i];i++) if(!args[i] || strcmp(args[i],expected[i])) _exit(91);
    if(args[i]) _exit(91);
    for(i=0;environment[i];i++) if(!env[i] || strcmp(env[i],environment[i])) _exit(91);
    if(env[i] || read(0,b,1)!=0) _exit(91);
    for(i=0;i<3;i++) {
        if(fstat((int)i+3,&s) || !S_ISREG(s.st_mode) || s.st_nlink || s.st_size!=(off_t)lengths[i] ||
            (fcntl((int)i+3,F_GETFL)&O_ACCMODE)!=O_RDONLY || fcntl((int)i+3,F_GETFD)&FD_CLOEXEC ||
            lseek((int)i+3,0,SEEK_CUR)!=0) _exit(91);
        at=0;
        while((n=read((int)i+3,b,sizeof(b)))>0) {
            if((size_t)n>lengths[i]-at || memcmp(b,inputs[i]+at,(size_t)n)) _exit(91);
            at+=(size_t)n;
        }
        if(n<0 || at!=lengths[i]) _exit(91);
    }
    for(i=6;i<32;i++) if(fcntl((int)i,F_GETFD)!=-1 || errno!=EBADF) _exit(91);
    if(is("child-alarm")) for(;;) pause();
    if(is("child-signal")) {raise(SIGTERM);_exit(92);}
    if(is("child-reject")) _exit(1);
    if(is("exact-output") || is("overflow")) {
        memset(b,'X',sizeof(b));
        for(i=0;i<16;i++) if(write(i<8?1:2,b,sizeof(b))!=(ssize_t)sizeof(b)) _exit(92);
        if(is("overflow") && write(2,"X",1)!=1) _exit(92);
    } else if(write(1,"X",1)!=1 || write(2,"Y",1)!=1) _exit(92);
    _exit(0);
}

static size_t load(const char *path,unsigned char *b,size_t cap) {
    FILE *f=fopen(path,"rb");size_t n;
    if(!f) exit(93);
    n=fread(b,1,cap,f);
    if(ferror(f) || fclose(f)) exit(93);
    return n;
}

static int setup_failure(const char *stage,int result,const struct bw_verifier_material *m) {
    printf("SV_SETUP %s %d %d %d %d\n",stage,result,errno,(int)m->state,(int)m->cause);
    return 94;
}

int main(int argc,char **argv) {
    struct bw_verifier_material m={0},copy;
    struct bw_verifier_request request;
    struct bw_signature_observation r={0},saved;
    struct sigaction action;
    unsigned i,prior;
    int result,fresh,sentinel,fd;
    if(argc!=6) return 90;
    root=argv[1];fault=argv[5];owner=getpid();fake=!strncmp(fault,"modeled-",8);
    for(i=0;i<7;i++) fds[i]=-1;
    for(i=0;i<3;i++) lengths[i]=load(argv[i+2],inputs[i],sizeof(inputs[i]));
    request.key=inputs[0];request.key_length=lengths[0];
    request.signature=inputs[1];request.signature_length=lengths[1];
    request.definition=inputs[2];request.definition_length=lengths[2];
    result=bw_verifier_reserve(&cancelled,&m);
    if(result!=BW_MATERIAL_OK) return setup_failure("reserve",result,&m);
    result=bw_verifier_prepare(&request,&m);
    if(result!=BW_MATERIAL_OK) return setup_failure("prepare",result,&m);
    for(i=0;i<3;i++) {material[i]=m.fd[i];track(material[i]);}
    fd=open("/dev/null",O_RDONLY);
    if(fd<0 || (sentinel=fcntl(fd,F_DUPFD_CLOEXEC,20))<0) return setup_failure("sentinel",-1,&m);
    close(fd);track(sentinel);
    memset(&action,0,sizeof(action));sigemptyset(&action.sa_mask);action.sa_handler=SIG_IGN;
    if(sigaction(SIGPIPE,&action,NULL)) return setup_failure("pipe-disposition",-1,&m);
    action.sa_handler=SIG_DFL;
    if(is("sigchld-ignore")) action.sa_handler=SIG_IGN;
    if(is("sigchld-no-wait")) action.sa_flags=SA_NOCLDWAIT;
    if(sigaction(SIGCHLD,&action,NULL)) return setup_failure("child-disposition",-1,&m);
    if(is("sigpipe-default")) {action.sa_handler=SIG_DFL;action.sa_flags=0;if(sigaction(SIGPIPE,&action,NULL)) return setup_failure("pipe-default",-1,&m);}
    if(is("cancel-before")) cancelled=1;
    if(clock_gettime(CLOCK_MONOTONIC,&baseline)) return setup_failure("clock",-1,&m);
    active=1;
    result=bw_signature_verify(&m,&r);
    active=0;
    death_consume(0);death_consume(1);
    if(witness_error) return 98;
    if(bad_order || bad_close || r.state!=BW_SV_FINISHED || r.pid!=born_child) return 95;
    for(i=3;i<32;i++) if((int)i!=sentinel && fcntl((int)i,F_GETFD)!=-1) return 96;
    if(fcntl(sentinel,F_GETFD)<0) return 96;
    memcpy(&copy,&m,sizeof(m));memcpy(&saved,&r,sizeof(r));prior=calls;
    fresh=open("/dev/null",O_RDONLY);
    if(fresh<0 || bw_signature_verify(&m,&r)!=BW_SIGNATURE_REJECTED || prior!=calls ||
        memcmp(&copy,&m,sizeof(m)) || memcmp(&saved,&r,sizeof(r)) || fcntl(fresh,F_GETFD)<0) return 97;
    close(fresh);close(sentinel);
    printf("SV_RESULT %d %d %d %ld %u %u %u %u %u %u %u %u %u %u %u %u %d %d %d %u %u %u %u %u\n",
        result,r.state,r.cause,(long)r.pid,r.group_owned,r.launch_granted,r.stop_attempted,r.wait_consumed,r.wait_lost,
        r.diagnostic_bytes,r.close_errors,r.unknown_closes,r.material_close_errors,r.material_unknown_closes,
        r.cleanup_budget_failed,r.final_boundary_failed,stop_errno,modeled_stop,fake,waits,signals_sent,final_calls,bad_order,bad_close);
    return 0;
}
