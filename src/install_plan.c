/* SPDX-License-Identifier: BSD-3-Clause */
#include "install_profile.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef char step_capacity[(6 + BWI_PRINCIPALS + BWI_FILES + BWI_INPUTS +
    BWI_SERVICES <= BWI_STEPS) ? 1 : -1];

static void
add(struct bwi_plan *p, enum bwi_operation op, size_t index)
{
    p->step[p->count].operation = op;
    p->step[p->count++].index = index;
}
int
bwi_plan_build(const struct bwi_profile *p, struct bwi_plan *out)
{
    struct bwi_plan plan;
    size_t i;
    if (!out || !bwi_profile_validate(p)) return 0;
    memset(&plan, 0, sizeof(plan));
    add(&plan,BWI_SELECT_RELEASE,0);
    add(&plan,BWI_CONFIRM_TARGET,0);
    add(&plan,BWI_INSTALL_STOCK,0);
    for (i=0;i<p->principals;i++) add(&plan,BWI_CREATE_PRINCIPAL,i);
    for (i=0;i<p->files;i++) add(&plan,BWI_PLACE_VERIFIED,i);
    for (i=0;i<p->inputs;i++) add(&plan,BWI_BIND_PRIVATE_INPUT,i);
    add(&plan,BWI_CONFIGURE_POLICY,0);
    add(&plan,BWI_CONFIGURE_OBSERVABILITY,0);
    for (i=0;i<p->services;i++) add(&plan,BWI_REGISTER_SERVICE,i);
    add(&plan,BWI_OBSERVE_HANDOFF,0);
    if (plan.count > BWI_STEPS) return 0;
    *out=plan;
    return 1;
}
static int
line(char *buffer, size_t *used, const char *format, ...)
{
    int n;
    va_list args;
    va_start(args,format);
    n=vsnprintf(buffer+*used,BWI_PLAN_MAX-*used,format,args);
    va_end(args);
    if (n<0 || (size_t)n>=BWI_PLAN_MAX-*used) return 0;
    *used+=(size_t)n;
    return 1;
}
int
bwi_plan_render(const struct bwi_profile *p, const struct bwi_plan *given,
    char *out, size_t capacity, size_t *length)
{
    struct bwi_plan plan;
    char buffer[BWI_PLAN_MAX];
    size_t used=0,i;
    if (!length) return 0;
    *length=0;
    if (!out || !given || !bwi_plan_build(p,&plan) || given->count!=plan.count) return 0;
    for (i=0;i<plan.count;i++) if (given->step[i].operation!=plan.step[i].operation || given->step[i].index!=plan.step[i].index) return 0;
    if (!line(buffer,&used,"UNVERIFIED INSTALLER EFFECT PLAN - NO OPERATIONS AUTHORIZED\n"
        "Project %s; OpenBSD %s %s %s; handoff %s\n",p->project,p->release,p->architecture,p->firmware,p->handoff)) return 0;
    for (i=0;i<p->artifacts;i++) if (!line(buffer,&used,"Artifact %s %s %s SHA256 %s\n",
        p->artifact[i].id,p->artifact[i].kind,p->artifact[i].file,p->artifact[i].sha256)) return 0;
    if (!line(buffer,&used,"SelectRelease: authenticate complete original artifact closure\n"
        "ConfirmFreshTarget: actual exclusive target(s) and complete destruction confirmation required\n"
        "InstallStock: fixed qualified driver; selected sets/packages only\n")) return 0;
    for (i=0;i<p->principals;i++) if (!line(buffer,&used,"CreatePrincipal %s disabled-login; resolve/verify actual target UID/GID\n",p->principal[i].name)) return 0;
    for (i=0;i<p->files;i++) if (!line(buffer,&used,"PlaceVerified %s:%s -> %s %s:%s %s exclusive\n",
        p->file[i].artifact,p->file[i].member,p->file[i].target,p->file[i].owner,p->file[i].group,p->file[i].mode)) return 0;
    for (i=0;i<p->inputs;i++) if (!line(buffer,&used,"BindPrivateInput %s -> %s root:%s %s; confidential bytes separately bound\n",
        p->input[i].name,p->input[i].target,p->input[i].group,p->input[i].mode)) return 0;
    if (!line(buffer,&used,"ConfigurePolicy management=%s ssh=%s network=%s\nConfigureObservability log=%s health=%s\n",
        p->policy.management,p->policy.ssh,p->policy.network,p->observability.log,p->observability.health)) return 0;
    for (i=0;i<p->services;i++) if (!line(buffer,&used,"RegisterService %s entry=%s template=%s; no supplied argv/hooks\n",
        p->service[i].name,p->service[i].entry,p->service[i].template)) return 0;
    if (!line(buffer,&used,"ObserveAndHandoff %s: actual installed state/ownership readback; first-boot acceptance separate, not traffic readiness\n",p->handoff) || used>capacity) return 0;
    memcpy(out,buffer,used);
    *length=used;
    return 1;
}
