/* SPDX-License-Identifier: BSD-3-Clause */
/* Portable boundary/dispatcher model. No host security/credential/timer action. */
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "private_entry.h"
#include "private_cli.h"
#include "private_start_material.h"
#include "trusted_start.h"
#include "plan.h"
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined(NE_BOUNDARY_PREAMBLE)
int ne_live(void);int ne_identity(uid_t *);int ne_fstat(int,struct stat *);int ne_lstat(const char *,struct stat *);
int ne_unveil(const char *,const char *);int ne_pledge(void);
#undef fstat
#undef lstat
#define bw_entry_live() ne_live()
#define bw_en_identity(o) ne_identity(o)
#define fstat(f,s) ne_fstat(f,s)
#define lstat(p,s) ne_lstat(p,s)
#define bw_en_unveil(p,s) ne_unveil(p,s)
#define bw_en_pledge() ne_pledge()
#else
int ne_normal_identity(uid_t *);int ne_normal_unveil(const char *,const char *);int ne_normal_pledge(void);
int ne_actual_main(int,char **);
static const char *ne_mode,*ne_field;
static int ne_index,ne_value;
static struct bw_verifier_material ne_material;
static volatile sig_atomic_t ne_flag;
static unsigned ne_lives,ne_work,ne_security;
struct ne_event { int kind,number,result,null_path,null_permissions;char path[256],permissions[64]; };
static struct ne_event ne_events[32];static unsigned ne_event_count;
static int ne_available_value,ne_entry_value,ne_start_ok,ne_write_ok,ne_running_main;
static unsigned ne_availability_calls,ne_entry_calls,ne_legacy_calls,ne_start_calls,ne_writes,ne_arg_identity=1;
static int ne_argc;static char **ne_argv;
static char *ne_original_pointers[8];static char ne_original_bytes[8][128];
static unsigned char ne_out[4096],ne_err[4096];static size_t ne_out_n,ne_err_n;
static const char ne_scratch[]="/tmp/bootweave-verify.Ab0C1d2E3f";
static int ne_is(const char *name){return !strcmp(ne_mode,name);}
static void ne_copy(char *out,size_t cap,const char *in){size_t i=0;if(!in){out[0]=0;return;}for(;i+1<cap&&in[i];i++)out[i]=in[i];out[i]=0;}
static void ne_event(int kind,int number,const char *path,const char *permissions,int result){struct ne_event *e;
 if(ne_event_count>=32)exit(90);e=&ne_events[ne_event_count++];e->kind=kind;e->number=number;e->result=result;
 e->null_path=path==NULL;e->null_permissions=permissions==NULL;ne_copy(e->path,sizeof(e->path),path);ne_copy(e->permissions,sizeof(e->permissions),permissions);}
int ne_live(void){int ok=!(ne_is("live")&&(int)ne_lives==ne_index);ne_lives++;ne_event(0,ok,NULL,NULL,ok);return ok;}
int ne_identity(uid_t *owner){int result=ne_is("identity")?0:ne_is("identity-result")?ne_value:1;
 if(!owner)exit(90);ne_event(1,0,NULL,NULL,result);ne_work++;if(result==1)*owner=1000;return result;}
static int ne_stat(int kind,int fd,const char *path,struct stat *out){int result=ne_is("stat-result")&&ne_index==kind?ne_value:0;
 memcpy(out,&ne_material.identity,sizeof(*out));
 if(ne_is("positive-links"))out->st_nlink=kind==1?2:3;
 if(ne_is("metadata")&&ne_index==kind){
  if(!strcmp(ne_field,"device"))out->st_dev++;
  else if(!strcmp(ne_field,"inode"))out->st_ino++;
  else if(!strcmp(ne_field,"type"))out->st_mode=S_IFREG|0700;
  else if(!strcmp(ne_field,"mode"))out->st_mode=S_IFDIR|0755;
  else if(!strcmp(ne_field,"uid"))out->st_uid++;
  else if(!strcmp(ne_field,"gid"))out->st_gid++;
  else if(!strcmp(ne_field,"links-zero"))out->st_nlink=0;
  else exit(90);
 }
 ne_event(kind+1,fd,path,NULL,result);ne_work++;return result;
}
int ne_fstat(int fd,struct stat *out){return ne_stat(1,fd,NULL,out);}
int ne_lstat(const char *path,struct stat *out){return ne_stat(2,0,path,out);}
int ne_unveil(const char *path,const char *permissions){int result=ne_is("security-result")&&(int)ne_work==ne_index?ne_value:0;
 ne_event(4,0,path,permissions,result);ne_work++;ne_security++;return result;}
