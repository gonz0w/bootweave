/* SPDX-License-Identifier: BSD-3-Clause */
/* Synthetic syscall/PID model only; no OS limit, process or security mutation.
 * The wrapper is separately linked with all four OS/PID calls renamed here.
 * No state reset/profile API; each fixed case is a fresh owned invocation.
 */
#define _POSIX_C_SOURCE 200809L
#include <sys/resource.h>
#include <sys/types.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "trace_limits.h"

#define FOUR_MIB ((rlim_t)4194304)
#define EIGHT_MIB ((rlim_t)8388608)
#define MAX_STEPS 12
#define COLUMNS 22
struct observation { intmax_t cell[COLUMNS];unsigned pid_calls,ppid_calls; };
static struct observation observations[MAX_STEPS];
static unsigned used,get_calls,set_calls,fsize_get_calls,fsize_set_calls,other_get_calls,other_set_calls;
static unsigned pid_calls,ppid_calls,unexpected_resources,pointer_errors;
static rlim_t actual_soft=EIGHT_MIB,actual_hard=EIGHT_MIB;
static intmax_t last_set_soft=-2,last_set_hard=-2;
static pid_t model_pid=100,model_ppid=10;
static unsigned get_fault_at,set_fault_at,mismatch_at;
static int get_fault_return=-1,get_fault_errno=EIO,get_write_before_error;
static int other_return,other_errno=EDOM,other_write_before_error;
static const void *expected_other_pointer;

static intmax_t number(rlim_t value){return value==RLIM_INFINITY?-1:(intmax_t)value;}
int nq_model_getrlimit(int resource,struct rlimit *value)
{
    get_calls++;
    if(resource==RLIMIT_FSIZE){
        fsize_get_calls++;
        if(get_fault_at==fsize_get_calls){
            if(value&&get_write_before_error){value->rlim_cur=123;value->rlim_max=456;}
            errno=get_fault_errno;return get_fault_return;
        }
        if(mismatch_at==fsize_get_calls){actual_soft=FOUR_MIB-1;actual_hard=FOUR_MIB;}
        if(!value){errno=EFAULT;return -1;}
        value->rlim_cur=actual_soft;value->rlim_max=actual_hard;
        errno=E2BIG;return 0;
    }
    if(resource==RLIMIT_CPU){
        other_get_calls++;
        if((const void *)value!=expected_other_pointer)pointer_errors++;
        if(value&&(other_return==0||other_return==7||other_write_before_error)){
            value->rlim_cur=123;value->rlim_max=456;
        }
        errno=other_errno;return other_return;
    }
    unexpected_resources++;errno=EINVAL;return -1;
}
int nq_model_setrlimit(int resource,const struct rlimit *value)
{
    set_calls++;
    if(resource==RLIMIT_FSIZE){
        fsize_set_calls++;
        if(!value){errno=EFAULT;return -1;}
        last_set_soft=number(value->rlim_cur);last_set_hard=number(value->rlim_max);
        if(set_fault_at==fsize_set_calls){errno=EPERM;return -1;}
        actual_soft=value->rlim_cur;actual_hard=value->rlim_max;
        errno=E2BIG;return 0;
    }
    if(resource==RLIMIT_CPU){
        other_set_calls++;
        if((const void *)value!=expected_other_pointer)pointer_errors++;
        errno=other_errno;return other_return;
    }
    unexpected_resources++;errno=EINVAL;return -1;
}
pid_t nq_model_getpid(void){pid_calls++;return model_pid;}
pid_t nq_model_getppid(void){ppid_calls++;return model_ppid;}

/* Fixed numeric columns are documented in trace_oracles.json. Set arguments are
 * compared by fields, never padding. Failed getter output remains observable.
 */
