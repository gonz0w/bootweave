/* SPDX-License-Identifier: BSD-3-Clause */
/* Original fixed native test supervisor; no configurable effects or shell. */
#include "auth_native.h"
#include "placement.h"
#include <stdio.h>
#ifndef __OpenBSD__
int main(int argc, char **argv) {
 (void)argv; fputs("Native placement checks require OpenBSD.\n",stderr); return argc==1?77:2;
}
#else
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sha2.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#define ROOT "/root/payload-placement/"
#define WORK ROOT "work/"
#define AUTH "/root/payload-auth/"
#ifndef AUTH_INSPECTOR
#define AUTH_INSPECTOR AUTH "inspect"
#endif
static int tests, hook_mode, current; static pid_t worker;
static long peak_controller, peak_child; static struct stat sentinel;
static struct pl_expected expected[128]; static char names[128][256];
static unsigned char input[AUTH_INPUT+1], signature[AUTH_RECORD];
static size_t input_length, signature_length;
static struct auth_context context;
struct packet { struct auth_result auth; struct pl_result placement; int flags, hook_error; };
static int hook_error;
static void fail(const char *message) {
 if(worker>0) { int s; (void)kill(-worker,SIGKILL); (void)kill(worker,SIGKILL); while(waitpid(worker,&s,0)<0 && errno==EINTR) {} }
 fprintf(stderr,"Native placement check failed: %s; check %d case %d.\n",message,tests,current); exit(1);
}
static void require(int ok,const char *message) { if(!ok) fail(message); }
static void hash(const unsigned char *b,size_t n,unsigned char d[32]) { SHA2_CTX c; SHA256Init(&c); SHA256Update(&c,b,n); SHA256Final(d,&c); }
static size_t read_file(const char *path,unsigned char *b,size_t cap) {
 struct stat s; int fd=open(path,O_RDONLY|O_NOFOLLOW|O_NONBLOCK); size_t at=0; ssize_t n;
 require(fd>=0 && !fstat(fd,&s) && S_ISREG(s.st_mode) && s.st_size>=0 && (size_t)s.st_size<=cap,"fixture bounds");
 while(at<cap && (n=read(fd,b+at,cap-at))!=0) { if(n>0) at+=(size_t)n; else if(errno!=EINTR) fail("fixture read"); }
 require(at==(size_t)s.st_size && !close(fd),"fixture exact EOF"); return at;
}
static void write_all(int fd,const unsigned char *b,size_t n) {
 while(n) { ssize_t w=write(fd,b,n); if(w>0) { b+=w; n-=(size_t)w; } else if(w<0 && errno==EINTR) continue; else fail("owned fixture write"); }
}
static void source(const unsigned char *b,size_t n) {
 int fd=open(WORK "source",O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW,0600);
 require(fd>=0,"exclusive source"); write_all(fd,b,n); require(!close(fd),"source close");
}
void auth_test_hook(int phase,const char *dir,const unsigned char *b,size_t n) {
 (void)dir; (void)b; (void)n;
 if(phase==2 && (hook_mode==1 || hook_mode==2)) {
  int fd;
  if(hook_mode==2 && unlink(WORK "source")) { hook_error=1; return; }
  fd=open(WORK "source",O_WRONLY|O_TRUNC|O_NOFOLLOW|(hook_mode==2?O_CREAT|O_EXCL:0),0600);
  if(fd<0 || write(fd,"changed",7)!=7 || close(fd)) hook_error=1;
 }
}
int auth_test_child(int stage) { if(stage==1 && hook_mode==3) return 1; return -1; }
static void canary(void) {
 unsigned char b[32]; struct stat s;
 require(!lstat(WORK "canary",&s) && S_ISREG(s.st_mode) && s.st_dev==sentinel.st_dev &&
   s.st_ino==sentinel.st_ino && s.st_uid==sentinel.st_uid && s.st_gid==sentinel.st_gid &&
   s.st_mode==sentinel.st_mode && s.st_nlink==sentinel.st_nlink && s.st_size==sentinel.st_size,
   "outside canary identity/type/ownership/mode/link/size");
 require(read_file(WORK "canary",b,sizeof(b))==14 && !memcmp(b,"owned sentinel",14),"outside canary bytes");
}
static size_t expected_file(const char *path) {
 unsigned char b[49152]; size_t n=read_file(path,b,sizeof(b)-1), count=0; char *line,*next;
 b[n]=0; line=(char *)b;
 while(*line) {
  int used=0,kind; unsigned long size; char digest[65]; size_t j;
  require(count<128,"expected node bound"); next=strchr(line,'\n'); require(next!=NULL,"expected record EOF"); *next=0;
  require(sscanf(line,"%255s %d %lu %64s%n",names[count],&kind,&size,digest,&used)==4 &&
    used==(int)strlen(line) && (kind==0 || kind==1) && size<=262144 && strlen(digest)==64,"fixed expected fixture record");
  expected[count].path=names[count]; expected[count].directory=kind; expected[count].size=(size_t)size;
  for(j=0;j<32;j++) {
   unsigned int byte; require(sscanf(digest+j*2,"%2x",&byte)==1,"expected digest"); expected[count].digest[j]=(unsigned char)byte;
  }
  count++; line=next+1;
 }
 return count;
}
static void keys(void) {
 static const char *const names4[]={AUTH "keys/selected.pub",AUTH "keys/selected.sec",AUTH "keys/other.pub",AUTH "keys/other.sec"};
 unsigned char actual[128],previous[128]; size_t i; struct stat st;
 for(i=0;i<4;i++) require(!lstat(names4[i],&st) && S_ISREG(st.st_mode) &&
     st.st_uid==0 && (st.st_mode&07777)==0600 && !auth_hash_file(names4[i],actual+i*32),"shared native pair identities");
 require(read_file(AUTH "key-lifetime.private",previous,sizeof(previous))==sizeof(previous) &&
     !memcmp(actual,previous,sizeof(actual)),"exact same two pairs lifetime");
}
static void load(const char *path,int other) {
 struct auth_child child; struct auth_result metrics={0};
 char *const args[]={"/usr/bin/signify","-S","-s",other?AUTH "keys/other.sec":AUTH "keys/selected.sec","-m","-","-x",WORK "supplied.sig",NULL};
 keys(); input_length=read_file(path,input,sizeof(input));
 memset(&context,0,sizeof(context)); context.length=input_length; hash(input,input_length,context.digest);
 context.key_length=read_file(AUTH "keys/selected.pub",context.key,AUTH_RECORD);
 require(!auth_hash_file("/usr/bin/signify",context.verifier_digest) &&
     !auth_hash_file(AUTH_INSPECTOR,context.inspector_digest),"pinned native tools");
 require(!auth_run(args,input,input_length,&child,auth_now()+45000,2,&metrics) &&
     WIFEXITED(child.status) && !WEXITSTATUS(child.status) && !child.out_length && !child.err_length && metrics.reaped==1,
     "real native fixture signing");
 if(metrics.child_peak_kib>peak_child) peak_child=metrics.child_peak_kib;
 signature_length=read_file(WORK "supplied.sig",signature,AUTH_RECORD);
 require(!unlink(WORK "supplied.sig"),"signature disposed");
}
static void remove_tree(int fd,int depth) {
 int copy=openat(fd,".",O_RDONLY|O_DIRECTORY|O_NOFOLLOW); DIR *d; struct dirent *e; size_t count=0;
 require(copy>=0 && depth<=17,"owned cleanup bound"); d=fdopendir(copy); require(d!=NULL,"cleanup inventory");
 for(;;) {
  struct stat s; int child;
  errno=0; e=readdir(d); if(!e) { require(!errno,"cleanup EOF"); break; }
  if(!strcmp(e->d_name,".")||!strcmp(e->d_name,"..")) continue;
  require(++count<=130 && !fstatat(fd,e->d_name,&s,AT_SYMLINK_NOFOLLOW),"cleanup type/count");
  if(S_ISDIR(s.st_mode)) {
   child=openat(fd,e->d_name,O_RDONLY|O_DIRECTORY|O_NOFOLLOW); require(child>=0,"cleanup directory reference");
   remove_tree(child,depth+1); require(!close(child) && !unlinkat(fd,e->d_name,AT_REMOVEDIR),"cleanup directory");
  } else require(!unlinkat(fd,e->d_name,0),"owned unlink (no link following)");
 }
 require(!closedir(d),"cleanup close");
}
static struct packet run(struct pl_context *pl) {
 int in[2],out[2],saved,status=0,done=0; size_t sent=0,got=0; pid_t w; struct rusage usage;
 struct packet p; long long deadline=auth_now()+45000; memset(&p,0,sizeof(p));
 require(!pipe(in) && !pipe(out),"controller pipes"); saved=fcntl(in[0],F_GETFL); require(saved>=0,"saved input flags");
 source(input,input_length); context.placement=pl; hook_error=0;
 worker=fork(); require(worker>=0,"controller fork");
 if(!worker) {
  struct rlimit data={64*1048576,64*1048576}, core={0,0}, fsize={1048576,1048576},cpu={15,15},fds={160,160};
  int sink=fcntl(out[1],F_DUPFD,16), root=pl?fcntl(pl->rootfd,F_DUPFD,16):-1,
      parent=pl?fcntl(pl->parentfd,F_DUPFD,16):-1;
  if(setsid()<0 || sink<0 || dup2(in[0],0)<0 || dup2(sink,5)<0 ||
      (pl && (root<0 || parent<0 || dup2(root,3)<0 || dup2(parent,4)<0))) _exit(125);
  closefrom(6); if(!pl) { close(3); close(4); } else { pl->rootfd=3; pl->parentfd=4; }
  if(setrlimit(RLIMIT_DATA,&data)||setrlimit(RLIMIT_RSS,&data)||setrlimit(RLIMIT_CORE,&core)||
     setrlimit(RLIMIT_FSIZE,&fsize)||setrlimit(RLIMIT_CPU,&cpu)||setrlimit(RLIMIT_NOFILE,&fds)) _exit(125);
  p.auth=auth_native(&context,signature,signature_length); if(pl) p.placement=pl->result;
  p.flags=fcntl(0,F_GETFL); p.hook_error=hook_error;
  if(write(5,&p,sizeof(p))!=(ssize_t)sizeof(p)) _exit(125); _exit(0);
 }
 close(in[0]); close(out[1]);
 { int f=fcntl(in[1],F_GETFL); require(f>=0 && !fcntl(in[1],F_SETFL,f|O_NONBLOCK),"nonblocking delivery");
  f=fcntl(out[0],F_GETFL); require(f>=0 && !fcntl(out[0],F_SETFL,f|O_NONBLOCK),"nonblocking result"); }
 while(!done || got<sizeof(p)) {
  ssize_t n; struct pollfd f[2];
  require(auth_now()<deadline,"external workflow deadline");
  if(in[1]>=0) {
   if(sent==input_length) { require(!close(in[1]),"input close"); in[1]=-1; }
   else { size_t amount=input_length-sent; if(amount>4096) amount=4096;
    n=write(in[1],input+sent,amount); if(n>0) sent+=(size_t)n;
    else if(n<0 && errno==EPIPE) { require(!close(in[1]),"rejected input close"); in[1]=-1; }
    else require(n<0 && (errno==EAGAIN||errno==EINTR),"input delivery"); }
  }
  if(got<sizeof(p)) { n=read(out[0],(unsigned char *)&p+got,sizeof(p)-got);
   if(n>0) got+=(size_t)n; else require(n<0 && (errno==EAGAIN||errno==EINTR),"result packet"); }
  if(!done) { w=wait4(worker,&status,WNOHANG,&usage); if(w==worker) done=1; else require(w==0 || (w<0 && errno==EINTR),"controller wait4"); }
  f[0].fd=in[1]; f[0].events=POLLOUT; f[0].revents=0; f[1].fd=out[0]; f[1].events=POLLIN; f[1].revents=0;
  if(!done || got<sizeof(p)) require(poll(f,2,10)>=0 || errno==EINTR,"controller poll");
 }
 require(WIFEXITED(status) && !WEXITSTATUS(status) && got==sizeof(p) && !p.hook_error,"controller fixed result/reap");
 require(usage.ru_maxrss>0 && usage.ru_maxrss<=65536 && p.auth.controller_peak_kib>0 && p.auth.controller_peak_kib<=65536,"controller measured RSS");
 if(usage.ru_maxrss>peak_controller) peak_controller=usage.ru_maxrss;
 if(p.auth.child_peak_kib>peak_child) peak_child=p.auth.child_peak_kib;
 require(p.flags==saved,"exact stdin flags restored");
 { unsigned char x; require(read(out[0],&x,1)==0 && !close(out[0]),"fixed packet EOF/close"); }
 if(in[1]>=0) require(!close(in[1]),"input remaining close");
 require(kill(worker,0)==-1 && errno==ESRCH && kill(-worker,0)==-1 && errno==ESRCH,"controller/children absent");
 worker=0; require(!unlink(WORK "source"),"owned changed source disposed"); canary(); tests++; return p;
}
static struct pl_context root(size_t count,enum pl_test test) {
 struct pl_context c; memset(&c,0,sizeof(c)); c.expected=expected; c.count=count; c.test=test;
 c.parentfd=open(WORK,O_RDONLY|O_DIRECTORY|O_NOFOLLOW); require(c.parentfd>=0,"trusted parent reference");
 require(!mkdirat(c.parentfd,"case",0700),"fresh empty case root");
 c.rootfd=openat(c.parentfd,"case",O_RDONLY|O_DIRECTORY|O_NOFOLLOW); require(c.rootfd>=0 && !fchown(c.rootfd,32767,32767) &&
     !fstat(c.rootfd,&c.identity) && S_ISDIR(c.identity.st_mode) && (c.identity.st_mode&07777)==0700,"fixed nobody root identity");
 return c;
}
static void cleanup(struct pl_context *c) {
 const char *name="case"; struct stat s;
 if(fstatat(c->parentfd,name,&s,AT_SYMLINK_NOFOLLOW) || !S_ISDIR(s.st_mode) || s.st_ino!=c->identity.st_ino || s.st_dev!=c->identity.st_dev) name="moved";
 remove_tree(c->rootfd,0); require(!close(c->rootfd),"case reference close");
 require(!unlinkat(c->parentfd,name,AT_REMOVEDIR),"case disposed");
 if(!strcmp(name,"moved")) { if(!fstatat(c->parentfd,"case",&s,AT_SYMLINK_NOFOLLOW)) require(!unlinkat(c->parentfd,"case",S_ISDIR(s.st_mode)?AT_REMOVEDIR:0),"substituted root disposed"); }
 require(!close(c->parentfd),"parent close"); canary();
}
static void ordinary(const char *stem,int accepted) {
 char path[128]; size_t count=0; struct pl_context c; struct packet p;
 snprintf(path,sizeof(path),ROOT "%s.tar",stem); load(path,0);
 if(accepted) { snprintf(path,sizeof(path),ROOT "%s.expected",stem); count=expected_file(path); }
 c=root(count,PL_REAL); p=run(&c);
 if(accepted) require(p.auth.completed && p.placement.complete && p.placement.planned==count &&
     p.placement.submitted==count && p.placement.observed==count && p.placement.reaped==1 &&
     p.placement.peak_kib>0 && !p.placement.uncertain,"independent exact positive effects");
 else require(!p.auth.completed && !p.placement.launched && !p.placement.observed,"rejection before member effects");
 cleanup(&c);
}
int main(int argc,char **argv) {
 int i; char stem[80]; struct termios tty; struct packet p; struct pl_context c; size_t count; long prerequisite_peak=0;
 (void)argv; require(argc==1 && getuid()==0 && isatty(0) && !tcgetattr(0,&tty),"fixed native tty/root invocation");
 require(signal(SIGPIPE,SIG_IGN)!=SIG_ERR,"bounded rejected-input delivery");
 require(!lstat(WORK "canary",&sentinel) && S_ISREG(sentinel.st_mode),"owned outside sentinel");
 require(tty.c_lflag&ICANON,"native canonical TTY");
 load(ROOT "cases/case-00.tar",0); count=expected_file(ROOT "cases/case-00.expected");
 /* Real crypto prerequisite before any placed member. */
 p=run(NULL); require(p.auth.verified && p.auth.failure==AUTH_PLACEMENT_BAD && p.auth.inspector_launches==1,"real stdin positive prerequisite");
 input[1025]^=1; hash(input,input_length,context.digest); p=run(NULL);
 require(p.auth.failure==AUTH_SIGNATURE && !p.auth.verified && !p.auth.inspector_launches,"real stdin negative prerequisite");
 input[1025]^=1; hash(input,input_length,context.digest);
 c=root(count,PL_REAL); require(!pl_prerequisites(c.rootfd,&c.identity,&prerequisite_peak),"locked unveil/drop/pledge prerequisite denials"); cleanup(&c);
 if(prerequisite_peak>peak_child) peak_child=prerequisite_peak;
 puts("Native placement crypto and confinement prerequisites passed.");
 for(i=0;i<35;i++) { current=i; snprintf(stem,sizeof(stem),"cases/case-%02d",i); ordinary(stem,i<8); }
 for(i=0;i<26;i++) { current=35+i; snprintf(stem,sizeof(stem),"aliases/alias-%02d",i); ordinary(stem,i<2 || (i>=6 && i<10)); }
 for(i=0;i<5;i++) { current=61+i; snprintf(stem,sizeof(stem),"placement/new-%02d",i); ordinary(stem,i==0||i==1||i==3); }
 load(ROOT "cases/case-00.tar",0); count=expected_file(ROOT "cases/case-00.expected");
 for(i=1;i<=3;i++) { hook_mode=i; c=root(count,PL_REAL); p=run(&c);
  require(i<3 ? p.auth.completed && p.placement.complete : !p.auth.completed && !p.placement.launched,"retained created bytes / failed inspector"); cleanup(&c); }
 hook_mode=0;
 /* Independently expected partial sizes/counts; synthetic injection cannot select policy. */
 for(i=PL_SETUP;i<=PL_DIAGNOSTIC;i++) {
  c=root(count,(enum pl_test)i); p=run(&c);
  require(!p.auth.completed && !p.placement.complete && p.placement.launched && p.placement.reaped && p.placement.peak_kib>0,"failure never completes; killed usage and reap");
  if(i==PL_SHORT) { struct stat s; unsigned char b[8]; require(!fstatat(c.rootfd,"readme.txt",&s,AT_SYMLINK_NOFOLLOW) && s.st_size==1,"exact partial file size");
   require(read_file(WORK "case/readme.txt",b,sizeof(b))==1 && b[0]=='h',"independent partial byte"); }
  if(i==PL_SETUP||i==PL_SIGNAL||i==PL_LOSS||i==PL_STALL||i==PL_OUTPUT||i==PL_DIAGNOSTIC)
   require(p.placement.observed==0,"failure before effects observed empty");
  cleanup(&c);
 }
 c=root(count,PL_EXIST); p=run(&c); require(!p.auth.completed && p.placement.observed==1,"existing final never overwritten");
 { struct stat s; require(!fstatat(c.rootfd,"readme.txt",&s,AT_SYMLINK_NOFOLLOW) && s.st_size==0,"preexisting final unchanged"); } cleanup(&c);
 load(ROOT "placement/new-00.tar",0); count=expected_file(ROOT "placement/new-00.expected");
 for(i=PL_LINK;i<=PL_AFTER_HOLD;i++) {
  c=root(count,(enum pl_test)i); p=run(&c); require(!p.auth.completed && !p.placement.complete,"held/missing/substituted binding rejects"); cleanup(&c);
 }
 /* Root validation, expected inventory mismatch and authentic trust rejection. */
 load(ROOT "cases/case-00.tar",0); count=expected_file(ROOT "cases/case-00.expected");
 c=root(count,PL_FILE_SWAP); p=run(&c); require(!p.auth.completed && !p.placement.complete,"held file substitution rejects before content write"); cleanup(&c);
 c=root(count,PL_CONTROLLER_SIGNAL); p=run(&c); require(!p.auth.completed && !p.placement.complete &&
     p.placement.launched && p.placement.reaped && p.placement.observed==0 && p.placement.peak_kib>0,
     "caught controller interruption stops dependent effects and records killed usage"); cleanup(&c);
 for(i=0;i<7;i++) {
  c=root(count,PL_REAL);
  if(i==0) { require(!renameat(c.parentfd,"case",c.parentfd,"moved"),"root missing fixture"); }
  if(i==1) { require(!renameat(c.parentfd,"case",c.parentfd,"moved") && !symlinkat(WORK "canary",c.parentfd,"case"),"symlink root fixture"); }
  if(i==2) { int fd=openat(c.rootfd,"unexpected",O_WRONLY|O_CREAT|O_EXCL,0600); require(fd>=0 && !close(fd),"nonempty root fixture"); }
  if(i==3) c.count--;
  if(i==4) context.digest[0]^=1;
  if(i==5) { int fd; require(!renameat(c.parentfd,"case",c.parentfd,"moved") && !mkdirat(c.parentfd,"case",0700),"root replaced fixture");
   fd=openat(c.parentfd,"case",O_RDONLY|O_DIRECTORY|O_NOFOLLOW); require(fd>=0 && !fchown(fd,32767,32767) && !close(fd),"replacement owner"); }
  if(i==6) require(!fchmod(c.rootfd,0755),"changed root mode fixture");
  p=run(&c); require(!p.auth.completed && !p.placement.launched,"root/context/identity rejection before writer");
  if(i==4) context.digest[0]^=1; cleanup(&c);
 }
 load(ROOT "cases/case-00.tar",1); c=root(count,PL_REAL); p=run(&c);
 require(p.auth.failure==AUTH_SIGNATURE && !p.placement.launched,"wrong signer no writer"); cleanup(&c);
 load(ROOT "cases/case-00.tar",0); signature_length--; c=root(count,PL_REAL); p=run(&c);
 require(p.auth.failure==AUTH_CONTEXT && !p.placement.launched,"incomplete signature no writer"); cleanup(&c);
 keys(); canary(); require(access(WORK "core",F_OK)==-1 && errno==ENOENT,"no core file");
 printf("Native placement checks passed: %d; peak controller %ld KiB; peak child %ld KiB.\n",tests,peak_controller,peak_child);
 return 0;
}
#endif
