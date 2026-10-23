/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_PRIVATE_INSTALL_H
#define BOOTWEAVE_PRIVATE_INSTALL_H
#include "install_profile.h"

/* Private fixed platform boundary: never a caller-supplied provider/vtable. */
struct bwi_owner;
enum bwi_observation { BWI_NO_EFFECT = 0, BWI_OBSERVED = 1, BWI_UNKNOWN = 2 };
enum bwi_status { BWI_NOT_TAKEN = 0, BWI_REJECTED, BWI_PARTIAL,
    BWI_UNCERTAIN, BWI_HANDED_OFF };
struct bwi_result {
    enum bwi_status status;
    size_t intended, submitted, observed;
    enum bwi_operation operation;
    size_t index;
    int retention_requested;
};
/* A future native constructor binds actual verified originals and one lifetime.
 * Every required capability, target/material/resource proof precedes writes.
 * The normal source implementation now refuses without any native effect. */
enum bwi_observation bwi_native_admit(const struct bwi_profile *,
    const struct bwi_plan *, const char *, size_t, struct bwi_owner **);
enum bwi_observation bwi_native_preflight(struct bwi_owner *);
int bwi_native_live(struct bwi_owner *);
enum bwi_observation bwi_native_intent(struct bwi_owner *, const struct bwi_step *);
enum bwi_observation bwi_native_step(struct bwi_owner *, const struct bwi_step *);
enum bwi_observation bwi_native_release_untouched(struct bwi_owner *);
void bwi_native_retain(struct bwi_owner *);

void bwi_execute(const struct bwi_profile *, struct bwi_result *);
/* Not linked by production PLAN_SOURCES. Once-only; observations grant nothing. */
enum bwi_status bw_install_source(const unsigned char *, size_t);
const struct bwi_result *bw_install_observe(void);
#endif
