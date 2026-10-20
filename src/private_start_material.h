/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_PRIVATE_START_MATERIAL_H
#define BOOTWEAVE_PRIVATE_START_MATERIAL_H
#include "auth_material.h"
/* Closed implementation bridges; no caller seed or boundary selector. */
enum bw_material_result bw_material_reserve_started(const struct timespec *,
    const volatile sig_atomic_t *, struct bw_verifier_material *);
int bw_entry_boundary(const struct bw_verifier_material *);
int bw_entry_live(void);
#endif
