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
enum bw_capture { BW_CAPTURE_NOT_TAKEN = -2, BW_CAPTURE_UNCERTAIN,
    BW_CAPTURE_REJECTED, BW_CAPTURED };
/* Internal, unlinked; no authentication or operation authority.
 * Exclusive valid storage/OFDs required. Full ownership, clock, cancellation
 * and refusal contract: docs/design/authenticated-io.md. */
enum bw_capture bw_auth_capture(struct bw_auth_snapshot *);
struct bw_verifier_material;
enum bw_capture bw_auth_capture_reserved(struct bw_verifier_material *,
    struct bw_auth_snapshot *);
#endif
