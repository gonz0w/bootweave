/* SPDX-License-Identifier: BSD-3-Clause */
#ifdef __OpenBSD__
#define _BSD_SOURCE
#endif
#include "snapshot.h"
#include <string.h>
#ifdef __OpenBSD__
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/disklabel.h>
#include <sys/dkio.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <fcntl.h>
#include <unistd.h>
static int
platform(void)
{
    int release[] = { CTL_KERN, KERN_OSRELEASE };
    int machine[] = { CTL_HW, HW_MACHINE };
    char value[32];
    size_t size = sizeof(value);
    if (sysctl(release, 2, value, &size, NULL, 0) == -1 || size != 4 ||
        memcmp(value, "7.9\0", 4) != 0) return 0;
    size = sizeof(value);
    return sysctl(machine, 2, value, &size, NULL, 0) == 0 && size == 6 &&
        memcmp(value, "amd64\0", 6) == 0;
}

static int
discovery(char data[STORAGE_DISCOVERY_MAX], size_t *size)
{
    int mib[] = { CTL_HW, HW_DISKNAMES };
    *size = STORAGE_DISCOVERY_MAX;
    return sysctl(mib, 2, data, size, NULL, 0) == 0 &&
        *size <= STORAGE_DISCOVERY_MAX;
}

int
handoff_observe(struct handoff_snapshot *snapshot)
{
    memset(snapshot, 0, sizeof(*snapshot));
    char before[STORAGE_DISCOVERY_MAX], after[STORAGE_DISCOVERY_MAX];
    unsigned char uid[8], check_uid[8];
    size_t before_size, after_size, i;
    enum storage_result result;
    struct stat status;
    struct disklabel native;
    struct storage_label label;
    uint64_t bytes;
    int fd, valid;

    if (!platform() || !discovery(before, &before_size)) {
        return STORAGE_INVALID;
    }
    result = storage_discovery(before, before_size, uid);
    if (result == STORAGE_NONE) return STORAGE_NONE;
    if (result != STORAGE_VALID) { return STORAGE_INVALID; }
    /* Fixed reviewed amd64 sd0 raw partition, never derived from discovery. */
    fd = open("/dev/rsd0c", O_RDONLY | O_NOFOLLOW);
    if (fd == -1) { return STORAGE_INVALID; }
    memset(&native, 0, sizeof(native));
    valid = fstat(fd, &status) == 0 && S_ISCHR(status.st_mode) &&
        major(status.st_rdev) == 13 && minor(status.st_rdev) == 2;
    if (valid) valid = ioctl(fd, DIOCGDINFO, &native) == 0;
    if (close(fd) == -1) valid = 0;
    if (!valid) { return STORAGE_INVALID; }
    memset(&label, 0, sizeof(label));
    label.magic = native.d_magic;
    label.magic2 = native.d_magic2;
    label.version = native.d_version;
    label.count = native.d_npartitions;
    label.sector_bytes = native.d_secsize;
    label.sectors = DL_GETDSIZE(&native);
    if (label.count > STORAGE_PARTITIONS || label.count > MAXPARTITIONS) {
        return STORAGE_INVALID;
    }
    for (i = 0; i < label.count; i++) {
        label.partition[i].offset = DL_GETPOFFSET(&native.d_partitions[i]);
        label.partition[i].size = DL_GETPSIZE(&native.d_partitions[i]);
    }
    if (storage_label_check(&label, &bytes) != STORAGE_VALID) {
        return STORAGE_INVALID;
    }
    if (!discovery(after, &after_size) || before_size != after_size ||
        memcmp(before, after, before_size) != 0 ||
        storage_discovery(after, after_size, check_uid) != STORAGE_VALID ||
        memcmp(uid, check_uid, 8) != 0 || memcmp(uid, native.d_uid, 8) != 0) {
        return STORAGE_INVALID;
    }
    snapshot->label = label;
    memcpy(snapshot->uid, uid, sizeof(snapshot->uid));
    return STORAGE_VALID;
}
#else
int
handoff_observe(struct handoff_snapshot *snapshot)
{
    memset(snapshot, 0, sizeof(*snapshot));
    return STORAGE_INVALID;
}
#endif

int
handoff_same(const struct handoff_snapshot *a, const struct handoff_snapshot *b)
{
    uint64_t bytes;
    size_t i;
    if (storage_label_check(&a->label, &bytes) != STORAGE_VALID ||
        storage_label_check(&b->label, &bytes) != STORAGE_VALID ||
        memcmp(a->uid, b->uid, sizeof(a->uid)) ||
        a->label.magic != b->label.magic || a->label.magic2 != b->label.magic2 ||
        a->label.version != b->label.version || a->label.count != b->label.count ||
        a->label.sectors != b->label.sectors ||
        a->label.sector_bytes != b->label.sector_bytes) return 0;
    for (i = 0; i < a->label.count; i++)
        if (a->label.partition[i].offset != b->label.partition[i].offset ||
            a->label.partition[i].size != b->label.partition[i].size) return 0;
    return 1;
}
