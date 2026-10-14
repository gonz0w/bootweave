/* SPDX-License-Identifier: BSD-3-Clause */
#define ROOT "/mnt/installed/"
#define WORK "/bootweave-research/"
#define auth_native ipf_auth
#define rv_test_hook ipf_component_hook
#include "../payload/review_check.c"
#undef main
#undef auth_native
#undef rv_test_hook
#ifdef __OpenBSD__
#include "root.h"
#include "fixture_keys.h"
static unsigned probe;
struct auth_result auth_native(const struct auth_context *,const unsigned char *,size_t);
struct auth_result ipf_auth(const struct auth_context *c,const unsigned char *sig,size_t n){
struct auth_result r={0};struct auth_context local=*c;struct ip_context gate;
if(ip_open(&gate)){r.failure=AUTH_CONTEXT;return r;}
local.installed=&gate;r=auth_native(&local,sig,n);
if(ip_close(&gate)){r.failure=AUTH_CLEANUP;r.completed=0;}return r;
}
void rv_test_hook(int phase,struct auth_context *c,unsigned char *b,size_t n){
ipf_component_hook(phase,c,b,n);if(phase==1) c->installed->substitute=probe;
}
static void keys(void){require(sizeof(ip_key)<=AUTH_RECORD&&sizeof(ip_sig)<=AUTH_RECORD,"records");}
static void load(const char *path,int other){
(void)path;keys();input_length=read_file(ROOT "data.tar",input,sizeof(input));memset(&context,0,sizeof(context));
context.length=input_length;hash(input,input_length,context.digest);require(!memcmp(context.digest,ip_data_hash,32),"payload hash");
context.key_length=sizeof(ip_key);memcpy(context.key,ip_key,sizeof(ip_key));
signature_length=other?sizeof(ip_other_sig):sizeof(ip_sig);memcpy(signature,other?ip_other_sig:ip_sig,signature_length);
require(!auth_hash_file("/usr/bin/signify",context.verifier_digest)&&!auth_hash_file(AUTH_INSPECTOR,context.inspector_digest),"tool hashes");
memset(expected,0,sizeof(expected));expected[0].path="viewer";expected[0].directory=1;expected[1].path="viewer/readme.txt";expected[1].size=5;
hash((const unsigned char *)"hello",5,expected[1].digest);
}
int main(int argc,char **argv){
struct pl_context c;struct packet p;struct ip_context gate;struct ip_binding observed;long peak=0;int i;
(void)argv;(void)&p;(void)&i;(void)&peak;require(argc==1&&getuid()==0&&isatty(0),"fixed root/TTY");
require(!pl_limits(1),"limits");require(!lstat(WORK "canary",&sentinel),"canary stat");load(NULL,0);
#ifdef IPF_READBACK
memset(&c,0,sizeof(c));c.expected=expected;c.count=2;c.parentfd=open(WORK,O_RDONLY|O_DIRECTORY|O_NOFOLLOW);
c.rootfd=openat(c.parentfd,"case",O_RDONLY|O_DIRECTORY|O_NOFOLLOW);require(c.rootfd>=3&&!fstat(c.rootfd,&c.identity)&&!ip_open(&gate)&&!ip_capture(&gate,&c,&observed),"readback binding");
pl_observe(&c);{unsigned char b[8];require(!c.result.uncertain&&c.result.observed==2&&read_file(WORK "case/viewer/readme.txt",b,sizeof(b))==5&&!memcmp(b,"hello",5),"content");}
canary();require(!ip_close(&gate)&&!close(c.rootfd)&&!close(c.parentfd),"readback FDs");puts("Installed later readback passed.");return pl_limits(1)!=0;
#else
c=root(2,PL_REAL);require(!ip_open(&gate),"open");require(!ip_capture(&gate,&c,&observed),"capture");require(!ip_check(&gate,&c,&observed),"recheck");require(!ip_close(&gate),"close");require(!pl_prerequisites(c.rootfd,&c.identity,&peak),"prereq");cleanup(&c);
#ifdef IPF_KEEP
c=root(2,PL_REAL);p=reviewed_run(&c,CONFIRM,1);require(p.auth.completed&&p.placement.complete,"keep outcome");require(!close(c.rootfd)&&!close(c.parentfd),"keep FDs");canary();puts("Installed final positive retained.");return pl_limits(1)!=0;
#else
#ifdef REVIEW_MANUAL
for(i=0;i<5;i++){load(NULL,0);probe=i==3?1:0;change_mode=i==2?1:0;c=root(2,i==4?PL_SHORT:PL_REAL);
printf("Installed manual case %d: %s\n",i,i==1?"type q":"review, then exact fresh phrase");fflush(stdout);
p=reviewed_run(&c,CONFIRM,1);require(i==0?p.auth.completed&&p.placement.complete:!p.auth.completed,"actual outcome");
if(i>0&&i<4) require(!p.placement.launched&&!p.placement.observed,"actual refusal");
if(i==4) require(p.placement.launched&&p.placement.observed==2&&!p.placement.complete,"actual partial");cleanup(&c);
}
#else
for(i=0;i<12;i++){load(NULL,0);probe=i>=2&&i<=10?(unsigned)i-1:0;change_mode=i==11?1:0;c=root(2,PL_REAL);
p=reviewed_run(&c,i==1?CANCEL:CONFIRM,0);require(i==0?p.auth.completed&&p.placement.complete:!p.auth.completed&&!p.placement.launched,"refusal");cleanup(&c);}
probe=0;change_mode=0;
for(i=PL_SHORT;i<=PL_SYNC;i++){load(NULL,0);c=root(2,(enum pl_test)i);p=reviewed_run(&c,CONFIRM,0);require(!p.auth.completed&&p.placement.launched&&p.placement.observed==2&&!p.placement.complete,"partial writer");cleanup(&c);}
for(i=0;i<2;i++){load(NULL,i==0);if(i==1) signature_length--;c=root(2,PL_REAL);p=reviewed_run(&c,CONFIRM,0);require(!p.auth.completed&&!p.placement.launched,"signature");cleanup(&c);}
require(tests==17,"case count");
#endif
printf("Installed checks passed: %d; controller %ld KiB; child %ld KiB.\n",tests,peak_controller,peak_child);return pl_limits(1)!=0;
#endif
#endif
}
#else
int main(int argc,char **argv){(void)argv;fputs("Installed checks require OpenBSD.\n",stderr);return argc==1?77:2;}
#endif
