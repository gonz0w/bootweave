/* SPDX-License-Identifier: BSD-3-Clause */
/* Test-only static call bindings; no host guard/security mutation or child. */
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "trusted_start.h"
#include "private_start_material.h"
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

#if defined(EO_ENTRY_PREAMBLE)
int eo_supported(void); uid_t eo_uid(void); uid_t eo_euid(void); gid_t eo_gid(void); gid_t eo_egid(void);
int eo_empty(sigset_t *); int eo_add(sigset_t *,int); int eo_action(int,const struct sigaction *,struct sigaction *);
int eo_mask(int,const sigset_t *,sigset_t *); int eo_getlimit(int,struct rlimit *); int eo_setlimit(int,const struct rlimit *);
int eo_gettimer(int,struct itimerval *); int eo_settimer(int,const struct itimerval *,struct itimerval *);
int eo_entry_clock(clockid_t,struct timespec *); int eo_presence(int,int,...); int eo_entry_close(int);
int eo_closefrom(int); int eo_entry_boundary(const struct bw_verifier_material *); int eo_entry_final(int);
void eo_exit(int);
int eo_guard_return(int);
enum bw_material_result eo_reserve(const struct timespec *,const volatile sig_atomic_t *,struct bw_verifier_material *);
enum bw_material_result eo_entry_release(struct bw_verifier_material *);
enum bw_capture eo_capture(struct bw_verifier_material *,struct bw_auth_snapshot *);
enum bw_authenticated_result eo_auth(struct bw_verifier_material *,const struct bw_auth_snapshot *,struct bw_authenticated_observation *);
#undef sigemptyset
#undef sigaddset
#define bw_sv_supported() eo_supported()
#define getuid() eo_uid()
#define geteuid() eo_euid()
#define getgid() eo_gid()
#define getegid() eo_egid()
#define sigemptyset(s) eo_empty(s)
#define sigaddset(s,n) eo_add(s,n)
#define sigaction(n,a,o) eo_action(n,a,o)
#define sigprocmask(h,s,o) eo_mask(h,s,o)
#define getrlimit(k,s) eo_getlimit(k,s)
#define setrlimit(k,s) eo_setlimit(k,s)
#define getitimer(k,s) eo_gettimer(k,s)
#define setitimer(k,s,o) eo_settimer(k,s,o)
#define clock_gettime(k,s) eo_entry_clock(k,s)
#define fcntl(...) eo_presence(__VA_ARGS__)
#define close(f) eo_entry_close(f)
#define bw_sv_closefrom(n) eo_closefrom(n)
#define bw_material_reserve_started(t,f,m) eo_reserve(t,f,m)
#define bw_verifier_release(m) eo_entry_release(m)
#define bw_auth_capture_reserved(m,s) eo_capture(m,s)
#define bw_authenticated_output(m,s,o) eo_auth(m,s,o)
#define bw_sv_parent_boundary(n) eo_entry_final(n)
#define _exit(n) eo_exit(n)
/* Static definition/call dispatch only: the definition's literal `int lifetime`
 * becomes eo_actual_guard(int lifetime); calls guard(0/1) delegate the SAME
 * actual body then observe its returned result. No result is synthesized. */
#define guard(x) EO_GUARD_##x)
#define EO_GUARD_int eo_actual_guard(int
#define EO_GUARD_0 eo_guard_return(eo_actual_guard(0)
#define EO_GUARD_1 eo_guard_return(eo_actual_guard(1)
#define bw_entry_boundary(m) eo_entry_boundary(m)
#elif defined(EO_SIGNATURE_PREAMBLE)
int eo_signature_clock(clockid_t,struct timespec *); int eo_signature_action(int,const struct sigaction *,struct sigaction *);
pid_t eo_fork(void); int eo_kill(pid_t,int); int eo_waitid(idtype_t,id_t,siginfo_t *,int);
pid_t eo_wait4(pid_t,int *,int,struct rusage *); ssize_t eo_gate(int,const void *,size_t);
int bw_v_open(const char *,int,...); int bw_v_pipe(int *); int bw_v_fcntl(int,int,...);
off_t bw_v_lseek(int,off_t,int); int bw_v_fstat(int,struct stat *); int bw_v_close(int);
int bw_v_setpgid(pid_t,pid_t); int bw_v_dup2(int,int); ssize_t bw_v_read(int,void *,size_t);
int bw_v_poll(struct pollfd *,nfds_t,int); uid_t bw_v_getuid(void); int bw_v_execve(const char *,char *const[],char *const[]);
void eo_exit(int); int eo_child_mask(int,const sigset_t *,sigset_t *);
#define clock_gettime(k,s) eo_signature_clock(k,s)
#define sigaction(n,a,o) eo_signature_action(n,a,o)
#define sigprocmask(h,s,o) eo_child_mask(h,s,o)
#define open(...) bw_v_open(__VA_ARGS__)
#define pipe(p) bw_v_pipe(p)
#define fcntl(...) bw_v_fcntl(__VA_ARGS__)
#define lseek(f,o,w) bw_v_lseek(f,o,w)
#define fstat(f,s) bw_v_fstat(f,s)
#define close(f) bw_v_close(f)
#define fork() eo_fork()
#define setpgid(p,g) bw_v_setpgid(p,g)
#define dup2(f,n) bw_v_dup2(f,n)
#define write(f,b,n) eo_gate(f,b,n)
#define read(f,b,n) bw_v_read(f,b,n)
#define poll(p,n,t) bw_v_poll(p,n,t)
#define waitid(t,p,s,o) eo_waitid(t,p,s,o)
#define wait4(p,s,o,u) eo_wait4(p,s,o,u)
#define kill(p,s) eo_kill(p,s)
#define getuid() bw_v_getuid()
#define execve(p,a,e) bw_v_execve(p,a,e)
#define _exit(n) eo_exit(n)
#elif defined(EO_UNUSED_DEPENDENCIES)
/* Legacy guard/start fixtures never call the coordinator. Resolve its newly
 * linked policy/hash prerequisites fail-closed while linking all real stages. */
