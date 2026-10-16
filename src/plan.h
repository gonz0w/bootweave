/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_PLAN_H
#define BOOTWEAVE_PLAN_H

#include <stddef.h>

#define BW_INPUT_MAX 16384
#define BW_OUTPUT_MAX 4096

struct bw_plan {
    char id[65];
    char name[81];
    char release[4];
    char architecture[6];
    char file[129];
    char sha256[65];
};

int bw_parse(const unsigned char *, size_t, struct bw_plan *, size_t *);
int bw_validate(const struct bw_plan *);
int bw_render(const struct bw_plan *, char *, size_t, size_t *);
int bw_confine(void);
int bw_start(void);
int bw_write(int, const char *, size_t);
int bw_run(void);

#endif
