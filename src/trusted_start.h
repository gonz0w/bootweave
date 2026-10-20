/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_TRUSTED_START_H
#define BOOTWEAVE_TRUSTED_START_H
/* Process guard only; failure is terminal, no auth or confinement. */
int bw_process_guard(void);
#endif
