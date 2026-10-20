/* SPDX-License-Identifier: BSD-3-Clause */
/* Mac-only finite byte fixture. No product callback, signer or filesystem API. */
#include "content_review.h"
#include "auth_input.h"
#include <CommonCrypto/CommonDigest.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char key[BW_POLICY_KEY_MAX + 1], definition[BW_AUTH_DEFINITION_MAX + 1];
static unsigned char payload[BW_AUTH_PAYLOAD_MAX + 1];
static unsigned char text[BW_CONTENT_OUTPUT_MAX + 3], text_before[sizeof(text)];
static struct bw_content_observation observation;
static unsigned policy_calls, hash_calls, inspect_calls, format_calls, fail_at;
static int span_identity = 1, format_fault;
static const unsigned char *expected_definition, *expected_payload;
static size_t expected_definition_length, expected_payload_length;

#ifndef CR_NATIVE
int
bw_policy_sha256(const unsigned char *data, size_t length, unsigned char out[32])
{
    hash_calls++;
    if (hash_calls % 2 == 1) {
        if (data != expected_definition || length != expected_definition_length) span_identity = 0;
    } else if (data != expected_payload || length != expected_payload_length) span_identity = 0;
    if (hash_calls == fail_at) { memset(out, 0x5a, 32); return 0; }
    return length <= BW_AUTH_PAYLOAD_MAX && CC_SHA256(data, (CC_LONG)length, out) == out;
}
#endif

int
bw_c_policy(const struct bw_policy_request *r, struct bw_policy_observation *o)
{
    policy_calls++;
    return bw_policy_bind(r, o);
}

int
bw_c_structure(const unsigned char *data, size_t length, struct bw_payload_summary *s)
{
    inspect_calls++;
    if (data != expected_payload || length != expected_payload_length) span_identity = 0;
    return bw_payload_structure_review(data, length, s);
}

int
bw_c_snprintf(char *out, size_t capacity, const char *format, ...)
{
    int result;
    va_list ap;
    format_calls++;
    va_start(ap, format);
    result = vsnprintf(out, capacity, format, ap);
    va_end(ap);
    if (format_fault == 1) return -1;
    if (format_fault == 2) return (int)capacity;
    return result;
}

static unsigned
number(const unsigned char *b)
{
    return ((unsigned)b[0] << 24) | ((unsigned)b[1] << 16) | ((unsigned)b[2] << 8) | b[3];
}

static int
zero(const void *data, size_t length)
{
    const unsigned char *p = data;
    size_t i;
    for (i = 0; i < length; i++) if (p[i]) return 0;
    return 1;
}

static void
hex(const char *s)
{
    while (*s) printf("%02x", (unsigned char)*s++);
}

static void
emit(int result, size_t capacity, int immutable)
{
    int outside = text[0] == 0xa5;
    struct bw_content_observation cleared;
    size_t i;
    memcpy(&cleared, &observation, sizeof(cleared));
    cleared.cause = BW_CR_NONE;
    cleared.policy_cause = BW_PB_NONE;
    for (i = capacity <= BW_CONTENT_OUTPUT_MAX ? capacity + 1 : 1; i < sizeof(text); i++)
        if (text[i] != 0xa5) outside = 0;
    if (result && (observation.length >= capacity || capacity > BW_CONTENT_OUTPUT_MAX ||
        !zero(text + 1 + observation.length, capacity - observation.length))) outside = 0;
    printf("CR_RESULT %d %d %d %zu %u %u %u %u %d %d %d %d %d %zu %zu %zu\n",
        result, observation.cause, observation.policy_cause, observation.length,
        policy_calls, hash_calls, inspect_calls, format_calls,
        zero(&cleared, sizeof(cleared)),
        immutable, span_identity, outside,
        capacity <= BW_CONTENT_OUTPUT_MAX ? zero(text + 1, capacity) :
        !memcmp(text, text_before, sizeof(text)),
        observation.summary.members, observation.summary.files, observation.summary.content_bytes);
    if (result) {
        printf("CR_MODEL "); hex(observation.plan.id); putchar(' ');
        hex(observation.plan.name); putchar(' '); hex(observation.plan.release); putchar(' ');
        hex(observation.plan.architecture); putchar(' '); hex(observation.plan.file); putchar(' ');
        hex(observation.plan.sha256); putchar('\n');
        if (observation.length > capacity || fwrite(text + 1, 1, observation.length, stdout) != observation.length)
            exit(96);
    }
}

