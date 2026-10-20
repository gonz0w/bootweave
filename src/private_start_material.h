/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_PRIVATE_START_MATERIAL_H
#define BOOTWEAVE_PRIVATE_START_MATERIAL_H
#include "auth_material.h"
/* Implementation bridge only. The closed startup wrapper supplies its own
 * normalized pre-arm origin and permanent private signal flag. Neither is a
 * caller-selectable token. Do not expose this in the material API or renew it.
 */
enum bw_material_result bw_material_reserve_started(const struct timespec *,
    const volatile sig_atomic_t *, struct bw_verifier_material *);
#endif