int ne_pledge(void){int result=ne_is("security-result")&&(int)ne_work==ne_index?ne_value:0;
 ne_event(5,0,"stdio rpath wpath cpath proc exec",NULL,result);ne_work++;ne_security++;return result;}
static void ne_init_material(void){unsigned i;memset(&ne_material,0,sizeof(ne_material));
 ne_material.state=BW_VM_RESERVED;ne_material.directory=5;ne_material.directory_known=1;ne_material.cancelled=&ne_flag;
 ne_material.started.tv_sec=1000;ne_material.started.tv_nsec=1234;
 ne_material.identity.st_dev=11;ne_material.identity.st_ino=23;ne_material.identity.st_mode=S_IFDIR|0700;
 ne_material.identity.st_uid=1000;ne_material.identity.st_gid=1000;ne_material.identity.st_nlink=1;
 for(i=0;i<3;i++)ne_material.writer[i]=ne_material.fd[i]=-1;
 memcpy(ne_material.path,ne_scratch,sizeof(ne_scratch));
}
static void ne_shape(void){
 if(!strcmp(ne_field,"null"))return;
 if(!strcmp(ne_field,"state"))ne_material.state=(enum bw_material_state)ne_value;
 else if(!strcmp(ne_field,"directory"))ne_material.directory=ne_value;
 else if(!strcmp(ne_field,"directory_known"))ne_material.directory_known=(unsigned)ne_value;
 else if(!strcmp(ne_field,"directory_lost"))ne_material.directory_lost=(unsigned)ne_value;
 else if(!strcmp(ne_field,"created"))ne_material.created=(unsigned)ne_value;
 else if(!strcmp(ne_field,"known"))ne_material.known=(unsigned)ne_value;
 else if(!strcmp(ne_field,"cause"))ne_material.cause=(enum bw_material_cause)ne_value;
 else if(!strcmp(ne_field,"uncertain"))ne_material.uncertain=(unsigned)ne_value;
 else if(!strcmp(ne_field,"close_errors"))ne_material.close_errors=(unsigned)ne_value;
 else if(!strcmp(ne_field,"unknown_closes"))ne_material.unknown_closes=(unsigned)ne_value;
 else if(!strcmp(ne_field,"capture_taken"))ne_material.capture_taken=(unsigned)ne_value;
 else if(!strcmp(ne_field,"capture_close_errors"))ne_material.capture_close_errors=(unsigned)ne_value;
 else if(!strcmp(ne_field,"capture_unknown_closes"))ne_material.capture_unknown_closes=(unsigned)ne_value;
 else if(!strcmp(ne_field,"cancelled"))ne_material.cancelled=NULL;
 else if(!strcmp(ne_field,"seconds"))ne_material.started.tv_sec=ne_value;
 else if(!strcmp(ne_field,"nanoseconds"))ne_material.started.tv_nsec=ne_value;
 else if(!strcmp(ne_field,"record_type"))ne_material.identity.st_mode=S_IFREG|0700;
 else if(!strcmp(ne_field,"record_mode"))ne_material.identity.st_mode=(mode_t)(S_IFDIR|ne_value);
 else if(!strcmp(ne_field,"record_links"))ne_material.identity.st_nlink=0;
 else if(!strcmp(ne_field,"writer")&&ne_index>=0&&ne_index<3)ne_material.writer[ne_index]=ne_value;
 else if(!strcmp(ne_field,"fd")&&ne_index>=0&&ne_index<3)ne_material.fd[ne_index]=ne_value;
 else exit(90);
}
static void ne_bad_path(void){size_t prefix=sizeof("/tmp/bootweave-verify.")-1;
 if(!strcmp(ne_field,"empty"))ne_material.path[0]=0;
 else if(!strcmp(ne_field,"wrong-prefix"))ne_material.path[1]='x';
 else if(!strcmp(ne_field,"short-suffix"))ne_material.path[prefix+9]=0;
 else if(!strcmp(ne_field,"long-suffix")){ne_material.path[prefix+10]='X';ne_material.path[prefix+11]=0;}
 else if(!strcmp(ne_field,"early-nul"))ne_material.path[prefix+2]=0;
 else if(!strcmp(ne_field,"unterminated")){memset(ne_material.path,'X',sizeof(ne_material.path));memcpy(ne_material.path,"/tmp/bootweave-verify.",prefix);}
 else if(!strcmp(ne_field,"suffix-dash"))ne_material.path[prefix+2]='-';
 else if(!strcmp(ne_field,"suffix-slash"))ne_material.path[prefix+2]='/';
 else if(!strcmp(ne_field,"suffix-space"))ne_material.path[prefix+2]=' ';
 else if(!strcmp(ne_field,"suffix-underscore"))ne_material.path[prefix+2]='_';
 else if(!strcmp(ne_field,"suffix-control"))ne_material.path[prefix+2]=27;
 else if(!strcmp(ne_field,"suffix-nonascii"))ne_material.path[prefix+2]=(char)0xff;
 else exit(90);
}
static void ne_json_string(const char *text){const unsigned char *p=(const unsigned char *)text;if(!text){printf("null");return;}putchar('"');
 for(;*p;p++){if(*p=='"'||*p=='\\'){putchar('\\');putchar(*p);}else if(*p<32||*p>=127)printf("\\u%04x",*p);else putchar(*p);}putchar('"');}