static void operation(int op,int null_argument,rlim_t soft,rlim_t hard)
{
    struct rlimit value;
    struct observation *out;
    int result,saved,setting=op==1||op==3,resource=op<2?RLIMIT_FSIZE:RLIMIT_CPU;
    if(used>=MAX_STEPS)exit(90);
    value.rlim_cur=setting?soft:17;value.rlim_max=setting?hard:19;
    expected_other_pointer=null_argument?NULL:(const void *)&value;
    errno=ERANGE;
    if(setting)result=bw_nq_trace_setrlimit(resource,null_argument?NULL:&value);
    else result=bw_nq_trace_getrlimit(resource,null_argument?NULL:&value);
    saved=errno;out=&observations[used++];
    out->cell[0]=op;out->cell[1]=null_argument;out->cell[2]=ERANGE;
    out->cell[3]=result;out->cell[4]=saved;
    out->cell[5]=setting&&!null_argument?number(soft):-2;
    out->cell[6]=setting&&!null_argument?number(hard):-2;
    out->cell[7]=!setting&&!null_argument?number(value.rlim_cur):-2;
    out->cell[8]=!setting&&!null_argument?number(value.rlim_max):-2;
    out->cell[9]=setting&&!null_argument?(value.rlim_cur==soft&&value.rlim_max==hard):-1;
    out->cell[10]=number(actual_soft);out->cell[11]=number(actual_hard);
    out->cell[12]=get_calls;out->cell[13]=set_calls;
    out->cell[14]=fsize_get_calls;out->cell[15]=fsize_set_calls;
    out->cell[16]=other_get_calls;out->cell[17]=other_set_calls;
    out->cell[18]=last_set_soft;out->cell[19]=last_set_hard;
    out->cell[20]=model_pid;out->cell[21]=model_ppid;
    out->pid_calls=pid_calls;out->ppid_calls=ppid_calls;
}
static void get_fsize(void){operation(0,0,0,0);}
static void set_parent(void){operation(1,0,65536,65536);}
static void set_child(void){operation(1,0,0,0);}
static void parent_sequence(void){get_fsize();set_parent();get_fsize();}
static void direct_child(void){model_pid=101;model_ppid=100;}
static void child_sequence(void){direct_child();get_fsize();set_child();get_fsize();}
static void sticky_checks(void){get_fsize();set_parent();}
static void get_error(unsigned ordinal,int error){get_fault_at=ordinal;get_fault_errno=error;}
static void fault_case(unsigned index)
{
    switch(index){
    case 0: actual_soft=actual_hard=RLIM_INFINITY;parent_sequence();break;
    case 1: actual_hard=16777216;parent_sequence();break;
    case 2: actual_soft=actual_hard=FOUR_MIB;parent_sequence();break;
    case 3: parent_sequence();child_sequence();break;
    case 4: parent_sequence();get_fsize();break;
    case 5: parent_sequence();set_child();break;
    case 6: get_fsize();direct_child();get_fsize();break;
    case 7: get_fsize();set_parent();direct_child();get_fsize();break;
    case 8: parent_sequence();direct_child();get_fsize();model_pid=102;model_ppid=101;set_child();break;
    case 9: parent_sequence();child_sequence();model_pid=102;model_ppid=101;get_fsize();break;
    case 10: parent_sequence();model_pid=101;model_ppid=1;get_fsize();break;
    case 11: parent_sequence();model_pid=201;model_ppid=99;get_fsize();break;
    case 12: model_pid=0;get_fsize();break;
    case 13: model_pid=-1;get_fsize();break;
    case 14: get_fsize();direct_child();set_parent();break;
    case 15: parent_sequence();model_pid=101;model_ppid=0;get_fsize();break;
    case 16: get_fsize();operation(1,0,65535,65536);break;
    case 17: get_fsize();operation(1,0,65536,0);break;
    case 18: set_parent();break;
    case 19: set_child();break;
    case 20: get_fsize();get_fsize();break;
    case 21: get_fsize();set_parent();set_parent();break;
    case 22: parent_sequence();set_parent();break;
    case 23: parent_sequence();direct_child();get_fsize();set_parent();break;
    case 24: parent_sequence();direct_child();get_fsize();set_child();set_child();break;
    case 25: parent_sequence();child_sequence();get_fsize();break;
    case 26: actual_soft=2097152;get_fsize();break;
    case 27: actual_soft=1048576;actual_hard=2097152;get_fsize();break;
    case 28: actual_soft=actual_hard=0;get_fsize();break;
    case 29: actual_hard=FOUR_MIB;get_fsize();break;
    case 30: get_fsize();actual_hard=16777216;set_parent();break;
    case 31: get_error(1,EACCES);get_fsize();break;
    case 32: get_error(2,EINTR);get_fsize();set_parent();break;
    case 33: set_fault_at=1;get_fsize();set_parent();break;
    case 34: get_error(3,EIO);get_fsize();set_parent();break;
    case 35: mismatch_at=3;get_fsize();set_parent();break;
    case 36: get_error(4,EINTR);parent_sequence();break;
    case 37: mismatch_at=4;parent_sequence();break;
    case 38: get_error(5,EACCES);parent_sequence();direct_child();get_fsize();break;
    case 39: mismatch_at=5;parent_sequence();direct_child();get_fsize();break;
    case 40: get_error(6,EINTR);parent_sequence();direct_child();get_fsize();set_child();break;
    case 41: parent_sequence();direct_child();get_fsize();actual_soft=actual_hard=EIGHT_MIB;set_child();break;
    case 42: set_fault_at=2;parent_sequence();direct_child();get_fsize();set_child();break;
    case 43: get_error(7,EIO);parent_sequence();direct_child();get_fsize();set_child();break;
    case 44: mismatch_at=7;parent_sequence();direct_child();get_fsize();set_child();break;
    case 45: get_error(8,EINTR);parent_sequence();child_sequence();break;
    case 46: mismatch_at=8;parent_sequence();child_sequence();break;
    case 47: operation(0,1,0,0);break;
    case 48: get_fsize();operation(1,1,0,0);break;
    case 49: parent_sequence();direct_child();operation(0,1,0,0);break;
    case 50: parent_sequence();direct_child();get_fsize();operation(1,1,0,0);break;
    case 51: operation(2,0,0,0);parent_sequence();break;
    case 52: other_return=-1;other_errno=EIO;operation(2,0,0,0);parent_sequence();break;
    case 53: operation(3,0,123,456);parent_sequence();break;
    case 54: other_return=-1;other_errno=EIO;operation(3,0,123,456);parent_sequence();break;
    case 55: other_return=7;operation(2,0,0,0);parent_sequence();break;
    case 56: other_return=7;operation(3,0,123,456);parent_sequence();break;
    case 57: get_fsize();operation(2,0,0,0);operation(3,0,123,456);set_parent();get_fsize();break;
    case 58: operation(0,1,0,0);operation(2,0,0,0);other_return=-1;other_errno=EIO;operation(3,0,123,456);break;
    case 59: get_error(1,EIO);get_fault_return=7;get_fsize();break;
    case 60: other_return=-1;other_errno=EFAULT;operation(2,1,0,0);parent_sequence();break;
    case 61: other_return=-1;other_errno=EFAULT;operation(3,1,0,0);parent_sequence();break;
    case 62: get_error(1,EIO);get_write_before_error=1;get_fsize();break;
    case 63: other_return=-1;other_errno=EIO;other_write_before_error=1;operation(2,0,0,0);parent_sequence();break;
    default: exit(90);
    }
    if((index>=4&&index<=50)||index==58||index==59||index==62)sticky_checks();
}
struct fixed_case { const char *index,*name; };
static const struct fixed_case cases[]={
    {"0","TM-00-parent-infinity"},
    {"1","TM-01-parent-finite"},
    {"2","TM-02-parent-exact4m"},
    {"3","TM-03-direct-child"},
    {"4","TM-04-same-pid-extra-get"},
    {"5","TM-05-same-pid-child-set"},
    {"6","TM-06-fork-after-first-get"},
    {"7","TM-07-fork-before-final-get"},
    {"8","TM-08-nested-during-child"},
    {"9","TM-09-nested-after-child"},
    {"10","TM-10-orphan-child"},
    {"11","TM-11-unrelated-parent"},
    {"12","TM-12-pid-zero"},
    {"13","TM-13-pid-negative"},
    {"14","TM-14-identity-before-set"},
    {"15","TM-15-child-ppid-zero"},
    {"16","TM-16-wrong-parent-cap"},
    {"17","TM-17-mixed-parent-pair"},
    {"18","TM-18-set-before-get"},
    {"19","TM-19-child-zero-before-parent"},
    {"20","TM-20-duplicate-initial-get"},
    {"21","TM-21-duplicate-pending-set"},
    {"22","TM-22-duplicate-completed-set"},
    {"23","TM-23-wrong-child-cap"},
    {"24","TM-24-duplicate-child-set"},
    {"25","TM-25-extra-child-get"},
    {"26","TM-26-lower-soft"},
    {"27","TM-27-lower-hard"},
    {"28","TM-28-zero-bounds"},
    {"29","TM-29-inconsistent-bounds"},
    {"30","TM-30-parent-before-set-drift"},
    {"31","TM-31-parent-first-get-error"},
    {"32","TM-32-parent-observe-error"},
    {"33","TM-33-parent-set-error"},
    {"34","TM-34-parent-readback-error"},
    {"35","TM-35-parent-readback-mismatch"},
    {"36","TM-36-parent-final-get-error"},
    {"37","TM-37-parent-final-get-mismatch"},
    {"38","TM-38-child-first-get-error"},
    {"39","TM-39-child-first-get-mismatch"},
    {"40","TM-40-child-observe-error"},
    {"41","TM-41-child-before-set-drift"},
    {"42","TM-42-child-set-error"},
    {"43","TM-43-child-readback-error"},
    {"44","TM-44-child-readback-mismatch"},
    {"45","TM-45-child-final-get-error"},
    {"46","TM-46-child-final-get-mismatch"},
    {"47","TM-47-null-parent-get"},
    {"48","TM-48-null-parent-set"},
    {"49","TM-49-null-child-get"},
    {"50","TM-50-null-child-set"},
    {"51","TM-51-other-get-success"},
    {"52","TM-52-other-get-error"},
    {"53","TM-53-other-set-success"},
    {"54","TM-54-other-set-error"},
    {"55","TM-55-other-get-return7"},
    {"56","TM-56-other-set-return7"},
    {"57","TM-57-other-mid-sequence"},
    {"58","TM-58-other-after-poison"},
    {"59","TM-59-fsize-get-return7"},
    {"60","TM-60-other-null-get"},
    {"61","TM-61-other-null-set"},
    {"62","TM-62-fsize-write-before-error"},
    {"63","TM-63-other-write-before-error"},
};
int main(int argc,char **argv)
{
    unsigned index,i,j;
    if(argc!=2)return 64;
    for(index=0;index<sizeof(cases)/sizeof(cases[0]);index++)
        if(strcmp(argv[1],cases[index].index)==0)break;
    if(index==sizeof(cases)/sizeof(cases[0]))return 64;
    fault_case(index);
    printf("NQ_TRACE_MODEL {\"case_index\":%u,\"id\":\"%s\",\"steps\":[",index,cases[index].name);
    for(i=0;i<used;i++){
        if(i)putchar(',');putchar('[');
        for(j=0;j<COLUMNS;j++)printf("%s%jd",j?",":"",observations[i].cell[j]);
        putchar(']');
    }
    printf("],\"unexpected_resource_calls\":%u,\"passthrough_pointer_errors\":%u}\n",unexpected_resources,pointer_errors);
    printf("NQ_TRACE_PID {\"case_index\":%u,\"counts\":[",index);
    for(i=0;i<used;i++)printf("%s[%u,%u]",i?",":"",observations[i].pid_calls,observations[i].ppid_calls);
    printf("]}\n");
    return 0;
}
