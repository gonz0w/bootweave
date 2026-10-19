/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_AUTH_MATERIAL_H
#define BOOTWEAVE_AUTH_MATERIAL_H
#include <sys/stat.h>
#include <signal.h>
#include <stddef.h>
#include <time.h>

enum bw_material_state { BW_VM_EMPTY, BW_VM_RESERVED, BW_VM_WORKING, BW_VM_READY,
    BW_VM_CLOSED, BW_VM_REJECTED, BW_VM_UNCERTAIN };
enum bw_material_cause { BW_VM_NONE, BW_VM_ARGUMENT, BW_VM_RECORD, BW_VM_CANCEL,
    BW_VM_DEADLINE, BW_VM_IO, BW_VM_OWNERSHIP, BW_VM_CLOSE };
enum bw_material_result { BW_MATERIAL_UNCERTAIN = -1, BW_MATERIAL_REJECTED,
    BW_MATERIAL_OK };
struct bw_verifier_request {
    const unsigned char *key, *signature, *definition;
    size_t key_length, signature_length, definition_length;
};
/* Internal exclusive ownership object, initially {0}; not a wire format.
 * Only state/cause, reserved directory, ready fd[0..2] (key/signature/definition), and the private
 * observation fields path/identity/close masks are caller-visible. Do not mutate
 * live internals, concurrently rebind/advance owned fds, or reuse consumed state.
 */
struct bw_verifier_material {
    enum bw_material_state state;
    enum bw_material_cause cause;
    int directory, writer[3], fd[3];
    unsigned created, known, directory_known, directory_lost, uncertain, close_errors, unknown_closes;
    struct stat identity, file[3];
    struct timespec started;
    const volatile sig_atomic_t *cancelled;
    char path[256];
};
/* Format checks only; never cryptographic verification. */
int bw_signify_records(const unsigned char *, size_t, const unsigned char *, size_t);
/* Reserve before confinement/capture; directory >=5 leaves fixed roles intact.
 * Original two-second cooperative deadline and cancellation span both calls.
 * Request, material, flag and immutable bytes must be valid/disjoint; no callback,
 * path/tool/policy selection. Future entry must install hard timers/confinement.
 * Preparation produces only three anonymous read-only CLOEXEC handles.
 */
enum bw_material_result bw_verifier_reserve(const volatile sig_atomic_t *, struct bw_verifier_material *);
enum bw_material_result bw_verifier_prepare(const struct bw_verifier_request *, struct bw_verifier_material *);
/* Consume RESERVED or READY exactly once; unknown closure never authorizes retry. */
enum bw_material_result bw_verifier_release(struct bw_verifier_material *);
#endif
