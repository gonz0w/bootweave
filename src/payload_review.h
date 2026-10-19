/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_PAYLOAD_REVIEW_H
#define BOOTWEAVE_PAYLOAD_REVIEW_H
#include <stddef.h>
struct bw_payload_summary { size_t members, files, content_bytes; };
/* Internal pure model: 0 is structure-only success, never authentication.
 * Immutable input and output storage must be valid/disjoint; alias/overflow
 * rejects without writing. Other rejections zero the disjoint summary.
 * No pathname, descriptor, allocator, subprocess, target or privilege API.
 */
int bw_payload_structure_review(const unsigned char *, size_t,
    struct bw_payload_summary *);
#endif
