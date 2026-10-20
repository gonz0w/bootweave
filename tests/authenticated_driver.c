/* SPDX-License-Identifier: BSD-3-Clause */
/* Portable composition fixture only. The included fixed exec double performs
 * actual descriptor/argv/environment checks; it never executes signify. */
#ifdef __OpenBSD__
#define _BSD_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include "authenticated_review.h"
#define main bw_ar_legacy_signature_main
#define bw_v_close bw_ar_base_close
#define bw_v_fork bw_ar_base_fork
#define bw_v_setpgid bw_ar_base_setpgid
#define bw_v_waitid bw_ar_base_waitid
#define bw_v_wait4 bw_ar_base_wait4
#define bw_v_kill bw_ar_base_kill
#define bw_v_open bw_ar_base_open
#define bw_v_pipe bw_ar_base_pipe
#ifndef TEST_REAL_PLATFORM
#define bw_sv_parent_boundary bw_ar_base_boundary
#endif
#include "signature_driver.c"
#undef main
#undef bw_v_close
#undef bw_v_fork
#undef bw_v_setpgid
#undef bw_v_waitid
#undef bw_v_wait4
#undef bw_v_kill
#undef bw_v_open
#undef bw_v_pipe
#ifndef TEST_REAL_PLATFORM
#undef bw_sv_parent_boundary
#endif
#include <CommonCrypto/CommonDigest.h>

static struct bw_auth_snapshot ar_snapshot;
static struct bw_verifier_material ar_material, ar_invalid_material;
static struct bw_authenticated_observation ar_observation;
static unsigned char ar_text[BW_CONTENT_OUTPUT_MAX + 3], ar_text_before[sizeof(ar_text)];
static const char *ar_fault, *ar_phase, *ar_origin;
static long long ar_offset, ar_pure_offset;
static struct timespec ar_started;
static const unsigned char *ar_key;
static size_t ar_key_length;
static unsigned ar_prepare_calls, ar_signature_calls, ar_content_calls, ar_policy_calls;
static unsigned ar_hash_calls, ar_inspect_calls, ar_content_formats, ar_formats;
static unsigned ar_fused_releases, ar_signature_releases, ar_terminal_consumptions;
static unsigned ar_early_boundaries, ar_signature_boundaries, ar_clock_calls;
static unsigned ar_after_final, ar_order_errors, ar_unlink_failures, ar_hash_fail;
static int ar_in_call, ar_in_signature, ar_in_release, ar_in_early_boundary, ar_final;
static int ar_prepare_identity = 1, ar_span_identity = 1, ar_setup_uncertain;

enum bw_material_result bw_ar_real_release(struct bw_verifier_material *);

static int
ar_is(const char *name)
{
    return !strcmp(ar_fault, name);
}

static int
ar_at(const char *phase)
{
    return !strcmp(ar_phase, phase);
}

static void
ar_process_call(void)
{
    if (ar_in_call && ar_final && parent()) ar_after_final++;
}

/* Delegate the existing custody double, adding only the final-boundary witness. */
int bw_v_close(int fd) { ar_process_call(); return bw_ar_base_close(fd); }
pid_t bw_v_fork(void) { ar_process_call(); return bw_ar_base_fork(); }
int bw_v_setpgid(pid_t pid, pid_t group) {
    ar_process_call(); return bw_ar_base_setpgid(pid, group);
}
int bw_v_waitid(idtype_t type, id_t pid, siginfo_t *info, int options) {
    ar_process_call(); return bw_ar_base_waitid(type, pid, info, options);
}
pid_t bw_v_wait4(pid_t pid, int *status, int options, struct rusage *usage) {
    ar_process_call(); return bw_ar_base_wait4(pid, status, options, usage);
}
int bw_v_kill(pid_t pid, int signal_number) {
    ar_process_call(); return bw_ar_base_kill(pid, signal_number);
}
int bw_v_open(const char *path, int flags, ...) {
    ar_process_call(); return bw_ar_base_open(path, flags);
}
int bw_v_pipe(int pair[2]) { ar_process_call(); return bw_ar_base_pipe(pair); }

int
bw_ar_material_close(int fd)
{
    int result;
    ar_process_call();
    result = bw_v_close(fd);
    if ((ar_in_call && ar_in_release && ar_is("release-close-uncertain")) || ar_setup_uncertain) {
        errno = EINTR;
        return -1; /* The actual owned close occurred; uncertainty is modeled. */
    }
    return result;
}

