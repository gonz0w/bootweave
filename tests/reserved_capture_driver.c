/* SPDX-License-Identifier: BSD-3-Clause */
/* Real private reserve/material handoff, fixed actual input roles, inert data. */
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "auth_input.h"
#include "auth_material.h"
#include <CommonCrypto/CommonDigest.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct bw_auth_snapshot snapshot;
static struct bw_verifier_material material;
static volatile sig_atomic_t cancelled;
static struct timespec origin;
static const int roles[] = {0, 3, 4};
static const char *const names[] = {"definition", "payload", "signature"};
static const char *root, *mode, *phase;
static unsigned selected, reads, stats[5], seeks[5], closes[5], ticks, phase_seen;
static unsigned reservation_stats, queries, opens, writes, material_closes, stage;
static int capture_protocol = 1;
static unsigned char key[1024], signature[16384];
static size_t key_length, signature_length;
static int is(const char *name) { return !strcmp(mode, name); }
static int at(const char *name) { return !strcmp(phase, name); }
static void capture_effect(void) {
    if (stage != 1 || material.capture_taken != 1) capture_protocol = 0;
}
static int role_index(int fd) {
    unsigned i; for (i = 0; i < 3; i++) if (roles[i] == fd) return (int)i;
    return -1;
}
static void path(char out[512], const char *name) {
    int n = snprintf(out, 512, "%s/%s", root, name);
    if (n < 0 || n >= 512) exit(90);
}
static size_t load(const char *name, unsigned char *out, size_t cap) {
    char name_path[512]; FILE *f; size_t n;
    path(name_path, name); f = fopen(name_path, "rb"); if (!f) exit(91);
    n = fread(out, 1, cap, f); if (ferror(f) || fclose(f)) exit(91); return n;
}
static void offset_time(struct timespec *out, long long ns) {
    *out = origin;
    out->tv_sec += (time_t)(ns / 1000000000LL);
    out->tv_nsec += (long)(ns % 1000000000LL);
    if (out->tv_nsec >= 1000000000L) { out->tv_sec++; out->tv_nsec -= 1000000000L; }
    if (out->tv_nsec < 0) { out->tv_sec--; out->tv_nsec += 1000000000L; }
}
int rc_capture_clock(clockid_t id, struct timespec *out) {
    int triggered = at("preflight") || phase_seen;
    capture_effect(); ticks++; if (id != CLOCK_MONOTONIC) exit(92);
    offset_time(out, is("near-deadline") ? 1999999999LL : 500000000LL);
    if (!triggered) return 0;
    if (is("clock-error")) return -1;
    if (is("clock-invalid")) out->tv_nsec = 1000000000L;
    if (is("clock-backward")) offset_time(out, -500000000LL);
    if (is("deadline")) offset_time(out, 2000000000LL);
    return 0;
}
int rc_material_clock(clockid_t id, struct timespec *out) {
    if (stage < 2) return clock_gettime(id, out);
    /* A direct valid-request poison probe removes external fault stimuli. It
     * must still refuse BEFORE member creation because the cause is latched. */
    if (id != CLOCK_MONOTONIC) exit(92);
    offset_time(out, is("near-deadline") ? 1999999999LL : 500000000LL); return 0;
}
char *rc_mkdtemp(char *template) {
    int n;
    if (strcmp(template, "/tmp/bootweave-verify.XXXXXXXXXX")) exit(92);
    n = snprintf(template, 256, "%s/material.XXXXXXXXXX", root);
    return n < 0 || n >= 256 ? NULL : mkdtemp(template);
}
int rc_material_openat(int directory, const char *name, int flags, ...) {
    va_list args; mode_t permissions = 0;
    if (stage >= 2) opens++;
    if (flags & O_CREAT) { va_start(args, flags); permissions = (mode_t)va_arg(args, int); va_end(args); }
    return flags & O_CREAT ? openat(directory, name, flags, permissions) : openat(directory, name, flags);
}
ssize_t rc_material_write(int fd, const void *bytes, size_t length) {
    if (stage >= 2) writes++;
    return write(fd, bytes, length);
}
int rc_material_close(int fd) {
    if (stage >= 2) material_closes++;
    return close(fd);
}
int rc_capture_fstat(int fd, struct stat *out) {
    int result = fstat(fd, out), i = role_index(fd);
    capture_effect();
    if (fd == material.directory) {
        reservation_stats++;
        if (is("reserve-stat-error")) { errno = EIO; return -1; }
        if (!result && is("reserve-identity")) out->st_ino++;
        return result;
    }
    if (fd < 0 || fd > 4) exit(92);
    stats[fd]++; queries++;
    if (is("output-stat-error") && fd == (int)selected) { errno = EIO; return -1; }
    if (result) return result;
    if (is("output-alias") && fd == (int)selected) {
        struct stat other;
        if (fstat(0, &other)) exit(92);
        out->st_dev = other.st_dev; out->st_ino = other.st_ino; out->st_mode = S_IFREG | 0600;
    }
    if (i >= 0 && stats[fd] == 1) {
        if (i == (int)selected) {
            if (is("empty")) out->st_size = 0;
            if (is("overcap")) out->st_size = i == 1 ? 1048577 : 16385;
            if (is("nonregular")) out->st_mode = S_IFIFO | 0600;
            if (is("input-alias") || is("input-alias-last")) {
                struct stat other;
                if (fstat(is("input-alias-last") ? 3 : 0, &other)) exit(92);
                out->st_dev = other.st_dev; out->st_ino = other.st_ino;
            }
        }
        return 0;
    }
    if (fd == 0 && stats[fd] == 2) {
        if (is("changed-device")) out->st_dev++;
        if (is("changed-inode")) out->st_ino++;
        if (is("changed-mode")) out->st_mode ^= 1;
        if (is("changed-uid")) out->st_uid++;
        if (is("changed-gid")) out->st_gid++;
        if (is("changed-link")) out->st_nlink++;
        if (is("changed-size")) out->st_size++;
#ifdef __APPLE__
        if (is("changed-mtime")) out->st_mtimespec.tv_nsec ^= 1;
        if (is("changed-ctime")) out->st_ctimespec.tv_nsec ^= 1;
        if (is("atime")) out->st_atimespec.tv_nsec ^= 1;
#else
        if (is("changed-mtime")) out->st_mtim.tv_nsec ^= 1;
        if (is("changed-ctime")) out->st_ctim.tv_nsec ^= 1;
        if (is("atime")) out->st_atim.tv_nsec ^= 1;
#endif
    }
    return 0;
}
int rc_capture_fcntl(int fd, int command, ...) {
    capture_effect();
    if (role_index(fd) < 0 || command != F_GETFL) exit(92);
    queries++;
    if (is("flags-error") && role_index(fd) == (int)selected) { errno = EIO; return -1; }
    return fcntl(fd, command);
}
off_t rc_capture_lseek(int fd, off_t offset, int whence) {
    off_t result;
    capture_effect(); if (role_index(fd) < 0 || offset || whence != SEEK_CUR) exit(92);
    queries++; seeks[fd]++;
    result = lseek(fd, offset, whence);
    return fd == 0 && seeks[fd] == 2 && is("changed-offset") ? result + 1 : result;
}
ssize_t rc_capture_read(int fd, void *bytes, size_t amount) {
    ssize_t n;
    capture_effect(); if (role_index(fd) < 0 || !amount || amount > 4096) exit(92);
    reads++;
    if ((at("eintr") || is("short")) && reads == 1) {
        phase_seen = at("eintr"); if (is("cancel")) cancelled = 1;
        errno = EINTR; return -1;
    }
    if (is("read-error") || is("read-error-close-eintr")) { errno = EIO; return -1; }
    if (is("shrink")) return 0;
    if (is("grow") && fd == 4) { memset(bytes, 'g', amount); return (ssize_t)amount; }
    if (is("short") && amount > 7) amount = 7;
    n = read(fd, bytes, amount);
    if (at("read") && n > 0) { phase_seen = 1; if (is("cancel")) cancelled = 1; }
    return n;
}
int rc_capture_close(int fd) {
    int i = role_index(fd), result;
    capture_effect(); if (i < 0) exit(92);
    closes[fd]++; result = close(fd);
    if (i == (int)selected && (is("close-eintr") || is("read-error-close-eintr"))) { errno = EINTR; result = -1; }
    if (i == (int)selected && is("close-ebadf")) { errno = EBADF; result = -1; }
    if (closes[0] && closes[3] && closes[4] && at("final-close")) {
        phase_seen = 1; if (is("cancel")) cancelled = 1;
    }
    return result;
}
static int zero(const void *bytes, size_t length) {
    const unsigned char *p = bytes; while (length--) if (*p++) return 0; return 1;
}
static void digest(const void *bytes, size_t length, unsigned char out[32]) {
    if (CC_SHA256(bytes, (CC_LONG)length, out) != out) exit(93);
}
static void hex(const unsigned char *bytes, size_t length) {
    while (length--) printf("%02x", *bytes++);
}
static int high_open(const char *name, int flags, int base) {
    char filename[512]; int fd, held;
    path(filename, name); fd = open(filename, flags);
    if (fd < 0 || (held = fcntl(fd, F_DUPFD_CLOEXEC, base)) < 0 || close(fd)) exit(94);
    return held;
}
static void shape(struct bw_verifier_material **m, struct bw_auth_snapshot **s) {
    if (is("null-material")) *m = NULL;
    if (is("null-snapshot")) *s = NULL;
    if (is("null-flag")) material.cancelled = NULL;
    if (is("overflow-material")) *m = (void *)(UINTPTR_MAX - 8);
    if (is("overflow-snapshot")) *s = (void *)(UINTPTR_MAX - 8);
    if (is("overflow-flag")) material.cancelled = (void *)(UINTPTR_MAX - 1);
    if (is("material-snapshot")) *s = (void *)&material;
    if (is("material-flag")) material.cancelled = (void *)&material;
    if (is("snapshot-flag")) material.cancelled = (void *)&snapshot;
    if (is("state-empty")) material.state = BW_VM_EMPTY;
    if (is("state-working")) material.state = BW_VM_WORKING;
    if (is("state-ready")) material.state = BW_VM_READY;
    if (is("state-closed")) material.state = BW_VM_CLOSED;
    if (is("state-rejected")) material.state = BW_VM_REJECTED;
    if (is("state-uncertain")) material.state = BW_VM_UNCERTAIN;
    if (is("state-verifying")) material.state = BW_VM_VERIFYING;
    if (is("taken")) material.capture_taken = 1;
    if (is("cause")) material.cause = BW_VM_IO;
    if (is("uncertain")) material.uncertain = 1;
    if (is("close-errors")) material.close_errors = 1;
    if (is("unknown-closes")) material.unknown_closes = 1;
    if (is("capture-close-errors")) material.capture_close_errors = 1;
    if (is("capture-unknown-closes")) material.capture_unknown_closes = 1;
    if (is("directory-low")) material.directory = 0;
    if (is("directory-unknown")) material.directory_known = 0;
    if (is("directory-lost")) material.directory_lost = 1;
    if (is("created")) material.created = 1;
    if (is("known")) material.known = 1;
    if (is("writer")) material.writer[0] = 0;
    if (is("ready-fd")) material.fd[0] = 0;
    if (is("path-empty")) material.path[0] = 0;
    if (is("started-negative")) material.started.tv_sec = -1;
    if (is("started-invalid")) material.started.tv_nsec = 1000000000L;
    if (is("identity-type")) material.identity.st_mode = S_IFREG | 0700;
    if (is("identity-mode")) material.identity.st_mode ^= 1;
    if (is("identity-links")) material.identity.st_nlink = 0;
}
static int ready_bytes(const struct bw_verifier_request *request) {
    const unsigned char *input[3] = {request->key, request->signature, request->definition};
    size_t length[3] = {request->key_length, request->signature_length, request->definition_length};
    unsigned char buffer[4096]; struct stat metadata; unsigned i; size_t offset, amount;
    for (i = 0; i < 3; i++) {
        int fd = material.fd[i];
        if (fstat(fd, &metadata) || !S_ISREG(metadata.st_mode) || metadata.st_uid != geteuid() ||
            metadata.st_nlink || metadata.st_size != (off_t)length[i] ||
            (fcntl(fd, F_GETFL) & O_ACCMODE) != O_RDONLY || !(fcntl(fd, F_GETFD) & FD_CLOEXEC) || lseek(fd, 0, SEEK_CUR)) return 0;
        for (offset = 0; offset < length[i]; offset += amount) {
            amount = length[i] - offset; if (amount > sizeof(buffer)) amount = sizeof(buffer);
            if (pread(fd, buffer, amount, (off_t)offset) != (ssize_t)amount || memcmp(buffer, input[i] + offset, amount)) return 0;
        }
    }
    return !material.path[0];
}
int main(int argc, char **argv) {
    struct bw_verifier_material original, before, capture_state, repeated;
    struct bw_verifier_material *m = &material;
    struct bw_auth_snapshot *s = &snapshot;
    struct bw_verifier_request request;
    unsigned char before_hash[32], after_hash[32], repeat_hash[32], content_hash[3][32];
    unsigned saved_counters[6], after_counters[6], i, release_calls = 0, terminal = 0;
    int held[3], canary, result, prepare = -9, released = -9, repeat_result, no_write, repeat_ok, exact_fds = 0;
    static const unsigned char valid_definition[] = "direct valid definition\n";
    if (argc != 6) return 90;
    root = argv[1]; mode = argv[2]; selected = (unsigned)strtoul(argv[3], NULL, 10); phase = argv[4];
    if (selected > 2) return 90;
    key_length = load("key", key, sizeof(key)); signature_length = load("signature", signature, sizeof(signature));
    if (bw_verifier_reserve(&cancelled, &material) != BW_MATERIAL_OK) return 94;
    origin = material.started; memcpy(&original, &material, sizeof(original));
    for (i = 0; i < 3; i++) {
        held[i] = high_open(names[i], ((is("readwrite") && selected == i) ? O_RDWR : O_RDONLY) | O_NONBLOCK, 10);
        if (dup2(held[i], roles[i]) != roles[i]) return 94;
    }
    if (is("offset") && lseek(roles[selected], 1, SEEK_SET) != 1) return 94;
    if (is("missing") && close(roles[selected])) return 94;
    memset(&snapshot, 0xa5, sizeof(snapshot)); shape(&m, &s);
    if (is("cancel") && at("preflight")) cancelled = 1;
    memcpy(&before, &material, sizeof(before)); digest(&snapshot, sizeof(snapshot), before_hash);
    stage = 1; result = bw_auth_capture_reserved(m, s); stage = 0;
    memcpy(&capture_state, &material, sizeof(capture_state)); digest(&snapshot, sizeof(snapshot), after_hash);
    no_write = !memcmp(&before, &material, sizeof(before)) && !memcmp(before_hash, after_hash, 32);
    for (i = 0; i < 3; i++) if (result == BW_CAPTURED) {
        const unsigned char *bytes = i == 0 ? snapshot.definition : i == 1 ? snapshot.payload : snapshot.signature;
        size_t length = i == 0 ? snapshot.definition_length : i == 1 ? snapshot.payload_length : snapshot.signature_length;
        digest(bytes, length, content_hash[i]);
    } else memset(content_hash[i], 0, 32);
    /* Caller rebinds fresh canary OFDs only AFTER the first controlled return.
     * A repeated capture must not inspect, advance, close or mutate these. */
    canary = high_open("canary", O_RDONLY, 15);
    for (i = 0; i < 3; i++) if (dup2(canary, roles[i]) != roles[i]) return 94;
    saved_counters[0] = reads; saved_counters[1] = queries; saved_counters[2] = ticks;
    saved_counters[3] = closes[0] + closes[3] + closes[4]; saved_counters[4] = reservation_stats; saved_counters[5] = phase_seen;
    memcpy(&repeated, &material, sizeof(repeated));
    stage = 1; repeat_result = bw_auth_capture_reserved(m, s); stage = 0;
    digest(&snapshot, sizeof(snapshot), repeat_hash);
    after_counters[0] = reads; after_counters[1] = queries; after_counters[2] = ticks;
    after_counters[3] = closes[0] + closes[3] + closes[4]; after_counters[4] = reservation_stats; after_counters[5] = phase_seen;
    repeat_ok = repeat_result == BW_CAPTURE_NOT_TAKEN && !memcmp(&repeated, &material, sizeof(repeated)) &&
        !memcmp(after_hash, repeat_hash, 32) && !memcmp(saved_counters, after_counters, sizeof(saved_counters));
    for (i = 0; i < 3; i++) if (fcntl(roles[i], F_GETFD) < 0 || lseek(roles[i], 0, SEEK_CUR)) repeat_ok = 0;
    if (result == BW_CAPTURE_NOT_TAKEN) memcpy(&material, &original, sizeof(material));
    cancelled = 0; stage = 2;
    if (result != BW_CAPTURE_NOT_TAKEN && !strcmp(argv[5], "prepare")) {
        request = (struct bw_verifier_request){key, result == BW_CAPTURED ? snapshot.signature : signature,
            result == BW_CAPTURED ? snapshot.definition : valid_definition, key_length,
            result == BW_CAPTURED ? snapshot.signature_length : signature_length,
            result == BW_CAPTURED ? snapshot.definition_length : sizeof(valid_definition) - 1};
        prepare = bw_verifier_prepare(&request, &material);
        if (prepare == BW_MATERIAL_OK) exact_fds = ready_bytes(&request); else terminal++;
    }
    if (material.state == BW_VM_RESERVED || material.state == BW_VM_READY) {
        release_calls++; released = bw_verifier_release(&material); terminal++;
    }
    stage = 0;
    for (i = 0; i < 3; i++) {
        if (fcntl(roles[i], F_GETFD) < 0 || lseek(roles[i], 0, SEEK_CUR)) repeat_ok = 0;
        if (close(roles[i]) || close(held[i])) return 95;
    }
    if (close(canary)) return 95;
    printf("RC_RESULT {\"return\":%d,\"state\":%d,\"cause\":%d,\"taken\":%u,\"uncertain\":%u,"
        "\"capture_close_errors\":%u,\"capture_unknown_closes\":%u,\"reads\":%u,\"queries\":%u,\"ticks\":%u,"
        "\"reservation_stats\":%u,\"closes\":[%u,%u,%u],\"no_write\":%d,\"zero\":%d,\"repeat_ok\":%d,"
        "\"origin_flag\":%d,\"prepare\":%d,\"member_opens\":%u,\"member_writes\":%u,\"material_closes\":%u,"
        "\"release_calls\":%u,\"released\":%d,\"terminal\":%u,\"final_state\":%d,\"final_cause\":%d,"
        "\"final_uncertain\":%u,\"ready_bytes\":%d,\"capture_protocol\":%d,\"final_origin_flag\":%d,"
        "\"lengths\":[%zu,%zu,%zu],\"digests\":[",
        result, capture_state.state, capture_state.cause, capture_state.capture_taken, capture_state.uncertain,
        capture_state.capture_close_errors, capture_state.capture_unknown_closes, reads, queries, ticks,
        reservation_stats, closes[0], closes[3], closes[4], no_write, zero(&snapshot, sizeof(snapshot)), repeat_ok,
        result == BW_CAPTURE_NOT_TAKEN ? no_write : capture_state.started.tv_sec == origin.tv_sec &&
        capture_state.started.tv_nsec == origin.tv_nsec && capture_state.cancelled == &cancelled,
        prepare, opens, writes, material_closes, release_calls, released, terminal, material.state, material.cause,
        material.uncertain, exact_fds, capture_protocol,
        material.started.tv_sec == origin.tv_sec && material.started.tv_nsec == origin.tv_nsec && material.cancelled == &cancelled,
        snapshot.definition_length, snapshot.payload_length, snapshot.signature_length);
    for (i = 0; i < 3; i++) { printf("%s\"", i ? "," : ""); hex(content_hash[i], 32); putchar('"'); }
    printf("]}\n");
    return 0;
}
