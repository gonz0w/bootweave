/* SPDX-License-Identifier: BSD-3-Clause */
#include "plan.h"

/* Portable timer stimulus only; no production confinement or native claim. */
int
main(void)
{
    volatile unsigned long counter = 0;
    if (!bw_start())
        return 4;
    for (;;)
        counter++;
}