int
bw_ar_unlinkat(int directory, const char *name, int flags)
{
    ar_process_call();
    if (ar_in_call && ar_is("prepare-unlink-error") && !ar_unlink_failures++) {
        errno = EIO;
        return -1;
    }
    return unlinkat(directory, name, flags);
}

enum bw_material_result
bw_verifier_release(struct bw_verifier_material *material_object)
{
    enum bw_material_result result;
    int was_live = material_object && (material_object->state == BW_VM_RESERVED ||
        material_object->state == BW_VM_READY || material_object->state == BW_VM_VERIFYING);
    if (ar_in_call) {
        ar_process_call();
        if (ar_in_signature) ar_signature_releases++;
        else ar_fused_releases++;
    }
    ar_in_release = 1;
    result = bw_ar_real_release(material_object);
    ar_in_release = 0;
    if (ar_in_call && was_live) ar_terminal_consumptions++;
    return result;
}

#ifndef TEST_REAL_PLATFORM
int
bw_sv_parent_boundary(int final)
{
    if (ar_in_call && final) {
        if (ar_material.state == BW_VM_RESERVED || ar_material.state == BW_VM_READY ||
            ar_material.state == BW_VM_VERIFYING) ar_order_errors++;
        if (!ar_in_early_boundary) ar_signature_boundaries++;
        ar_final = 1;
    }
    return bw_ar_base_boundary(final);
}
#endif

int
bw_ar_early_boundary(int final)
{
    int result;
    if (final != 1) ar_order_errors++;
    ar_early_boundaries++;
    if (ar_material.state == BW_VM_RESERVED || ar_material.state == BW_VM_READY ||
        ar_material.state == BW_VM_VERIFYING) ar_order_errors++;
    ar_in_early_boundary = 1;
    result = bw_sv_parent_boundary(final);
    ar_in_early_boundary = 0;
    ar_final = 1;
    return ar_is("early-final-boundary-failure") ? -1 : result;
}

enum bw_material_result
bw_ar_prepare(const struct bw_verifier_request *request,
    struct bw_verifier_material *material_object)
{
    enum bw_material_result result;
    unsigned i;
    ar_prepare_calls++;
    ar_process_call();
    if (material_object != &ar_material || request->key == bw_compiled_policy.key ||
        request->key_length != bw_compiled_policy.key_length ||
        memcmp(request->key, bw_compiled_policy.key, request->key_length) ||
        request->definition != ar_snapshot.definition ||
        request->definition_length != ar_snapshot.definition_length ||
        request->signature != ar_snapshot.signature ||
        request->signature_length != ar_snapshot.signature_length) ar_prepare_identity = 0;
    ar_key = request->key;
    ar_key_length = request->key_length;
    result = bw_verifier_prepare(request, material_object);
    if (result != BW_MATERIAL_OK) ar_terminal_consumptions++;
    else {
        for (i = 0; i < 3; i++) {
            material[i] = material_object->fd[i];
            track(material[i]);
        }
        active = 1;
        if (ar_is("cancel") && ar_at("after-prepare")) cancelled = 1;
        if (ar_is("mutate-definition-after-prepare")) ar_snapshot.definition[0] ^= 1;
        if (ar_is("mutate-key-after-prepare")) ((unsigned char *)ar_key)[0] ^= 1;
    }
    return result;
}

enum bw_signature_result
bw_ar_signature(struct bw_verifier_material *material_object,
    struct bw_signature_observation *observation_object)
{
    enum bw_signature_result result;
    ar_signature_calls++;
    ar_process_call();
    if (material_object != &ar_material || observation_object != &ar_observation.signature ||
        material_object->state != BW_VM_READY || observation_object->state != BW_SV_EMPTY)
        ar_order_errors++;
    ar_in_signature = 1;
    active = 1;
    result = bw_signature_verify(material_object, observation_object);
    ar_in_signature = 0;
    if (ar_is("cancel") && ar_at("after-signature")) cancelled = 1;
    if (ar_is("mutate-definition-after-signature")) ar_snapshot.definition[0] ^= 1;
    if (ar_is("mutate-payload-after-signature")) ar_snapshot.payload[0] ^= 1;
    return result;
}

