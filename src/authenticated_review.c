/* SPDX-License-Identifier: BSD-3-Clause */
#define _POSIX_C_SOURCE 200809L
#include "authenticated_review.h"
#include "private_span.h"
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#define BW_REPORT_TITLE "AUTHENTICATED CONTENT REVIEW - NO OPERATIONS AUTHORIZED"
#define BW_REPORT_SIGNATURE "Signature authentication: accepted by fixed verifier"
#include "private_content_format.h"

struct workspace {
    unsigned char key[BW_POLICY_KEY_MAX];
    char text[2048];
    struct bw_content_observation content;
    struct bw_verifier_request verifier;
    struct bw_policy_request policy;
    struct timespec last, phase;
};
typedef char local_bound[(sizeof(struct workspace) + 6 * sizeof(struct span) +
    sizeof(struct timespec) + 256 <= 8192) ? 1 : -1];
typedef char application_bound[(sizeof(struct bw_auth_snapshot) +
    BW_CONTENT_OUTPUT_MAX + sizeof(struct bw_verifier_material) +
    sizeof(struct bw_authenticated_observation) +
    8192 + 8192 + 8192 + 4096 + 2048 + 34816 + 24576 <= 2 * 1024 * 1024) ? 1 : -1];

static int
disjoint(struct bw_verifier_material *m, const struct bw_auth_snapshot *snapshot,
    char *text, size_t capacity, struct bw_authenticated_observation *out)
{
    struct span s[6];
    if (!m || !snapshot || !out || !range(m, sizeof(*m), &s[0]) ||
        !range(snapshot, sizeof(*snapshot), &s[1]) ||
        !range(out, sizeof(*out), &s[2]) || !range(text, capacity, &s[3]) ||
        !range(&bw_compiled_policy, sizeof(bw_compiled_policy), &s[4]) ||
        !separate(s, 5)) return 0;
    /* All fixed peers precede member reads, including the signal-flag pointer. */
    return range(m->cancelled, sizeof(*m->cancelled), &s[5]) && separate(s, 6) &&
        m->state == BW_VM_RESERVED && out->state == BW_AR_EMPTY;
}

static int
earlier(const struct timespec *a, const struct timespec *b)
{
    return a->tv_sec < b->tv_sec ||
        (a->tv_sec == b->tv_sec && a->tv_nsec < b->tv_nsec);
}

static int
within(const struct timespec *now, const struct timespec *origin, time_t seconds)
{
    return now->tv_sec - origin->tv_sec < seconds ||
        (now->tv_sec - origin->tv_sec == seconds && now->tv_nsec < origin->tv_nsec);
}

static int
live(struct bw_verifier_material *m, struct workspace *w,
    const struct timespec *phase, time_t seconds,
    struct bw_authenticated_observation *out)
{
    struct timespec now;
    if (*m->cancelled) { out->cause = BW_AR_CANCEL; return 0; }
    if (clock_gettime(CLOCK_MONOTONIC, &now) == -1 || now.tv_sec < 0 ||
        now.tv_nsec < 0 || now.tv_nsec >= 1000000000L || earlier(&now, &w->last)) {
        out->cause = BW_AR_CLOCK; return 0;
    }
    w->last = now;
    if (!within(&now, &m->started, 11) || !within(&now, phase, seconds)) {
        out->cause = BW_AR_DEADLINE; return 0;
    }
    return 1;
}

static int
output_ready(void)
{
    struct stat sink;
    int flags = fcntl(STDOUT_FILENO, F_GETFL);
    return flags != -1 && (flags & O_NONBLOCK) &&
        ((flags & O_ACCMODE) == O_WRONLY || (flags & O_ACCMODE) == O_RDWR) &&
        fstat(STDOUT_FILENO, &sink) == 0 && (S_ISREG(sink.st_mode) ||
        S_ISFIFO(sink.st_mode) || (S_ISCHR(sink.st_mode) && isatty(STDOUT_FILENO)));
}

