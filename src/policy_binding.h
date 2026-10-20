/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_POLICY_BINDING_H
#define BOOTWEAVE_POLICY_BINDING_H
#include "plan.h"
#include <stddef.h>

#define BW_POLICY_KEY_MAX 1024
#define BW_POLICY_COMMON_WORKSPACE 1024
/* Closed resource profile: adopted read-only restricted ustar; not a selector. */
#define BW_POLICY_READONLY_V1 1
struct bw_policy_definition {
    unsigned profile;
    size_t key_length;
    unsigned char key[BW_POLICY_KEY_MAX], definition_sha256[32];
    char project_id[65], release[4], architecture[6];
};
/* Trusted linked data only. Never chosen/modified by project or operator input.
 * Future entry makes one disjoint immutable key copy for all verification stages.
 */
extern const struct bw_policy_definition bw_compiled_policy;
struct bw_policy_request {
    const unsigned char *key, *definition, *payload;
    size_t key_length, definition_length, payload_length;
};
enum bw_policy_cause { BW_PB_NONE, BW_PB_ARGUMENT, BW_PB_POLICY, BW_PB_KEY,
    BW_PB_HASH_UNAVAILABLE, BW_PB_DEFINITION, BW_PB_SYNTAX, BW_PB_TUPLE,
    BW_PB_PAYLOAD };
struct bw_policy_observation { enum bw_policy_cause cause; struct bw_plan plan; };
/* 1 means policy/digest match ONLY: no signature, structure or effect authority.
 * Valid immutable spans/request/result/policy are pairwise disjoint. Invalid
 * ranges (including positive-length NULL), overflow, overlap or NULL request/out
 * reject without writing. Valid empty/over-cap spans clear disjoint output+ARG;
 * other failures clear it+fixed cause, never a partial plan. Caller owns mapped
 * storage/lifetime and checks cancellation/original budget before/after this pure
 * stage; future hard timers remain mandatory. No cache or runtime policy API.
 */
int bw_policy_bind(const struct bw_policy_request *, struct bw_policy_observation *);
/* Fixed base-libc native adapter; separately linked Mac fixture adapter only. */
int bw_policy_sha256(const unsigned char *, size_t, unsigned char [32]);
#endif