int
bw_ar_content(const struct bw_policy_request *request, char *text, size_t capacity,
    struct bw_content_observation *observation_object)
{
    int result;
    ar_content_calls++;
    if (!ar_final || request->key != ar_key || request->key_length != ar_key_length ||
        request->definition != ar_snapshot.definition ||
        request->definition_length != ar_snapshot.definition_length ||
        request->payload != ar_snapshot.payload ||
        request->payload_length != ar_snapshot.payload_length) ar_span_identity = 0;
    result = bw_content_review(request, text, capacity, observation_object);
    if (ar_is("cancel") && ar_at("after-content")) cancelled = 1;
    return result;
}

int
bw_ar_policy(const struct bw_policy_request *request, struct bw_policy_observation *out)
{
    ar_policy_calls++;
    if (request->key != ar_key || request->key_length != ar_key_length ||
        request->definition != ar_snapshot.definition || request->payload != ar_snapshot.payload)
        ar_span_identity = 0;
    return bw_policy_bind(request, out);
}

int
bw_policy_sha256(const unsigned char *bytes, size_t length, unsigned char digest[32])
{
    ar_hash_calls++;
    if (ar_hash_calls == 1) {
        if (bytes != ar_snapshot.definition || length != ar_snapshot.definition_length) ar_span_identity = 0;
    } else if (bytes != ar_snapshot.payload || length != ar_snapshot.payload_length) ar_span_identity = 0;
    if (ar_hash_calls == ar_hash_fail) { memset(digest, 0x5a, 32); return 0; }
    return length <= BW_AUTH_PAYLOAD_MAX && CC_SHA256(bytes, (CC_LONG)length, digest) == digest;
}

int
bw_ar_structure(const unsigned char *bytes, size_t length, struct bw_payload_summary *summary)
{
    ar_inspect_calls++;
    if (bytes != ar_snapshot.payload || length != ar_snapshot.payload_length) ar_span_identity = 0;
    return bw_payload_structure_review(bytes, length, summary);
}

int
bw_ar_content_snprintf(char *text, size_t capacity, const char *format, ...)
{
    va_list args;
    int result;
    ar_content_formats++;
    va_start(args, format); result = vsnprintf(text, capacity, format, args); va_end(args);
    return result;
}

int
bw_ar_snprintf(char *text, size_t capacity, const char *format, ...)
{
    va_list args;
    int result;
    ar_formats++;
    va_start(args, format); result = vsnprintf(text, capacity, format, args); va_end(args);
    if (ar_is("cancel") && ar_at("after-format")) cancelled = 1;
    if (ar_is("authenticated-format-negative")) return -1;
    if (ar_is("authenticated-format-truncated")) return (int)capacity;
    return result;
}

void *
bw_ar_memcpy(void *destination, const void *source, size_t length)
{
    void *result = memcpy(destination, source, length);
    if (ar_is("cancel")) {
        if (ar_at("before-prepare") && source == bw_compiled_policy.key) cancelled = 1;
        if (ar_at("after-publication") && destination == ar_text + 1) cancelled = 1;
    }
    return result;
}

static unsigned
ar_clock_target(void)
{
    if (ar_at("before-prepare")) return 2;
    if (ar_at("after-prepare")) return 3;
    if (ar_at("after-signature")) return 4;
    if (ar_at("after-content")) return 6;
    if (ar_at("after-format")) return 7;
    if (ar_at("after-publication")) return 8;
    return 0;
}

static void
ar_add_time(struct timespec *out, long long nanoseconds)
{
    out->tv_sec += (time_t)(nanoseconds / 1000000000LL);
    out->tv_nsec += (long)(nanoseconds % 1000000000LL);
    if (out->tv_nsec >= 1000000000L) { out->tv_sec++; out->tv_nsec -= 1000000000L; }
}