static void ne_print_events(void){unsigned i;putchar('[');for(i=0;i<ne_event_count;i++){struct ne_event *e=&ne_events[i];if(i)putchar(',');
 if(e->kind==0)printf("[\"live\",%d]",e->number);
 else if(e->kind==1)printf("[\"identity\"]");
 else if(e->kind==2)printf("[\"fstat\",%d]",e->number);
 else if(e->kind==3){printf("[\"lstat\",");ne_json_string(e->null_path?NULL:e->path);putchar(']');}
 else if(e->kind==4){printf("[\"unveil\",");ne_json_string(e->null_path?NULL:e->path);putchar(',');ne_json_string(e->null_permissions?NULL:e->permissions);putchar(']');}
 else if(e->kind==5){printf("[\"pledge\",");ne_json_string(e->path);printf(",null]");}else exit(90);
 }putchar(']');}
int ne_available(void){ne_availability_calls++;return ne_available_value;}
enum bw_authenticated_result ne_closed_entry(void){ne_entry_calls++;return (enum bw_authenticated_result)ne_entry_value;}
int ne_legacy_call(int argc,char **argv){int i;ne_legacy_calls++;
 if(argc!=ne_argc||argv!=ne_argv)ne_arg_identity=0;for(i=0;i<argc;i++)if(argv[i]!=ne_original_pointers[i]||strcmp(argv[i],ne_original_bytes[i]))ne_arg_identity=0;
 return bw_legacy_cli(argc,argv);}
int ne_start(void){ne_start_calls++;if(ne_running_main)ne_legacy_calls++;return ne_start_ok;}
int ne_write(int fd,const char *bytes,size_t length){unsigned char *out;size_t *used;ne_writes++;
 if(fd==1){out=ne_out;used=&ne_out_n;}else if(fd==2){out=ne_err;used=&ne_err_n;}else exit(90);
 if(length>4096-*used)exit(90);memcpy(out+*used,bytes,length);*used+=length;return ne_write_ok;}
int ne_confine(void){exit(97);}int ne_run(void){exit(97);}
struct ne_fd {int fd,fd_flags,fd_errno,status_flags,status_errno,stat_ok,stat_errno;dev_t device;ino_t inode;mode_t mode;};
static void ne_probe(int fd,struct ne_fd *out){struct stat s;int result;memset(out,0,sizeof(*out));out->fd=fd;
 out->fd_flags=fcntl(fd,F_GETFD);out->fd_errno=out->fd_flags<0?errno:0;
 out->status_flags=fcntl(fd,F_GETFL);out->status_errno=out->status_flags<0?errno:0;
 result=fstat(fd,&s);out->stat_errno=result<0?errno:0;out->stat_ok=result==0;
 if(out->stat_ok){out->device=s.st_dev;out->inode=s.st_ino;out->mode=s.st_mode;}}
