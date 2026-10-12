/* SPDX-License-Identifier: BSD-3-Clause */
#include "validate.h"
#include <string.h>

static int
hex(unsigned char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

enum storage_result
storage_discovery(const char *data, size_t length, unsigned char uid[8])
{
    size_t at = 0, end, colon, i;
    unsigned int seen = 0, bit;
    unsigned char decoded[8];
    int high, low;

    memset(uid, 0, 8);
    if (length == 0 || length > STORAGE_DISCOVERY_MAX ||
        data[length - 1] != '\0' || memchr(data, '\0', length - 1) != NULL)
        return STORAGE_INVALID;
    if (length == 1) return STORAGE_NONE;
    while (at < length - 1) {
        for (end = at; end < length - 1 && data[end] != ','; end++) {}
        for (colon = at; colon < end && data[colon] != ':'; colon++) {}
        /* These names are the reviewed virtio research fixture only. */
        if (colon - at != 3 || colon == end) return STORAGE_INVALID;
        if (memcmp(data + at, "sd0", 3) == 0) bit = 1;
        else if (memcmp(data + at, "cd0", 3) == 0) bit = 2;
        else if (memcmp(data + at, "rd0", 3) == 0) bit = 4;
        else return STORAGE_INVALID;
        if ((seen & bit) != 0) return STORAGE_INVALID;
        seen |= bit;
        memset(decoded, 0, sizeof(decoded));
        if (end - colon != 1) {
            if (end - colon != 17) return STORAGE_INVALID;
            for (i = 0; i < 8; i++) {
                high = hex((unsigned char)data[colon + 1 + i * 2]);
                low = hex((unsigned char)data[colon + 2 + i * 2]);
                if (high < 0 || low < 0) return STORAGE_INVALID;
                decoded[i] = (unsigned char)(high * 16 + low);
            }
        }
        if (bit == 1) memcpy(uid, decoded, 8);
        if (end == length - 1) break;
        at = end + 1;
        if (at == length - 1) return STORAGE_INVALID;
    }
    return (seen & 1) != 0 ? STORAGE_VALID : STORAGE_NONE;
}

enum storage_result
storage_label_check(const struct storage_label *label, uint64_t *bytes)
{
    size_t i;
    uint32_t sector = label->sector_bytes;
    *bytes = 0;
    if (label->magic != STORAGE_MAGIC || label->magic2 != STORAGE_MAGIC ||
        label->version != 1 || label->count < 3 ||
        label->count > STORAGE_PARTITIONS || sector < 512 || sector > 65536 ||
        (sector & (sector - 1)) != 0 || label->sectors == 0 ||
        label->sectors > UINT64_MAX / sector)
        return STORAGE_INVALID;
    for (i = 0; i < label->count; i++) {
        if (label->partition[i].offset > label->sectors ||
            label->partition[i].size > label->sectors - label->partition[i].offset)
            return STORAGE_INVALID;
    }
    if (label->partition[2].offset != 0 ||
        label->partition[2].size != label->sectors ||
        label->sectors * sector != STORAGE_EXPECTED_BYTES)
        return STORAGE_INVALID;
    *bytes = label->sectors * sector;
    return STORAGE_VALID;
}