const struct bw_policy_definition bw_compiled_policy = {0};
int bw_policy_sha256(const unsigned char *b,size_t n,unsigned char out[32]) {
    (void)b; (void)n; memset(out,0,32); return 0;
}
#else
#include <CommonCrypto/CommonDigest.h>
#define main eo_unused_signature_main
#include "signature_driver.c"
#undef main

static const char *eo_mode;
static const int eo_roles[]={0,3,4}, eo_signals[]={SIGINT,SIGTERM,SIGHUP,SIGALRM,SIGPROF,SIGXCPU,SIGXFSZ,SIGPIPE,SIGCHLD};
static const int eo_resources[]={RLIMIT_CPU,RLIMIT_DATA,RLIMIT_STACK,RLIMIT_NOFILE,RLIMIT_CORE,RLIMIT_FSIZE};
static struct sigaction eo_actions[9]; static unsigned eo_installed[9];
static struct rlimit eo_limits[6]; static struct itimerval eo_timers[2];
static struct bw_verifier_material *eo_material;
static const struct bw_auth_snapshot *eo_snapshot;
static struct bw_authenticated_observation *eo_authenticated;
static const volatile sig_atomic_t *eo_flag;
static struct timespec eo_origin;
static unsigned eo_guard_calls,eo_origins,eo_arms,eo_presence_count,eo_closefrom_calls,eo_reserves,eo_boundaries,eo_captures,eo_auths;
static unsigned eo_entry_releases,eo_total_consumptions,eo_entry_finals,eo_total_finals,eo_after_final;
static unsigned eo_entry_closes[3],eo_capture_closes[3],eo_rebound[3],eo_members,eo_writes,eo_capture_reads,eo_prepares,eo_signatures,eo_content_calls,eo_hashes,eo_inspects;
static unsigned eo_prefix_ticks,eo_guard_done,eo_inside_reserve,eo_inside_capture,eo_inside_auth,eo_inside_release,eo_final;
static unsigned eo_flag_identity=1,eo_origin_identity=1,eo_span_identity=1,eo_purity=1,eo_wire_protocol=1,eo_high_closed;
static unsigned eo_presence_roles[5],eo_output_calls,eo_forks,eo_stops,eo_consumes;
static unsigned eo_captured_exact=1;
static unsigned eo_record;
static int eo_unrelated_fd=-1;
static unsigned char eo_wire[2048]; static size_t eo_wire_length;
static const unsigned char *eo_definition,*eo_payload,*eo_signature;
static const unsigned char *eo_key;
static size_t eo_key_length;
static size_t eo_definition_length,eo_payload_length,eo_signature_length;
static long long eo_now=250000000LL; static int eo_clock_error,eo_clock_back;
struct eo_event { char call[40]; long long value; };
static struct eo_event eo_events[128]; static unsigned eo_events_count;
static int eo_is(const char *s){return !strcmp(eo_mode,s);}
static int eo_has(const char *s){return !strncmp(eo_mode,s,strlen(s));}
static int eo_error(void){errno=EIO;return -1;}
static void eo_event(const char *call,long long value){
 if(eo_events_count>=128||strlen(call)>=sizeof(eo_events[0].call))exit(90);
 memcpy(eo_events[eo_events_count].call,call,strlen(call)+1);eo_events[eo_events_count++].value=value;
}
static void eo_effect(const char *call,long long value){if(eo_final&&eo_record)eo_after_final++;eo_event(call,value);}
static size_t eo_load(const char *,unsigned char *,size_t);
static unsigned eo_index(const int *values,unsigned count,int value){unsigned i;for(i=0;i<count;i++)if(values[i]==value)return i;exit(90);}
static void eo_time(struct timespec *t,long long ns){t->tv_sec=1000+(time_t)(ns/1000000000LL);t->tv_nsec=1234+(long)(ns%1000000000LL);
 if(t->tv_nsec>=1000000000L){t->tv_sec++;t->tv_nsec-=1000000000L;}if(t->tv_nsec<0){t->tv_sec--;t->tv_nsec+=1000000000L;}}
