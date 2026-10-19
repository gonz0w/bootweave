/* SPDX-License-Identifier: BSD-3-Clause */
/* Only the test object substitutes these syscalls; no credentials/child launch. */
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "auth_material.h"
#include "auth_input.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static const char *fixture, *fault;
static volatile sig_atomic_t cancelled;
static int directory = -1, original = -1, writer[3] = {-1,-1,-1}, reader[3] = {-1,-1,-1};
static unsigned calls, opens, writes, reads, attempts, generations[128], consumed[128], duplicate_close;
static int preparing, replaced, observed;
static struct bw_auth_snapshot capture;
static int mode(const char *s) { return !strcmp(fault, s); }
static int role(const char *s) { return !strcmp(s,"key.pub") ? 0 : !strcmp(s,"signature.sig") ? 1 : 2; }
static int named(const char *prefix, int i) { char b[80]; snprintf(b,sizeof(b),"%s-%d",prefix,i);return mode(b); }
static void born(int fd) { if (fd >= 0 && fd < 128) generations[fd]++; }

uid_t bw_m_getuid(void) { return mode("root") ? 0 : getuid(); }

int
bw_m_clock_gettime(clockid_t id, struct timespec *t)
{
    int r = clock_gettime(id,t);
    if (mode("clock-error")) return -1;
    if (mode("clock-invalid")) t->tv_nsec = 1000000000L;
    if (preparing && mode("backward")) t->tv_sec -= 3;
    if (observed && mode("elapsed-observe")) t->tv_sec += 3;
    if (preparing && (mode("elapsed") || (directory < 0 && mode("close-deadline")))) t->tv_sec += 3;
    if (directory < 0 && preparing && mode("close-clock-error")) return -1;
    if (mode("write-eintr-forever") || mode("read-eintr-forever")) t->tv_sec += attempts / 3;
    return r;
}

char *
bw_m_mkdtemp(char *path)
{
    int n;
    char *r;
    calls++;
    if (mode("mkdir-error")) { errno=EIO;return NULL; }
    n=snprintf(path,256,"%s/verify.XXXXXXXXXX",fixture);
    if (n<0 || n>=256) { errno=ENAMETOOLONG;return NULL; }
    r=mkdtemp(path);
    if (mode("cancel-create")) cancelled=1;
    return r;
}

int
bw_m_open(const char *path,int flags,...)
{
    int fd;
    calls++;
    if (mode("open-directory-error")) { errno=EIO;return -1; }
    fd=open(path,flags);born(fd);directory=fd;original=fd;
    return fd;
}

int
bw_m_fcntl(int fd,int command,...)
{
    int r,minimum;
    va_list ap;
    if (command==F_DUPFD_CLOEXEC) {
        va_start(ap,command);minimum=va_arg(ap,int);va_end(ap);
        if (mode("dup-error")) { errno=EIO;return -1; }
        r=fcntl(fd,command,minimum);born(r);directory=r;return r;
    }
    return fcntl(fd,command);
}

int
bw_m_openat(int dir,const char *name,int flags,...)
{
    int fd,i=role(name);
    mode_t permissions=0;
    va_list ap;
    calls++;opens++;
    if ((flags&O_CREAT) && named("open-write-error",i)) { errno=EIO;return -1; }
    if (!(flags&O_CREAT) && named("open-read-error",i)) { errno=EIO;return -1; }
    if (flags&O_CREAT) {
        va_start(ap,flags);permissions=(mode_t)va_arg(ap,int);va_end(ap);
        fd=openat(dir,name,flags,permissions);writer[i]=fd;
    } else { fd=openat(dir,name,flags);reader[i]=fd; }
    born(fd);
    if (!(flags&O_CREAT) && i==2 && mode("extra-name")) {
        int extra=openat(dir,"extra",O_WRONLY|O_CREAT|O_EXCL,0600);
        if (extra<0 || write(extra,"untouched",9)!=9 || close(extra)) _exit(91);
    }
    return fd;
}

int
bw_m_fstat(int fd,struct stat *s)
{
    int r=fstat(fd,s),i;
    calls++;
    if (fd==directory && preparing) {
        if(mode("cancel-observe")) cancelled=1;
        if(mode("directory-zero-links") && !r) s->st_nlink=0;
        observed=1;
    }
    if (fd==directory && !preparing && mode("stat-directory-error")) { errno=EIO;return -1; }
    if (fd==directory && !preparing && mode("directory-reserve-zero-links") && !r) s->st_nlink=0;
    for(i=0;i<3;i++) {
        if(fd==writer[i] && named("stat-created-error",i)) { errno=EIO;return -1; }
        if(fd!=reader[i] || r) continue;
        if(mode("changed-inode")) s->st_ino++;
        if(mode("changed-mode")) s->st_mode^=1;
        if(mode("changed-owner")) s->st_uid++;
        if(mode("changed-links")) s->st_nlink++;
        if(mode("changed-size")) s->st_size++;
    }
    return r;
}

int bw_m_lstat(const char *p,struct stat *s) {
    int r=lstat(p,s);calls++;
    if(preparing && mode("cancel-named-observe")) cancelled=1;
    if(preparing && mode("directory-named-zero-links") && !r) s->st_nlink=0;
    return r;
}