int
main(int argc, char **argv)
{
    unsigned char sizes[12], before[3][32], after[32], result_before[sizeof(observation)];
    struct bw_policy_request request, original, *r = &request;
    struct bw_content_observation *out = &observation;
    char *output = (char *)text + 1;
    const char *mode = argc > 1 ? argv[1] : "normal";
    size_t i, lengths[3], capacity = argc > 2 ? (size_t)strtoul(argv[2], NULL, 10) : 2048;
    unsigned char *inputs[3] = {key, definition, payload};
    int result, immutable = 1, no_write = 0;
    if (argc > 3 || capacity > 65537 || fread(sizes, 1, 12, stdin) != 12) return 90;
    for (i = 0; i < 3; i++) lengths[i] = number(sizes + 4 * i);
    if (lengths[0] > sizeof(key) || lengths[1] > sizeof(definition) || lengths[2] > sizeof(payload)) return 91;
    for (i = 0; i < 3; i++) if (fread(inputs[i], 1, lengths[i], stdin) != lengths[i]) return 92;
    if (getchar() != EOF || ferror(stdin)) return 93;
    request.key = key; request.definition = definition; request.payload = payload;
    request.key_length = lengths[0]; request.definition_length = lengths[1]; request.payload_length = lengths[2];
    expected_definition = definition; expected_definition_length = lengths[1];
    expected_payload = payload; expected_payload_length = lengths[2];
    if (!strcmp(mode, "hashfail1")) fail_at = 1;
    if (!strcmp(mode, "hashfail2")) fail_at = 2;
    if (!strcmp(mode, "negative")) format_fault = 1;
    if (!strcmp(mode, "truncated")) format_fault = 2;
    if (!strcmp(mode, "null-zero")) output = NULL;
    if (!strcmp(mode, "repeat") || !strcmp(mode, "repeat-format")) {
        for (i = 0; i < 3; i++) {
            policy_calls = hash_calls = inspect_calls = format_calls = 0;
            format_fault = !strcmp(mode, "repeat-format") && i == 1;
            memset(text, 0xa5, sizeof(text)); memcpy(text_before, text, sizeof(text));
            result = bw_content_review(&request, output, capacity, out);
            emit(result, capacity, 1);
            if (i < 2 && !strcmp(mode, "repeat")) payload[0] ^= 1;
        }
        return 0;
    }
    if (!strcmp(mode, "null-request")) { r = NULL; no_write = 1; }
    if (!strcmp(mode, "null-result")) { out = NULL; no_write = 1; }
    if (!strcmp(mode, "null-output")) { output = NULL; no_write = 1; }
    if (!strcmp(mode, "overflow-request")) { r = (void *)(UINTPTR_MAX - 8); no_write = 1; }
    if (!strcmp(mode, "overflow-result")) { out = (void *)(UINTPTR_MAX - 8); no_write = 1; }
    if (!strcmp(mode, "overflow-output")) { output = (void *)(UINTPTR_MAX - 8); no_write = 1; }
    if (!strcmp(mode, "request-output")) { output = (void *)&request; no_write = 1; }
    if (!strcmp(mode, "request-result")) { out = (void *)&request; no_write = 1; }
    if (!strcmp(mode, "request-policy")) { r = (void *)&bw_compiled_policy; no_write = 1; }
    if (!strcmp(mode, "output-result")) { output = (void *)&observation; no_write = 1; }
    if (!strcmp(mode, "output-policy")) { output = (void *)&bw_compiled_policy; no_write = 1; }
    if (!strcmp(mode, "result-policy")) { out = (void *)&bw_compiled_policy; no_write = 1; }
    for (i = 0; i < 3; i++) {
        static const char *labels[] = {"key", "definition", "payload"};
        char name[64];
        const unsigned char *p = inputs[i];
        size_t n = lengths[i];
#define MODE(prefix) (snprintf(name, sizeof(name), prefix "-%s", labels[i]), !strcmp(mode, name))
        if (MODE("null")) { p = NULL; no_write = 1; }
        if (MODE("overflow")) { p = (void *)(UINTPTR_MAX - 8); n = 16; no_write = 1; }
        if (MODE("request")) { p = (void *)&request; n = 16; no_write = 1; }
        if (MODE("output")) { output = (void *)inputs[i]; no_write = 1; }
        if (MODE("result")) { out = (void *)inputs[i]; no_write = 1; }
        if (MODE("policy")) { p = (void *)&bw_compiled_policy; n = 16; no_write = 1; }
#undef MODE
        if (!i) { request.key = p; request.key_length = n; }
        if (i == 1) { request.definition = p; request.definition_length = n; }
        if (i == 2) { request.payload = p; request.payload_length = n; }
    }
    if (!strcmp(mode, "key-definition")) { request.definition = key; no_write = 1; }
    if (!strcmp(mode, "key-payload")) { request.payload = key; no_write = 1; }
    if (!strcmp(mode, "definition-payload")) { request.payload = definition; no_write = 1; }
    original = request;
    memset(&observation, 0xa5, sizeof(observation));
    memset(text, 0xa5, sizeof(text));
    memcpy(result_before, &observation, sizeof(observation)); memcpy(text_before, text, sizeof(text));
    for (i = 0; i < 3; i++) CC_SHA256(inputs[i], (CC_LONG)lengths[i], before[i]);
    result = bw_content_review(r, output, capacity, out);
    for (i = 0; i < 3; i++) {
        CC_SHA256(inputs[i], (CC_LONG)lengths[i], after);
        if (memcmp(before[i], after, 32)) immutable = 0;
    }
    if (memcmp(&original, &request, sizeof(request))) immutable = 0;
    if (no_write) printf("CR_NOWRITE %d %u %u %u %u %d %d %d\n", result,
        policy_calls, hash_calls, inspect_calls, format_calls, immutable,
        !memcmp(result_before, &observation, sizeof(observation)),
        !memcmp(text_before, text, sizeof(text)));
    else emit(result, capacity, immutable);
    return immutable ? 0 : 95;
}
