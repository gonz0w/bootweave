/* SPDX-License-Identifier: BSD-3-Clause */
/* Original bounded console and pure binding checks; no dispatch capability. */
#include "review.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
static volatile sig_atomic_t stopped;
static void stop_handler(int n) { stopped=n; }
long long rv_now(void) {
 struct timespec t;
 if(clock_gettime(CLOCK_MONOTONIC,&t)) return -1;
 return (long long)t.tv_sec*1000+t.tv_nsec/1000000;
}
static int path_ok(const char *p) {
 size_t i,n=0,part=0,depth=0;
 for(i=0;i<256;i++) {
  unsigned char c=(unsigned char)p[i];
  if(!c || c=='/') {
   size_t len=i-part;
   if(!len || (len==1 && p[part]=='.') || (len==2 && p[part]=='.' && p[part+1]=='.') || ++depth>16) return 0;
   if(!c) return n!=0;
   part=i+1;
  } else if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='.'||c=='_'||c=='-')) return 0;
  n++;
 }
 return 0;
}
int rv_valid(const struct rv_binding *b) {
 size_t i,files=0,bytes=0;
 if(!b || !b->length || b->length>1048576 || !b->inode || b->owner!=32767 || b->group!=32767 ||
    b->mode!=0700 || !b->plan.count || b->plan.count>16 || !b->plan.files || b->plan.members>128) return 0;
 for(i=0;i<b->plan.count;i++) {
  const struct pl_node *n=&b->plan.nodes[i];
  if(!path_ok(n->path) || (n->directory!=0&&n->directory!=1) ||
     (n->explicit_member!=0&&n->explicit_member!=1) || n->parent < -1 || n->parent>=(int)i ||
     (n->parent>=0 && !b->plan.nodes[n->parent].directory) || n->size>262144 ||
     n->offset>b->length || n->size>b->length-n->offset || (n->directory&&n->size) ||
     n->size>524288-bytes || (i && strcmp(b->plan.nodes[i-1].path,n->path)>=0)) return 0;
  bytes+=n->size; if(!n->directory) files++;
 }
 return files==b->plan.files && bytes==b->plan.bytes;
}
int rv_same(const struct rv_binding *a,const struct rv_binding *b) {
 size_t i;
 if(!rv_valid(a)||!rv_valid(b)||a->length!=b->length||a->device!=b->device||a->inode!=b->inode||
    a->owner!=b->owner||a->group!=b->group||a->mode!=b->mode||
    a->plan.count!=b->plan.count||a->plan.members!=b->plan.members||a->plan.files!=b->plan.files||a->plan.bytes!=b->plan.bytes||
    memcmp(a->payload,b->payload,32)||memcmp(a->key,b->key,32)||memcmp(a->verifier,b->verifier,32)||memcmp(a->inspector,b->inspector,32)) return 0;
 for(i=0;i<a->plan.count;i++) {
  const struct pl_node *x=&a->plan.nodes[i],*y=&b->plan.nodes[i];
  if(strcmp(x->path,y->path)||x->directory!=y->directory||x->explicit_member!=y->explicit_member||
     x->parent!=y->parent||x->offset!=y->offset||x->size!=y->size) return 0;
 }
 return 1;
}
struct io { struct rv_console *c; size_t output,work; long long end,response; int skip_lf; };
static int wait_fd(struct io *s,int fd,short events,long long deadline) {
 struct pollfd p={fd,events,0};
 for(;;) {
  long long now=rv_now(),left; int n;
  if(stopped||now<0||now>=s->end||now>=deadline) return -1;
  left=deadline-now; if(left>s->end-now) left=s->end-now;
  p.revents=0; n=poll(&p,1,(int)left);
  if(n<0&&errno==EINTR) continue;
  if(n<=0||(p.revents&(POLLERR|POLLNVAL))) return -1;
  if(p.revents&(events|POLLHUP)) return 0;
 }
}
static int emit(struct io *s,const char *b,size_t n) {
 long long end=rv_now();
 if(end<0||n>16384-s->output) return -1;
 end+=5000; if(end>s->response) end=s->response;
 while(n) {
  ssize_t w;
  if(wait_fd(s,s->c->out,POLLOUT,end)) return -1;
  w=write(s->c->out,b,n);
  if(w<0&&(errno==EINTR||errno==EAGAIN)) continue;
  if(w<=0) return -1;
  b+=w; n-=(size_t)w; s->output+=(size_t)w;
 }
 return 0;
}
static int line(struct io *s,const char *p) {
 size_t n=strlen(p);
 while(n) { size_t chunk=n>40?40:n; if(emit(s,p,chunk)||emit(s,"\n",1)) return -1; p+=chunk;n-=chunk; }
 return 0;
}
static int quiet(struct io *s) {
 struct pollfd p={s->c->in,POLLIN,0}; long long end=rv_now();
 if(end<0) return -1;
 end+=50;
 for(;;) {
  long long now=rv_now(); unsigned char c; int n;
  if(stopped||now<0||now>=s->end) return -1;
  if(now>=end) return 0;
  p.revents=0; n=poll(&p,1,(int)(end-now));
  if(n<0&&errno==EINTR) continue;
  if(n<0||(p.revents&(POLLHUP|POLLERR|POLLNVAL))) return -1;
  if(!n) return 0;
  if(!s->skip_lf || read(s->c->in,&c,1)!=1 || c!='\n' || ++s->work>65536) return -1;
  s->skip_lf=0;
 }
}
int rv_quiet(struct rv_console *c) {
 struct io s; memset(&s,0,sizeof(s)); s.c=c; s.end=rv_now()+1000;
 return !c || c->in<3 ? -1:quiet(&s);
}
static int read_line(struct io *s,char b[1025]) {
 size_t n=0; int bad=0; long long end=rv_now();
 if(end<0) return -1;
 end+=10000;
 for(;;) {
  unsigned char c; ssize_t r;
  if(wait_fd(s,s->c->in,POLLIN,end)) return -1;
  r=read(s->c->in,&c,1);
  if(r<0&&(errno==EINTR||errno==EAGAIN)) continue;
  if(r!=1||++s->work>65536) return -1;
  if(s->skip_lf) { s->skip_lf=0; if(c=='\n') continue; }
  if(c=='\r'||c=='\n') { s->skip_lf=c=='\r';b[n]=0;s->response=rv_now()+5000;return bad?-1:0; }
  if(c<32||c>126) bad=1;
  if(n<1024) b[n++]=(char)c; else bad=1;
 }
}
static int term_same(const struct termios *a,const struct termios *b) {
 return a->c_iflag==b->c_iflag&&a->c_oflag==b->c_oflag&&a->c_cflag==b->c_cflag&&a->c_lflag==b->c_lflag&&
  !memcmp(a->c_cc,b->c_cc,sizeof(a->c_cc))&&cfgetispeed(a)==cfgetispeed(b)&&cfgetospeed(a)==cfgetospeed(b);
}
static int hex_line(struct io *s,const char *label,const unsigned char d[32]) {
 static const char hex[]="0123456789abcdef"; char b[65]; size_t i;
 for(i=0;i<32;i++) {b[i*2]=hex[d[i]>>4];b[i*2+1]=hex[d[i]&15];}b[64]=0;
 return line(s,label)||line(s,b)?-1:0;
}
static int review(struct io *s,const struct rv_binding *b) {
 char text[384]; size_t i; int n;
 if(line(s,"Bootweave file placement research")||line(s,"Only this fresh case root is writable.")||line(s,s->c->case_id)) return -1;
 n=snprintf(text,sizeof(text),"Root dev=%" PRIu64 " inode=%" PRIu64,b->device,b->inode);
 if(n<0||(size_t)n>=sizeof(text)||line(s,text)||line(s,"Directories: 32767/32767 mode 0700")||line(s,"Files: 32767/32767 mode 0600")||
    hex_line(s,"Payload SHA256:",b->payload)||hex_line(s,"Explicit key SHA256:",b->key)||
    hex_line(s,"Verifier SHA256:",b->verifier)||hex_line(s,"Inspector SHA256:",b->inspector)) return -1;
 n=snprintf(text,sizeof(text),"Planned=%zu files=%zu bytes=%zu input=%" PRIu64,b->plan.count,b->plan.files,b->plan.bytes,b->length);
 if(n<0||(size_t)n>=sizeof(text)||line(s,text)||line(s,"Limits: 16 objects, depth 16, path 255")||line(s,"Input 1 MiB; content 512 KiB total")||
    line(s,"Each file 256 KiB; writer 15 seconds")||line(s,"Controller 64 MiB; child 32 MiB")||line(s,"No overwrite, links or executable data")) return -1;
 for(i=0;i<b->plan.count;i++) {
  const struct pl_node *p=&b->plan.nodes[i];
  n=snprintf(text,sizeof(text),"%s %s bytes=%zu",p->directory?"Directory":"File",p->path,p->size);
  if(n<0||(size_t)n>=sizeof(text)||line(s,text)) return -1;
 }
 return line(s,"h help | b back | q cancel")||line(s,"Type review to confirm these effects.")?-1:0;
}
int rv_console(struct rv_console *c,const struct rv_binding *b,long long deadline) {
 struct io s; struct stat in_id,out_id,after; struct termios saved,t,restored; struct winsize size;
 struct sigaction action,old[6]; const int signals[]={SIGINT,SIGTERM,SIGHUP,SIGWINCH,SIGALRM,SIGPIPE};
 int handlers=0,term_saved=0,in_flags=-1,out_flags=-1,result=2,n,state=0,pending=0; size_t i; char input[1025],command[96];
 long long start=rv_now();
 memset(&s,0,sizeof(s));s.c=c;s.end=start+60000;s.response=start+5000;
 if(s.end>deadline) s.end=deadline;
 if(start<0||!c||!rv_valid(b)||c->in<3||c->out<3||deadline-start<15000||
    !memchr(c->case_id,0,sizeof(c->case_id))||!c->case_id[0]) return 3;
 for(i=0;c->case_id[i];i++) { unsigned char x=(unsigned char)c->case_id[i]; if(!((x>='a'&&x<='z')||(x>='0'&&x<='9')||x=='-')) return 3; }
 if(fstat(c->in,&in_id)||fstat(c->out,&out_id)||!S_ISCHR(in_id.st_mode)||!S_ISCHR(out_id.st_mode)||
    in_id.st_dev!=out_id.st_dev||in_id.st_ino!=out_id.st_ino||in_id.st_rdev!=out_id.st_rdev||!isatty(c->in)||!isatty(c->out)) return 3;
 in_flags=fcntl(c->in,F_GETFL);out_flags=fcntl(c->out,F_GETFL);
 if(in_flags<0||out_flags<0) return 3;
 stopped=0;memset(&action,0,sizeof(action));sigemptyset(&action.sa_mask);
 for(i=0;i<6;i++) {action.sa_handler=signals[i]==SIGPIPE?SIG_IGN:stop_handler;if(sigaction(signals[i],&action,&old[i])) {result=3;goto cleanup;}handlers++;}
 /* Do not flush: queued confirmation is a rejection, never hidden by setup. */
 if(tcgetattr(c->in,&saved)) {result=3;goto cleanup;}term_saved=1;t=saved;
 t.c_iflag=0;t.c_lflag&=~(ECHO|ECHONL|ICANON|IEXTEN|ISIG);t.c_cc[VMIN]=1;t.c_cc[VTIME]=0;
 if(tcsetattr(c->in,TCSANOW,&t)||fcntl(c->in,F_SETFL,in_flags|O_NONBLOCK)||fcntl(c->out,F_SETFL,out_flags|O_NONBLOCK)) {result=3;goto cleanup;}
 memset(&size,0,sizeof(size));
 if(ioctl(c->in,TIOCGWINSZ,&size)) {result=3;goto cleanup;}
 if((size.ws_col&&(size.ws_col<40||size.ws_col>240))||(size.ws_row&&(size.ws_row<12||size.ws_row>100))) goto cleanup;
 if((!size.ws_col||!size.ws_row)&&line(&s,"Dimensions unknown; fit unqualified.")) goto cleanup;
 for(;;) {
  if(quiet(&s)) break;
  if(!state) {
   if(c->revision==UINT64_MAX) break;
   c->revision++;
   if(review(&s,b)) break;
  } else {
   n=snprintf(command,sizeof(command),"place %s %" PRIu64,c->case_id,c->revision);
   if(n<0||(size_t)n>=sizeof(command)||line(&s,"Fresh one-use confirmation:")||line(&s,command)) break;
  }
  if(read_line(&s,input)||stopped) break;
  if(!strcmp(input,"q")) break;
  if(!strcmp(input,"h")||!strcmp(input,"b")) {
   state=0;if(line(&s,"Review invalidated; no file effects.")) break;continue;
  }
  if(!state&&!strcmp(input,"review")) {state=1;continue;}
  if(state&&!strcmp(input,command)&&!quiet(&s)&&rv_now()>=0&&deadline-rv_now()>=15000) result=0;
  break;
 }
cleanup:
 if(term_saved) {
  if(tcsetattr(c->in,TCSANOW,&saved)||tcgetattr(c->in,&restored)||
     ioctl(c->in,FIONREAD,&pending)||
     tcsetattr(c->in,TCSANOW,&saved)||tcgetattr(c->in,&restored)||!term_same(&saved,&restored)) result=3;
  if(!result&&pending) result=2;
  /* Only cancellation discards queued bytes after the decision has been refused. */
  if(result && tcflush(c->in,TCIFLUSH)) result=3;
 }
 if(in_flags>=0&&fcntl(c->in,F_SETFL,in_flags)) result=3;
 if(out_flags>=0&&fcntl(c->out,F_SETFL,out_flags)) result=3;
 if(fcntl(c->in,F_GETFL)!=in_flags||fcntl(c->out,F_GETFL)!=out_flags) result=3;
 if(fstat(c->in,&after)||after.st_dev!=in_id.st_dev||after.st_ino!=in_id.st_ino||after.st_rdev!=in_id.st_rdev) result=3;
 if(!result&&(stopped||quiet(&s))) result=2;
 for(i=(size_t)handlers;i>0;i--) if(sigaction(signals[i-1],&old[i-1],NULL)) result=3;
 return result;
}
