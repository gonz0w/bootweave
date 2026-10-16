/* SPDX-License-Identifier: BSD-3-Clause */
#include "plan.h"

/* Private portable harness: no production confinement or native claim. */
int
main(void)
{
    if (!bw_start())
        return 4;
    return bw_run();
}