int
bw_m_fstatat(int dir,const char *name,struct stat *s,int flags)
{
    calls++;
    if (!strcmp(name,"key.pub") && mode("replace-name") && !replaced++) {
        if (renameat(dir,"key.pub",dir,"key.saved") || symlinkat("../canary",dir,"key.pub")) _exit(92);
    }
    return fstatat(dir,name,s,flags);
}

ssize_t
bw_m_write(int fd,const void *b,size_t n)
{
    ssize_t r;
    calls++;writes++;attempts++;
    if(mode("write-error")) { errno=EIO;return -1; }
    if(mode("write-zero")) return 0;
    if(mode("write-eintr-forever") || (mode("short") && writes==1)) { errno=EINTR;return -1; }
    if(mode("short") && n>7) n=7;
    r=write(fd,b,n);
    if(mode("cancel-write") || mode("cancel-write-close")) cancelled=1;
    return r;
}

ssize_t
bw_m_read(int fd,void *b,size_t n)
{
    ssize_t r;
    calls++;reads++;attempts++;
    if(mode("read-error")) { errno=EIO;return -1; }
    if(mode("read-truncate")) return 0;
    if(mode("read-eintr-forever") || (mode("short") && reads==1)) { errno=EINTR;return -1; }
    if(mode("short") && n>5) n=5;
    r=read(fd,b,n);
    if(r>0 && mode("read-corrupt")) ((unsigned char *)b)[0]^=1;
    if(mode("cancel-read")) cancelled=1;
    return r;
}

int
bw_m_close(int fd)
{
    int r=close(fd),i,uncertain=0,absent=0;
    calls++;
    if(fd>=0 && fd<128) {
        if(consumed[fd]==generations[fd]) duplicate_close++;
        consumed[fd]=generations[fd];
    }
    if(fd==original && fd!=directory && mode("close-original")) uncertain=1;
    for(i=0;i<3;i++) {
        if(fd==writer[i] && named("close-writer",i)) uncertain=1;
        if(fd==reader[i] && named("close-reader",i)) uncertain=1;
        if(fd==reader[i] && named("ebadf-reader",i)) absent=1;
    }
    if(fd==writer[0] && mode("cancel-write-close")) uncertain=1;
    if(fd==directory) {
        if(mode("close-directory")) uncertain=1;
        if(mode("cancel-close")) cancelled=1;
        directory=-1;
    }
    if(uncertain) { errno=EINTR;return -1; }
    if(absent) { errno=EBADF;return -1; }
    return r;
}

int
bw_m_unlinkat(int dir,const char *name,int flags)
{
    int r;
    calls++;
    if(named("unlink-error",role(name))) { errno=EIO;return -1; }
    r=unlinkat(dir,name,flags);
    if(mode("cancel-unlink")) cancelled=1;
    return r;
}

int
bw_m_rmdir(const char *path)
{
    int r;
    calls++;
    if(mode("rmdir-error")) { errno=EIO;return -1; }
    r=rmdir(path);
    if(mode("cancel-rmdir")) cancelled=1;
    return r;
}

static size_t
load(const char *path,unsigned char *b,size_t cap)
{
    FILE *f=fopen(path,"rb");size_t n;
    if(!f) _exit(93);
    n=fread(b,1,cap,f);
    if(ferror(f) || fclose(f)) _exit(93);
    return n;
}

static int
missing_role(int hole,char **argv,struct bw_verifier_material *m)
{
    int rolefd[3]={0,3,4},held[3],saved[2],i,fd,r;
    struct stat before,after;
    for(i=0;i<2;i++) if((saved[i]=fcntl(i+1,F_DUPFD_CLOEXEC,20))<0) return 94;
    for(i=0;i<3;i++) {
        fd=open(argv[2+(i==0?2:i==1?0:1)],O_RDONLY);
        if(fd<0 || (held[i]=fcntl(fd,F_DUPFD_CLOEXEC,10))<0) return 94;
        close(fd);
    }
    for(i=0;i<3;i++) if(dup2(held[i],rolefd[i])<0) return 94;
    close(hole);
    r=bw_verifier_reserve(&cancelled,m);
    if(r!=BW_MATERIAL_OK || m->directory<5 || fcntl(hole,F_GETFD)!=-1 || errno!=EBADF ||
        fstat(m->directory,&before)) return 95;
    r=bw_auth_capture(&capture);
    if(r!=BW_CAPTURE_REJECTED || fstat(m->directory,&after) ||
        before.st_dev!=after.st_dev || before.st_ino!=after.st_ino) return 95;
    if(dup2(saved[0],1)<0 || dup2(saved[1],2)<0) return 95;
    for(i=0;i<3;i++) close(held[i]);
    for(i=0;i<2;i++) close(saved[i]);
    return bw_verifier_release(m)==BW_MATERIAL_OK && !duplicate_close ? 0 : 96;
}

