/* SPDX-License-Identifier: BSD-3-Clause */
#include "validate.h"
#include <stdio.h>
#include <string.h>

static unsigned int checks, failures;
static void
check(int passed)
{
    checks++;
    if (!passed) failures++;
}

int
main(void)
{
    static const struct { const char *data; enum storage_result want; } cases[] = {
        { "", STORAGE_NONE }, { "rd0:,cd0:", STORAGE_NONE },
        { "sd0:", STORAGE_VALID },
        { "rd0:,sd0:0123456789abcdef,cd0:", STORAGE_VALID },
        { "sd0:,sd0:", STORAGE_INVALID }, { "cd0:,cd0:", STORAGE_INVALID },
        { "sd0:,wd0:", STORAGE_INVALID }, { "sd1:", STORAGE_INVALID },
        { "sd0:,", STORAGE_INVALID }, { ",sd0:", STORAGE_INVALID },
        { "sd0", STORAGE_INVALID }, { "sd0:0", STORAGE_INVALID },
        { "sd0:0123456789abcdeg", STORAGE_INVALID },
        { "sd0:0123456789ABCDEF", STORAGE_INVALID },
        { "../:", STORAGE_INVALID }, { "sd0:\033[2J", STORAGE_INVALID },
        { "sd0:$(id)", STORAGE_INVALID }, { "sd0:\n", STORAGE_INVALID },
        { "sd0:\200", STORAGE_INVALID }, { "sd0:;", STORAGE_INVALID }
    };
    struct storage_label good, bad;
    unsigned char uid[8];
    uint64_t bytes = 0;
    char oversized[STORAGE_DISCOVERY_MAX + 1];
    size_t i;
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
        check(storage_discovery(cases[i].data, strlen(cases[i].data) + 1, uid) == cases[i].want);
    check(storage_discovery("sd0:", 0, uid) == STORAGE_INVALID);
    check(storage_discovery("sd0:", 4, uid) == STORAGE_INVALID);
    check(storage_discovery("sd0:\0cd0:", 10, uid) == STORAGE_INVALID);
    memset(oversized, 'a', sizeof(oversized));
    oversized[sizeof(oversized) - 1] = '\0';
    check(storage_discovery(oversized, sizeof(oversized), uid) == STORAGE_INVALID);
    check(storage_discovery("sd0:0123456789abcdef", 21, uid) == STORAGE_VALID &&
        uid[0] == 0x01 && uid[7] == 0xef);
    memset(&good, 0, sizeof(good));
    good.magic = good.magic2 = STORAGE_MAGIC;
    good.version = 1; good.count = 16; good.sector_bytes = 512;
    good.sectors = STORAGE_EXPECTED_BYTES / 512;
    good.partition[2].size = good.sectors;
    check(storage_label_check(&good, &bytes) == STORAGE_VALID && bytes == STORAGE_EXPECTED_BYTES);
    good.partition[15].offset = good.sectors - 1;
    good.partition[15].size = 1;
    check(storage_label_check(&good, &bytes) == STORAGE_VALID);
    for (i = 0; i < 15; i++) {
        bad = good;
        switch (i) {
        case 0: bad.magic = 0; break;
        case 1: bad.magic2 = 0; break;
        case 2: bad.version = 0; break;
        case 3: bad.count = 2; break;
        case 4: bad.count = 17; break;
        case 5: bad.sector_bytes = 0; break;
        case 6: bad.sector_bytes = 256; break;
        case 7: bad.sector_bytes = 513; break;
        case 8: bad.sector_bytes = 131072; break;
        case 9: bad.sectors = UINT64_MAX; break;
        case 10: bad.sectors = 0; break;
        case 11: bad.partition[2].offset = 1; break;
        case 12: bad.partition[2].size--; break;
        case 13: bad.partition[15].offset++; break;
        default: bad.partition[15].size = UINT64_MAX; break;
        }
        bytes = 123;
        check(storage_label_check(&bad, &bytes) == STORAGE_INVALID && bytes == 0);
    }
    memset(&bad, 0, sizeof(bad));
    bad = good; bad.sectors = UINT64_C(2147483648) / 512;
    bad.partition[2].size = bad.sectors;
    bad.partition[15].offset = bad.partition[15].size = 0;
    check(storage_label_check(&bad, &bytes) == STORAGE_INVALID);
    good.sector_bytes = 4096; good.sectors = STORAGE_EXPECTED_BYTES / 4096;
    good.partition[2].size = good.sectors;
    good.partition[15].offset = good.partition[15].size = 0;
    check(storage_label_check(&good, &bytes) == STORAGE_VALID);
    printf("Validator checks: %u; failures: %u\n", checks, failures);
    return failures != 0;
}
