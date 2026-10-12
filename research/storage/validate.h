/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef STORAGE_VALIDATE_H
#define STORAGE_VALIDATE_H

#include <stddef.h>
#include <stdint.h>

#define STORAGE_DISCOVERY_MAX 4096
#define STORAGE_PARTITIONS 16
#define STORAGE_MAGIC UINT32_C(0x82564557)
#define STORAGE_EXPECTED_BYTES UINT64_C(8589934592)

/* Internal test model, not an ABI, persisted format or installer interface. */
struct storage_label {
    uint32_t magic, magic2, sector_bytes;
    uint16_t version, count;
    uint64_t sectors;
    struct { uint64_t offset, size; } partition[STORAGE_PARTITIONS];
};

enum storage_result { STORAGE_VALID, STORAGE_NONE, STORAGE_INVALID };
enum storage_result storage_discovery(const char *, size_t, unsigned char [8]);
enum storage_result storage_label_check(const struct storage_label *, uint64_t *);

#endif