static void eo_fire(int signal_number){unsigned events=eo_events_count;struct bw_verifier_material saved;
 if(eo_material)memcpy(&saved,eo_material,sizeof(saved));
 if(!eo_installed[eo_index(eo_signals,9,signal_number)])exit(90);
 eo_actions[eo_index(eo_signals,9,signal_number)].sa_handler(signal_number);
 if(events!=eo_events_count||(eo_material&&memcmp(&saved,eo_material,sizeof(saved))))eo_purity=0;
}
static void eo_fault(const char *phase){char prefix[96];const char *fault_name;
 snprintf(prefix,sizeof(prefix),"prefix:%s:",phase);
 if(!strncmp(eo_mode,prefix,strlen(prefix))){fault_name=eo_mode+strlen(prefix);
  if(!strcmp(fault_name,"cancel"))eo_fire(SIGINT);
  if(!strcmp(fault_name,"clock-error"))eo_clock_error=1;
  if(!strcmp(fault_name,"clock-backward"))eo_clock_back=1;
  if(!strcmp(fault_name,"deadline"))eo_now=2000000000LL;
  if(!strcmp(fault_name,"below"))eo_now=1999999999LL;
 }
}
static void eo_pending_cancel(const char *phase){char token[96];snprintf(token,sizeof(token),"prefix:%s:cancel",phase);if(eo_is(token))eo_fire(SIGINT);}
static int eo_requested_signal(void){size_t n=strlen(eo_mode);return n>=4&&!strcmp(eo_mode+n-4,"TERM")?SIGTERM:n>=3&&!strcmp(eo_mode+n-3,"HUP")?SIGHUP:SIGINT;}
int eo_guard_return(int result){if(result){eo_guard_done=1;eo_pending_cancel("presence-0");if(eo_has("cancel:after-guard:"))eo_fire(eo_requested_signal());}return result;}
int eo_supported(void){eo_guard_calls++;eo_effect("guard",0);return !eo_is("guard:unsupported")&&!eo_is("combined:guard-input");}
uid_t eo_uid(void){return eo_is("guard:root")?0:getuid();}
uid_t eo_euid(void){return eo_is("guard:setid")?geteuid()+1:geteuid();}
gid_t eo_gid(void){return getgid();}gid_t eo_egid(void){return getegid();}
int eo_empty(sigset_t *s){return sigemptyset(s);}int eo_add(sigset_t *s,int n){return sigaddset(s,n);}
int eo_action(int n,const struct sigaction *a,struct sigaction *o){unsigned i=eo_index(eo_signals,9,n);if(o||!a)exit(90);
 memcpy(&eo_actions[i],a,sizeof(*a));eo_installed[i]=1;return 0;}
int eo_mask(int how,const sigset_t *s,sigset_t *old){if(!s||old||(how!=SIG_BLOCK&&how!=SIG_UNBLOCK))exit(90);
 if(how==SIG_BLOCK&&eo_is("guard:block-error"))return eo_error();return 0;}
int eo_getlimit(int kind,struct rlimit *out){unsigned i=eo_index(eo_resources,6,kind);memcpy(out,&eo_limits[i],sizeof(*out));return 0;}
int eo_setlimit(int kind,const struct rlimit *in){unsigned i=eo_index(eo_resources,6,kind);if(eo_is("guard:limit-error"))return eo_error();
 memcpy(&eo_limits[i],in,sizeof(*in));return 0;}
int eo_gettimer(int kind,struct itimerval *out){unsigned i=kind==ITIMER_REAL?0:kind==ITIMER_PROF?1:2;if(i>1)exit(90);
 memcpy(out,&eo_timers[i],sizeof(*out));if(!eo_arms&&eo_is(i?"guard:active-prof":"guard:active-real"))out->it_value.tv_sec=1;return 0;}
int eo_settimer(int kind,const struct itimerval *in,struct itimerval *old){unsigned i=kind==ITIMER_REAL?0:kind==ITIMER_PROF?1:2;
 if(i>1||old||in->it_value.tv_sec!=(i?4:15)||in->it_value.tv_usec||in->it_interval.tv_sec||in->it_interval.tv_usec)exit(90);
 eo_arms++;eo_event("arm",kind);if(eo_is(i?"guard:prof-arm-error":"guard:real-arm-error"))return eo_error();
 memcpy(&eo_timers[i],in,sizeof(*in));return 0;}
void eo_exit(int n){(void)n;exit(97);}int eo_child_mask(int h,const sigset_t *s,sigset_t *o){(void)h;(void)s;(void)o;exit(97);}
int eo_entry_clock(clockid_t kind,struct timespec *out){char phase[40];if(kind!=CLOCK_MONOTONIC)exit(90);
 if(!eo_origins){eo_origins++;eo_time(out,0);memcpy(&eo_origin,out,sizeof(eo_origin));eo_event("origin",0);return 0;}
 if(eo_presence_count<5){snprintf(phase,sizeof(phase),"presence-%u",eo_presence_count);eo_fault(phase);}
 else if(!eo_closefrom_calls){eo_fault("before-closefrom");if(eo_is("spent:presence"))eo_now=2000000000LL;if(eo_is("below:presence"))eo_now=1999999999LL;}
 else if(!eo_reserves)eo_fault("after-closefrom");
 else if(!eo_boundaries)eo_fault("before-boundary");
 else if(eo_captures&&!eo_auths)eo_fault("before-auth");
 eo_prefix_ticks++;eo_event("prefix-clock",eo_now);eo_time(out,eo_clock_back?-500000000LL:eo_now);return eo_clock_error?-1:0;
}
int eo_presence(int fd,int command,...){char mode[32],phase[40];int result;if(fd<0||fd>4||command!=F_GETFD||eo_presence_count>=5)exit(90);
 eo_effect("presence",fd);eo_presence_roles[eo_presence_count++]=(unsigned)fd;
 snprintf(mode,sizeof(mode),"missing:%d",fd);if(eo_is(mode)){errno=EBADF;return -1;}
 snprintf(mode,sizeof(mode),"query-error:%d",fd);if(eo_is(mode))return eo_error();result=fcntl(fd,command);
 snprintf(phase,sizeof(phase),"presence-%d",fd+1);eo_pending_cancel(fd==4?"before-closefrom":phase);return result;}
