/* SPDX-License-Identifier: BSD-3-Clause */
#include "private_start_material.h"
/* Native entry remains unqualified; always refuse. */
int
bw_entry_boundary(const struct bw_verifier_material *material)
{
    (void)material;
    return -1;
}
