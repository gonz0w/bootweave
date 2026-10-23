/* SPDX-License-Identifier: BSD-3-Clause */
#include "private_install.h"
#include <string.h>

static int
live(struct bwi_owner *owner, struct bwi_result *r)
{
    int state=bwi_native_live(owner);
    if (state==1) return 1;
    r->status=state<0 ? BWI_UNCERTAIN : (r->intended ? BWI_PARTIAL : BWI_REJECTED);
    return 0;
}
static int
observed(enum bwi_observation observation, struct bwi_result *r)
{
    if (observation==BWI_OBSERVED) return 1;
    r->status=observation==BWI_NO_EFFECT ? (r->intended ? BWI_PARTIAL : BWI_REJECTED) : BWI_UNCERTAIN;
    return 0;
}
void
bwi_execute(const struct bwi_profile *profile, struct bwi_result *r)
{
    struct bwi_plan plan;
    char review[BWI_PLAN_MAX];
    size_t review_size=0;
    struct bwi_owner *owner=NULL;
    enum bwi_observation admission;
    size_t i;
    if (!r) return;
    memset(r,0,sizeof(*r));
    r->status=BWI_REJECTED;
    if (!bwi_plan_build(profile,&plan) ||
        !bwi_plan_render(profile,&plan,review,sizeof(review),&review_size)) return;
    admission=bwi_native_admit(profile,&plan,review,review_size,&owner);
    if (!observed(admission,r) || !owner) {
        if (admission==BWI_OBSERVED && !owner) r->status=BWI_UNCERTAIN;
        if (owner) { r->retention_requested=1; bwi_native_retain(owner); }
        return;
    }
    /* These are actual read-only admission/confirmation boundaries, not booleans
     * copied from the profile or a prior report. No intent/write precedes them. */
    if (!live(owner,r) || !observed(bwi_native_preflight(owner),r) || !live(owner,r)) goto stop;
    for (i=0;i<plan.count;i++) {
        const struct bwi_step *step=&plan.step[i];
        r->operation=step->operation;
        r->index=step->index;
        if (!live(owner,r)) goto stop;
        if (step->operation>=BWI_INSTALL_STOCK) {
            /* Intent can itself have been partly effected. Retain from here. */
            r->intended++;
            if (!observed(bwi_native_intent(owner,step),r) || !live(owner,r)) goto stop;
        }
        r->submitted++;
        if (!observed(bwi_native_step(owner,step),r)) goto stop;
        r->observed++;
        /* Final handoff includes its last original-clock check before the
         * actual one-way transfer. Never touch a transferred target afterward. */
        if (step->operation!=BWI_OBSERVE_HANDOFF && !live(owner,r)) goto stop;
    }
    /* ObserveAndHandoff consumes/transfers the actual owner once. No following
     * userspace release/cleanup can retroactively invalidate known handoff. */
    r->status=BWI_HANDED_OFF;
    return;
stop:
    if (r->intended || r->status==BWI_UNCERTAIN) {
        r->retention_requested=1;
        bwi_native_retain(owner);
    } else if (bwi_native_release_untouched(owner)!=BWI_OBSERVED) {
        r->status=BWI_UNCERTAIN;
        r->retention_requested=1;
        bwi_native_retain(owner);
    }
}