static void eo_rebind(unsigned role){char path[512];int fd;
 if(snprintf(path,sizeof(path),"%s/canary",root)>=(int)sizeof(path)||(fd=open(path,O_RDONLY|O_NONBLOCK))<0)exit(91);
 if(fd!=eo_roles[role]){if(dup2(fd,eo_roles[role])!=eo_roles[role]||close(fd))exit(91);}eo_rebound[role]=1;
}
static int eo_input_close(int fd,int capture_owner){unsigned i=eo_index(eo_roles,3,fd);int result,saved;char mode[80];
 eo_effect(capture_owner?"capture-close":"entry-close",fd);
 if(capture_owner)eo_capture_closes[i]++;else eo_entry_closes[i]++;
 result=close(fd);saved=errno;eo_rebind(i);
 snprintf(mode,sizeof(mode),"%s:ebadf:%u",capture_owner?"capture":"entry",i);if(eo_is(mode)){errno=EBADF;return -1;}
 snprintf(mode,sizeof(mode),"%s:unknown:%u",capture_owner?"capture":"entry",i);
 if(eo_is(mode)||(!capture_owner&&((i==0&&eo_is("combined:guard-input"))||(i==1&&eo_is("combined:boundary-input-material-final"))||
  (i==2&&eo_is("combined:capture-not-taken-input-material-final"))))){errno=EINTR;return -1;}
 errno=saved;return result;
}
int eo_entry_close(int fd){return eo_input_close(fd,0);}int eo_capture_close(int fd){return eo_input_close(fd,1);}
int eo_closefrom(int low){eo_effect("closefrom",low);eo_closefrom_calls++;if(low!=5)exit(90);
 if(eo_unrelated_fd>=5){int fd=eo_unrelated_fd;eo_unrelated_fd=-1;if(close(fd))exit(91);}
 if(eo_is("closefrom-high")){eo_high_closed=1;eo_event("synthetic-high-close",70);}
 if(eo_is("closefrom-error"))return eo_error();
 if(eo_is("spent:closefrom"))eo_now=2000000000LL;if(eo_is("below:closefrom"))eo_now=1999999999LL;
 eo_pending_cancel("after-closefrom");
 if(eo_has("cancel:after-closefrom:"))eo_fire(eo_requested_signal());
 return 0;
}
int eo_material_clock(clockid_t kind,struct timespec *out){if(kind!=CLOCK_MONOTONIC)exit(90);
 if(eo_inside_reserve&&eo_is("reserve:deadline"))eo_now=2000000000LL;
 if(eo_inside_reserve&&eo_is("reserve:unknown-close")&&eo_material->directory_known)eo_now=2000000000LL;
 eo_time(out,eo_now);return 0;}
char *eo_mkdtemp(char *template){int n;eo_effect("mkdtemp",0);if(strcmp(template,"/tmp/bootweave-verify.XXXXXXXXXX"))exit(90);
 if(eo_is("reserve:io"))return NULL;n=snprintf(template,256,"%s/material.XXXXXXXXXX",root);return n<0||n>=256?NULL:mkdtemp(template);}
int eo_material_fstat(int fd,struct stat *s){int result=fstat(fd,s);if(!result&&eo_inside_reserve&&eo_is("reserve:ownership"))s->st_mode^=S_IWOTH;return result;}
int eo_material_openat(int fd,const char *name,int flags,...){mode_t mode=0;va_list args;eo_effect("member-open",flags);eo_members++;
 if(flags&O_CREAT){va_start(args,flags);mode=(mode_t)va_arg(args,int);va_end(args);return openat(fd,name,flags,mode);}return openat(fd,name,flags);}
ssize_t eo_material_write(int fd,const void *b,size_t n){eo_effect("member-write",n);eo_writes++;return write(fd,b,n);}
int eo_material_close(int fd){int result;eo_effect("material-close",fd);result=active?bw_v_close(fd):close(fd);
 if((eo_inside_reserve&&eo_is("reserve:unknown-close")&&eo_material->directory==-1)||
 (eo_inside_release&&(eo_is("entry-release-unknown")||eo_has("combined:")))){errno=EINTR;return -1;}return result;}
enum bw_material_result eo_reserve(const struct timespec *origin,const volatile sig_atomic_t *flag,struct bw_verifier_material *m){
 enum bw_material_result result;eo_effect("reserve",0);eo_reserves++;eo_material=m;eo_flag=flag;
 if(origin->tv_sec!=1000||origin->tv_nsec!=1234)eo_origin_identity=0;
 eo_inside_reserve=1;result=bw_material_reserve_started(origin,flag,m);eo_inside_reserve=0;
 if(result!=BW_MATERIAL_OK)eo_total_consumptions++;
 eo_pending_cancel("before-boundary");
 if(eo_has("cancel:after-reserve:"))eo_fire(eo_requested_signal());
 return result;
}
static enum bw_material_result eo_release(struct bw_verifier_material *m){enum bw_material_result result;eo_total_consumptions++;
 eo_inside_release=1;result=bw_verifier_release(m);eo_inside_release=0;return result;}
