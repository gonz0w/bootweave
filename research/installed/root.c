/* SPDX-License-Identifier: BSD-3-Clause */
#include "root.h"
#include <stdio.h>
#include <string.h>
/* No-hotplug guest only; host proves no floppy backing. */
enum storage_result ip_names(const char *s,size_t n,unsigned char uid[8]){
char out[STORAGE_DISCOVERY_MAX];size_t at=0,end,k=0;int fd=0;
memset(uid,0,8);
if(!s||!n||n>sizeof(out)||s[n-1]||memchr(s,0,n-1))return STORAGE_INVALID;
while(at<n-1){
for(end=at;end<n-1&&s[end]!=',';end++){}
if(end==at||(end<n-1&&end==n-2))return STORAGE_INVALID;
if(end-at==4&&!memcmp(s+at,"fd0:",4)){if(fd++)return STORAGE_INVALID;}
else{
if(end-at>4&&s[at]!='s')return STORAGE_INVALID;
if(k) out[k++]=',';memcpy(out+k,s+at,end-at);k+=end-at;
}
at=end+1;
}
out[k++]=0;return storage_discovery(out,k,uid);
}
int ip_valid(const struct ip_bind *b){
uint64_t bytes;char spec[19];size_t i;int nonzero=0;
if(!b||storage_label_check(&b->label,&bytes)!=STORAGE_VALID||b->label.sector_bytes!=512||
!b->label.partition[0].size||b->types[0]!=7||b->h.rootdev!=1024||b->h.fsid[0]!=1024||
!b->h.nodeino||!b->h.nodelinks||(b->h.nodemode&0170000)!=0060000||(b->h.nodemode&0022)||
!b->h.rootino||!b->h.parentino||!b->h.caseino||b->h.rootino==b->h.parentino||
b->h.parentino==b->h.caseino||(b->h.flags&0x5000)!=0x5000||(b->h.flags&0x7e1)||
b->h.rootfd<3||b->h.rawfd<3||b->h.partfd<3||b->h.parentfd<3||b->h.casefd<3||
!memchr(b->h.spec,0,sizeof(b->h.spec)))return 0;
for(i=0;i<8;i++){if(b->uid[i]) nonzero=1;snprintf(spec+i*2,3,"%02x",b->uid[i]);}
spec[16]='.';spec[17]='a';spec[18]=0;
return nonzero&&(!strcmp(b->h.spec,"/dev/sd0a")||!strcmp(b->h.spec,spec));
}
int ip_same(const struct ip_bind *a,const struct ip_bind *b){
size_t i;
#define EQ(n)(a->n==b->n)
if(!ip_valid(a)||!ip_valid(b)||!(EQ(h.rootdev)&&EQ(h.rootino)&&EQ(h.parentino)&&EQ(h.caseino)&&
EQ(h.mounttime)&&EQ(h.flags)&&EQ(h.fsid[0])&&EQ(h.fsid[1])&&EQ(labelflags)&&EQ(checksum)&&
EQ(h.nodeino)&&EQ(h.nodemode)&&EQ(h.nodegid)&&EQ(h.nodelinks)&&EQ(h.nodeflags)&&
EQ(h.rootfd)&&EQ(h.rawfd)&&EQ(h.partfd)&&EQ(h.parentfd)&&EQ(h.casefd)&&EQ(label.magic)&&EQ(label.magic2)&&
EQ(label.sector_bytes)&&EQ(label.version)&&EQ(label.count)&&EQ(label.sectors))||
strcmp(a->h.spec,b->h.spec)||memcmp(a->uid,b->uid,8))return 0;
#undef EQ
for(i=0;i<a->label.count;i++) if(a->label.partition[i].offset!=b->label.partition[i].offset||
a->label.partition[i].size!=b->label.partition[i].size||a->types[i]!=b->types[i])return 0;
return 1;
}
