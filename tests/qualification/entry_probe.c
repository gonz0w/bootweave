/* SPDX-License-Identifier: BSD-3-Clause */
/* Qualification source only. Never a product entry or readiness override. */
#define _POSIX_C_SOURCE 200809L
#include "trusted_start.h"
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

#if defined(__OpenBSD__) || defined(BW_NQ_SOURCE_CHECK)
static int
probe(void)
{
    const struct bw_entry_observation *entry;
    const struct bw_authenticated_observation *auth;
    const struct bw_signature_observation *signature;
    enum bw_authenticated_result result;
    char line[512];
    int length;

    result = bw_entry_output();
    entry = bw_entry_observe();
    if (entry == NULL || result != entry->result) return 70;
    auth = &entry->authenticated;
    signature = &auth->signature;
    /* These are observations, never instructions to retry or recover custody.
     * fd2 is the fixture's exclusive writable nonblocking receipt pipe. A
     * partial write is a failed receipt, not an excuse to resend any prefix. */
    length = snprintf(line, sizeof(line),
        "NQ_ENTRY %d %d %d %d %u %u %u "
        "%d %d %d %u %u %zu "
        "%d %d %jd %u %u %u %u %u %u %u %u %u %u %u %u %d\n",
        (int)entry->result, (int)entry->state,
        (int)entry->cause, (int)entry->capture,
        entry->input_close_errors, entry->input_unknown_closes,
        entry->final_boundary_failed,
        (int)auth->state, (int)auth->cause, (int)auth->material_cause,
        auth->final_boundary_failed, auth->output_attempted, auth->output_bytes,
        (int)signature->state, (int)signature->cause, (intmax_t)signature->pid,
        signature->group_owned, signature->launch_granted,
        signature->stop_attempted, signature->wait_consumed,
        signature->wait_lost, signature->diagnostic_bytes,
        signature->close_errors, signature->unknown_closes,
        signature->final_boundary_failed, signature->material_close_errors,
        signature->material_unknown_closes, signature->cleanup_budget_failed,
        signature->wait_status);
    if (length < 0 || (size_t)length >= sizeof(line) ||
        write(2, line, (size_t)length) != length) return 70;
    if (result == BW_AUTH_COMPLETE) return 0;
    if (result == BW_AUTH_REJECTED) return 3;
    return result == BW_AUTH_UNCERTAIN ? 5 : 4;
}
#endif

int
main(int argc, char **argv)
{
    (void)argv;
#ifdef __OpenBSD__
    if (argc != 1) return 2;
    return probe();
#else
    (void)argc;
#ifdef BW_NQ_SOURCE_CHECK
    /* Expose the native body for host syntax checks, never host execution. */
    (void)&probe;
#endif
    return 77;
#endif
}
