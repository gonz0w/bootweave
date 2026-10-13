/* SPDX-License-Identifier: BSD-3-Clause */
/* Fixed composition supervisor. Reuse original placement fixture/oracle helpers. */
#define main placement_component_main
#include "placement_check.c"
#undef main
#include "review.h"
#ifndef __OpenBSD__
int main(int argc,char **argv) {(void)argv;fputs("Native review checks require OpenBSD.\n",stderr);return argc==1?77:2;}
#else
#include <util.h>
#include <sys/ioctl.h>
static int change_mode;
void rv_test_hook(int phase,struct auth_context *c,unsigned char *b,size_t n) {
 (void)n;
 if(phase!=1) return;
 switch(change_mode) {
 case 1:b[1024]^=1;break;
 case 2:c->key[0]^=1;break;
 case 3:c->verifier_digest[0]^=1;break;
 case 4:c->inspector_digest[0]^=1;break;
 case 5:c->length--;break;
 case 6:c->placement->identity.st_ino++;break;
 case 7:if(fchmod(c->placement->rootfd,0755)) hook_error=1;break;
 case 8:if(renameat(c->placement->parentfd,"case",c->placement->parentfd,"moved")) hook_error=1;break;
 case 9:{int fd=openat(c->placement->rootfd,"unexpected",O_CREAT|O_EXCL|O_WRONLY,0600);if(fd<0||close(fd)) hook_error=1;break;}
 case 10:expected[0].size++;break;
 case 11:c->review->case_id[0]='x';break;
 default:break;
 }
}
enum action { CONFIRM,CANCEL,HELP,BACK,STALE,PREQUEUE,CONTROL,OVERFLOW,BLANK,RESIZE,INTERRUPT,IDLE,LOST,DOUBLE,REVISION,NARROW,UNKNOWN,SMALL };
#ifndef REVIEW_MANUAL
static void killed_console(void) {
 struct rv_binding b;struct rv_console c;struct termios t,a;struct rusage u;int m=-1,s=-1,status,f;char text[2048];size_t n=0;pid_t pid;long long end=auth_now()+20000;
 memset(&b,0,sizeof(b));memset(&c,0,sizeof(c));b.length=input_length;b.inode=1;b.owner=b.group=32767;b.mode=0700;
 require(!pl_plan(input,input_length,&b.plan)&&!openpty(&m,&s,NULL,NULL,NULL)&&!tcgetattr(s,&t),"death console setup");
 f=fcntl(s,F_GETFL);c.in=c.out=s;strcpy(c.case_id,"death-case");pid=fork();require(pid>=0,"death console fork");
 if(!pid) {close(m);_exit(rv_console(&c,&b,end));}
 while(n<sizeof(text)-1) {struct pollfd p={m,POLLIN,0};ssize_t r;require(auth_now()<end&&poll(&p,1,100)>=0,"death console deadline");
  if(!(p.revents&POLLIN)) continue;r=read(m,text+n,sizeof(text)-1-n);require(r>0,"death console capture");n+=(size_t)r;text[n]=0;if(strstr(text,"Type review")) break;}
 require(!kill(pid,SIGKILL)&&wait4(pid,&status,0,&u)==pid&&WIFSIGNALED(status)&&WTERMSIG(status)==SIGKILL&&u.ru_maxrss>0&&u.ru_maxrss<=32768,"death console usage/reap");
 require(!tcsetattr(s,TCSANOW,&t)&&!ioctl(s,FIONREAD,&status)&&!tcsetattr(s,TCSANOW,&t)&&!tcgetattr(s,&a)&&
  t.c_iflag==a.c_iflag&&t.c_oflag==a.c_oflag&&t.c_cflag==a.c_cflag&&t.c_lflag==a.c_lflag&&!memcmp(t.c_cc,a.c_cc,sizeof(t.c_cc))&&
  cfgetispeed(&t)==cfgetispeed(&a)&&cfgetospeed(&t)==cfgetospeed(&a)&&!fcntl(s,F_SETFL,f)&&fcntl(s,F_GETFL)==f&&!close(m)&&!close(s),"owned death console restoration");
 tests++;
}
#endif
static struct packet reviewed_run(struct pl_context *pl,enum action action,int manual) {
 int in[2],out[2],master=-1,slave=-1,status=0,done=0,saved,phase=0;size_t sent=0,got=0,used=0,cursor=0;
 struct packet p;struct rv_console console;struct rusage usage;struct termios initial,after;struct winsize size={24,80,0,0};
 long long end=auth_now()+90000;char capture[32769];pid_t w;
 memset(&p,0,sizeof(p));memset(&console,0,sizeof(console));memset(capture,0,sizeof(capture));strcpy(console.case_id,"viewer-case");
 console.revision=action==REVISION?UINT64_MAX:0;
 if(action==NARROW) {size.ws_col=40;size.ws_row=12;}
 if(action==UNKNOWN) {size.ws_col=0;size.ws_row=0;}
 if(action==SMALL) {size.ws_col=39;size.ws_row=11;}
 require(!pipe(in)&&!pipe(out),"review controller pipes");
 if(manual) {slave=fcntl(0,F_DUPFD,3);require(slave>=0,"held actual console");}
 else require(!openpty(&master,&slave,NULL,NULL,&size),"owned native review PTY");
 require(!tcgetattr(slave,&initial),"saved native review TTY");
 if(action==PREQUEUE) {initial.c_lflag&=~ECHO;require(!tcsetattr(slave,TCSANOW,&initial),"quiet fixture echo");write_all(master,(const unsigned char *)"review\nplace viewer-case 1\n",27);}
 saved=fcntl(in[0],F_GETFL);require(saved>=0,"review archive flags");source(input,input_length);hook_error=0;
 context.placement=pl;context.review=&console;
 worker=fork();require(worker>=0,"review controller fork");
 if(!worker) {
  struct rlimit data={64*1048576,64*1048576},core={0,0},fsize={1048576,1048576},cpu={15,15},fds={160,160};
  int sink=fcntl(out[1],F_DUPFD,16),rootfd=fcntl(pl->rootfd,F_DUPFD,16),parent=fcntl(pl->parentfd,F_DUPFD,16),tty=fcntl(slave,F_DUPFD,16);
  if(setsid()<0||sink<0||rootfd<0||parent<0||tty<0||dup2(in[0],0)<0||dup2(rootfd,3)<0||dup2(parent,4)<0||dup2(sink,5)<0||dup2(tty,7)<0) _exit(125);
  close(6);closefrom(8);console.in=7;console.out=7;pl->rootfd=3;pl->parentfd=4;
  if(setrlimit(RLIMIT_DATA,&data)||setrlimit(RLIMIT_RSS,&data)||setrlimit(RLIMIT_CORE,&core)||setrlimit(RLIMIT_FSIZE,&fsize)||setrlimit(RLIMIT_CPU,&cpu)||setrlimit(RLIMIT_NOFILE,&fds)) _exit(125);
  p.auth=auth_native(&context,signature,signature_length);p.placement=pl->result;p.flags=fcntl(0,F_GETFL);p.hook_error=hook_error;
  if(write(5,&p,sizeof(p))!=(ssize_t)sizeof(p)) _exit(125);_exit(0);
 }
 close(in[0]);close(out[1]);
 {int f=fcntl(in[1],F_GETFL);require(f>=0&&!fcntl(in[1],F_SETFL,f|O_NONBLOCK),"review input delivery flags");
  f=fcntl(out[0],F_GETFL);require(f>=0&&!fcntl(out[0],F_SETFL,f|O_NONBLOCK),"review result flags");
  if(master>=0) {f=fcntl(master,F_GETFL);require(f>=0&&!fcntl(master,F_SETFL,f|O_NONBLOCK),"review master flags");}}
 while(!done||got<sizeof(p)) {
  ssize_t n;struct pollfd f[3];
  require(auth_now()<end,"review external deadline");
  if(in[1]>=0) {
   if(sent==input_length) {require(!close(in[1]),"review archive EOF");in[1]=-1;}
   else {size_t amount=input_length-sent;if(amount>4096) amount=4096;n=write(in[1],input+sent,amount);
    if(n>0) sent+=(size_t)n;else if(n<0&&errno==EPIPE) {close(in[1]);in[1]=-1;}else require(n<0&&(errno==EINTR||errno==EAGAIN),"review archive delivery");}
  }
  if(master>=0&&used<32768) {
   n=read(master,capture+used,32768-used);
   if(n>0) {used+=(size_t)n;capture[used]=0;require(!memchr(capture,27,used),"no control injection in review output");}
   else require(!n||(n<0&&(errno==EINTR||errno==EAGAIN||errno==EIO)),"review capture read");
  }
  if(!manual&&phase==0&&strstr(capture+cursor,"Type review to confirm these effects.")) {
   cursor=used;phase=1;
   if(action==RESIZE) {struct winsize small={12,40,0,0};require(!ioctl(slave,TIOCSWINSZ,&small)&&!kill(worker,SIGWINCH),"native review resize");}
   else if(action==INTERRUPT) require(!kill(worker,SIGINT),"native review interrupt");
   else if(action==LOST) {close(master);master=-1;}
   else if(action==IDLE) {} else if(action==CANCEL) write_all(master,(const unsigned char *)"q\n",2);
   else if(action==BLANK) write_all(master,(const unsigned char *)"\n",1);
   else if(action==CONTROL) write_all(master,(const unsigned char *)"review\033[31m\n",12);
   else if(action==OVERFLOW) {unsigned char huge[1026];memset(huge,'x',1025);huge[1025]='\n';write_all(master,huge,sizeof(huge));}
   else write_all(master,(const unsigned char *)"review\n",7);
  }
  if(!manual&&phase==1&&strstr(capture+cursor,"place viewer-case 1")) {
   cursor=used;phase=2;
   if(action==HELP||action==BACK||action==STALE) write_all(master,(const unsigned char *)(action==BACK?"b\n":"h\n"),2);
   else if(action==DOUBLE) write_all(master,(const unsigned char *)"place viewer-case 1\nq\n",22);
   else write_all(master,(const unsigned char *)"place viewer-case 1\n",20);
  }
  if(!manual&&phase==2&&(action==HELP||action==BACK||action==STALE)&&strstr(capture+cursor,"Type review to confirm these effects.")) {
   cursor=used;phase=3;write_all(master,(const unsigned char *)"review\n",7);
  }
  if(!manual&&phase==3&&strstr(capture+cursor,"place viewer-case 2")) {
   cursor=used;phase=4;write_all(master,(const unsigned char *)(action==STALE?"place viewer-case 1\n":"place viewer-case 2\n"),20);
  }
  if(got<sizeof(p)) {n=read(out[0],(unsigned char *)&p+got,sizeof(p)-got);if(n>0) got+=(size_t)n;else require(n<0&&(errno==EINTR||errno==EAGAIN),"review result packet");}
  if(!done) {w=wait4(worker,&status,WNOHANG,&usage);if(w==worker) done=1;else require(w==0||(w<0&&errno==EINTR),"review controller wait4");}
  f[0].fd=in[1];f[0].events=POLLOUT;f[0].revents=0;f[1].fd=out[0];f[1].events=POLLIN;f[1].revents=0;f[2].fd=master;f[2].events=POLLIN;f[2].revents=0;
  if(!done||got<sizeof(p)) require(poll(f,3,10)>=0||errno==EINTR,"review supervisor poll");
 }
 require(WIFEXITED(status)&&!WEXITSTATUS(status)&&got==sizeof(p)&&!p.hook_error,"review result/reap");
 require(usage.ru_maxrss>0&&usage.ru_maxrss<=65536&&p.auth.controller_peak_kib>0&&p.auth.controller_peak_kib<=65536,"review measured controller RSS");
 if(usage.ru_maxrss>peak_controller) peak_controller=usage.ru_maxrss;if(p.auth.child_peak_kib>peak_child) peak_child=p.auth.child_peak_kib;
 require(p.flags==saved,"archive stdin restored separately");
 if(action!=LOST) require(!tcgetattr(slave,&after)&&initial.c_iflag==after.c_iflag&&initial.c_oflag==after.c_oflag&&initial.c_cflag==after.c_cflag&&initial.c_lflag==after.c_lflag&&
    !memcmp(initial.c_cc,after.c_cc,sizeof(initial.c_cc))&&cfgetispeed(&initial)==cfgetispeed(&after)&&cfgetospeed(&initial)==cfgetospeed(&after),"native review exact TTY restoration");
 if(master>=0) require(!close(master),"owned review master close");require(!close(slave),"review slave close");
 {unsigned char x;require(read(out[0],&x,1)==0&&!close(out[0]),"review packet EOF");}
 if(in[1]>=0) require(!close(in[1]),"review remaining input close");
 require(kill(worker,0)==-1&&errno==ESRCH&&kill(-worker,0)==-1&&errno==ESRCH,"review worker/group absent");
 worker=0;require(!unlink(WORK "source"),"review input disposal");canary();tests++;
 return p;
}
int main(int argc,char **argv) {
 struct termios tty;struct pl_context c;struct packet p;size_t count;int i;long prerequisite_peak=0;
 (void)argv;require(argc==1&&getuid()==0&&isatty(0)&&!tcgetattr(0,&tty),"fixed review native root/TTY invocation");
 require(signal(SIGPIPE,SIG_IGN)!=SIG_ERR,"review bounded delivery");require(!lstat(WORK "canary",&sentinel),"review outside canary");
 load(ROOT "cases/case-00.tar",0);count=expected_file(ROOT "cases/case-00.expected");
 c=root(count,PL_REAL);require(!pl_prerequisites(c.rootfd,&c.identity,&prerequisite_peak),"review confinement prerequisites");cleanup(&c);
#ifdef REVIEW_MANUAL
 puts("Actual-console composition: confirm, cancel, changed context, partial writer.");
 for(i=0;i<4;i++) {
  load(ROOT "cases/case-00.tar",0);count=expected_file(ROOT "cases/case-00.expected");change_mode=i==2?1:0;
  c=root(count,i==3?PL_SHORT:PL_REAL);
  printf("Manual case %d: %s\n",i,i==1?"type q at review":"type review, then the exact displayed phrase");fflush(stdout);
  p=reviewed_run(&c,CONFIRM,1);
  require(i==0?p.auth.completed&&p.placement.complete:!p.auth.completed,"actual console observed outcome");
  if(i==1||i==2) require(!p.placement.launched,"actual cancel/substitution no writer");
  if(i==3) require(p.placement.launched&&p.placement.observed==2&&!p.placement.complete,"actual partial writer remains incomplete");
  cleanup(&c);
 }
#else
 killed_console();
 for(i=CONFIRM;i<=SMALL;i++) {
  load(i==NARROW?ROOT "placement/new-01.tar":ROOT "cases/case-00.tar",0);count=expected_file(i==NARROW?ROOT "placement/new-01.expected":ROOT "cases/case-00.expected");change_mode=0;c=root(count,PL_REAL);p=reviewed_run(&c,(enum action)i,0);
  require(i==CONFIRM||i==HELP||i==BACK||i==NARROW||i==UNKNOWN?p.auth.completed&&p.placement.complete:!p.auth.completed&&!p.placement.launched,"native PTY confirmation/cancel/replay outcomes");cleanup(&c);
 }
 for(i=1;i<=11;i++) {
  load(ROOT "cases/case-00.tar",0);count=expected_file(ROOT "cases/case-00.expected");change_mode=i;c=root(count,PL_REAL);p=reviewed_run(&c,CONFIRM,0);
  require(!p.auth.completed&&!p.placement.launched,"changed complete context cannot dispatch");
  if(i==9) require(p.placement.observed==1&&p.placement.uncertain,"changed inventory observed honestly");cleanup(&c);
 }
 change_mode=0;
 for(i=PL_SHORT;i<=PL_SYNC;i++) {
  load(ROOT "cases/case-00.tar",0);count=expected_file(ROOT "cases/case-00.expected");c=root(count,(enum pl_test)i);p=reviewed_run(&c,CONFIRM,0);
  require(!p.auth.completed&&!p.placement.complete&&p.placement.launched&&p.placement.observed==2&&p.placement.reaped,"observed partial writer after fresh authorization");cleanup(&c);
 }
 load(ROOT "cases/case-00.tar",1);c=root(count,PL_REAL);p=reviewed_run(&c,CONFIRM,0);require(!p.auth.completed&&!p.placement.launched,"wrong signer cannot review or place");cleanup(&c);
 load(ROOT "cases/case-00.tar",0);signature_length--;c=root(count,PL_REAL);p=reviewed_run(&c,CONFIRM,0);require(!p.auth.completed&&!p.placement.launched,"incomplete signature no authority");cleanup(&c);
 require(tests==35,"native review complete fixed test count");
#endif
 keys();canary();require(access(WORK "core",F_OK)==-1&&errno==ENOENT,"review no core");
 printf("Native review checks passed: %d; peak controller %ld KiB; peak child %ld KiB.\n",tests,peak_controller,peak_child);
 return 0;
}
#endif
