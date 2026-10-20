/* SPDX-License-Identifier: BSD-3-Clause */
#include "content_review.h"
#include "auth_input.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct span { uintptr_t start, end; };
struct workspace {
    struct bw_policy_observation policy;
    struct bw_payload_summary summary;
    char text[2048];
};
/* Includes the nested preflight's spans and scalar allowance. Existing parser,
 * policy and inspector allowances are additional, not hidden by this bound. */
typedef char local_bound[(sizeof(struct workspace) + 7 * sizeof(struct span) +
    256 <= 4096) ? 1 : -1];
typedef char application_bound[(sizeof(struct bw_auth_snapshot) +
    BW_CONTENT_OUTPUT_MAX + sizeof(struct bw_content_observation) +
    4096 + 2048 + 34816 + 24576 <= 2 * 1024 * 1024) ? 1 : -1];

static int
range(const void *p, size_t n, struct span *s)
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

static int
disjoint(const struct bw_policy_request *r, char *text, size_t capacity,
    struct bw_content_observation *out)
{
    struct span s[7];
    if (!r || !out || !range(r, sizeof(*r), &s[0]) ||
        !range(text, capacity, &s[1]) || !range(out, sizeof(*out), &s[2]) ||
        !range(&bw_compiled_policy, sizeof(bw_compiled_policy), &s[3]) ||
        !separate(s, 4)) return 0;
    /* Do not dereference request until its fixed peers are established. */
    return range(r->key, r->key_length, &s[4]) &&
        range(r->definition, r->definition_length, &s[5]) &&
        range(r->payload, r->payload_length, &s[6]) && separate(s, 7);
}

int
bw_content_review(const struct bw_policy_request *r, char *text, size_t capacity,
    struct bw_content_observation *out)
{
    struct workspace w;
    int length;
    if (!disjoint(r, text, capacity, out)) return 0;
    memset(out, 0, sizeof(*out));
    out->cause = BW_CR_ARGUMENT;
    if (!capacity || capacity > BW_CONTENT_OUTPUT_MAX) return 0;
    memset(text, 0, capacity);
    if (!r->key_length || r->key_length > BW_POLICY_KEY_MAX ||
        !r->definition_length || r->definition_length > BW_AUTH_DEFINITION_MAX ||
        !r->payload_length || r->payload_length > BW_AUTH_PAYLOAD_MAX) return 0;
    out->cause = BW_CR_POLICY;
    if (!bw_policy_bind(r, &w.policy)) {
        out->policy_cause = w.policy.cause;
        return 0;
    }
    out->cause = BW_CR_STRUCTURE;
    if (bw_payload_structure_review(r->payload, r->payload_length, &w.summary)) return 0;
    out->cause = BW_CR_FORMAT;
    length = snprintf(w.text, sizeof(w.text),
        "UNAUTHENTICATED CONTENT REVIEW - NO OPERATIONS AUTHORIZED\n"
        "Compiled policy: matched\n"
        "Definition SHA-256: matched compiled pin\n"
        "Payload SHA-256: matched definition\n"
        "Archive structure: complete restricted ustar\n"
        "Signature authentication: not established\n"
        "Project ID (policy-bound): %s\n"
        "Project name (supplied): %s\n"
        "Requested release (policy-bound): %s\n"
        "Requested architecture (policy-bound): %s\n"
        "Payload file (supplied label): %s\n"
        "Archive members: %zu\n"
        "Regular files: %zu\n"
        "Content bytes: %zu\n"
        "Target: unselected; no device identity collected\n"
        "Operations authorized: none\n"
        "Operations performed: none\n"
        "Operations independently observed: none\n",
        w.policy.plan.id, w.policy.plan.name, w.policy.plan.release,
        w.policy.plan.architecture, w.policy.plan.file,
        w.summary.members, w.summary.files, w.summary.content_bytes);
    if (length < 0 || (size_t)length >= sizeof(w.text) ||
        (size_t)length >= capacity) return 0;
    memcpy(text, w.text, (size_t)length + 1);
    out->plan = w.policy.plan;
    out->summary = w.summary;
    out->length = (size_t)length;
    out->cause = BW_CR_NONE;
    return 1;
}