static int ne_equal_fd(const struct ne_fd *a,const struct ne_fd *b){return a->fd_flags==b->fd_flags&&a->status_flags==b->status_flags&&a->stat_ok==b->stat_ok&&
 a->device==b->device&&a->inode==b->inode&&a->mode==b->mode;}
static void ne_print_probes(const struct ne_fd *probes){unsigned i;putchar('[');for(i=0;i<7;i++){const struct ne_fd *p=&probes[i];
 printf("%s{\"fd\":%d,\"fd_flags\":%d,\"fd_errno\":%d,\"status_flags\":%d,\"status_errno\":%d,\"stat_ok\":%d,\"stat_errno\":%d,\"device\":%llu,\"inode\":%llu,\"mode\":%llu}",
  i?",":"",p->fd,p->fd_flags,p->fd_errno,p->status_flags,p->status_errno,p->stat_ok,p->stat_errno,
  (unsigned long long)p->device,(unsigned long long)p->inode,(unsigned long long)p->mode);}putchar(']');}
static void ne_hex(const unsigned char *b,size_t n){size_t i;for(i=0;i<n;i++)printf("%02x",b[i]);}
int main(int argc,char **argv){int result,unchanged=1,owner_unchanged=1,normal_unveil=-9,normal_pledge=-9,canaries=1;
 struct bw_verifier_material before;unsigned i;
 if(argc<4)return 90;
 if(!strcmp(argv[2],"boundary")){uid_t owner=17;
  if(argc!=8)return 90;ne_mode=argv[3];ne_index=atoi(argv[4]);ne_field=argv[5];ne_value=atoi(argv[6]);
  if(strcmp(argv[7],"finite"))return 90;ne_init_material();
  if(ne_is("shape"))ne_shape();if(ne_is("path"))ne_bad_path();
  if(ne_is("directory"))ne_material.directory=ne_value;
  if(ne_is("record-owner"))ne_material.identity.st_uid=999;
  memcpy(&before,&ne_material,sizeof(before));
  if(ne_is("normal-boundary"))result=bw_entry_boundary(&ne_material);
  else if(ne_is("normal-available"))result=bw_entry_available();
  else if(ne_is("normal-identity")){result=ne_normal_identity(&owner);owner_unchanged=owner==17;
   normal_unveil=ne_normal_unveil(ne_scratch,"rwc");normal_pledge=ne_normal_pledge();}
  else result=bw_entry_native(ne_is("shape")&&!strcmp(ne_field,"null")?NULL:&ne_material);
  unchanged=!memcmp(&before,&ne_material,sizeof(before))&&!ne_flag;
  printf("NE_RESULT {\"family\":\"boundary\",\"return\":%d,\"material_unchanged\":%d,\"owner_unchanged\":%d,\"normal_unveil\":%d,\"normal_pledge\":%d,\"lives\":%u,\"work\":%u,\"security\":%u,\"trace\":",
   result,unchanged,owner_unchanged,normal_unveil,normal_pledge,ne_lives,ne_work,ne_security);ne_print_events();printf(",\"outcomes\":[");
  for(i=0;i<ne_event_count;i++)printf("%s%d",i?",":"",ne_events[i].result);printf("]}\n");
 }else if(!strcmp(argv[2],"cli")){struct ne_fd before_fd[7],ready_fd[7],after_fd[7];int pair[2],owned[2],roles[7]={0,1,2,3,4,-1,-1};
  unsigned setup_mask=0,call_mask=0,private_setup_mask=0,private_call_mask=0,close_calls[2]={0,0};
  int close_result[2],close_error[2],poll_result,poll_error,read_calls=0,read_error=0,data_ok;ssize_t read_result=-2;
  unsigned char bytes[7]={0};struct pollfd poller;char *arguments[8];int count;
  if(argc<9||argc>16)return 90;ne_mode=argv[3];ne_available_value=atoi(argv[4]);ne_entry_value=atoi(argv[5]);ne_start_ok=atoi(argv[6]);ne_write_ok=atoi(argv[7]);
  count=argc-8;if(count>7)return 90;for(i=0;i<(unsigned)count;i++){arguments[i]=argv[8+i];ne_original_pointers[i]=arguments[i];ne_copy(ne_original_bytes[i],sizeof(ne_original_bytes[i]),arguments[i]);}arguments[count]=NULL;ne_argc=count;ne_argv=arguments;
  for(i=0;i<5;i++)ne_probe((int)i,&before_fd[i]);
  if(pipe(pair))return 91;for(i=0;i<2;i++){owned[i]=fcntl(pair[i],F_DUPFD_CLOEXEC,5);if(owned[i]<5||close(pair[i]))return 91;roles[5+i]=owned[i];ne_probe(owned[i],&before_fd[5+i]);}
  if(write(owned[1],"canary\n",7)!=7)return 91;
  /* Setup is complete before the preservation interval around the dispatcher. */
  for(i=0;i<7;i++)ne_probe(roles[i],&ready_fd[i]);
  for(i=0;i<5;i++)if(!ne_equal_fd(&before_fd[i],&ready_fd[i]))setup_mask|=1U<<i;
  for(i=0;i<2;i++)if(!ne_equal_fd(&before_fd[5+i],&ready_fd[5+i]))private_setup_mask|=1U<<i;
  ne_running_main=ne_is("main");result=ne_running_main?ne_actual_main(count,arguments):bw_entry_cli(count,arguments);
  for(i=0;i<7;i++)ne_probe(roles[i],&after_fd[i]);
  for(i=0;i<5;i++)if(!ne_equal_fd(&ready_fd[i],&after_fd[i]))call_mask|=1U<<i;
  for(i=0;i<2;i++)if(!ne_equal_fd(&ready_fd[5+i],&after_fd[5+i]))private_call_mask|=1U<<i;
  poller.fd=owned[0];poller.events=POLLIN;poller.revents=0;
  poll_result=poll(&poller,1,0);poll_error=poll_result<0?errno:0;
  if(poll_result==1&&(poller.revents&POLLIN)){read_calls++;read_result=read(owned[0],bytes,7);read_error=read_result<0?errno:0;}
  data_ok=poll_result==1&&(poller.revents&POLLIN)&&read_calls==1&&read_result==7&&!memcmp(bytes,"canary\n",7);
  if(setup_mask||call_mask||private_call_mask||!data_ok)canaries=0;
  for(i=0;i<(unsigned)count;i++)if(arguments[i]!=ne_original_pointers[i]||strcmp(ne_original_bytes[i],arguments[i]))ne_arg_identity=0;
  for(i=0;i<2;i++){close_calls[i]++;close_result[i]=close(owned[i]);close_error[i]=close_result[i]<0?errno:0;if(close_result[i])canaries=0;}
  printf("NE_RESULT {\"family\":\"cli\",\"status\":%d,\"legacy_calls\":%u,\"start_calls\":%u,\"entry_calls\":%u,\"availability_calls\":%u,\"argv_identity\":%u,\"canaries_unchanged\":%d,\"writes\":%u,\"stdout_hex\":\"",
   result,ne_legacy_calls,ne_start_calls,ne_entry_calls,ne_availability_calls,ne_arg_identity,canaries,ne_writes);ne_hex(ne_out,ne_out_n);printf("\",\"stderr_hex\":\"");ne_hex(ne_err,ne_err_n);
  printf("\",\"fixture_pipe_bytes\":7,\"fixture_private_fds\":2,\"inherited_setup_mismatch_mask\":%u,\"inherited_call_mismatch_mask\":%u,\"private_setup_mismatch_mask\":%u,\"private_call_mismatch_mask\":%u,\"fd_before\":",setup_mask,call_mask,private_setup_mask,private_call_mask);
  ne_print_probes(before_fd);printf(",\"fd_ready\":");ne_print_probes(ready_fd);printf(",\"fd_after\":");ne_print_probes(after_fd);
  printf(",\"canary_poll\":{\"calls\":1,\"result\":%d,\"errno\":%d,\"requested\":%d,\"revents\":%d},\"canary_read\":{\"calls\":%d,\"result\":%ld,\"errno\":%d,\"hex\":\"",poll_result,poll_error,(int)poller.events,(int)poller.revents,read_calls,(long)read_result,read_error);
  if(read_result>0)ne_hex(bytes,(size_t)read_result);printf("\",\"exact_bytes\":%d},\"fixture_close_dispositions\":[",data_ok);
  for(i=0;i<2;i++)printf("%s{\"fd\":%d,\"calls\":%u,\"result\":%d,\"errno\":%d}",i?",":"",owned[i],close_calls[i],close_result[i],close_error[i]);printf("]}\n");
 }else return 90;
 return 0;
}
#endif