int
bw_ar_clock_gettime(clockid_t id, struct timespec *out)
{
    unsigned target = ar_clock_target();
    ar_clock_calls++;
    if (id != CLOCK_MONOTONIC) ar_order_errors++;
    *out = ar_started;
    /* The default observed time is one second after the ORIGINAL reserve.
     * Backward controls stay above that origin and test last-sample ordering. */
    ar_add_time(out, 1000000000LL);
    if (ar_is("clock-at-offset") && ar_clock_calls >= 4 && ar_pure_offset >= 0) {
        *out = ar_started; ar_add_time(out, ar_pure_offset);
    }
    if (ar_clock_calls != target) return 0;
    if (ar_is("clock-error")) return -1;
    if (ar_is("clock-backward")) { *out = ar_started; ar_add_time(out, 500000000LL); }
    if (ar_is("clock-negative-seconds")) out->tv_sec = -1;
    if (ar_is("clock-negative-nanoseconds")) out->tv_nsec = -1;
    if (ar_is("clock-nanoseconds-billion")) out->tv_nsec = 1000000000L;
    if (ar_is("clock-at-offset")) {
        *out = ar_started;
        if (!strcmp(ar_origin, "pure-phase")) ar_add_time(out, ar_pure_offset >= 0 ? ar_pure_offset : 1000000000LL);
        ar_add_time(out, ar_offset);
    }
    return 0;
}

static int
ar_zero(const void *bytes, size_t length)
{
    const unsigned char *p = bytes;
    size_t i;
    for (i = 0; i < length; i++) if (p[i]) return 0;
    return 1;
}

static unsigned
ar_number(const unsigned char *bytes)
{
    return ((unsigned)bytes[0] << 24) | ((unsigned)bytes[1] << 16) |
        ((unsigned)bytes[2] << 8) | bytes[3];
}

static void
ar_hex(const char *text)
{
    while (*text) printf("%02x", (unsigned char)*text++);
}

static int
ar_alias(struct bw_verifier_material **m, const struct bw_auth_snapshot **snapshot,
    char **text, struct bw_authenticated_observation **out)
{
    static const char *const labels[] = {"material", "snapshot", "output", "result", "policy", "cancelled"};
    const void *pointers[5] = {&ar_material, &ar_snapshot, ar_text + 1, &ar_observation, &bw_compiled_policy};
    char name[80];
    unsigned i, j;
    for (i = 0; i < 5; i++) for (j = i + 1; j < 6; j++) {
        snprintf(name, sizeof(name), "%s-%s", labels[i], labels[j]);
        if (!ar_is(name)) continue;
        if (j == 5) ar_material.cancelled = pointers[i];
        else if (i == 0) *m = (void *)pointers[j];
        else if (i == 1) *snapshot = pointers[j];
        else if (i == 2) *text = (void *)pointers[j];
        else if (i == 3) *out = (void *)pointers[j];
        return 1;
    }
    return 0;
}

