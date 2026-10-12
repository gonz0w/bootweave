/* SPDX-License-Identifier: BSD-3-Clause */
/* Separate console fixture: no installer execution or writable backend. */
#include "fixture.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
static int changed, unavailable, late_input;
static int
observe(struct handoff_snapshot *s)
{
    *s = fixture();
    if (changed) s->uid[0] ^= 1;
    if (late_input) {
        const struct timespec pause = { 0, 250000000 };
        const char marker[] = "FIXTURE OBSERVING\n";
        if (write(STDOUT_FILENO, marker, sizeof(marker) - 1) != sizeof(marker) - 1 ||
            nanosleep(&pause, NULL)) return STORAGE_INVALID;
    }
    if (unavailable) return STORAGE_INVALID;
    return STORAGE_VALID;
}
int
main(int argc, char **argv)
{
    struct handoff_snapshot s = fixture();
    int result;
    if (argc == 2 && !strcmp(argv[1], "changed")) changed = 1;
    else if (argc == 2 && !strcmp(argv[1], "unavailable")) unavailable = 1;
    else if (argc == 2 && !strcmp(argv[1], "late-input")) late_input = 1;
    else if (argc != 1) return 3;
    result = handoff_console(&s, observe);
    if (!result) puts("FIXTURE CONFIRMED");
    return result;
}