int
main(int argc,char **argv)
{
    unsigned char key[17000],signature[17000],definition[17000],b[4096];
    const unsigned char *data[3];size_t lengths[3],i,at;ssize_t n;
    struct bw_verifier_material m={0},copy;
    struct bw_verifier_request r;
    struct stat seen[3];int result,old[3],fresh;
    unsigned prior;
    if(argc!=7) return 90;
    fixture=argv[1];fault=argv[6];
    r.key=key;r.signature=signature;r.definition=definition;
    r.key_length=load(argv[2],key,sizeof(key));r.signature_length=load(argv[3],signature,sizeof(signature));
    r.definition_length=load(argv[4],definition,sizeof(definition));
    if(!strcmp(argv[5],"records")) return bw_signify_records(key,r.key_length,signature,r.signature_length) && !calls ? 0 : 2;
    if(!strncmp(argv[5],"hole-",5)) return missing_role(argv[5][5]-'0',argv,&m);
    if(mode("cancel-before")) cancelled=1;
    result=bw_verifier_reserve(&cancelled,&m);
    if(result!=BW_MATERIAL_OK) goto failure;
    memcpy(&copy,&m,sizeof(m));prior=calls;
    if(bw_verifier_reserve(&cancelled,&m)!=BW_MATERIAL_REJECTED || memcmp(&copy,&m,sizeof(m)) || prior!=calls) return 97;
    if(!strcmp(argv[5],"abort")) return bw_verifier_release(&m)==BW_MATERIAL_OK && m.state==BW_VM_CLOSED && !m.path[0] ? 0 : 97;
    if(!strcmp(argv[5],"null-key")) r.key=NULL;
    if(!strcmp(argv[5],"null-signature")) r.signature=NULL;
    if(!strcmp(argv[5],"null-definition")) r.definition=NULL;
    if(mode("replace-directory")) {
        char moved[300];int fd;
        snprintf(moved,sizeof(moved),"%s/moved",fixture);
        if(rename(m.path,moved) || mkdir(m.path,0700)) return 98;
        fd=openat(directory,"original-sentinel",O_CREAT|O_WRONLY|O_EXCL,0600);
        if(fd<0 || write(fd,"original",8)!=8 || close(fd)) return 98;
    }
    preparing=1;
    result=bw_verifier_prepare(!strcmp(argv[5],"null-request")?NULL:&r,&m);
    if(result!=BW_MATERIAL_OK) goto failure;
    memcpy(&copy,&m,sizeof(m));prior=calls;
    if(bw_verifier_prepare(&r,&m)!=BW_MATERIAL_REJECTED || memcmp(&copy,&m,sizeof(m)) || prior!=calls) return 97;
    data[0]=key;data[1]=signature;data[2]=definition;
    lengths[0]=r.key_length;lengths[1]=r.signature_length;lengths[2]=r.definition_length;
    if(m.state!=BW_VM_READY || m.path[0]) return 99;
    printf("prepared-only\n");
    for(i=0;i<3;i++) {
        old[i]=m.fd[i];
        if(fstat(old[i],&seen[i]) || !S_ISREG(seen[i].st_mode) || seen[i].st_nlink ||
            seen[i].st_size!=(off_t)lengths[i] || (fcntl(old[i],F_GETFL)&O_ACCMODE)!=O_RDONLY ||
            !(fcntl(old[i],F_GETFD)&FD_CLOEXEC) || lseek(old[i],0,SEEK_CUR)!=0) return 99;
        if(i && seen[i].st_dev==seen[i-1].st_dev && seen[i].st_ino==seen[i-1].st_ino) return 99;
        at=0;
        while((n=read(old[i],b,sizeof(b)))>0) {
            if((size_t)n>lengths[i]-at || memcmp(b,data[i]+at,(size_t)n) || fwrite(b,1,(size_t)n,stdout)!=(size_t)n) return 99;
            at+=(size_t)n;
        }
        if(n<0 || at!=lengths[i]) return 99;
    }
    result=bw_verifier_release(&m);
    if(result!=BW_MATERIAL_OK) goto failure;
    for(i=0;i<3;i++) if(fcntl(old[i],F_GETFD)!=-1 || errno!=EBADF) return 99;
    fresh=open("/dev/null",O_RDONLY);
    prior=calls;
    if(fresh<0 || bw_verifier_release(&m)!=BW_MATERIAL_REJECTED || prior!=calls || fcntl(fresh,F_GETFD)<0) return 99;
    close(fresh);
    return duplicate_close ? 99 : 0;
failure:
    if(duplicate_close) return 99;
    if(m.state!=BW_VM_REJECTED && m.state!=BW_VM_UNCERTAIN) return 99;
    for(i=0;i<3;i++) if(m.writer[i]!=-1 || m.fd[i]!=-1) return 99;
    if(m.directory!=-1) return 99;
    printf("\nMATERIAL_FAILURE %d %d %d %u %u %u %u %u %ju %ju %s\n",m.state,m.cause,
        (int)cancelled,m.unknown_closes,m.close_errors,m.directory_lost,opens,writes,
        (uintmax_t)m.identity.st_dev,(uintmax_t)m.identity.st_ino,m.path[0]?m.path:"-");
    return result==BW_MATERIAL_UNCERTAIN ? 3 : 2;
}
