/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_AUTHENTICATED_REVIEW_H
#define BOOTWEAVE_AUTHENTICATED_REVIEW_H
#include "auth_input.h"
#include "content_review.h"
#include "signature_verify.h"

enum bw_authenticated_state { BW_AR_EMPTY, BW_AR_WORKING, BW_AR_FINISHED };
enum bw_authenticated_cause { BW_AR_NONE, BW_AR_ARGUMENT, BW_AR_POLICY,
    BW_AR_MATERIAL, BW_AR_SIGNATURE, BW_AR_CONTENT, BW_AR_FORMAT,
    BW_AR_CANCEL, BW_AR_CLOCK, BW_AR_DEADLINE, BW_AR_OUTPUT };
enum bw_authenticated_result { BW_AUTH_NOT_TAKEN = -2, BW_AUTH_UNCERTAIN = -1,
    BW_AUTH_REJECTED, BW_AUTH_COMPLETE };
struct bw_authenticated_observation {
    enum bw_authenticated_state state;
    enum bw_authenticated_cause cause;
    enum bw_material_cause material_cause;
    unsigned final_boundary_failed, output_attempted;
    size_t output_bytes;
    struct bw_signature_observation signature;
    struct bw_content_observation content;
};
/* Exclusive initially-zero result; genuine RESERVED material and immutable
 * snapshot. Numeric preflight/reuse refusal has no writes or resource effects.
 * Every taken ordinary path consumes ownership once; observations grant none.
 * COMPLETE publishes memory only, no target/operation or native qualification.
 * Exact range, cleanup, original budget and future output gates: design contract.
 */
enum bw_authenticated_result bw_authenticated_review(struct bw_verifier_material *,
    const struct bw_auth_snapshot *, char *, size_t,
    struct bw_authenticated_observation *);
/* Closed fixed-fd1 sibling; no prior result/report/deadline/fd selector.
 * Trusted startup/capture owns an immutable snapshot, original flag and genuine
 * reservation; fixed fd1 remains exclusively owned, writable/nonblocking and
 * distinct from all original inputs. Caller supplies native hard limits and
 * confinement. This sibling does not change fd1 flags or close fd1. Positive write counts
 * are private observations, not delivery/durability or operation authority.
 * Failure after any write attempt is UNCERTAIN even when zero bytes were counted.
 */
enum bw_authenticated_result bw_authenticated_output(struct bw_verifier_material *,
    const struct bw_auth_snapshot *, struct bw_authenticated_observation *);
#endif
