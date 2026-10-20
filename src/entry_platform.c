/* SPDX-License-Identifier: BSD-3-Clause */
#include "private_start_material.h"
#include "private_entry.h"
/* Native entry remains unqualified; always refuse. */
int
bw_entry_boundary(const struct bw_verifier_material *material)
{
    (void)material;
    return -1;
}

int
bw_entry_available(void)
{
    return 0;
}
