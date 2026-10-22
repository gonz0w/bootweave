/* SPDX-License-Identifier: BSD-3-Clause */
/* Separate qualification object only; never link this into the product. */
#include "private_entry.h"

int
bw_entry_boundary(const struct bw_verifier_material *material)
{
#ifdef __OpenBSD__
    return bw_entry_native(material);
#else
    (void)material;
    return -1;
#endif
}

int
bw_entry_available(void)
{
    /* Qualification does not advertise or activate a CLI. */
    return 0;
}
