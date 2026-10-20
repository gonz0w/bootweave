/* SPDX-License-Identifier: BSD-3-Clause */
#include "private_cli.h"
#include "private_entry.h"
#include "trusted_start.h"
#include <string.h>

int
bw_entry_cli(int argc, char **argv)
{
    enum bw_authenticated_result result;
    if (argc != 2 || strcmp(argv[1], "review") != 0)
        return bw_legacy_cli(argc, argv);
    /* Normal readiness is always zero. Never enter legacy timers or output. */
    if (bw_entry_available() != 1) return 4;
    result = bw_entry_output();
    if (result == BW_AUTH_COMPLETE) return 0;
    if (result == BW_AUTH_REJECTED) return 3;
    return result == BW_AUTH_UNCERTAIN ? 5 : 4;
}