static void
ar_emit(int result, size_t capacity, int no_write, int immutable, int reuse_ok,
    int material_state, int outside)
{
    struct bw_content_observation *content = &ar_observation.content;
    struct bw_signature_observation *signature = &ar_observation.signature;
    unsigned boundaries = ar_early_boundaries + ar_signature_boundaries;
    int cleared = !content->length && ar_zero(&content->plan, sizeof(content->plan)) &&
        ar_zero(&content->summary, sizeof(content->summary));
    int descriptor = !fake && signature->launch_granted && signature->wait_consumed &&
        !signature->wait_lost && WIFEXITED(signature->wait_status) &&
        (WEXITSTATUS(signature->wait_status) == 0 || WEXITSTATUS(signature->wait_status) == 1);
    const char *boundary_owner = result == BW_AUTH_NOT_TAKEN ? "caller" : !boundaries ? "none" :
        ar_signature_calls ? "signature" : "fused";
    printf("AR_RESULT {\"return\":%d,\"state\":%d,\"cause\":%d,\"material_cause\":%d,"
        "\"signature_cause\":%d,\"content_cause\":%d,\"policy_cause\":%d,\"length\":%zu,"
        "\"prepare\":%u,\"signature\":%u,\"content\":%u,\"policy\":%u,\"hash\":%u,\"inspect\":%u,"
        "\"content_format\":%u,\"authenticated_format\":%u,\"fused_release_calls\":%u,"
        "\"signature_release_calls\":%u,\"material_terminal_consumptions\":%u,"
        "\"final_boundary_calls\":%u,\"boundary_owner\":\"%s\",\"final_boundary_failed\":%u,"
        "\"signature_final_boundary_failed\":%u,\"path_or_process_calls_after_final_boundary\":%u,"
        "\"close_retries\":%u,\"bad_order\":%u,\"prepare_identity\":%d,\"span_identity\":%d,"
        "\"descriptor_witness\":%d,\"cleared\":%d,\"output_zero\":%d,\"outside\":%d,"
        "\"no_write\":%d,\"immutable\":%d,\"reuse_ok\":%d,\"material_state\":%d,"
        "\"clock_calls\":%u,\"wait_consumed\":%u,\"wait_lost\":%u,\"fake\":%d,"
        "\"members\":%zu,\"files\":%zu,\"content_bytes\":%zu}\n",
        result, ar_observation.state, ar_observation.cause, ar_observation.material_cause,
        signature->cause, content->cause, content->policy_cause, content->length,
        ar_prepare_calls, ar_signature_calls, ar_content_calls, ar_policy_calls, ar_hash_calls,
        ar_inspect_calls, ar_content_formats, ar_formats, ar_fused_releases, ar_signature_releases,
        ar_terminal_consumptions, boundaries, boundary_owner, ar_observation.final_boundary_failed,
        signature->final_boundary_failed, ar_after_final, bad_close, bad_order + ar_order_errors,
        ar_prepare_identity, ar_span_identity, descriptor, cleared,
        capacity && capacity <= BW_CONTENT_OUTPUT_MAX ? ar_zero(ar_text + 1, capacity) :
        !memcmp(ar_text, ar_text_before, sizeof(ar_text)), outside, no_write, immutable,
        reuse_ok, material_state, ar_clock_calls, signature->wait_consumed, signature->wait_lost, fake,
        content->summary.members, content->summary.files, content->summary.content_bytes);
    if (result == BW_AUTH_COMPLETE) {
        printf("AR_MODEL "); ar_hex(content->plan.id); putchar(' '); ar_hex(content->plan.name); putchar(' ');
        ar_hex(content->plan.release); putchar(' '); ar_hex(content->plan.architecture); putchar(' ');
        ar_hex(content->plan.file); putchar(' '); ar_hex(content->plan.sha256); putchar('\n');
        if (content->length >= capacity || fwrite(ar_text + 1, 1, content->length, stdout) != content->length)
            exit(96);
    }
}