enum bw_material_result eo_entry_release(struct bw_verifier_material *m){eo_effect("entry-release",0);eo_entry_releases++;return eo_release(m);}
enum bw_material_result eo_fused_release(struct bw_verifier_material *m){eo_effect("fused-release",0);return eo_release(m);}
enum bw_material_result eo_signature_release(struct bw_verifier_material *m){eo_effect("signature-release",0);return eo_release(m);}
int eo_entry_final(int final){eo_effect("entry-final",final);if(final!=1)exit(90);eo_entry_finals++;eo_total_finals++;eo_final=1;
 return eo_is("entry-final-error")||eo_has("combined:")?eo_error():0;}
int eo_fused_final(int final){eo_effect("fused-final",final);if(final!=1)exit(90);eo_total_finals++;eo_final=1;return 0;}
int eo_entry_boundary(const struct bw_verifier_material *m){eo_effect("entry-boundary",0);eo_boundaries++;
 if(m!=eo_material||m->state!=BW_VM_RESERVED||m->cancelled!=eo_flag||m->started.tv_sec!=1000||m->started.tv_nsec!=1234)eo_origin_identity=0;
 if(eo_is("normal-boundary"))return bw_entry_boundary(m);
 return eo_is("boundary-refusal")||eo_has("entry:")||eo_is("entry-final-error")||
 eo_is("entry-release-unknown")||eo_is("closefrom-high")||eo_is("combined:boundary-input-material-final")||eo_has("below:")||
 (eo_has("prefix:")&&!eo_has("prefix:before-auth:"))?eo_error():0;
}
ssize_t eo_capture_read(int fd,void *b,size_t n){eo_effect("capture-read",fd);eo_capture_reads++;
 if(eo_is("capture:read-error"))return eo_error();return read(fd,b,n);}
int eo_capture_fstat(int fd,struct stat *s){int result=fstat(fd,s);if(!result&&eo_is("capture:metadata-error")&&fd==0)s->st_mode=S_IFIFO;return result;}
int eo_capture_clock(clockid_t kind,struct timespec *t){if(eo_is("capture:deadline"))eo_now=2000000000LL;
 if(eo_is("capture:clock-error"))return eo_error();return eo_material_clock(kind,t);}
enum bw_capture eo_capture(struct bw_verifier_material *m,struct bw_auth_snapshot *s){enum bw_capture result;
 eo_effect("capture",0);eo_captures++;eo_snapshot=s;
 if(eo_is("capture:not-taken")||eo_is("combined:capture-not-taken-input-material-final"))return BW_CAPTURE_NOT_TAKEN;
 if(eo_is("capture:cancel"))eo_fire(SIGINT);
 eo_inside_capture=1;result=bw_auth_capture_reserved(m,s);eo_inside_capture=0;
 if(result==BW_CAPTURED){unsigned char b[17000];size_t n;
  n=eo_load("definition",b,sizeof(b));if(n!=s->definition_length||memcmp(b,s->definition,n))eo_captured_exact=0;
  n=eo_load("payload",b,sizeof(b));if(n!=s->payload_length||memcmp(b,s->payload,n))eo_captured_exact=0;
  n=eo_load("signature",b,sizeof(b));if(n!=s->signature_length||memcmp(b,s->signature,n))eo_captured_exact=0;}
 eo_definition=s->definition;eo_definition_length=s->definition_length;eo_payload=s->payload;eo_payload_length=s->payload_length;
 eo_signature=s->signature;eo_signature_length=s->signature_length;
 eo_pending_cancel("before-auth");
 if(eo_is("cancel:after-capture"))eo_fire(SIGINT);return result;}
enum bw_authenticated_result eo_auth(struct bw_verifier_material *m,const struct bw_auth_snapshot *s,struct bw_authenticated_observation *o){
 enum bw_authenticated_result result;eo_effect("authenticated",0);eo_auths++;eo_authenticated=o;
 if(eo_is("auth:not-taken")||eo_is("combined:auth-not-taken-material-final"))return BW_AUTH_NOT_TAKEN;
 if(m!=eo_material||s!=eo_snapshot||m->cancelled!=eo_flag)eo_flag_identity=0;
 eo_inside_auth=1;result=bw_authenticated_output(m,s,o);eo_inside_auth=0;return result;}
enum bw_material_result eo_prepare(const struct bw_verifier_request *r,struct bw_verifier_material *m){enum bw_material_result result;unsigned i;
 eo_effect("prepare",0);eo_prepares++;if(r->definition!=eo_definition||r->signature!=eo_signature||r->definition_length!=eo_definition_length||r->signature_length!=eo_signature_length)eo_span_identity=0;
 eo_key=r->key;eo_key_length=r->key_length;
 result=bw_verifier_prepare(r,m);if(result!=BW_MATERIAL_OK)eo_total_consumptions++;
 else for(i=0;i<3;i++){material[i]=m->fd[i];track(material[i]);}return result;}
int eo_signature_clock(clockid_t kind,struct timespec *out){return eo_material_clock(kind,out);}
int eo_signature_action(int n,const struct sigaction *a,struct sigaction *old){unsigned i=eo_index(eo_signals,9,n);if(a||!old||!eo_installed[i])exit(90);memcpy(old,&eo_actions[i],sizeof(*old));return 0;}
pid_t eo_fork(void){eo_effect("synthetic-fork",12345);eo_forks++;born_child=12345;return born_child;}
ssize_t eo_gate(int fd,const void *b,size_t n){if(fd!=fds[6]||n!=1||*(const char *)b!='V')exit(90);
 if(eo_is("cancel:signature-gate"))eo_fire(SIGINT);return 1;}
