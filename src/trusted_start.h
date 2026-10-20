/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_TRUSTED_START_H
#define BOOTWEAVE_TRUSTED_START_H
#include "auth_material.h"
/* Process guard only; failure is terminal, no auth or confinement. */
int bw_process_guard(void);
/* Closed startup/reservation lifetime; no caller clock or cancellation source.
 * Exclusive EMPTY material only. Guard failure leaves it untouched and is
 * terminal; reservation failure retains its own cleanup observations.
 * No input takeover, authentication or confinement. See trusted-entry-output.md.
 */
enum bw_material_result bw_start_reserve(struct bw_verifier_material *);
#endif
