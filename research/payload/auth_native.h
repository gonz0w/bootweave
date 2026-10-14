/* SPDX-License-Identifier: BSD-3-Clause */
/* Private research; no installer/wire API. */
#ifndef AUTH_NATIVE_H
#define AUTH_NATIVE_H
#include <stddef.h>
#define AUTH_INPUT 1048576U
#define AUTH_RECORD 2048U
enum auth_failure { AUTH_OK, AUTH_CONTEXT, AUTH_TOOLS, AUTH_INPUT_BAD,
 AUTH_IDENTITY, AUTH_SIGNATURE, AUTH_INSPECTION, AUTH_IO, AUTH_DEADLINE,
 AUTH_OUTPUT, AUTH_RESOURCE, AUTH_INTERRUPTED, AUTH_CLEANUP, AUTH_PLACEMENT_BAD };
#ifdef AUTH_PLACEMENT
struct pl_context;
#endif
#ifdef AUTH_REVIEW
struct rv_console;
#endif
#ifdef AUTH_INSTALLED
struct ip_context;
#endif
struct auth_context {
#ifdef AUTH_INSTALLED
 struct ip_context *installed;
#endif
 size_t length, key_length;
 unsigned char digest[32], verifier_digest[32], inspector_digest[32];
 unsigned char key[AUTH_RECORD];
#ifdef AUTH_PLACEMENT
 struct pl_context *placement;
#endif
#ifdef AUTH_REVIEW
 struct rv_console *review;
#endif
};
struct auth_result {
 enum auth_failure failure;
 int verified, completed, verifier_launches, inspector_launches, reaped;
 long child_peak_kib, controller_peak_kib;
 char summary[128];
};
struct auth_result auth_native(const struct auth_context *, const unsigned char *, size_t);
#ifdef __OpenBSD__
struct auth_child {
 int status;
 size_t out_length, err_length;
 unsigned char out[4096], err[4096];
 long peak_kib;
};
int auth_run(char *const [], const unsigned char *, size_t, struct auth_child *,
 long long, int, struct auth_result *);
int auth_hash_file(const char *, unsigned char [32]);
long long auth_now(void);
int auth_live(long long);
#endif
#endif
