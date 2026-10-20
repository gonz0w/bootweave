/* SPDX-License-Identifier: BSD-3-Clause */
/* Owned byte fixture only; CommonCrypto is a Mac test adapter, never product. */
#include "policy_binding.h"
#include "auth_input.h"
#include <CommonCrypto/CommonDigest.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static unsigned char key[BW_POLICY_KEY_MAX + 1], definition[BW_AUTH_DEFINITION_MAX + 1];
static unsigned char payload[BW_AUTH_PAYLOAD_MAX + 1];
static struct bw_policy_observation observation;
static unsigned hash_calls, fail_at;

#ifndef PB_NATIVE
int
bw_policy_sha256(const unsigned char *data, size_t length, unsigned char out[32])
{
    hash_calls++;
    if (hash_calls == fail_at) { memset(out, 0x5a, 32); return 0; }
    return length <= BW_AUTH_PAYLOAD_MAX && CC_SHA256(data, (CC_LONG)length, out) == out;
}
#endif

static unsigned
number(const unsigned char *b)
{
    return ((unsigned)b[0] << 24) | ((unsigned)b[1] << 16) | ((unsigned)b[2] << 8) | b[3];
}

static void
emit(int result)
{
    const unsigned char *p = (const unsigned char *)&observation.plan;
    size_t i;
    int cleared = 1;
    for (i = 0; i < sizeof(observation.plan); i++) if (p[i]) cleared = 0;
    printf("PB_RESULT %d %d %u %d\n", result, observation.cause, hash_calls, cleared);
    if (result) printf("%s|%s|%s|%s|%s|%s\n", observation.plan.id, observation.plan.name,
        observation.plan.release, observation.plan.architecture, observation.plan.file,
        observation.plan.sha256);
}

int
main(int argc, char **argv)
{
    unsigned char sizes[12], before[3][32], after[32], out_before[sizeof(observation)];
    struct bw_policy_request request, original, *r = &request;
    struct bw_policy_observation *out = &observation;
    const char *mode = argc == 2 ? argv[1] : "normal";
    int result, immutable = 1;
    size_t i, lengths[3];
    unsigned char *inputs[3] = {key, definition, payload};
    if (argc > 2 || fread(sizes, 1, 12, stdin) != 12) return 90;
    for (i = 0; i < 3; i++) lengths[i] = number(sizes + 4 * i);
    if (lengths[0] > sizeof(key) || lengths[1] > sizeof(definition) || lengths[2] > sizeof(payload)) return 91;
    for (i = 0; i < 3; i++)
        if (fread(inputs[i], 1, lengths[i], stdin) != lengths[i]) return 92;
    if (getchar() != EOF || ferror(stdin)) return 93;
    request.key = key; request.definition = definition; request.payload = payload;
    request.key_length = lengths[0]; request.definition_length = lengths[1]; request.payload_length = lengths[2];
    if (!strcmp(mode, "hash")) {
        if (!bw_policy_sha256(payload, lengths[2], after)) return 94;
        for (i = 0; i < 32; i++) printf("%02x", after[i]);
        putchar('\n'); return 0;
    }
    if (!strcmp(mode, "hashfail1")) fail_at = 1;
    if (!strcmp(mode, "hashfail2")) fail_at = 2;
    if (!strcmp(mode, "repeat")) {
        emit(bw_policy_bind(&request, &observation));
        payload[0] ^= 1; emit(bw_policy_bind(&request, &observation));
        payload[0] ^= 1; emit(bw_policy_bind(&request, &observation)); return 0;
    }
    if (!strcmp(mode, "null-request")) r = NULL;
    if (!strcmp(mode, "null-output")) out = NULL;
    if (!strcmp(mode, "overflow-request")) r = (void *)(UINTPTR_MAX - 8);
    if (!strcmp(mode, "overflow-output")) out = (void *)(UINTPTR_MAX - 8);
    if (!strcmp(mode, "request-output")) out = (void *)&request;
    if (!strcmp(mode, "policy-request")) r = (void *)&bw_compiled_policy;
    if (!strcmp(mode, "policy-output")) out = (void *)&bw_compiled_policy;
    for (i = 0; i < 3; i++) {
        char name[48];
        const unsigned char *p = inputs[i];
        size_t n = lengths[i];
        snprintf(name, sizeof(name), "null-%zu", i); if (!strcmp(mode, name)) p = NULL;
        snprintf(name, sizeof(name), "empty-%zu", i); if (!strcmp(mode, name)) n = 0;
        snprintf(name, sizeof(name), "overflow-%zu", i); if (!strcmp(mode, name)) { p = (void *)(UINTPTR_MAX - 8); n = 16; }
        snprintf(name, sizeof(name), "request-%zu", i); if (!strcmp(mode, name)) { p = (void *)&request; n = sizeof(request); }
        snprintf(name, sizeof(name), "output-%zu", i); if (!strcmp(mode, name)) out = (void *)inputs[i];
        snprintf(name, sizeof(name), "policy-%zu", i); if (!strcmp(mode, name)) { p = (void *)&bw_compiled_policy; n = 32; }
        if (i == 0) { request.key = p; request.key_length = n; }
        if (i == 1) { request.definition = p; request.definition_length = n; }
        if (i == 2) { request.payload = p; request.payload_length = n; }
    }
    if (!strcmp(mode, "input-01")) request.definition = key;
    if (!strcmp(mode, "input-02")) request.payload = key;
    if (!strcmp(mode, "input-12")) request.payload = definition;
    original = request;
    memset(&observation, 0xa5, sizeof(observation));
    memcpy(out_before, &observation, sizeof(observation));
    for (i = 0; i < 3; i++) CC_SHA256(inputs[i], (CC_LONG)lengths[i], before[i]);
    result = bw_policy_bind(r, out);
    for (i = 0; i < 3; i++) {
        CC_SHA256(inputs[i], (CC_LONG)lengths[i], after);
        if (memcmp(before[i], after, 32)) immutable = 0;
    }
    if (memcmp(&original, &request, sizeof(request))) immutable = 0;
    if (!strcmp(mode, "normal") || !strcmp(mode, "hashfail1") || !strcmp(mode, "hashfail2") ||
        !strncmp(mode, "empty-", 6)) emit(result);
    else printf("PB_NOWRITE %d %u %d %d\n", result, hash_calls, immutable,
        memcmp(out_before, &observation, sizeof(observation)) == 0);
    return immutable ? 0 : 95;
}
