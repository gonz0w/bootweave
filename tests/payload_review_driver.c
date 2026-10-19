/* SPDX-License-Identifier: BSD-3-Clause */
/* Test-only byte ingress. No production CLI or signature/policy injection. */
#include "payload_review.h"
#include <stdio.h>
#include <string.h>
static unsigned char bytes[1048577], snapshot[1048577];
int main(int argc, char **argv)
{
    struct bw_payload_summary result = {99, 99, 99};
    size_t length = fread(bytes, 1, sizeof(bytes), stdin);
    int status;
    if (argc == 2 && !strcmp(argv[1], "--selftest")) {
        struct bw_payload_summary before = {7, 8, 9}, expected;
        unsigned char invalid[512] = {0};
        if (!length) return 3;
        memcpy(snapshot, bytes, length);
        if (bw_payload_structure_review(bytes, length, &expected)) return 3;
        if (!bw_payload_structure_review(invalid, sizeof(invalid), &result) ||
            result.members || result.files || result.content_bytes) return 3;
        if (bw_payload_structure_review(bytes, length, &result) ||
            result.members != expected.members || result.files != expected.files ||
            result.content_bytes != expected.content_bytes ||
            memcmp(bytes, snapshot, length)) return 3;
        result = before;
        if (!bw_payload_structure_review((const unsigned char *)&result,
            sizeof(result), &result) || memcmp(&before, &result, sizeof(result))) return 3;
        if (!bw_payload_structure_review(NULL, 0, &result) ||
            result.members || result.files || result.content_bytes) return 3;
        if (!bw_payload_structure_review(bytes, 1, NULL)) return 3;
        return 0;
    }
    if (argc != 1 || ferror(stdin)) return 3;
    status = bw_payload_structure_review(bytes, length, &result);
    if (status) return result.members || result.files || result.content_bytes ? 3 : 2;
    printf("structure-only %zu %zu %zu\n", result.members, result.files, result.content_bytes);
    return 0;
}
