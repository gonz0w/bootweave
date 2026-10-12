/* SPDX-License-Identifier: BSD-3-Clause */
#include "fixture.h"
#include <assert.h>
#include <stdio.h>
int
main(void)
{
    struct handoff_snapshot a = fixture(), b = a;
    size_t i;
    assert(handoff_same(&a, &b));
    for (i = 0; i < sizeof(b.uid); i++) {
        b = a; b.uid[i] ^= 1; assert(!handoff_same(&a, &b));
    }
    b = a; b.label.magic ^= 1; assert(!handoff_same(&a, &b));
    b = a; b.label.magic2 ^= 1; assert(!handoff_same(&a, &b));
    b = a; b.label.version++; assert(!handoff_same(&a, &b));
    b = a; b.label.count++; assert(!handoff_same(&a, &b));
    b = a; b.label.sectors++; assert(!handoff_same(&a, &b));
    b = a; b.label.sector_bytes++; assert(!handoff_same(&a, &b));
    for (i = 0; i < a.label.count; i++) {
        b = a; b.label.partition[i].offset++; assert(!handoff_same(&a, &b));
        b = a; b.label.partition[i].size++; assert(!handoff_same(&a, &b));
    }
    puts("snapshot comparison cases passed");
    return 0;
}