int
main(int argc, char **argv)
{
    struct bw_verifier_material *m = &ar_material, before_material, before_invalid;
    const struct bw_auth_snapshot *snapshot = &ar_snapshot;
    struct bw_authenticated_observation *out = &ar_observation, before_observation;
    struct bw_verifier_request prepare_request;
    struct sigaction action;
    unsigned char sizes[16], snapshot_before[32], snapshot_after[32];
    unsigned frame_lengths[4], i;
    unsigned prior_calls[8];
    long long length_override;
    size_t capacity;
    char *text = (char *)ar_text + 1;
    int result, immutable, no_write = 0, reuse_ok = 1, sentinel, opened, material_state, outside = 1;
    if (argc != 9) return 90;
    root = argv[1]; ar_fault = argv[2]; ar_phase = argv[4]; ar_origin = argv[5]; owner = getpid();
    capacity = (size_t)strtoul(argv[3], NULL, 10);
    ar_offset = strtoll(argv[6], NULL, 10); ar_pure_offset = strtoll(argv[7], NULL, 10);
    length_override = strtoll(argv[8], NULL, 10);
    if (capacity > 65537 || ar_offset < 0 || ar_offset > 20000000000LL ||
        ar_pure_offset < -1 || ar_pure_offset > 10000000000LL ||
        fread(sizes, 1, sizeof(sizes), stdin) != sizeof(sizes)) return 90;
    for (i = 0; i < 4; i++) frame_lengths[i] = ar_number(sizes + 4 * i);
    if (frame_lengths[0] > sizeof(inputs[0]) || frame_lengths[1] > sizeof(ar_snapshot.signature) ||
        frame_lengths[2] > sizeof(ar_snapshot.definition) || frame_lengths[3] > sizeof(ar_snapshot.payload)) return 91;
    if (fread(inputs[0], 1, frame_lengths[0], stdin) != frame_lengths[0] ||
        fread(ar_snapshot.signature, 1, frame_lengths[1], stdin) != frame_lengths[1] ||
        fread(ar_snapshot.definition, 1, frame_lengths[2], stdin) != frame_lengths[2] ||
        fread(ar_snapshot.payload, 1, frame_lengths[3], stdin) != frame_lengths[3] ||
        getchar() != EOF || ferror(stdin)) return 92;
    ar_snapshot.signature_length = frame_lengths[1]; ar_snapshot.definition_length = frame_lengths[2];
    ar_snapshot.payload_length = frame_lengths[3];
    /* An ordinary malformed retained record is present before the immutable
     * entry witness; only explicit mutate-* cases change bytes during review. */
    if (ar_is("prepare-record-refusal")) ar_snapshot.signature[0] ^= 1;
    lengths[0] = bw_compiled_policy.key_length <= BW_POLICY_KEY_MAX ? bw_compiled_policy.key_length : 0;
    lengths[1] = frame_lengths[1]; lengths[2] = frame_lengths[2];
    memcpy(inputs[0], bw_compiled_policy.key, lengths[0]);
    memcpy(inputs[1], ar_snapshot.signature, lengths[1]); memcpy(inputs[2], ar_snapshot.definition, lengths[2]);
    fault = ar_is("child-reject") || ar_is("material-close-0") || ar_is("final-boundary") ||
        ar_is("modeled-observer-loss") ? ar_fault : "none";
    fake = !strncmp(fault, "modeled-", 8);
    if (ar_is("hashfail1")) ar_hash_fail = 1;
    if (ar_is("hashfail2")) ar_hash_fail = 2;
    for (i = 0; i < 7; i++) fds[i] = -1;
    for (i = 0; i < 3; i++) material[i] = -1;
    memset(&action, 0, sizeof(action)); sigemptyset(&action.sa_mask); action.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &action, NULL)) return 93;
    action.sa_handler = SIG_DFL;
    if (sigaction(SIGCHLD, &action, NULL)) return 93;
    if (bw_verifier_reserve(&cancelled, &ar_material) != BW_MATERIAL_OK) return 94;
    ar_started = ar_material.started;
    opened = open("/dev/null", O_RDONLY);
    if (opened < 0 || (sentinel = fcntl(opened, F_DUPFD_CLOEXEC, 20)) < 0 || close(opened)) return 94;
    if (ar_is("definition-length")) ar_snapshot.definition_length = (size_t)length_override;
    if (ar_is("signature-length")) ar_snapshot.signature_length = (size_t)length_override;
    if (ar_is("payload-length")) ar_snapshot.payload_length = (size_t)length_override;
    if (ar_is("null-zero")) text = NULL;
    if (ar_is("material-ready")) {
        prepare_request = (struct bw_verifier_request){inputs[0], inputs[1], inputs[2], lengths[0], lengths[1], lengths[2]};
        if (bw_verifier_prepare(&prepare_request, &ar_material) != BW_MATERIAL_OK) return 94;
    }
    if (ar_is("material-closed") || ar_is("material-uncertain")) {
        ar_setup_uncertain = ar_is("material-uncertain");
        (void)bw_verifier_release(&ar_material); ar_setup_uncertain = 0;
    }
    if (ar_is("material-empty")) m = &ar_invalid_material;
    if (ar_is("result-working")) ar_observation.state = BW_AR_WORKING;
    if (ar_is("result-finished")) ar_observation.state = BW_AR_FINISHED;
    if (ar_is("null-material")) m = NULL;
    if (ar_is("null-snapshot")) snapshot = NULL;
    if (ar_is("null-output")) text = NULL;
    if (ar_is("null-result")) out = NULL;
    if (ar_is("null-cancelled")) ar_material.cancelled = NULL;
    if (ar_is("overflow-material")) m = (void *)(UINTPTR_MAX - 8);
    if (ar_is("overflow-snapshot")) snapshot = (void *)(UINTPTR_MAX - 8);
    if (ar_is("overflow-output")) text = (void *)(UINTPTR_MAX - 8);
    if (ar_is("overflow-result")) out = (void *)(UINTPTR_MAX - 8);
    if (ar_is("overflow-cancelled")) ar_material.cancelled = (void *)(UINTPTR_MAX - 1);
    (void)ar_alias(&m, &snapshot, &text, &out);
    memset(ar_text, 0xa5, sizeof(ar_text)); memcpy(ar_text_before, ar_text, sizeof(ar_text));
    memcpy(&before_material, &ar_material, sizeof(before_material));
    memcpy(&before_invalid, &ar_invalid_material, sizeof(before_invalid));
    memcpy(&before_observation, &ar_observation, sizeof(before_observation));
    if (CC_SHA256(&ar_snapshot, (CC_LONG)sizeof(ar_snapshot), snapshot_before) != snapshot_before) return 94;
    ar_in_call = 1;
    result = bw_authenticated_review(m, snapshot, text, capacity, out);
    ar_in_call = 0; active = 0;
    if (CC_SHA256(&ar_snapshot, (CC_LONG)sizeof(ar_snapshot), snapshot_after) != snapshot_after) return 94;
    immutable = !memcmp(snapshot_before, snapshot_after, 32);
    material_state = ar_material.state;
    if (result == BW_AUTH_NOT_TAKEN) {
        no_write = immutable && !memcmp(&before_material, &ar_material, sizeof(ar_material)) &&
            !memcmp(&before_invalid, &ar_invalid_material, sizeof(ar_invalid_material)) &&
            !memcmp(&before_observation, &ar_observation, sizeof(ar_observation)) &&
            !memcmp(ar_text, ar_text_before, sizeof(ar_text));
    } else {
        struct bw_verifier_material saved_material;
        struct bw_authenticated_observation saved_observation;
        unsigned char saved_text[32], repeated_text[32];
        unsigned current_calls[8];
        memcpy(&saved_material, &ar_material, sizeof(saved_material));
        memcpy(&saved_observation, &ar_observation, sizeof(saved_observation));
        prior_calls[0] = ar_prepare_calls; prior_calls[1] = ar_signature_calls;
        prior_calls[2] = ar_content_calls; prior_calls[3] = ar_clock_calls;
        prior_calls[4] = ar_fused_releases; prior_calls[5] = ar_signature_releases;
        prior_calls[6] = ar_early_boundaries; prior_calls[7] = ar_signature_boundaries;
        if (CC_SHA256(ar_text, (CC_LONG)sizeof(ar_text), saved_text) != saved_text) return 94;
        opened = open("/dev/null", O_RDONLY);
        if (opened < 0) return 94;
        ar_in_call = 1;
        reuse_ok = bw_authenticated_review(&ar_material, &ar_snapshot, (char *)ar_text + 1,
            capacity, &ar_observation) == BW_AUTH_NOT_TAKEN;
        ar_in_call = 0;
        current_calls[0] = ar_prepare_calls; current_calls[1] = ar_signature_calls;
        current_calls[2] = ar_content_calls; current_calls[3] = ar_clock_calls;
        current_calls[4] = ar_fused_releases; current_calls[5] = ar_signature_releases;
        current_calls[6] = ar_early_boundaries; current_calls[7] = ar_signature_boundaries;
        if (CC_SHA256(ar_text, (CC_LONG)sizeof(ar_text), repeated_text) != repeated_text) return 94;
        reuse_ok = reuse_ok && !memcmp(&saved_material, &ar_material, sizeof(ar_material)) &&
            !memcmp(&saved_observation, &ar_observation, sizeof(ar_observation)) &&
            !memcmp(saved_text, repeated_text, 32) && !memcmp(prior_calls, current_calls, sizeof(prior_calls)) &&
            fcntl(opened, F_GETFD) >= 0;
        if (close(opened)) return 94;
    }
    if (ar_text[0] != 0xa5) outside = 0;
    for (i = capacity && capacity <= BW_CONTENT_OUTPUT_MAX ? (unsigned)capacity + 1 : 1;
         i < sizeof(ar_text); i++) if (ar_text[i] != 0xa5) outside = 0;
    if (result == BW_AUTH_COMPLETE && (ar_observation.content.length >= capacity ||
        !ar_zero(ar_text + 1 + ar_observation.content.length,
            capacity - ar_observation.content.length))) outside = 0;
    /* Caller retains responsibility on NOT_TAKEN; these separately labeled
     * cleanup effects are excluded from the refused call's counters. */
    ar_material.cancelled = &cancelled;
    if (ar_material.state == BW_VM_RESERVED || ar_material.state == BW_VM_READY)
        (void)bw_verifier_release(&ar_material);
    fixture_consume(0); fixture_consume(1);
    if (witness_error) return 98;
    for (i = 3; i < 32; i++) if ((int)i != sentinel && fcntl((int)i, F_GETFD) != -1) return 96;
    if (fcntl(sentinel, F_GETFD) < 0 || close(sentinel)) return 96;
    ar_emit(result, capacity, no_write, immutable, reuse_ok, material_state, outside);
    return 0;
}
