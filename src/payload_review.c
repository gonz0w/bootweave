/* SPDX-License-Identifier: BSD-3-Clause */
/* Pure retained-buffer structure review; no authenticity or effect authority. */
#include "payload_review.h"
#include <stdint.h>
#include <string.h>

#define BLOCK 512
#define INPUT_CAP 1048576
#define MEMBERS 128
#define FILE_CAP 262144
#define CONTENT_CAP 524288
struct member { char path[256]; int directory; };
struct review_state {
    struct member inventory[MEMBERS];
    size_t member_count, file_count, content_bytes;
};
typedef char workspace_budget[(sizeof(struct review_state) <= 34816) ? 1 : -1];

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
    size_t name, prefix, length, start = 0, depth = 0, i;
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
            if (++depth > 16) return -1;
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
header_check(struct review_state *state, const unsigned char header[BLOCK], struct member *entry, size_t *size)
{
    uint64_t mode, uid, gid, bytes, mtime, expected, device;
    unsigned int checksum = 0;
    size_t i;
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
    if (path(header, entry->path, entry->directory)) return -1;
    if ((entry->directory && bytes) || bytes > FILE_CAP || bytes > CONTENT_CAP - state->content_bytes)
        return -1;
    *size = (size_t)bytes;
    for (i = 0; i < state->member_count; i++) {
        if (!strcmp(state->inventory[i].path, entry->path) ||
            (!state->inventory[i].directory && ancestor(state->inventory[i].path, entry->path)) ||
            (!entry->directory && ancestor(entry->path, state->inventory[i].path))) return -1;
    }
    return 0;
}

int
bw_payload_structure_review(const unsigned char *data, size_t length,
    struct bw_payload_summary *summary)
{
    struct review_state state = {0};
    struct member entry;
    size_t offset = 0, bytes, padded;
    int trailer = 0;
    uintptr_t begin = (uintptr_t)data, output = (uintptr_t)summary;
    if (!summary) return -1;
    /* Reject alias/overflow without modifying either caller buffer. */
    if (length > UINTPTR_MAX - begin || sizeof(*summary) > UINTPTR_MAX - output)
        return -1;
    if (data && begin < output + sizeof(*summary) && output < begin + length)
        return -1;
    memset(summary, 0, sizeof(*summary));
    if (!data || !length || length > INPUT_CAP || length % BLOCK) return -1;
    while (offset < length) {
        const unsigned char *block = data + offset;
        offset += BLOCK;
        if (zero(block, BLOCK)) { if (trailer < 2) trailer++; continue; }
        if (trailer || state.member_count == MEMBERS ||
            header_check(&state, block, &entry, &bytes)) return -1;
        state.inventory[state.member_count++] = entry;
        if (!entry.directory) state.file_count++;
        state.content_bytes += bytes;
        padded = ((bytes + BLOCK - 1) / BLOCK) * BLOCK;
        if (padded > length - offset || !zero(data + offset + bytes, padded - bytes))
            return -1;
        offset += padded;
    }
    if (trailer < 2 || !state.file_count) return -1;
    summary->members = state.member_count;
    summary->files = state.file_count;
    summary->content_bytes = state.content_bytes;
    return 0;
}
