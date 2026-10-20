/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_SIGNATURE_VERIFY_H
#define BOOTWEAVE_SIGNATURE_VERIFY_H
#include "auth_material.h"
#include <sys/resource.h>
#include <sys/types.h>

enum bw_signature_state { BW_SV_EMPTY, BW_SV_WORKING, BW_SV_FINISHED };
enum bw_signature_cause { BW_SV_NONE, BW_SV_UNSUPPORTED, BW_SV_IDENTITY,
    BW_SV_SIGNALS, BW_SV_DESCRIPTOR, BW_SV_CANCEL, BW_SV_CLOCK, BW_SV_DEADLINE,
    BW_SV_IO, BW_SV_FORK, BW_SV_GROUP, BW_SV_BOUNDARY, BW_SV_GATE,
    BW_SV_CHILD, BW_SV_OUTPUT, BW_SV_WAIT, BW_SV_STOP, BW_SV_CLOSE };
enum bw_signature_result { BW_SIGNATURE_UNCERTAIN = -1, BW_SIGNATURE_REJECTED,
    BW_SIGNATURE_ACCEPTED };
/* Exclusive one-shot internal result, initially {0}; no serialized authority.
 * Observations are private, never instructions to retry a close or signal.
 */
struct bw_signature_observation {
    enum bw_signature_state state;
    enum bw_signature_cause cause;
    pid_t pid;
    unsigned group_owned, launch_granted, stop_attempted, wait_consumed, wait_lost;
    unsigned diagnostic_bytes, close_errors, unknown_closes, final_boundary_failed;
    unsigned material_close_errors, material_unknown_closes;
    unsigned cleanup_budget_failed;
    int wait_status;
    struct rusage usage;
};
/* Consumes exclusive READY material. Result/material/flag storage is disjoint and
 * valid throughout. Trusted future entry supplies hard limits, locked exact unveil,
 * default SIGCHLD and ignored SIGPIPE; no standalone/CLI authority is provided.
 * Acceptance is detached-signature status only, never policy/digest authentication.
 */
enum bw_signature_result bw_signature_verify(struct bw_verifier_material *, struct bw_signature_observation *);

/* Fixed native adapter; separately linked modeled adapter in tests only. */
int bw_sv_supported(void);
int bw_sv_child_limits(void);
int bw_sv_child_boundary(void);
int bw_sv_parent_boundary(int);
int bw_sv_closefrom(int);
#endif
