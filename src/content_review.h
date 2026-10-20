/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_CONTENT_REVIEW_H
#define BOOTWEAVE_CONTENT_REVIEW_H
#include "policy_binding.h"
#include "payload_review.h"

#define BW_CONTENT_OUTPUT_MAX 65536
enum bw_content_cause { BW_CR_NONE, BW_CR_ARGUMENT, BW_CR_POLICY,
    BW_CR_STRUCTURE, BW_CR_FORMAT };
struct bw_content_observation {
    enum bw_content_cause cause;
    enum bw_policy_cause policy_cause;
    size_t length;
    struct bw_plan plan;
    struct bw_payload_summary summary;
};
/* Internal pure composition; 1 publishes a complete model/report, 0 refuses.
 * Signature authentication is ALWAYS unestablished. No operation is authorized.
 * Request/result/output/policy/key/definition/payload must be mapped, immutable
 * inputs with disjoint exclusive storage for this call. Invalid numeric ranges,
 * positive-length NULL, overlap or NULL request/result refuse WITHOUT writes.
 * Other failure clears result except fixed causes and zeros valid-capacity
 * output (1..65536 including NUL); invalid-capacity output is untouched.
 * Length excludes NUL. Caller checks cancellation/original remaining budget
 * before/after and supplies hard timers; no reusable authentication token.
 */
int bw_content_review(const struct bw_policy_request *, char *, size_t,
    struct bw_content_observation *);
#endif
