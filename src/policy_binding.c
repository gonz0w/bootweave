/* SPDX-License-Identifier: BSD-3-Clause */
/* Pure identity binding. A match grants no authentication or operation. */
#include "policy_binding.h"
#include "auth_input.h"
#include "auth_material.h"
#include "private_span.h"
#include <string.h>

typedef char workspace_bound[(2 * sizeof(struct bw_plan) +
    sizeof(struct bw_policy_request) + 6 * sizeof(struct span) + 64 <= BW_POLICY_COMMON_WORKSPACE) ? 1 : -1];

static int
disjoint(const struct bw_policy_request *r, struct bw_policy_observation *out)
{
    struct span spans[6];
    if (!r || !out || !range(r, sizeof(*r), &spans[0]) ||
        !range(out, sizeof(*out), &spans[1]) ||
        !range(&bw_compiled_policy, sizeof(bw_compiled_policy), &spans[2])) return 0;
    /* Establish request/output/policy disjointness before reading request fields. */
    if (!separate(spans, 3)) return 0;
    if (!range(r->key, r->key_length, &spans[3]) ||
        !range(r->definition, r->definition_length, &spans[4]) ||
        !range(r->payload, r->payload_length, &spans[5])) return 0;
    return separate(spans, 6);
}

static int
canonical(const char *s, size_t n)
{
    size_t i = 0;
    while (i < n && s[i]) i++;
    if (!i || i == n) return 0;
    for (; i < n; i++) if (s[i]) return 0;
    return 1;
}

static int
policy_valid(void)
{
    struct bw_plan p = {{0}, {0}, {0}, {0}, {0}, {0}};
    if (bw_compiled_policy.profile != BW_POLICY_READONLY_V1 ||
        !bw_signify_public_key(bw_compiled_policy.key, bw_compiled_policy.key_length) ||
        !canonical(bw_compiled_policy.project_id, sizeof(p.id)) ||
        memcmp(bw_compiled_policy.release, "7.9", sizeof(p.release)) ||
        memcmp(bw_compiled_policy.architecture, "amd64", sizeof(p.architecture))) return 0;
    memcpy(p.id, bw_compiled_policy.project_id, sizeof(p.id));
    memcpy(p.release, bw_compiled_policy.release, sizeof(p.release));
    memcpy(p.architecture, bw_compiled_policy.architecture, sizeof(p.architecture));
    p.name[0] = p.file[0] = 'x';
    memset(p.sha256, '0', 64);
    return bw_validate(&p);
}

int
bw_policy_bind(const struct bw_policy_request *r, struct bw_policy_observation *out)
{
    static const char digits[] = "0123456789abcdef";
    struct bw_plan p;
    unsigned char digest[32];
    size_t offset, i;
    if (!disjoint(r, out)) return 0;
    memset(out, 0, sizeof(*out));
    out->cause = BW_PB_ARGUMENT;
    if (!r->key_length || r->key_length > BW_POLICY_KEY_MAX ||
        !r->definition_length || r->definition_length > BW_AUTH_DEFINITION_MAX ||
        !r->payload_length || r->payload_length > BW_AUTH_PAYLOAD_MAX) return 0;
    out->cause = BW_PB_POLICY;
    if (!policy_valid()) return 0;
    out->cause = BW_PB_KEY;
    if (r->key_length != bw_compiled_policy.key_length ||
        memcmp(r->key, bw_compiled_policy.key, r->key_length)) return 0;
    out->cause = BW_PB_HASH_UNAVAILABLE;
    if (!bw_policy_sha256(r->definition, r->definition_length, digest)) return 0;
    out->cause = BW_PB_DEFINITION;
    if (memcmp(digest, bw_compiled_policy.definition_sha256, sizeof(digest))) return 0;
    out->cause = BW_PB_SYNTAX;
    if (!bw_parse(r->definition, r->definition_length, &p, &offset)) return 0;
    out->cause = BW_PB_TUPLE;
    if (memcmp(p.id, bw_compiled_policy.project_id, sizeof(p.id)) ||
        memcmp(p.release, bw_compiled_policy.release, sizeof(p.release)) ||
        memcmp(p.architecture, bw_compiled_policy.architecture, sizeof(p.architecture))) return 0;
    out->cause = BW_PB_HASH_UNAVAILABLE;
    if (!bw_policy_sha256(r->payload, r->payload_length, digest)) return 0;
    out->cause = BW_PB_PAYLOAD;
    for (i = 0; i < sizeof(digest); i++)
        if (p.sha256[2 * i] != digits[digest[i] >> 4] ||
            p.sha256[2 * i + 1] != digits[digest[i] & 15]) return 0;
    out->plan = p;
    out->cause = BW_PB_NONE;
    return 1;
}