int eo_waitid(idtype_t type,id_t pid,siginfo_t *out,int options){if(type!=P_PID||options!=(WEXITED|WNOHANG|WNOWAIT))exit(90);
 if(pid==(id_t)getpid()){errno=ECHILD;return -1;}if(pid!=12345||reaped_seen||lost_seen){bad_order++;return eo_error();}
 if(eo_is("signature-wait-lost")){lost_seen=1;errno=ECHILD;return -1;}memset(out,0,sizeof(*out));out->si_pid=12345;out->si_code=CLD_EXITED;return 0;}
int eo_kill(pid_t pid,int signal_number){eo_stops++;signals_sent++;if((pid!=12345&&pid!=-12345)||signal_number!=SIGKILL||reaped_seen||lost_seen){bad_order++;return eo_error();}
 return eo_is("signature-stop-unknown")?eo_error():0;}
pid_t eo_wait4(pid_t pid,int *status,int options,struct rusage *usage){if(pid!=12345||options!=WNOHANG||reaped_seen||lost_seen||!signals_sent){bad_order++;return eo_error();}
 eo_consumes++;reaped_seen=1;*status=eo_is("signature-rejected")?127:0;memset(usage,0,sizeof(*usage));return pid;}
enum bw_signature_result eo_signature_verify(struct bw_verifier_material *m,struct bw_signature_observation *o){enum bw_signature_result result;
 eo_effect("signature",0);eo_signatures++;active=1;result=bw_signature_verify(m,o);active=0;
 if(eo_is("cancel:after-signature"))eo_fire(SIGINT);return result;}
int eo_content(const struct bw_policy_request *r,char *text,size_t cap,struct bw_content_observation *out){eo_content_calls++;
 if(r->key!=eo_key||r->key_length!=eo_key_length||r->definition!=eo_definition||r->payload!=eo_payload||r->payload_length!=eo_payload_length||r->definition_length!=eo_definition_length)eo_span_identity=0;
 return bw_content_review(r,text,cap,out);}
int bw_policy_sha256(const unsigned char *bytes,size_t n,unsigned char digest[32]){eo_hashes++;
 if((eo_hashes==1&&(bytes!=eo_definition||n!=eo_definition_length))||(eo_hashes>1&&(bytes!=eo_payload||n!=eo_payload_length)))eo_span_identity=0;
 return n<=BW_AUTH_PAYLOAD_MAX&&CC_SHA256(bytes,(CC_LONG)n,digest)==digest;}
int eo_structure(const unsigned char *bytes,size_t n,struct bw_payload_summary *summary){eo_inspects++;if(bytes!=eo_payload||n!=eo_payload_length)eo_span_identity=0;return bw_payload_structure_review(bytes,n,summary);}
int eo_authenticated_clock(clockid_t kind,struct timespec *out){static unsigned ticks;ticks++;
 if(eo_is("auth-bound:original-eleven")&&ticks>=4)eo_now=11000000000LL;
 if(eo_is("auth-bound:phase-two")&&ticks>=6)eo_now=2250000000LL;
 if(eo_is("auth-bound:clock-backward")&&ticks>=4)eo_now=-500000000LL;
 if(eo_is("auth-bound:clock-error")&&ticks>=4)return eo_error();
 if(eo_clock_error)return eo_error();return eo_material_clock(kind,out);}
int eo_output_fcntl(int fd,int command,...){if(fd!=1||command!=F_GETFL)exit(90);return eo_is("output-preflight")?O_WRONLY:O_WRONLY|O_NONBLOCK;}
int eo_output_fstat(int fd,struct stat *s){if(fd!=1)exit(90);memset(s,0,sizeof(*s));s->st_dev=11;s->st_ino=23;s->st_nlink=1;s->st_mode=S_IFREG|0600;return 0;}
int eo_output_isatty(int fd){if(fd!=1)exit(90);return 0;}
ssize_t eo_output_write(int fd,const void *b,size_t n){ssize_t result=(ssize_t)n;unsigned index=eo_output_calls++;
 if(index>=4||fd!=1||n>sizeof(eo_wire)-eo_wire_length||!n)exit(90);
 if(!eo_final||!eo_authenticated||eo_authenticated->signature.cause!=BW_SV_NONE||!eo_authenticated->signature.wait_consumed||
 eo_material->state!=BW_VM_CLOSED||eo_total_consumptions!=1||eo_content_calls!=1||eo_hashes!=2||eo_inspects!=1)eo_wire_protocol=0;
 if((eo_is("output-short")||eo_is("output:partial-error"))&&!index)result=17;
 if(eo_is("output-eintr")&&!index){errno=EINTR;result=-1;}
 if(eo_is("output:partial-error")&&index){errno=EIO;result=-1;}
 if(eo_is("output:zero"))result=0;
 if(eo_is("output:eagain")||eo_is("output:error")){errno=eo_is("output:eagain")?EAGAIN:EIO;result=-1;}
 if(eo_is("output:oversized"))result=(ssize_t)n+1;
 if(result>0&&(size_t)result<=n){memcpy(eo_wire+eo_wire_length,b,(size_t)result);eo_wire_length+=(size_t)result;}
 if(eo_is("output:postwrite-cancel"))eo_fire(SIGINT);
 if(eo_is("output:postwrite-deadline"))eo_now=11000000000LL;
 if(eo_is("output:postwrite-clock"))eo_clock_error=1;
 return result;}
