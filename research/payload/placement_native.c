/* SPDX-License-Identifier: BSD-3-Clause */
/* Original fixed OpenBSD writer/supervisor; no archive-selected command. */
#include "placement.h"
#include "auth_native.h"
#ifndef __OpenBSD__
/* Deliberately no host writer. */
#else
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sha2.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifndef PL_WORK
#define PL_WORK "/root/payload-placement/work/"
#endif
#define WORK PL_WORK
static int identity(const struct stat *a, const struct stat *b, int dir) {
 return a->st_dev == b->st_dev && a->st_ino == b->st_ino &&
     (dir ? S_ISDIR(b->st_mode) : S_ISREG(b->st_mode)) &&
     b->st_uid == 32767 && b->st_gid == 32767 &&
     (b->st_mode & 07777) == (dir ? 0700 : 0600) && (dir || b->st_nlink == 1);
}
static int empty(int fd) {
 int copy = openat(fd, ".", O_RDONLY | O_DIRECTORY | O_NOFOLLOW), ok = 1;
 DIR *d; struct dirent *e;
 if (copy < 0) return 0;
 d = fdopendir(copy); if (!d) { close(copy); return 0; }
 for (;;) {
  errno = 0; e = readdir(d); if (!e) { if (errno) ok = 0; break; }
  if (strcmp(e->d_name, ".") && strcmp(e->d_name, "..")) ok = 0;
 }
 if (closedir(d)) ok = 0; return ok;
}
int pl_limits(int controller) {
 const int resources[] = {RLIMIT_CORE, RLIMIT_CPU, RLIMIT_FSIZE, RLIMIT_DATA, RLIMIT_RSS, RLIMIT_NOFILE};
 rlim_t values[] = {0, 15, 1048576, (controller ? 64U : 32U) * 1048576, (controller ? 64U : 32U) * 1048576, 160};
 size_t i; for (i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
  struct rlimit r = {values[i], values[i]}; if (setrlimit(resources[i], &r)) return -1;
 }
#ifdef AUTH_INSTALLED
 if(controller) {struct rusage u;if(getrusage(RUSAGE_SELF,&u)||u.ru_maxrss<=0||u.ru_maxrss>65536) return -1;
  printf("Installed supervisor RSS=%ld KiB.\n",u.ru_maxrss);}
#endif
 return 0;
}
static int setup(int fd, const struct stat *id) {
 struct stat st; uid_t u[3]; gid_t g[3]; struct sigaction sa;
 memset(&sa, 0, sizeof(sa)); sa.sa_handler = SIG_DFL; sigemptyset(&sa.sa_mask);
 if (sigaction(SIGINT, &sa, NULL) || sigaction(SIGTERM, &sa, NULL) ||
     sigaction(SIGHUP, &sa, NULL) || sigaction(SIGPIPE, &sa, NULL) || pl_limits(0)) return -1;
 umask(077);
 if (fstat(fd, &st) || !identity(id, &st, 1) || fchdir(fd) ||
     unveil(".", "rwc") || unveil(NULL, NULL) || setgroups(0, NULL) ||
     setresgid(32767,32767,32767) || setresuid(32767,32767,32767) ||
     getresuid(&u[0],&u[1],&u[2]) || getresgid(&g[0],&g[1],&g[2]) ||
     u[0] != 32767 || u[1] != 32767 || u[2] != 32767 ||
     g[0] != 32767 || g[1] != 32767 || g[2] != 32767 || getgroups(0,NULL) != 0) return -1;
 /* Observe irreversible identity drop separately from pledge's syscall denial. */
 errno = 0; if (setresuid(0,0,0) != -1 || errno != EPERM) return -1;
 errno = 0; if (setresgid(0,0,0) != -1 || errno != EPERM) return -1;
 /* Check the unveil lock before pledge makes the syscall itself forbidden. */
 errno = 0; if (unveil("/", "rwc") != -1 || errno != EPERM) return -1;
 return pledge("stdio rpath wpath cpath fattr", NULL);
}
static int reap(pid_t pid, int *status, long long deadline, long *peak, int *killed) {
 struct rusage u; pid_t w; *killed = 0;
 for (;;) {
  w = wait4(pid, status, WNOHANG, &u);
  if (w == pid) break;
  if (w < 0 && errno != EINTR) return -1;
  if (auth_now() < 0 || auth_now() >= deadline) {
   *killed = 1; (void)kill(pid, SIGKILL);
   do { w = wait4(pid, status, 0, &u); } while (w < 0 && errno == EINTR);
   if (w != pid) return -1; break;
  }
  (void)poll(NULL,0,10);
 }
 *peak = u.ru_maxrss;
 return *peak > 0 && *peak <= 32768 ? 0 : -1;
}
int pl_prerequisites(int fd, const struct stat *id, long *peak) {
 int probe; unsigned char expected[32], actual[32];
 /* Only the pinned stock benign true binary may be named by the exec denial probe. */
 if (auth_hash_file("/usr/bin/true", expected)) return -1;
 for (probe = 0; probe < 8; probe++) {
  int p[2], status=0, killed, ok; pid_t pid; long rss=0; unsigned char marker = 0;
  if (pipe(p)) return -1;
  pid = fork(); if (pid < 0) { close(p[0]); close(p[1]); return -1; }
  if (!pid) {
   int root = fcntl(fd,F_DUPFD,16), out = fcntl(p[1],F_DUPFD,16), rc = -1;
   if (root < 0 || out < 0 || dup2(root,3) < 0 || dup2(out,4) < 0 ||
       dup2(out,1) < 0 || dup2(out,2) < 0) _exit(125);
   close(0); closefrom(5);
   if (setup(3,id) || write(4,"P",1) != 1) _exit(125);
   errno = 0;
   if (probe == 0) rc = open(WORK "canary",O_RDONLY|O_NOFOLLOW);
   if (probe == 1) rc = openat(3,"../canary",O_RDONLY|O_NOFOLLOW);
   if (probe == 2) rc = openat(20,"canary",O_RDONLY|O_NOFOLLOW);
   if (probe == 3) rc = unveil("/", "rwc");
   if (probe == 4) { char *const a[] = {"/usr/bin/true",NULL}; char *const e[] = {NULL}; execve(a[0],a,e); rc = -1; }
   if (probe == 5) rc = socket(AF_INET,SOCK_STREAM,0);
   if (probe == 6) rc = mknodat(3,"forbidden-device",S_IFCHR|0600,0);
   if (probe == 7) rc = setresuid(0,0,0);
   /* Stock true exits zero if unexpectedly executed; a denial has a distinct code. */
   _exit(rc == -1 && errno != 0 ? 73 : 1);
  }
  close(p[1]);
  ok = !reap(pid,&status,auth_now()+15000,&rss,&killed) && !killed;
  if (rss > *peak) *peak = rss;
  if (read(p[0],&marker,1) != 1 || marker != 'P') ok = 0;
  { unsigned char b[4097]; ssize_t n = read(p[0],b,sizeof(b)); if (n != 0) ok = 0; }
  if (close(p[0])) ok = 0;
  if (!(WIFEXITED(status) && WEXITSTATUS(status) == 73) &&
      !(probe >= 3 && WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT)) ok = 0;
  if (!ok || !empty(fd)) return -1;
 }
 if (auth_hash_file("/usr/bin/true",actual) || memcmp(expected,actual,32)) return -1;
 return 0;
}
static int bound_root(struct pl_context *c) {
 struct stat a,b;
 return fstat(c->rootfd,&a) || fstatat(c->parentfd,"case",&b,AT_SYMLINK_NOFOLLOW) ||
     !identity(&c->identity,&a,1) || !identity(&c->identity,&b,1) ? -1 : 0;
}
static const char *leaf(const char *path) { const char *s = strrchr(path,'/'); return s ? s+1 : path; }
static int held(int root, const struct stat *id, const struct pl_plan *p,
 int fds[PL_NODES], struct stat ids[PL_NODES], size_t made) {
 struct stat st; size_t i;
 if (fstat(root,&st) || !identity(id,&st,1)) return -1;
 for (i=0;i<made;i++) {
  int parent = p->nodes[i].parent < 0 ? root : fds[p->nodes[i].parent];
  if (fstatat(parent,leaf(p->nodes[i].path),&st,AT_SYMLINK_NOFOLLOW) ||
      !identity(&ids[i],&st,p->nodes[i].directory)) return -1;
  if (fds[i]>=0 && (fstat(fds[i],&st) || !identity(&ids[i],&st,1))) return -1;
 }
 return 0;
}
struct report { size_t submitted; int ok; };
static int barrier(int stage) {
 unsigned char b=(unsigned char)stage, ack=0;
 return write(5,&b,1)!=1 || read(6,&ack,1)!=1 || ack!='A' ? -1 : 0;
}
static void writer(const struct pl_context *c, const struct pl_plan *p, const unsigned char *b, long long end) {
 int fds[PL_NODES], failure=0; struct stat ids[PL_NODES]; struct report r={0}; size_t i;
 for(i=0;i<PL_NODES;i++) fds[i]=-1;
 if (setup(3,&c->identity) || c->test==PL_SETUP || barrier(0)) _exit(125);
 if(c->test==PL_STALL) for(;;) pause();
 if(c->test==PL_LOSS) _exit(126);
 if(c->test==PL_SIGNAL) { raise(SIGTERM); _exit(126); }
 if(c->test==PL_OUTPUT) { unsigned char out[4097]; memset(out,'X',sizeof(out)); (void)write(1,out,sizeof(out)); _exit(126); }
 if(c->test==PL_DIAGNOSTIC) { (void)write(2,"\033[2Jsynthetic-private\n",23); _exit(126); }
 for(i=0;i<p->count;i++) {
  const struct pl_node *n=&p->nodes[i]; int parent=n->parent<0?3:fds[n->parent], fd=-1;
  struct stat st; size_t at=0;
  if(!auth_live(end) || held(3,&c->identity,p,fds,ids,i)) { failure=1; break; }
  if(n->directory) {
   r.submitted++;
   if(mkdirat(parent,leaf(n->path),0700)) { failure=1; break; }
   fd=openat(parent,leaf(n->path),O_RDONLY|O_DIRECTORY|O_NOFOLLOW);
   if(fd<0 || fstat(fd,&ids[i]) || !identity(&ids[i],&ids[i],1) || ids[i].st_dev!=c->identity.st_dev) failure=1;
   fds[i]=fd;
   if(!failure && i==0 && barrier(1)) failure=1;
   /* The PL_SYNC injection targets the data file, after its bytes exist. */
   if(!failure && fsync(fd)) failure=1;
  } else {
   r.submitted++;
   fd=openat(parent,leaf(n->path),O_RDWR|O_CREAT|O_EXCL|O_NOFOLLOW,0600);
   if(fd<0) { failure=1; break; }
   if(fstat(fd,&ids[i]) || !identity(&ids[i],&ids[i],0) || ids[i].st_dev!=c->identity.st_dev) failure=1;
   if(!failure && c->test==PL_FILE_SWAP && (barrier(2) || held(3,&c->identity,p,fds,ids,i+1))) failure=1;
   while(!failure && at<n->size) {
    size_t amount=n->size-at; ssize_t w;
    if(!auth_live(end)) { failure=1; break; }
    if(amount>4096) amount=4096;
    if(c->test==PL_SHORT && at>=1) { failure=1; break; }
    if(c->test==PL_SHORT) amount=1;
    w=write(fd,b+n->offset+at,amount);
    if(w>0) at+=(size_t)w; else if(w<0 && errno==EINTR) continue; else failure=1;
   }
   if(!failure && (c->test==PL_SYNC || fsync(fd))) failure=1;
   if(!failure && (fstat(fd,&st) || !identity(&ids[i],&st,0) || st.st_size!=(off_t)n->size)) failure=1;
   if(!failure) {
    unsigned char data[4096]; size_t read_at=0; ssize_t nr;
    if(lseek(fd,0,SEEK_SET)<0) failure=1;
    while(!failure && read_at<n->size) {
     size_t amount=n->size-read_at; if(amount>sizeof(data)) amount=sizeof(data);
     nr=read(fd,data,amount);
     if(nr<=0 || memcmp(data,b+n->offset+read_at,(size_t)nr)) failure=1; else read_at+=(size_t)nr;
    }
   }
   if(close(fd) || c->test==PL_CLOSE) failure=1;
  }
  if(failure) break;
 }
 if(!failure && held(3,&c->identity,p,fds,ids,p->count)) failure=1;
 for(i=p->count;i>0;i--) if(!failure && fds[i-1]>=0 && fsync(fds[i-1])) failure=1;
 if(!failure && fsync(3)) failure=1;
 for(i=0;i<p->count;i++) if(fds[i]>=0 && close(fds[i])) failure=1;
 r.ok=!failure;
 if(write(4,&r,sizeof(r))!=(ssize_t)sizeof(r)) _exit(125);
 _exit(failure?1:0);
}
/* Read-only oracle uses independent expected paths/hashes; no planner offsets. */
static int oracle(int fd, const char *prefix, const struct pl_context *c, size_t *count, int *exact) {
 int copy=openat(fd,".",O_RDONLY|O_DIRECTORY|O_NOFOLLOW), ok=1;
 DIR *d; struct dirent *e;
 if(copy<0) return -1;
 d=fdopendir(copy); if(!d) { close(copy); return -1; }
 for(;;) {
  char path[256]; size_t i; struct stat st; int child;
  errno=0; e=readdir(d); if(!e) { if(errno) ok=0; break; }
  if(!strcmp(e->d_name,".")||!strcmp(e->d_name,"..")) continue;
  if(++*count>128 || snprintf(path,sizeof(path),"%s%s%s",prefix,*prefix?"/":"",e->d_name)>255) { ok=0; break; }
  for(i=0;i<c->count;i++) if(!strcmp(path,c->expected[i].path)) break;
  if(i==c->count || fstatat(fd,e->d_name,&st,AT_SYMLINK_NOFOLLOW)) { ok=0; break; }
  if(!identity(&st,&st,c->expected[i].directory) || st.st_dev!=c->identity.st_dev) { ok=0; break; }
  child=openat(fd,e->d_name,O_RDONLY|O_NOFOLLOW|(c->expected[i].directory?O_DIRECTORY:0));
  if(child<0) { ok=0; break; }
  { struct stat held_st; if(fstat(child,&held_st) || !identity(&st,&held_st,c->expected[i].directory)) ok=0; }
  if(c->expected[i].directory) { if(oracle(child,path,c,count,exact)) ok=0; }
  else {
   unsigned char bytes[4096],digest[32]; SHA2_CTX hash; size_t used=0; ssize_t n;
   SHA256Init(&hash);
   while((n=read(child,bytes,sizeof(bytes)))!=0) {
    if(n<0 || (size_t)n>262144-used) { ok=0; break; }
    used+=(size_t)n; SHA256Update(&hash,bytes,(size_t)n);
   }
   SHA256Final(digest,&hash);
   if(used!=c->expected[i].size || memcmp(digest,c->expected[i].digest,32)) *exact=0;
  }
  if(close(child)) ok=0;
  if(!ok) break;
 }
 if(closedir(d)) ok=0; return ok?0:-1;
}
static int mutation(struct pl_context *c, int stage) {
 int fd;
 if(stage==0) {
  if(c->test==PL_LINK && symlinkat(WORK "canary",c->rootfd,"viewer")) return -1;
  if(c->test==PL_RENAME_ROOT && renameat(c->parentfd,"case",c->parentfd,"moved")) return -1;
 } else if(stage==1 && c->test==PL_EXIST) {
  int dir=openat(c->rootfd,"viewer",O_RDONLY|O_DIRECTORY|O_NOFOLLOW);
  if(dir<0) return -1;
  fd=openat(dir,"readme.txt",O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW,0600);
  if(fd<0 || fchown(fd,32767,32767)) { if(fd>=0) close(fd); close(dir); return -1; }
  { int failed=close(fd); if(close(dir)) failed=1; if(failed) return -1; }
 } else if(stage==1 && (c->test==PL_REPLACE_DIR || c->test==PL_AFTER_HOLD)) {
  if(renameat(c->rootfd,"viewer",c->rootfd,"moved")) return -1;
  if(c->test==PL_REPLACE_DIR) {
   fd=openat(c->rootfd,"viewer",O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW,0600);
   if(fd<0 || fchown(fd,32767,32767)) return -1;
   if(close(fd)) return -1;
  } else if(symlinkat(WORK,c->rootfd,"viewer")) return -1;
 } else if(stage==2 && c->test==PL_FILE_SWAP) {
  int dir=openat(c->rootfd,"viewer",O_RDONLY|O_DIRECTORY|O_NOFOLLOW);
  if(dir<0) return -1;
  if(renameat(dir,"readme.txt",dir,"moved") ||
      symlinkat(WORK "canary",dir,"readme.txt")) { close(dir); return -1; }
  if(close(dir)) return -1;
 }
 return 0;
}
int pl_phase(struct pl_context *c, const unsigned char *b, size_t length,
 const char *summary, long long deadline, struct auth_result *metrics) {
 struct pl_plan plan; struct report packet={0}; struct pl_result *r;
 int report[2]={-1,-1}, logs[2]={-1,-1}, request[2]={-1,-1}, ack[2]={-1,-1};
 int status=0, done=0, failure=1, exact=1, mutated=0, killed=0; pid_t pid=-1,w;
 size_t got=0,total=0,i; long rss=0; struct rusage usage; long long end=auth_now()+15000;
 unsigned long members,files,bytes;
 if(!c) return -1;
 r=&c->result; memset(r,0,sizeof(*r));
 if(end>deadline) end=deadline;
 if(pl_plan(b,length,&plan) ||
     sscanf(summary,"Inspection passed: %lu members, %lu files, %lu content bytes.",&members,&files,&bytes)!=3 ||
     members!=plan.members || files!=plan.files || bytes!=plan.bytes || c->count!=plan.count ||
     bound_root(c) || !empty(c->rootfd)) return -1;
 for(i=0;i<plan.count;i++) {
  size_t j;
  for(j=0;j<c->count;j++) if(!strcmp(plan.nodes[i].path,c->expected[j].path)) break;
  if(j==c->count || plan.nodes[i].directory!=c->expected[j].directory || plan.nodes[i].size!=c->expected[j].size) return -1;
  if(!plan.nodes[i].directory) {
   unsigned char digest[32]; SHA2_CTX hash;
   SHA256Init(&hash); SHA256Update(&hash,b+plan.nodes[i].offset,plan.nodes[i].size); SHA256Final(digest,&hash);
   if(memcmp(digest,c->expected[j].digest,32)) return -1;
  }
 }
 r->planned=plan.count;
 if(pipe(report)||pipe(logs)||pipe(request)||pipe(ack)) goto cleanup;
 pid=fork(); if(pid<0) goto cleanup;
 if(!pid) {
  /* Preserve capabilities at fixed descriptors, then close all unrelated FDs. */
  int saved[5], originals[]={c->rootfd,report[1],request[1],ack[0],logs[1]};
  for(i=0;i<5;i++) { saved[i]=fcntl(originals[i],F_DUPFD,16); if(saved[i]<0) _exit(125); }
  if(dup2(saved[0],3)<0 || dup2(saved[1],4)<0 || dup2(saved[2],5)<0 || dup2(saved[3],6)<0 ||
      dup2(saved[4],1)<0 || dup2(saved[4],2)<0) _exit(125);
  close(0); closefrom(7); writer(c,&plan,b,end); _exit(125);
 }
 r->launched=1;
 close(report[1]); report[1]=-1; close(logs[1]); logs[1]=-1;
 close(request[1]); request[1]=-1; close(ack[0]); ack[0]=-1;
 { int fds[]={report[0],logs[0],request[0]};
  for(i=0;i<3;i++) { int flags=fcntl(fds[i],F_GETFL); if(flags<0 || fcntl(fds[i],F_SETFL,flags|O_NONBLOCK)) goto cleanup; }
 }
 while(!done) {
  unsigned char log[512],stage; ssize_t n; struct pollfd f[3];
  if(!auth_live(end)) goto cleanup;
  n=read(request[0],&stage,1);
  if(n==1) {
   if(stage>2 || mutation(c,stage) || bound_root(c)) goto cleanup;
   if(stage==0 && c->test==PL_CONTROLLER_SIGNAL) (void)raise(SIGINT);
   if(!auth_live(end)) goto cleanup;
   if(c->test>=PL_EXIST) mutated=1;
   if(write(ack[1],"A",1)!=1) goto cleanup;
  } else if(n<0 && errno!=EAGAIN && errno!=EINTR) goto cleanup;
  n=read(logs[0],log,sizeof(log));
  if(n>0) { total+=(size_t)n; if(total>4096-sizeof(packet)) goto cleanup; }
  else if(n<0 && errno!=EAGAIN && errno!=EINTR) goto cleanup;
  if(got<sizeof(packet)) {
   n=read(report[0],(unsigned char *)&packet+got,sizeof(packet)-got);
   if(n>0) got+=(size_t)n; else if(n<0 && errno!=EAGAIN && errno!=EINTR) goto cleanup;
  }
  w=wait4(pid,&status,WNOHANG,&usage);
  if(w==pid) { done=1; rss=usage.ru_maxrss; r->reaped=1; }
  else if(w<0 && errno!=EINTR) goto cleanup;
  for(i=0;i<3;i++) { f[i].fd=i==0?report[0]:i==1?logs[0]:request[0]; f[i].events=POLLIN; f[i].revents=0; }
  if(!done && poll(f,3,10)<0 && errno!=EINTR) goto cleanup;
 }
 /* Drain all fixed result/diagnostics after reap; reject extra/partial output. */
 if(got<sizeof(packet)) {
  ssize_t n=read(report[0],(unsigned char *)&packet+got,sizeof(packet)-got);
  if(n>0) got+=(size_t)n;
 }
 { unsigned char extra[4097]; ssize_t n;
  while((n=read(logs[0],extra,sizeof(extra)))>0) { total+=(size_t)n; if(total>4096-sizeof(packet)) break; }
  n=read(report[0],extra,sizeof(extra)); if(n!=0) goto cleanup;
 }
 if(got==sizeof(packet) && packet.submitted<=plan.count) r->submitted=packet.submitted;
 if(got==sizeof(packet) && packet.ok==1 && WIFEXITED(status) && WEXITSTATUS(status)==0 && !total) failure=0;
cleanup:
 if(pid>0 && !done) {
  (void)kill(pid,SIGKILL);
  if(reap(pid,&status,auth_now()+1000,&rss,&killed)) failure=1;
  else { done=1; r->reaped=1; }
 }
 r->peak_kib=rss;
 if(r->launched && got!=sizeof(packet)) r->uncertain=1;
 if(pid>0 && (rss<=0 || rss>32768 || !done)) failure=1;
 if(rss>metrics->child_peak_kib) metrics->child_peak_kib=rss;
 if(r->reaped) metrics->reaped++;
 { int *pipes[]={report,logs,request,ack}; size_t j;
  for(i=0;i<4;i++) for(j=0;j<2;j++) if(pipes[i][j]>=0 && close(pipes[i][j])) failure=1;
 }
 if(bound_root(c) || oracle(c->rootfd,"",c,&r->observed,&exact)) { r->uncertain=1; failure=1; }
 if(r->observed!=plan.count || !exact || mutated) failure=1;
 r->complete=!failure; return failure?-1:0;
}
#ifdef AUTH_REVIEW
/* A refused review may still observe supervisor-induced root changes. */
void pl_observe(struct pl_context *c) {
 int exact=1;
 c->result.observed=0;
 if(c->count>128 || !c->expected || bound_root(c) ||
    oracle(c->rootfd,"",c,&c->result.observed,&exact)) c->result.uncertain=1;
}
#endif
#endif
