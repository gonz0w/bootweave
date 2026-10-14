/* SPDX-License-Identifier: BSD-3-Clause */
#include "root.h"
#include <stdio.h>
#include <string.h>
int ip_valid(const struct ip_binding *b) {
 uint64_t bytes;char spec[19];size_t i;int nonzero=0;
 if(!b||storage_label_check(&b->label,&bytes)!=STORAGE_VALID||b->label.sector_bytes!=512||
    !b->label.partition[0].size||b->types[0]!=7||b->held.root_device!=1024||b->held.fsid[0]!=1024||
    !b->held.root_inode||!b->held.parent_inode||!b->held.case_inode||b->held.root_inode==b->held.parent_inode||
    b->held.parent_inode==b->held.case_inode||(b->held.flags&0x5000)!=0x5000||(b->held.flags&0x7e1)||
    b->held.rootfd<3||b->held.rawfd<3||b->held.blockfd<3||b->held.parentfd<3||b->held.casefd<3||
    !memchr(b->held.spec,0,sizeof(b->held.spec))) return 0;
 for(i=0;i<8;i++) {if(b->uid[i]) nonzero=1;snprintf(spec+i*2,3,"%02x",b->uid[i]);}
 spec[16]='.';spec[17]='a';spec[18]=0;
 return nonzero&&(!strcmp(b->held.spec,"/dev/sd0a")||!strcmp(b->held.spec,spec));
}
int ip_same(const struct ip_binding *a,const struct ip_binding *b) {
 size_t i;
#define EQ(n) (a->n==b->n)
 if(!ip_valid(a)||!ip_valid(b)||!(EQ(held.root_device)&&EQ(held.root_inode)&&EQ(held.parent_inode)&&EQ(held.case_inode)&&
    EQ(held.mount_time)&&EQ(held.flags)&&EQ(held.fsid[0])&&EQ(held.fsid[1])&&EQ(label_flags)&&EQ(checksum)&&
    EQ(held.rootfd)&&EQ(held.rawfd)&&EQ(held.blockfd)&&EQ(held.parentfd)&&EQ(held.casefd)&&EQ(label.magic)&&EQ(label.magic2)&&
    EQ(label.sector_bytes)&&EQ(label.version)&&EQ(label.count)&&EQ(label.sectors))||
    strcmp(a->held.spec,b->held.spec)||memcmp(a->uid,b->uid,8)) return 0;
#undef EQ
 for(i=0;i<a->label.count;i++) if(a->label.partition[i].offset!=b->label.partition[i].offset||
    a->label.partition[i].size!=b->label.partition[i].size||a->types[i]!=b->types[i]) return 0;
 return 1;
}