static int eo_bytes_zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;}
static size_t eo_load(const char *name,unsigned char *b,size_t cap){char path[512];FILE *f;size_t n;
 if(snprintf(path,sizeof(path),"%s/%s",root,name)>=(int)sizeof(path)||!(f=fopen(path,"rb")))exit(92);
 n=fread(b,1,cap,f);if(ferror(f)||fclose(f))exit(92);return n;}
static int eo_canaries(void){unsigned i;char bytes[16];for(i=0;i<3;i++){
 if(!eo_rebound[i])return 0;if(lseek(eo_roles[i],0,SEEK_SET)!=0||read(eo_roles[i],bytes,sizeof(bytes))!=7||memcmp(bytes,"canary\n",7))return 0;}return 1;}
static void eo_bind_inputs(void){static const char *const names[]={"definition","payload","signature"};char path[512];unsigned i;int fd;
 for(i=0;i<3;i++){if(snprintf(path,sizeof(path),"%s/%s",root,names[i])>=(int)sizeof(path)||(fd=open(path,O_RDONLY|O_NONBLOCK))<0)exit(92);
  if(fd!=eo_roles[i]){if(dup2(fd,eo_roles[i])!=eo_roles[i]||close(fd))exit(92);}}
}
int eo_signature_boundary(int final){if(!final)return 0;eo_effect("signature-final",1);eo_total_finals++;eo_final=1;
 return eo_is("auth-final-boundary-error")?eo_error():0;}
