/* SPDX-License-Identifier: BSD-3-Clause */
#include "private_install.h"

static int attempted;
static struct bwi_profile profile;
static struct bwi_result result;

enum bwi_status
bw_install_source(const unsigned char *raw, size_t size)
{
    if (attempted) return BWI_NOT_TAKEN;
    attempted=1;
    result.status=BWI_REJECTED;
    if (!bwi_profile_parse(raw,size,&profile)) return result.status;
    bwi_execute(&profile,&result);
    return result.status;
}
const struct bwi_result *
bw_install_observe(void)
{
    return &result;
}
