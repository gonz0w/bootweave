/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_PRIVATE_ENTRY_H
#define BOOTWEAVE_PRIVATE_ENTRY_H
#include "auth_material.h"
#include <sys/types.h>
/* Source candidate only; normal entry and CLI readiness still refuse. */
int bw_entry_available(void);
int bw_entry_native(const struct bw_verifier_material *);
int bw_en_identity(uid_t *);
int bw_en_unveil(const char *, const char *);
int bw_en_pledge(void);
#endif