int main(int argc,char **argv){
 const struct bw_entry_observation *observation,*same;struct bw_entry_observation before_repeat,first_before;
 struct bw_verifier_material prior={0},probe={0},saved_probe;unsigned i,events_before,origins_before,arms_before,guard_count;
 int result,repeated,repeat_clean=1,shared_clean=1,flags_before[4],flags_after[4],flags_ok=1,canaries_ok=1,snapshot_zero=-1,first_no_write=1;
 unsigned char saved_snapshot_hash[32],after_snapshot_hash[32];unsigned flags_index=0;
 if(argc!=3)return 90;root=argv[1];eo_mode=argv[2];owner=getpid();fake=1;
 fault=eo_is("signature-material-close-unknown")?"material-close-0":"entry-modeled";
 for(i=0;i<7;i++)fds[i]=-1;for(i=0;i<3;i++)material[i]=-1;
 for(i=0;i<6;i++)eo_limits[i].rlim_cur=eo_limits[i].rlim_max=RLIM_INFINITY;
 eo_bind_inputs();
 if(eo_has("missing:")){int role=atoi(eo_mode+8);if(role<0||role>4||((role==0||role==3||role==4)&&close(role)))return 93;}
 if(eo_is("closefrom-high")){char path[512];int fd;
  if(snprintf(path,sizeof(path),"%s/unrelated",root)>=(int)sizeof(path)||(fd=open(path,O_RDONLY|O_NONBLOCK))<5)return 93;eo_unrelated_fd=fd;}
 for(i=1;i<=2;i++){flags_before[flags_index++]=fcntl((int)i,F_GETFD);flags_before[flags_index++]=fcntl((int)i,F_GETFL);}
 if(eo_is("prior:legacy")){if(bw_process_guard()!=1)return 94;eo_guard_calls=0;eo_events_count=0;eo_origins=0;eo_arms=0;}
 if(eo_is("prior:start")){if(bw_start_reserve(&prior)!=BW_MATERIAL_OK)return 94;
  eo_guard_calls=0;eo_events_count=0;eo_origins=0;eo_arms=0;eo_reserves=0;eo_material=NULL;eo_flag=NULL;}
 memcpy(&first_before,bw_entry_observe(),sizeof(first_before));
 eo_record=1;result=bw_entry_output();observation=bw_entry_observe();if(!observation)return 94;
 if(result==BW_AUTH_NOT_TAKEN&&memcmp(&first_before,observation,sizeof(first_before)))first_no_write=0;
 guard_count=eo_guard_calls;events_before=eo_events_count;origins_before=eo_origins;arms_before=eo_arms;
 if(eo_snapshot){snapshot_zero=eo_bytes_zero(eo_snapshot,sizeof(*eo_snapshot));CC_SHA256(eo_snapshot,(CC_LONG)sizeof(*eo_snapshot),saved_snapshot_hash);}
 memcpy(&before_repeat,observation,sizeof(before_repeat));
 for(i=0;i<3;i++)if(!eo_rebound[i])eo_rebind(i);
 canaries_ok=eo_canaries();
 repeated=bw_entry_output();same=bw_entry_observe();
 if(repeated!=BW_AUTH_NOT_TAKEN||same!=observation||memcmp(&before_repeat,same,sizeof(before_repeat))||
  events_before!=eo_events_count||origins_before!=eo_origins||arms_before!=eo_arms||!eo_canaries())repeat_clean=0;
 if(eo_snapshot){CC_SHA256(eo_snapshot,(CC_LONG)sizeof(*eo_snapshot),after_snapshot_hash);if(memcmp(saved_snapshot_hash,after_snapshot_hash,32))repeat_clean=0;}
 eo_record=0;
 memcpy(&saved_probe,&probe,sizeof(probe));
 if(!eo_is("prior:legacy")&&!eo_is("prior:start")){
  if(bw_process_guard()!=0||bw_start_reserve(&probe)!=BW_MATERIAL_REJECTED||memcmp(&saved_probe,&probe,sizeof(probe))||
   origins_before!=eo_origins||arms_before!=eo_arms||!eo_canaries())shared_clean=0;
 }
 flags_index=0;for(i=1;i<=2;i++){flags_after[flags_index++]=fcntl((int)i,F_GETFD);flags_after[flags_index++]=fcntl((int)i,F_GETFL);}
 for(i=0;i<4;i++)if(flags_before[i]!=flags_after[i])flags_ok=0;
 if(eo_material){if(eo_material->cancelled!=eo_flag)eo_flag_identity=0;if(eo_material->started.tv_sec!=1000||eo_material->started.tv_nsec!=1234)eo_origin_identity=0;}
 printf("EO_RESULT {\"return\":%d,\"state\":%d,\"result\":%d,\"cause\":%d,\"capture\":%d,\"input_close_errors\":%u,\"input_unknown_closes\":%u,\"final_boundary_failed\":%u,"
  "\"guard_calls\":%u,\"origin_count\":%u,\"arms\":%u,\"closefrom\":%u,\"reserve\":%u,\"boundary\":%u,\"capture_calls\":%u,\"auth_calls\":%u,"
  "\"entry_release\":%u,\"entry_final\":%u,\"total_final\":%u,\"material_consumptions\":%u,\"output_attempts\":%u,\"material_state\":%d,\"material_cause\":%d,"
  "\"material_close_errors\":%u,\"material_unknown_closes\":%u,\"capture_close_errors\":%u,\"capture_unknown_closes\":%u,\"capture_reads\":%u,"
  "\"snapshot_zero\":%d,\"repeat_clean\":%d,\"shared_clean\":%d,\"canaries\":%d,\"flags_unchanged\":%d,\"origin_identity\":%u,\"flag_identity\":%u,\"span_identity\":%u,"
  "\"callback_purity\":%u,\"wire_protocol\":%u,\"after_final\":%u,\"bad_order\":%u,\"bad_close\":%u,\"members\":%u,\"writes\":%u,\"prepare\":%u,\"signature\":%u,"
  "\"content\":%u,\"hashes\":%u,\"inspect\":%u,\"forks\":%u,\"stops\":%u,\"consumes\":%u,\"high_closed\":%u,\"captured_exact\":%u,\"first_no_write\":%d,"
  "\"auth_state\":%d,\"auth_cause\":%d,\"sv_cause\":%d,\"sv_final_failed\":%u,\"sv_unknown\":%u,\"sv_lost\":%u,\"sv_consumed\":%u,\"wire_hex\":\"",
  result,observation->state,observation->result,observation->cause,observation->capture,observation->input_close_errors,observation->input_unknown_closes,observation->final_boundary_failed,
  guard_count,eo_origins,eo_arms,eo_closefrom_calls,eo_reserves,eo_boundaries,eo_captures,eo_auths,eo_entry_releases,eo_entry_finals,eo_total_finals,eo_total_consumptions,eo_output_calls,
  eo_material?eo_material->state:BW_VM_EMPTY,eo_material?eo_material->cause:BW_VM_NONE,eo_material?eo_material->close_errors:0,eo_material?eo_material->unknown_closes:0,
  eo_material?eo_material->capture_close_errors:0,eo_material?eo_material->capture_unknown_closes:0,eo_capture_reads,snapshot_zero,repeat_clean,shared_clean,canaries_ok,flags_ok,
  eo_origin_identity,eo_flag_identity,eo_span_identity,eo_purity,eo_wire_protocol,eo_after_final,bad_order,bad_close,eo_members,eo_writes,eo_prepares,eo_signatures,eo_content_calls,
  eo_hashes,eo_inspects,eo_forks,eo_stops,eo_consumes,eo_high_closed,eo_captured_exact,first_no_write,
  observation->authenticated.state,observation->authenticated.cause,observation->authenticated.signature.cause,
  observation->authenticated.signature.final_boundary_failed,observation->authenticated.signature.material_unknown_closes,
  observation->authenticated.signature.wait_lost,observation->authenticated.signature.wait_consumed);
 for(i=0;i<eo_wire_length;i++)printf("%02x",eo_wire[i]);
 printf("\",\"entry_closes\":[%u,%u,%u],\"capture_closes\":[%u,%u,%u],\"presence\":[",eo_entry_closes[0],eo_entry_closes[1],eo_entry_closes[2],eo_capture_closes[0],eo_capture_closes[1],eo_capture_closes[2]);
 for(i=0;i<eo_presence_count;i++)printf("%s%u",i?",":"",eo_presence_roles[i]);
 printf("],\"events\":[");for(i=0;i<events_before;i++)printf("%s[\"%s\",%lld]",i?",":"",eo_events[i].call,eo_events[i].value);printf("]}\n");
 /* Fixture-owned canaries/prior scratch are released after the entire observed
  * call/repeat witness; no coordinator-owned descriptor is retried. */
 for(i=0;i<3;i++)if(close(eo_roles[i]))return 95;
 if(prior.state==BW_VM_RESERVED&&bw_verifier_release(&prior)!=BW_MATERIAL_OK)return 95;
 return 0;
}
#endif
