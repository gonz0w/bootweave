/* SPDX-License-Identifier: BSD-3-Clause */
#include "private_install.h"

/* All-platform normal refusal. No syscalls, root, target or artifact acquisition;
 * no caller switch or alternate path. Exact native actor is not qualified. */
enum bwi_observation
bwi_native_admit(const struct bwi_profile *p, const struct bwi_plan *plan,
    const char *review, size_t size, struct bwi_owner **owner)
{
    (void)p; (void)plan; (void)review; (void)size;
    *owner=NULL;
    return BWI_NO_EFFECT;
}
enum bwi_observation bwi_native_preflight(struct bwi_owner *o)
{ (void)o; return BWI_UNKNOWN; }
int bwi_native_live(struct bwi_owner *o)
{ (void)o; return -1; }
enum bwi_observation bwi_native_intent(struct bwi_owner *o, const struct bwi_step *s)
{ (void)o; (void)s; return BWI_UNKNOWN; }
enum bwi_observation bwi_native_step(struct bwi_owner *o, const struct bwi_step *s)
{ (void)o; (void)s; return BWI_UNKNOWN; }
enum bwi_observation bwi_native_release_untouched(struct bwi_owner *o)
{ (void)o; return BWI_UNKNOWN; }
void bwi_native_retain(struct bwi_owner *o) { (void)o; }
