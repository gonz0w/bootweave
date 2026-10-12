/* SPDX-License-Identifier: BSD-3-Clause */
/* Original stdin-only research; no extraction, path opening or execution. */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BLOCK 512
#define INPUT_CAP 1048576
#define MEMBERS 128
#define FILE_CAP 262144
#define CONTENT_CAP 524288
struct member { char path[256]; int directory; };
static struct member inventory[MEMBERS];
static volatile sig_atomic_t stopped;
static size_t input_bytes, member_count, file_count, content_bytes;
static long long input_end;
static const char *failure = "input";
/* All application arrays plus conservative scalar/stack allowance. */
typedef char buffer_budget[(sizeof(inventory) + 2048 + 4096 <= 65536) ? 1 : -1];

static void
stop_handler(int number)
{
    stopped = number;
}

static long long
now_ms(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value)) return -1;
    return (long long)value.tv_sec * 1000 + value.tv_nsec / 1000000;
}

static int
ready(int fd, short event, long long deadline)
{
    struct pollfd p = { fd, event, 0 };
    long long now;
    int result;
    for (;;) {
        now = now_ms();
        if (stopped || now < 0 || now >= deadline) return -1;
        p.revents = 0;
        result = poll(&p, 1, (int)(deadline - now));
        if (result < 0 && errno == EINTR) continue;
        if (result <= 0 || (p.revents & (POLLERR | POLLNVAL))) return -1;
        if (p.revents & (event | POLLHUP)) return 0;
    }
}

/* 1 complete block, 0 EOF between blocks, -1 incomplete/failed/over budget. */
static int
read_block(unsigned char data[BLOCK])
{
    size_t used = 0, amount;
    ssize_t count;
    while (used < BLOCK) {
        if (ready(STDIN_FILENO, POLLIN, input_end)) return -1;
        amount = BLOCK - used;
        if (amount > INPUT_CAP - input_bytes + 1)
            amount = INPUT_CAP - input_bytes + 1;
        count = read(STDIN_FILENO, data + used, amount);
        if (count < 0 && (errno == EINTR || errno == EAGAIN)) continue;
        if (count == 0) return used ? -1 : 0;
        if (count < 0) return -1;
        input_bytes += (size_t)count;
        if (input_bytes > INPUT_CAP) { failure = "input limit"; return -1; }
        used += (size_t)count;
    }
    return 1;
}

static int
zero(const unsigned char *bytes, size_t length)
{
    size_t i;
    for (i = 0; i < length; i++) if (bytes[i]) return 0;
    return 1;
}

static int
octal(const unsigned char *bytes, size_t length, uint64_t *value)
{
    size_t i = 0;
    *value = 0;
    while (i < length && bytes[i] >= '0' && bytes[i] <= '7') {
        unsigned int digit = bytes[i++] - '0';
        if (*value > (UINT64_MAX - digit) / 8) return -1;
        *value = *value * 8 + digit;
    }
    if (!i || i == length) return -1;
    for (; i < length; i++) if (bytes[i] != 0 && bytes[i] != ' ') return -1;
    return 0;
}

static int
field_length(const unsigned char *bytes, size_t length, size_t *used)
{
    size_t i = 0;
    while (i < length && bytes[i]) i++;
    *used = i;
    return zero(bytes + i, length - i) ? 0 : -1;
}

static int
path(const unsigned char *header, char result[256], int directory)
{
    size_t name, prefix, length, start = 0, i;
    if (field_length(header, 100, &name) || !name ||
        field_length(header + 345, 155, &prefix)) return -1;
    length = name + (prefix ? prefix + 1 : 0);
    if (length > 255) return -1;
    if (prefix) { memcpy(result, header + 345, prefix); result[prefix] = '/'; }
    memcpy(result + (prefix ? prefix + 1 : 0), header, name);
    if (directory && result[length - 1] == '/') length--;
    result[length] = '\0';
    if (!length) return -1;
    for (i = 0; i <= length; i++) {
        unsigned char c = (unsigned char)result[i];
        if (c == '/' || c == 0) {
            size_t part = i - start;
            if (!part || (part == 1 && result[start] == '.') ||
                (part == 2 && result[start] == '.' && result[start + 1] == '.')) return -1;
            start = i + 1;
        } else if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-')) return -1;
    }
    return 0;
}

static int
ancestor(const char *parent, const char *child)
{
    size_t length = strlen(parent);
    return strlen(child) > length && !memcmp(parent, child, length) && child[length] == '/';
}

