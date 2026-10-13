/* SPDX-License-Identifier: BSD-3-Clause */
/* Original fixed composition phase. No selectable operation or executable. */
#include "auth_native.h"
#include "review.h"
#ifdef __OpenBSD__
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sha2.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifndef AUTH_INSPECTOR
#define AUTH_INSPECTOR "/root/payload-auth/inspect"
#endif
#ifdef AUTH_TEST
extern void rv_test_hook(int,struct auth_context *,unsigned char *,size_t);
#define HOOK(n,c,b,l) rv_test_hook(n,(struct auth_context *)(c),b,l)
#else
#define HOOK(n,c,b,l) ((void)0)
#endif
static void hash(const unsigned char *b,size_t n,unsigned char d[32]) {
 SHA2_CTX c;SHA256Init(&c);SHA256Update(&c,b,n);SHA256Final(d,&c);
}
static int same_root(const struct stat *a,const struct stat *b) {
 return S_ISDIR(a->st_mode)&&S_ISDIR(b->st_mode)&&a->st_dev==b->st_dev&&a->st_ino==b->st_ino&&
  a->st_uid==b->st_uid&&a->st_gid==b->st_gid&&a->st_mode==b->st_mode&&a->st_nlink==b->st_nlink;
}
static int empty_root(const struct pl_context *p) {
 struct stat held,named,parent;DIR *d;struct dirent *e;int fd,ok=1;
 if(!p||p->rootfd<3||p->parentfd<3||fstat(p->rootfd,&held)||fstat(p->parentfd,&parent)||
    !S_ISDIR(parent.st_mode)||parent.st_uid!=0||(parent.st_mode&07777)!=0700||
    fstatat(p->parentfd,"case",&named,AT_SYMLINK_NOFOLLOW)||!same_root(&held,&p->identity)||!same_root(&named,&p->identity)||
    held.st_uid!=32767||held.st_gid!=32767||(held.st_mode&07777)!=0700) return -1;
 fd=openat(p->rootfd,".",O_RDONLY|O_DIRECTORY|O_NOFOLLOW);if(fd<0) return -1;
 d=fdopendir(fd);if(!d) {close(fd);return -1;}
 for(;;) {errno=0;e=readdir(d);if(!e) {if(errno) ok=0;break;}
  if(strcmp(e->d_name,".")&&strcmp(e->d_name,"..")) {ok=0;break;}}
 if(closedir(d)) ok=0;
 return ok?0:-1;
}
static int binding(struct rv_binding *v,const struct auth_context *c,const unsigned char *b,size_t n) {
 struct stat s;size_t i;
 memset(v,0,sizeof(*v));
 if(!c||!c->placement||c->placement->test<PL_REAL||c->placement->test>PL_CONTROLLER_SIGNAL||n!=c->length||n>AUTH_INPUT||c->key_length>AUTH_RECORD||empty_root(c->placement)||
    fstat(c->placement->rootfd,&s)||pl_plan(b,n,&v->plan)||v->plan.count!=c->placement->count||!c->placement->expected) return -1;
 v->length=n;v->device=(uint64_t)s.st_dev;v->inode=(uint64_t)s.st_ino;
 v->owner=s.st_uid;v->group=s.st_gid;v->mode=s.st_mode&07777;
 hash(b,n,v->payload);hash(c->key,c->key_length,v->key);
 if(memcmp(v->payload,c->digest,32)||auth_hash_file("/usr/bin/signify",v->verifier)||
    auth_hash_file(AUTH_INSPECTOR,v->inspector)||memcmp(v->verifier,c->verifier_digest,32)||
    memcmp(v->inspector,c->inspector_digest,32)||!rv_valid(v)) return -1;
 for(i=0;i<v->plan.count;i++) {
  const struct pl_node *a=&v->plan.nodes[i];size_t j;unsigned char d[32];
  for(j=0;j<c->placement->count;j++) {
   const struct pl_expected *e=&c->placement->expected[j];
   if(!e->path||!memchr(e->path,0,256)) return -1;
   if(!strcmp(e->path,a->path)) break;
  }
  if(j==c->placement->count||a->directory!=c->placement->expected[j].directory||a->size!=c->placement->expected[j].size) return -1;
  if(!a->directory) {hash(b+a->offset,a->size,d);if(memcmp(d,c->placement->expected[j].digest,32)) return -1;}
 }
 return 0;
}
int rv_phase(struct rv_console *console,const struct auth_context *c,unsigned char *b,size_t n,long long end) {
 struct rv_binding frozen,current;struct auth_context ac;struct pl_context pc;
 struct pl_expected expected[16];char paths[16][256],case_id[32];struct stat tty_in,tty_out,s;
 int in,out;size_t i;
 if(!console||!c||!c->placement||binding(&frozen,c,b,n)||console->in<3||console->out<3||
    fstat(console->in,&tty_in)||fstat(console->out,&tty_out)||
    !memchr(console->case_id,0,32)) return -1;
 memset(&c->placement->result,0,sizeof(c->placement->result));
 c->placement->result.planned=frozen.plan.count;
 ac=*c;pc=*c->placement;in=console->in;out=console->out;memcpy(case_id,console->case_id,32);
 for(i=0;i<pc.count;i++) {expected[i]=pc.expected[i];strcpy(paths[i],expected[i].path);expected[i].path=paths[i];}
 HOOK(0,c,b,n);
 if(!auth_live(end)||rv_console(console,&frozen,end)) return -1;
 HOOK(1,c,b,n);
 if(c->placement!=ac.placement||c->length!=ac.length||c->key_length!=ac.key_length||
    memcmp(c->key,ac.key,ac.key_length)||memcmp(c->digest,ac.digest,32)||
    memcmp(c->verifier_digest,ac.verifier_digest,32)||memcmp(c->inspector_digest,ac.inspector_digest,32)||
    c->placement->rootfd!=pc.rootfd||c->placement->parentfd!=pc.parentfd||c->placement->count!=pc.count||
    c->placement->expected!=pc.expected||c->placement->test!=pc.test||!same_root(&pc.identity,&c->placement->identity)||
    console->in!=in||console->out!=out||memcmp(console->case_id,case_id,32)||
    fstat(in,&s)||s.st_dev!=tty_in.st_dev||s.st_ino!=tty_in.st_ino||s.st_rdev!=tty_in.st_rdev||
    fstat(out,&s)||s.st_dev!=tty_out.st_dev||s.st_ino!=tty_out.st_ino||s.st_rdev!=tty_out.st_rdev) return -1;
 for(i=0;i<pc.count;i++) {
  const struct pl_expected *e=&pc.expected[i];
  if(!e->path||!memchr(e->path,0,256)||strcmp(e->path,expected[i].path)||e->directory!=expected[i].directory||
     e->size!=expected[i].size||memcmp(e->digest,expected[i].digest,32)) return -1;
 }
 if(binding(&current,c,b,n)||!rv_same(&frozen,&current)||!auth_live(end)||rv_quiet(console)||end-auth_now()<15000) return -1;
 return 0;
}
int rv_report(struct rv_console *c,const struct pl_result *p,const struct auth_result *r) {
 char text[256];size_t at=0;int n,flags,result=0;long long end;struct pollfd out;
 if(!c||!p||!r||c->out<3) return -1;
 n=snprintf(text,sizeof(text),"Planned=%zu submitted=%zu observed=%zu\nComplete=%d uncertain=%d failure=%d\n",
  p->planned,p->submitted,p->observed,r->completed,p->uncertain,(int)r->failure);
 if(n<0||(size_t)n>=sizeof(text)) return -1;
 flags=fcntl(c->out,F_GETFL);end=auth_now()+5000;out.fd=c->out;out.events=POLLOUT;
 if(flags<0||fcntl(c->out,F_SETFL,flags|O_NONBLOCK)) return -1;
 while(at<(size_t)n) {
  long long now=auth_now();ssize_t w;int polled;
  if(now<0||now>=end) {result=-1;break;}
  out.revents=0;polled=poll(&out,1,(int)(end-now));
  if(polled<0&&errno==EINTR) continue;
  if(polled<=0||(out.revents&(POLLERR|POLLHUP|POLLNVAL))) {result=-1;break;}
  w=write(c->out,text+at,(size_t)n-at);
  if(w<0&&(errno==EINTR||errno==EAGAIN)) continue;
  if(w<=0) {result=-1;break;}at+=(size_t)w;
 }
 if(fcntl(c->out,F_SETFL,flags)||fcntl(c->out,F_GETFL)!=flags) result=-1;
 return result;
}
#endif
