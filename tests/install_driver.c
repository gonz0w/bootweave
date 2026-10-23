/* SPDX-License-Identifier: BSD-3-Clause */
#include "private_install.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef BWI_NORMAL
struct bwi_owner { int actual_test_object; };
static struct bwi_owner owner;
static char events[8192];
static size_t event_size, calls[256];
static char fault, other;
static unsigned nth, nth_other, outcome, outcome_other;
static unsigned
notice(char kind, const struct bwi_step *step)
{
    int n;
    unsigned value=BWI_OBSERVED;
    calls[(unsigned char)kind]++;
    n=step ? snprintf(events+event_size,sizeof(events)-event_size," %c%d.%zu",kind,step->operation,step->index) :
        snprintf(events+event_size,sizeof(events)-event_size," %c",kind);
    if (n<0 || (size_t)n>=sizeof(events)-event_size) exit(90);
    event_size+=(size_t)n;
    if (kind==fault && calls[(unsigned char)kind]==nth) value=outcome;
    if (kind==other && calls[(unsigned char)kind]==nth_other) value=outcome_other;
    return value;
}
enum bwi_observation
bwi_native_admit(const struct bwi_profile *p, const struct bwi_plan *plan, const char *review, size_t size, struct bwi_owner **out)
{
    unsigned value=notice('A',NULL);
    (void)p; (void)plan;
    if(size<58 || memcmp(review,"UNVERIFIED INSTALLER EFFECT PLAN",31)) exit(92);
    *out=value==BWI_NO_EFFECT ? NULL : &owner;
    return (enum bwi_observation)value;
}
enum bwi_observation bwi_native_preflight(struct bwi_owner *o)
{ if(o!=&owner) exit(91); return (enum bwi_observation)notice('P',NULL); }
int bwi_native_live(struct bwi_owner *o)
{
    unsigned value;
    if(o!=&owner) exit(91);
    value=notice('L',NULL);
    return value==BWI_OBSERVED ? 1 : value==BWI_NO_EFFECT ? 0 : -1;
}
enum bwi_observation bwi_native_intent(struct bwi_owner *o,const struct bwi_step *s)
{ if(o!=&owner) exit(91); return (enum bwi_observation)notice('I',s); }
enum bwi_observation bwi_native_step(struct bwi_owner *o,const struct bwi_step *s)
{ if(o!=&owner) exit(91); return (enum bwi_observation)notice('S',s); }
enum bwi_observation bwi_native_release_untouched(struct bwi_owner *o)
{ if(o!=&owner) exit(91); return (enum bwi_observation)notice('U',NULL); }
void bwi_native_retain(struct bwi_owner *o)
{ if(o!=&owner) exit(91); (void)notice('R',NULL); }
#endif

int
main(int argc,char **argv)
{
    static unsigned char raw[BWI_PROFILE_MAX+1];
    static struct bwi_profile p, untouched;
    struct bwi_plan plan;
    struct bwi_result first;
    const struct bwi_result *r;
    char output[BWI_PLAN_MAX];
    size_t size,length=0,i;
    enum bwi_status status, repeated;
    if(argc<2) return 2;
    size=fread(raw,1,sizeof(raw),stdin);
    if(ferror(stdin)) return 3;
    memset(&p,0x5a,sizeof(p)); memcpy(&untouched,&p,sizeof(p));
    if(!strcmp(argv[1],"prefixes")) {
        for(i=0;i<size;i++) {
            if(bwi_profile_parse(raw,i,&p) || memcmp(&p,&untouched,sizeof(p))) return 4;
        }
        puts("PREFIXES-REJECTED-UNCHANGED"); return 0;
    }
    if(!bwi_profile_parse(raw,size,&p)) {
        if(!strcmp(argv[1],"invalid-entry")) goto run_entry;
        if(memcmp(&p,&untouched,sizeof(p))) return 4;
        puts("REJECTED-UNCHANGED"); return 0;
    }
    if(!strcmp(argv[1],"canaries")) {
        if(bwi_profile_parse(NULL,size,&p) || bwi_profile_parse(raw,size,NULL) ||
            bwi_profile_validate(NULL) || bwi_plan_build(&p,NULL) ||
            bwi_plan_render(&p,NULL,output,sizeof(output),&length) || length) return 9;
        memset(&untouched,0x7f,sizeof(untouched));
        if(bwi_profile_validate(&untouched)) return 9;
        memcpy(&untouched,&p,sizeof(p)); untouched.files=BWI_FILES+1;
        if(bwi_profile_validate(&untouched)) return 9;
        puts("CANARIES-REFUSED"); return 0;
    }
    if(!bwi_plan_build(&p,&plan)) return 5;
    if(!strcmp(argv[1],"parse")) {
        printf("PLAN");
        for(i=0;i<plan.count;i++) printf(" %d.%zu",plan.step[i].operation,plan.step[i].index);
        puts(""); return 0;
    }
    if(!strcmp(argv[1],"render") || !strcmp(argv[1],"forged") || !strcmp(argv[1],"short")) {
        memset(output,0x5a,sizeof(output));
        if(!strcmp(argv[1],"forged")) plan.step[plan.count-1].index++;
        if(!bwi_plan_render(&p,&plan,output,!strcmp(argv[1],"short") ? 1 : sizeof(output),&length)) {
            if(length) return 6;
            for(i=0;i<sizeof(output);i++) if((unsigned char)output[i]!=0x5a) return 6;
            puts("OUTPUT-UNCHANGED"); return 0;
        }
        if(fwrite(output,1,length,stdout)!=length) return 7;
        return 0;
    }
#ifndef BWI_NORMAL
    if(argc!=8) return 2;
    fault=argv[2][0];nth=(unsigned)strtoul(argv[3],NULL,10);outcome=(unsigned)strtoul(argv[4],NULL,10);
    other=argv[5][0];nth_other=(unsigned)strtoul(argv[6],NULL,10);outcome_other=(unsigned)strtoul(argv[7],NULL,10);
#endif
run_entry:
    status=bw_install_source(raw,size); r=bw_install_observe();memcpy(&first,r,sizeof(first));
    repeated=bw_install_source(raw,size);
    if(memcmp(r,&first,sizeof(first))) return 8;
    printf("RESULT %d %zu %zu %zu %d %zu %d REPEAT %d\n",status,r->intended,r->submitted,
        r->observed,r->operation,r->index,r->retention_requested,repeated);
#ifndef BWI_NORMAL
    printf("EVENTS%s\n",events);
#else
    puts("NORMAL-PLATFORM-REFUSAL");
#endif
    return 0;
}
