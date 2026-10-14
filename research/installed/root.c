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
int ip_valid(const struct ip_binding *b){
uint64_t bytes;char spec[19];size_t i;int nonzero=0;
if(!b||storage_label_check(&b->label,&bytes)!=STORAGE_VALID||b->label.sector_bytes!=512||
!b->label.partition[0].size||b->types[0]!=7||b->held.root_device!=1024||b->held.fsid[0]!=1024||
!b->held.root_inode||!b->held.parent_inode||!b->held.case_inode||b->held.root_inode==b->held.parent_inode||
b->held.parent_inode==b->held.case_inode||(b->held.flags&0x5000)!=0x5000||(b->held.flags&0x7e1)||
b->held.rootfd<3||b->held.rawfd<3||b->held.blockfd<3||b->held.parentfd<3||b->held.casefd<3||
!memchr(b->held.spec,0,sizeof(b->held.spec)))return 0;
for(i=0;i<8;i++){if(b->uid[i]) nonzero=1;snprintf(spec+i*2,3,"%02x",b->uid[i]);}
spec[16]='.';spec[17]='a';spec[18]=0;
return nonzero&&(!strcmp(b->held.spec,"/dev/sd0a")||!strcmp(b->held.spec,spec));
}
int ip_same(const struct ip_binding *a,const struct ip_binding *b){
size_t i;
#define EQ(n)(a->n==b->n)
if(!ip_valid(a)||!ip_valid(b)||!(EQ(held.root_device)&&EQ(held.root_inode)&&EQ(held.parent_inode)&&EQ(held.case_inode)&&
EQ(held.mount_time)&&EQ(held.flags)&&EQ(held.fsid[0])&&EQ(held.fsid[1])&&EQ(label_flags)&&EQ(checksum)&&
EQ(held.rootfd)&&EQ(held.rawfd)&&EQ(held.blockfd)&&EQ(held.parentfd)&&EQ(held.casefd)&&EQ(label.magic)&&EQ(label.magic2)&&
EQ(label.sector_bytes)&&EQ(label.version)&&EQ(label.count)&&EQ(label.sectors))||
strcmp(a->held.spec,b->held.spec)||memcmp(a->uid,b->uid,8))return 0;
#undef EQ
for(i=0;i<a->label.count;i++) if(a->label.partition[i].offset!=b->label.partition[i].offset||
a->label.partition[i].size!=b->label.partition[i].size||a->types[i]!=b->types[i])return 0;
return 1;
}