static enum bw_authenticated_result
review(struct bw_verifier_material *m,
    const struct bw_auth_snapshot *snapshot, char *text, size_t capacity,
    struct bw_authenticated_observation *out, int to_stdout)
{
    struct workspace w;
    enum bw_material_result material;
    enum bw_signature_result signature;
    enum bw_content_cause content_cause;
    enum bw_policy_cause policy_cause;
    int length, owned = 1, entered = 0, uncertain = 0, complete = 0;
    int valid_capacity = capacity && capacity <= BW_CONTENT_OUTPUT_MAX;
    ssize_t n;
    if (!disjoint(m, snapshot, text, capacity, out)) return BW_AUTH_NOT_TAKEN;
    memset(out, 0, sizeof(*out)); out->state = BW_AR_WORKING;
    out->cause = BW_AR_ARGUMENT;
    if (valid_capacity) memset(text, 0, capacity);
    if ((!to_stdout && !valid_capacity) || !snapshot->definition_length ||
        snapshot->definition_length > BW_AUTH_DEFINITION_MAX ||
        !snapshot->signature_length || snapshot->signature_length > BW_AUTH_SIGNATURE_MAX ||
        !snapshot->payload_length || snapshot->payload_length > BW_AUTH_PAYLOAD_MAX) goto finish;
    w.last = m->started;
    if (!live(m, &w, &m->started, 2, out)) goto finish;
    out->cause = BW_AR_POLICY;
    if (bw_compiled_policy.profile != BW_POLICY_READONLY_V1 ||
        !bw_compiled_policy.key_length || bw_compiled_policy.key_length > sizeof(w.key)) goto finish;
    memcpy(w.key, bw_compiled_policy.key, bw_compiled_policy.key_length);
    w.verifier = (struct bw_verifier_request){w.key, snapshot->signature, snapshot->definition,
        bw_compiled_policy.key_length, snapshot->signature_length, snapshot->definition_length};
    if (!live(m, &w, &m->started, 2, out)) goto finish;
    if (to_stdout && !output_ready()) { out->cause = BW_AR_OUTPUT; goto finish; }
    out->cause = BW_AR_MATERIAL;
    material = bw_verifier_prepare(&w.verifier, m);
    owned = material == BW_MATERIAL_OK; /* Failure has already consumed it. */
    if (!owned) { uncertain = material == BW_MATERIAL_UNCERTAIN; goto finish; }
    if (!live(m, &w, &m->started, 2, out)) goto finish;
    owned = 0; entered = 1; /* The sole signature stage now owns all cleanup. */
    out->cause = BW_AR_SIGNATURE;
    signature = bw_signature_verify(m, &out->signature);
    uncertain = signature == BW_SIGNATURE_UNCERTAIN;
    if (signature != BW_SIGNATURE_ACCEPTED) goto finish;
    if (!live(m, &w, &m->started, 11, out)) goto finish;
    w.phase = w.last;
    w.policy = (struct bw_policy_request){w.key, snapshot->definition, snapshot->payload,
        bw_compiled_policy.key_length, snapshot->definition_length, snapshot->payload_length};
    if (!live(m, &w, &w.phase, 2, out)) goto finish;
    out->cause = BW_AR_CONTENT;
    if (!bw_content_review(&w.policy, w.text, sizeof(w.text), &w.content)) {
        out->content.cause = w.content.cause;
        out->content.policy_cause = w.content.policy_cause;
        goto finish;
    }
    if (!live(m, &w, &w.phase, 2, out)) goto finish;
    out->cause = BW_AR_FORMAT;
    length = content_format(w.text, sizeof(w.text), &w.content.plan, &w.content.summary);
    if (length < 0 || (size_t)length >= sizeof(w.text) || (!to_stdout && (size_t)length >= capacity)) goto finish;
    if (!live(m, &w, &w.phase, 2, out)) goto finish;
    if (to_stdout) {
        out->cause = BW_AR_OUTPUT;
        while (out->output_bytes < (size_t)length) {
            if (!live(m, &w, &w.phase, 2, out)) goto finish;
            out->output_attempted = 1;
            n = write(STDOUT_FILENO, w.text + out->output_bytes,
                (size_t)length - out->output_bytes);
            if (n < 0 && errno == EINTR) continue;
            if (n <= 0 || (size_t)n > (size_t)length - out->output_bytes) goto finish;
            out->output_bytes += (size_t)n;
        }
    } else memcpy(text, w.text, (size_t)length + 1);
    out->content = w.content; out->content.length = (size_t)length;
    if (!live(m, &w, &w.phase, 2, out)) goto finish;
    out->cause = BW_AR_NONE; complete = 1;
finish:
    if (owned && bw_verifier_release(m) == BW_MATERIAL_UNCERTAIN) uncertain = 1;
    if (!entered && bw_sv_parent_boundary(1) == -1) out->final_boundary_failed = 1;
    /* After final reduction there are only private memory operations. */
    out->material_cause = m->cause;
    if (!complete) {
        if (valid_capacity) memset(text, 0, capacity);
        content_cause = out->content.cause; policy_cause = out->content.policy_cause;
        memset(&out->content, 0, sizeof(out->content));
        out->content.cause = content_cause; out->content.policy_cause = policy_cause;
    }
    out->state = BW_AR_FINISHED;
    return uncertain || (!complete && out->output_attempted) ? BW_AUTH_UNCERTAIN :
        complete ? BW_AUTH_COMPLETE : BW_AUTH_REJECTED;
}

enum bw_authenticated_result
bw_authenticated_review(struct bw_verifier_material *m,
    const struct bw_auth_snapshot *snapshot, char *text, size_t capacity,
    struct bw_authenticated_observation *out)
{
    return review(m, snapshot, text, capacity, out, 0);
}

enum bw_authenticated_result
bw_authenticated_output(struct bw_verifier_material *m,
    const struct bw_auth_snapshot *snapshot, struct bw_authenticated_observation *out)
{
    return review(m, snapshot, NULL, 0, out, 1);
}
