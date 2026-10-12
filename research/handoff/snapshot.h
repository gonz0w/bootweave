/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef HANDOFF_SNAPSHOT_H
#define HANDOFF_SNAPSHOT_H
#include "validate.h"
/* Internal research observations, never a wire format or stable disk handle. */
struct handoff_snapshot {
    struct storage_label label;
    unsigned char uid[8];
};
int handoff_same(const struct handoff_snapshot *, const struct handoff_snapshot *);
int handoff_observe(struct handoff_snapshot *);
int handoff_console(const struct handoff_snapshot *, int (*)(struct handoff_snapshot *));
int handoff_interrupted(void);
#endif
