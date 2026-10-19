/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_AUTH_INPUT_H
#define BOOTWEAVE_AUTH_INPUT_H
#include <stddef.h>

#define BW_AUTH_DEFINITION_MAX 16384
#define BW_AUTH_PAYLOAD_MAX 1048576
#define BW_AUTH_SIGNATURE_MAX 16384
struct bw_auth_snapshot {
    unsigned char definition[BW_AUTH_DEFINITION_MAX + 1];
    unsigned char payload[BW_AUTH_PAYLOAD_MAX + 1];
    unsigned char signature[BW_AUTH_SIGNATURE_MAX + 1];
    size_t definition_length, payload_length, signature_length;
};
enum bw_capture { BW_CAPTURE_UNCERTAIN = -1, BW_CAPTURE_REJECTED, BW_CAPTURED };
/* Internal, unlinked capture only; no authentication assertion.
 * Caller supplies valid exclusive storage and owns fds 0/3/4 and their OFDs.
 * Observes outputs 1/2, takes inputs even on failure; closes each once.
 * Failure clears storage. Two-second deadline is cooperative; the future entry
 * must install hard timers/confinement first. No pathname/output/process API.
 */
enum bw_capture bw_auth_capture(struct bw_auth_snapshot *);
#endif
