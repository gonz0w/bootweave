/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef HANDOFF_FIXTURE_H
#define HANDOFF_FIXTURE_H
#include "snapshot.h"
#include <string.h>
static struct handoff_snapshot
fixture(void)
{
    struct handoff_snapshot s;
    size_t i;
    memset(&s, 0, sizeof(s));
    s.label.magic = s.label.magic2 = STORAGE_MAGIC;
    s.label.version = 1; s.label.count = 3;
    s.label.sector_bytes = 512; s.label.sectors = STORAGE_EXPECTED_BYTES / 512;
    s.label.partition[2].size = s.label.sectors;
    for (i = 0; i < sizeof(s.uid); i++) s.uid[i] = (unsigned char)(i * 34 + 1);
    return s;
}
#endif
