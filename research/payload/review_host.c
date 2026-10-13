/* SPDX-License-Identifier: BSD-3-Clause */
/* Compiled synthetic host recorder. No authentication or file-write backend. */
#include "review.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
int main(int argc,char **argv) {
 struct rv_binding b;struct rv_console c;int r;
 (void)argv;
 if(argc!=1) return 2;
 memset(&b,0,sizeof(b));memset(&c,0,sizeof(c));
 b.length=3072;b.device=1;b.inode=2;b.owner=32767;b.group=32767;b.mode=0700;
 b.plan.count=2;b.plan.members=2;b.plan.files=1;b.plan.bytes=5;
 strcpy(b.plan.nodes[0].path,"viewer");b.plan.nodes[0].directory=1;b.plan.nodes[0].explicit_member=1;b.plan.nodes[0].parent=-1;
 strcpy(b.plan.nodes[1].path,"viewer/readme.txt");b.plan.nodes[1].parent=0;b.plan.nodes[1].explicit_member=1;b.plan.nodes[1].offset=1024;b.plan.nodes[1].size=5;
 memset(b.payload,1,32);memset(b.key,2,32);memset(b.verifier,3,32);memset(b.inspector,4,32);
 strcpy(c.case_id,"viewer-case");c.in=fcntl(0,F_DUPFD,3);c.out=c.in;
 if(c.in<0) return 3;
 close(0);if(open("/dev/null",O_RDONLY)!=0) return 3;
 r=rv_console(&c,&b,rv_now()+90000);
 if(!r&&rv_quiet(&c)) r=2;
 if(close(c.in)) r=3;
 printf("Host synthetic dispatch=%d revision=%llu\n",r==0,(unsigned long long)c.revision);
 return r;
}
