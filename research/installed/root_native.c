/* SPDX-License-Identifier: BSD-3-Clause */
#include "root.h"
#ifdef __OpenBSD__
#include "placement.h"
#include "review.h"
#include "auth_native.h"
#include <sys/mount.h>
#include <sys/disklabel.h>
#include <sys/dkio.h>
#include <sys/sysctl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
static int dir(int fd,struct stat *s,uid_t owner,mode_t mode){
return fd>=3&&!fstat(fd,s)&&S_ISDIR(s->st_mode)&&s->st_dev==makedev(4,0)&&
s->st_uid==owner&&s->st_gid==owner&&(s->st_mode&07777)==mode;
}
static int label(int fd,int raw,struct ip_bind *b){
struct stat s;struct disklabel d;size_t i;
memset(b,0,sizeof(*b));memset(&d,0,sizeof(d));
if(fd<3||fstat(fd,&s)||!S_ISCHR(s.st_mode)||
s.st_rdev!=makedev(13,raw?2:0)||s.st_uid||s.st_mode&0022||ioctl(fd,DIOCGDINFO,&d)||
d.d_npartitions>16)return -1;
b->label.magic=d.d_magic;b->label.magic2=d.d_magic2;b->label.version=d.d_version;
b->label.count=d.d_npartitions;b->label.sector_bytes=d.d_secsize;b->label.sectors=DL_GETDSIZE(&d);
b->labelflags=d.d_flags;b->checksum=d.d_checksum;memcpy(b->uid,d.d_uid,8);
for(i=0;i<b->label.count;i++){b->label.partition[i].offset=DL_GETPOFFSET(&d.d_partitions[i]);
b->label.partition[i].size=DL_GETPSIZE(&d.d_partitions[i]);b->types[i]=d.d_partitions[i].p_fstype;}
return 0;
}
static int fs(int fd,const struct statfs *root){
struct statfs f;
#define DIFF(n) memcmp(&f.n,&root->n,sizeof(f.n))
return fstatfs(fd,&f)||DIFF(f_fsid.val)||DIFF(f_flags)||DIFF(f_owner)||DIFF(f_ctime)||
DIFF(f_fstypename)||DIFF(f_mntonname)||DIFF(f_mntfromname)||DIFF(f_mntfromspec)?-1:0;
#undef DIFF
}
static int node(struct ip_files *h){
struct stat s;
if(lstat("/dev/sd0a",&s)||!S_ISBLK(s.st_mode)||s.st_rdev!=makedev(4,0)||
s.st_dev!=makedev(4,0)||s.st_uid||s.st_mode&0022)return -1;
h->nodeino=s.st_ino;h->nodemode=s.st_mode;h->nodegid=s.st_gid;
h->nodelinks=s.st_nlink;h->nodeflags=s.st_flags;return 0;
}
int ip_cap(const struct ip_ctx *c,const struct pl_context *p,struct ip_bind *b){
struct ip_bind part;struct stat r,parent,member,named;struct statfs f={0};
char pre[4096],after[4096],release[16],machine[16];unsigned char uid[8];
int names[]={CTL_HW,HW_DISKNAMES},rel[]={CTL_KERN,KERN_OSRELEASE},arch[]={CTL_HW,HW_MACHINE};
size_t n=sizeof(pre),m=sizeof(after),rn=sizeof(release),an=sizeof(machine),i;uint64_t bytes;
#define CF(n) (fprintf(stderr,"Capture %d errno=%d flags=%x fsid=%u\n",n,errno,f.f_flags,(unsigned)f.f_fsid.val[0]),-1)
if(!c||!p||getuid()!=0||geteuid()!=0||label(c->rawfd,1,b)||label(c->partfd,0,&part)||
storage_label_check(&b->label,&bytes)!=STORAGE_VALID||node(&b->h))return CF(1);
if(sysctl(rel,2,release,&rn,NULL,0)||rn!=4||memcmp(release,"7.9\0",4)||
sysctl(arch,2,machine,&an,NULL,0)||an!=6||memcmp(machine,"amd64\0",6)||
sysctl(names,2,pre,&n,NULL,0)||ip_names(pre,n,uid)!=STORAGE_VALID||memcmp(uid,b->uid,8))return CF(2);
if(!dir(c->rootfd,&r,0,0755)||!dir(p->parentfd,&parent,0,0700)||
!dir(p->rootfd,&member,32767,0700)||
fstatat(c->rootfd,"bootweave-research",&named,AT_SYMLINK_NOFOLLOW)||named.st_dev!=parent.st_dev||
named.st_ino!=parent.st_ino||named.st_mode!=parent.st_mode||named.st_uid||named.st_gid||
fstatat(p->parentfd,"case",&named,AT_SYMLINK_NOFOLLOW)||named.st_dev!=member.st_dev||
named.st_ino!=member.st_ino||named.st_mode!=member.st_mode||named.st_uid!=32767||named.st_gid!=32767||
lstat("/",&named)||named.st_dev!=r.st_dev||named.st_ino!=r.st_ino)return CF(3);
if(fstatfs(c->rootfd,&f)||f.f_owner||f.f_fsid.val[0]!=(int32_t)r.st_dev||
!memchr(f.f_fstypename,0,sizeof(f.f_fstypename))||!memchr(f.f_mntonname,0,sizeof(f.f_mntonname))||
!memchr(f.f_mntfromname,0,sizeof(f.f_mntfromname))||
strcmp(f.f_fstypename,"ffs")||strcmp(f.f_mntonname,"/")||strcmp(f.f_mntfromname,"/dev/sd0a")||
!memchr(f.f_mntfromspec,0,sizeof(f.f_mntfromspec))||fs(p->parentfd,&f)||fs(p->rootfd,&f))return CF(4);
b->h.rootdev=r.st_dev;b->h.rootino=r.st_ino;b->h.parentino=parent.st_ino;b->h.caseino=member.st_ino;
b->h.mounttime=f.f_ctime;b->h.flags=f.f_flags;b->h.fsid[0]=f.f_fsid.val[0];b->h.fsid[1]=f.f_fsid.val[1];
memcpy(b->h.spec,f.f_mntfromspec,sizeof(b->h.spec));
b->h.rootfd=c->rootfd;b->h.rawfd=c->rawfd;b->h.partfd=c->partfd;b->h.parentfd=p->parentfd;b->h.casefd=p->rootfd;
part.h=b->h;
if(!ip_same(b,&part)||sysctl(names,2,after,&m,NULL,0)||n!=m||memcmp(pre,after,n))return CF(5);
for(i=0;i<2;i++){struct ip_bind again;
if(label(i?c->partfd:c->rawfd,!i,&again))return CF(6);again.h=b->h;
if(node(&again.h))return CF(6);
if(!ip_same(b,&again))return CF(6);
}
return 0;
#undef CF
}
int ip_open(struct ip_ctx *c){
memset(c,0,sizeof(*c));c->rootfd=c->rawfd=c->partfd=-1;
c->rootfd=open("/",O_RDONLY|O_DIRECTORY|O_NOFOLLOW);
c->rawfd=open("/dev/rsd0c",O_RDONLY|O_NONBLOCK|O_NOFOLLOW);
c->partfd=open("/dev/rsd0a",O_RDONLY|O_NONBLOCK|O_NOFOLLOW);
if(c->rootfd<3||c->rawfd<3||c->partfd<3){ip_close(c);return -1;}return 0;
}
int ip_close(struct ip_ctx *c){
int failed=0,*fds[]={&c->rootfd,&c->rawfd,&c->partfd};size_t i;
for(i=0;i<3;i++){if(*fds[i]>=0&&close(*fds[i])) failed=1;*fds[i]=-1;}return failed?-1:0;
}
int ip_check(const struct ip_ctx *c,const struct pl_context *p,const struct ip_bind *b){
struct ip_bind now;if(ip_cap(c,p,&now))return -1;
#ifdef AUTH_TEST
switch(c->substitute){
case 1:now.h.flags^=8;break;case 2:now.h.fsid[1]++;break;case 3:now.h.mounttime++;break;
case 4:now.label.partition[0].offset++;break;case 5:now.h.partfd++;break;
case 6:now.uid[0]^=1;break;case 7:now.h.rootino++;break;
case 8:now.h.caseino++;break;case 9:now.labelflags++;break;default:break;
}
#endif
return ip_same(b,&now)?0:-1;
}
int ip_begin(const struct ip_ctx *c,const struct pl_context *p,struct rv_console *tty,struct ip_bind *b){
char text[1024],uid[17];int n,flags,result=0;size_t i,at=0;long long end=auth_now()+5000;
if(!tty||ip_cap(c,p,b)||rv_quiet(tty))return -1;
for(i=0;i<8;i++) snprintf(uid+i*2,3,"%02x",b->uid[i]);
n=snprintf(text,sizeof(text),"Installed /dev/sd0a FFS /:\nDUID=%s\nSectors=%"PRIu64 "\na.offset=%"PRIu64 "\na.size=%"PRIu64 "\nRoot.dev=%"PRIu64 "\nRoot.ino=%"PRIu64 "\nParent.ino=%"PRIu64 "\nCase.ino=%"PRIu64 "\nFSid=%u,%u\nFlags=%x\nMount.time=%"PRIu64 "\nBlock.node=%"PRIu64 ",%o,%u,%u,%x\n/bootweave-research/case\n",uid,b->label.sectors,b->label.partition[0].offset,b->label.partition[0].size,b->h.rootdev,b->h.rootino,b->h.parentino,b->h.caseino,b->h.fsid[0],b->h.fsid[1],b->h.flags,b->h.mounttime,b->h.nodeino,b->h.nodemode,b->h.nodegid,b->h.nodelinks,b->h.nodeflags);
if(n<=0||(size_t)n>=sizeof(text)||(flags=fcntl(tty->out,F_GETFL))<0||fcntl(tty->out,F_SETFL,flags|O_NONBLOCK))return -1;
while(at<(size_t)n){struct pollfd f={tty->out,POLLOUT,0};ssize_t w;
if(!auth_live(end)||poll(&f,1,10)<0||f.revents&(POLLERR|POLLHUP|POLLNVAL)){result=-1;break;}
if(!(f.revents&POLLOUT)) continue;w=write(tty->out,text+at,(size_t)n-at);
if(w>0) at+=(size_t)w;else if(w<0&&(errno==EINTR||errno==EAGAIN)) continue;else{result=-1;break;}
}
if(fcntl(tty->out,F_SETFL,flags)||fcntl(tty->out,F_GETFL)!=flags) result=-1;
return result;
}
#endif