static int
header_check(const unsigned char header[BLOCK], struct member *entry, size_t *size)
{
    uint64_t mode, uid, gid, bytes, mtime, expected, device;
    unsigned int checksum = 0;
    size_t i;
    failure = "header";
    for (i = 0; i < BLOCK; i++) checksum += (i >= 148 && i < 156) ? 32 : header[i];
    if (memcmp(header + 257, "ustar\0", 6) || memcmp(header + 263, "00", 2) ||
        octal(header + 148, 8, &expected) || expected != checksum ||
        !zero(header + 500, 12)) return -1;
    if (header[156] != '0' && header[156] != 0 && header[156] != '5') return -1;
    entry->directory = header[156] == '5';
    if (octal(header + 100, 8, &mode) || octal(header + 108, 8, &uid) ||
        octal(header + 116, 8, &gid) || octal(header + 124, 12, &bytes) ||
        octal(header + 136, 12, &mtime) || uid || gid ||
        mode != (entry->directory ? 0755U : 0644U) ||
        !zero(header + 157, 100) || !zero(header + 265, 64)) return -1;
    for (i = 329; i <= 337; i += 8)
        if (!zero(header + i, 8) && (octal(header + i, 8, &device) || device)) return -1;
    failure = "path";
    if (path(header, entry->path, entry->directory)) return -1;
    failure = "content limit";
    if ((entry->directory && bytes) || bytes > FILE_CAP || bytes > CONTENT_CAP - content_bytes)
        return -1;
    *size = (size_t)bytes;
    failure = "tree conflict";
    for (i = 0; i < member_count; i++) {
        if (!strcmp(inventory[i].path, entry->path) ||
            (!inventory[i].directory && ancestor(inventory[i].path, entry->path)) ||
            (!entry->directory && ancestor(entry->path, inventory[i].path))) return -1;
    }
    return 0;
}

static int
inspect(void)
{
    unsigned char block[BLOCK];
    struct member entry;
    size_t size, left;
    int status, trailer = 0;
    for (;;) {
        failure = "input";
        status = read_block(block);
        if (status < 0) return -1;
        if (!status) {
            failure = "trailer or empty archive";
            return trailer >= 2 && file_count ? 0 : -1;
        }
        if (zero(block, BLOCK)) { if (trailer < 2) trailer++; continue; }
        failure = "trailing data";
        if (trailer) return -1;
        failure = "member limit";
        if (member_count == MEMBERS) return -1;
        if (header_check(block, &entry, &size)) return -1;
        inventory[member_count++] = entry;
        if (!entry.directory) file_count++;
        content_bytes += size;
        left = size;
        while (left) {
            size_t take = left > BLOCK ? BLOCK : left;
            failure = "content input";
            if (read_block(block) != 1) return -1;
            failure = "content padding";
            if (take < BLOCK && !zero(block + take, BLOCK - take)) return -1;
            left -= take;
        }
    }
}

static int
emit(int fd, const char *message)
{
    size_t left = strlen(message);
    long long deadline = now_ms();
    ssize_t count;
    if (deadline < 0 || left > 4096) return -1;
    deadline += 5000;
    while (left) {
        if (ready(fd, POLLOUT, deadline)) return -1;
        count = write(fd, message, left);
        if (count < 0 && (errno == EINTR || errno == EAGAIN)) continue;
        if (count <= 0) return -1;
        message += count; left -= (size_t)count;
    }
    return 0;
}

int
main(int argc, char **argv)
{
    struct sigaction action;
    int flags[3] = { -1, -1, -1 }, signals[] = { SIGINT, SIGTERM, SIGHUP };
    int result = 3, i, length;
    char message[128];
    long long started = now_ms();
    (void)argv;
    memset(&action, 0, sizeof(action));
    action.sa_handler = stop_handler;
    if (sigemptyset(&action.sa_mask)) return 3;
    for (i = 0; i < 3; i++) if (sigaction(signals[i], &action, NULL)) return 3;
    action.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &action, NULL) || started < 0) return 3;
    for (i = 0; i < 3; i++) {
        flags[i] = fcntl(i, F_GETFL);
        if (flags[i] < 0 || fcntl(i, F_SETFL, flags[i] | O_NONBLOCK)) goto cleanup;
    }
    input_end = started + 10000;
    result = 2;
    if (argc != 1) failure = "arguments";
    else if (!inspect()) result = 0;
    if (stopped) result = 2;
    if (!result)
        length = snprintf(message, sizeof(message),
            "Inspection passed: %lu members, %lu files, %lu content bytes.\n",
            (unsigned long)member_count, (unsigned long)file_count, (unsigned long)content_bytes);
    else length = snprintf(message, sizeof(message), "Inspection rejected: %s.\n", failure);
    if (length < 0 || (size_t)length >= sizeof(message) || emit(result ? 2 : 1, message)) result = 3;
cleanup:
    for (i = 2; i >= 0; i--)
        if (flags[i] >= 0 && fcntl(i, F_SETFL, flags[i])) result = 3;
    return result;
}
