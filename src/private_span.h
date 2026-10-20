/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_PRIVATE_SPAN_H
#define BOOTWEAVE_PRIVATE_SPAN_H
#include <stddef.h>
#include <stdint.h>

struct span { uintptr_t start, end; };
static int
range(const volatile void *p, size_t n, struct span *s)
{
    s->start = (uintptr_t)p;
    if ((!p && n) || n > UINTPTR_MAX - s->start) return 0;
    s->end = s->start + n;
    return 1;
}

static int
separate(const struct span *s, size_t count)
{
    size_t i, j;
    for (i = 0; i < count; i++) for (j = 0; j < i; j++)
        if (s[i].start != s[i].end && s[j].start != s[j].end &&
            s[i].start < s[j].end && s[j].start < s[i].end) return 0;
    return 1;
}
#endif
