/* SPDX-License-Identifier: BSD-3-Clause */
#ifdef __OpenBSD__
#define _BSD_SOURCE
#endif
#include "validate.h"
#include <stdio.h>
#include <string.h>

#ifdef __OpenBSD__
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/disklabel.h>
#include <sys/dkio.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <fcntl.h>
#include <inttypes.h>
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

static int
probe(void)
{
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
        puts("Discovery unavailable."); return 2;
    }
    result = storage_discovery(before, before_size, uid);
    if (result == STORAGE_NONE) { puts("No fixture disk observed."); return 2; }
    if (result != STORAGE_VALID) { puts("Discovery rejected."); return 2; }
    /* Fixed reviewed amd64 sd0 raw partition, never derived from discovery. */
    fd = open("/dev/rsd0c", O_RDONLY | O_NOFOLLOW);
    if (fd == -1) { puts("Read-only device unavailable."); return 2; }
    memset(&native, 0, sizeof(native));
    valid = fstat(fd, &status) == 0 && S_ISCHR(status.st_mode) &&
        major(status.st_rdev) == 13 && minor(status.st_rdev) == 2;
    if (valid) valid = ioctl(fd, DIOCGDINFO, &native) == 0;
    if (close(fd) == -1) valid = 0;
    if (!valid) { puts("Read-only metadata unavailable."); return 2; }
    memset(&label, 0, sizeof(label));
    label.magic = native.d_magic;
    label.magic2 = native.d_magic2;
    label.version = native.d_version;
    label.count = native.d_npartitions;
    label.sector_bytes = native.d_secsize;
    label.sectors = DL_GETDSIZE(&native);
    if (label.count > STORAGE_PARTITIONS || label.count > MAXPARTITIONS) {
        puts("Label rejected."); return 2;
    }
    for (i = 0; i < label.count; i++) {
        label.partition[i].offset = DL_GETPOFFSET(&native.d_partitions[i]);
        label.partition[i].size = DL_GETPSIZE(&native.d_partitions[i]);
    }
    if (storage_label_check(&label, &bytes) != STORAGE_VALID) {
        puts("Label or fixture capacity rejected."); return 2;
    }
    if (!discovery(after, &after_size) || before_size != after_size ||
        memcmp(before, after, before_size) != 0 ||
        storage_discovery(after, after_size, check_uid) != STORAGE_VALID ||
        memcmp(uid, check_uid, 8) != 0 || memcmp(uid, native.d_uid, 8) != 0) {
        puts("Observation changed or disagreed."); return 2;
    }
    puts("Observed fixture disk: sd0");
    printf("Bytes: %" PRIu64 "\n", bytes);
    printf("Sector bytes: %" PRIu32 "\n", label.sector_bytes);
    fputs("DUID: ", stdout);
    for (i = 0; i < 8; i++) printf("%02x", (unsigned int)uid[i]);
    putchar('\n');
    puts("Inventory only; no readiness claim.");
    return 0;
}
#endif

int
main(int argc, char **argv)
{
    int result;
    (void)argv;
    if (argc != 1) return 2;
    if (setvbuf(stdout, NULL, _IONBF, 0) != 0) return 3;
    puts("Bootweave read-only research probe");
    puts("No installation authorized.");
#ifdef __OpenBSD__
    /* No terminal modes changed. Host supervisor also bounds the whole guest. */
    alarm(10);
    {
        struct winsize dimensions;
        memset(&dimensions, 0, sizeof(dimensions));
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &dimensions) == -1 ||
            dimensions.ws_col == 0 || dimensions.ws_row == 0)
            puts("Console dimensions unknown.");
        else if (dimensions.ws_col < 40) {
            puts("Console too narrow for this probe.");
            return 2;
        }
    }
    result = probe();
    {
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == -1 || usage.ru_maxrss < 0 ||
            usage.ru_maxrss > 32768) {
            puts("Resource observation rejected.");
            result = 3;
        } else {
            printf("Peak RSS KiB: %ld\n", usage.ru_maxrss);
        }
    }
#else
    puts("Requires OpenBSD 7.9 amd64.");
    result = 77;
#endif
    if (ferror(stdout) || fflush(stdout) == EOF) return 3;
    return result;
}
