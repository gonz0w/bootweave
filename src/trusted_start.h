/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_TRUSTED_START_H
#define BOOTWEAVE_TRUSTED_START_H
#include "authenticated_review.h"
/* Legacy guard/startup only: EMPTY material, terminal failure, no fd takeover. */
int bw_process_guard(void);
enum bw_material_result bw_start_reserve(struct bw_verifier_material *);
enum bw_entry_state { BW_ENTRY_EMPTY, BW_ENTRY_WORKING, BW_ENTRY_FINISHED };
enum bw_entry_cause { BW_ENTRY_NONE, BW_ENTRY_START, BW_ENTRY_INPUT,
    BW_ENTRY_UNRELATED, BW_ENTRY_MATERIAL, BW_ENTRY_CONFINEMENT,
    BW_ENTRY_CAPTURE, BW_ENTRY_AUTHENTICATED, BW_ENTRY_CANCEL,
    BW_ENTRY_CLOCK, BW_ENTRY_DEADLINE };
struct bw_entry_observation {
    enum bw_entry_state state;
    enum bw_authenticated_result result;
    enum bw_entry_cause cause;
    enum bw_capture capture;
    unsigned input_close_errors, input_unknown_closes;
    unsigned final_boundary_failed;
    struct bw_authenticated_observation authenticated;
};
/* One-shot owner of original0/3/4; normal boundary refuses. Observe only,
 * never mutate/retry. No CLI/operation authority. Lifetime/ownership:
 * docs/design/trusted-entry-output.md. */
enum bw_authenticated_result bw_entry_output(void);
const struct bw_entry_observation *bw_entry_observe(void);
#endif
