/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_PRIVATE_CLI_H
#define BOOTWEAVE_PRIVATE_CLI_H
/* Valid main argc/argv only. Candidate dispatcher is not in the product link. */
int bw_legacy_cli(int, char **);
int bw_entry_cli(int, char **);
#endif
