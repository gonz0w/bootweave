/* SPDX-License-Identifier: BSD-3-Clause */
/* Private compiled research context; never a public/wire contract. */
#ifndef PLACEMENT_H
#define PLACEMENT_H
#include <stddef.h>
#define PL_NODES 128
struct pl_node { char path[256]; size_t offset, size; int directory, explicit_member, parent; };
struct pl_plan { struct pl_node nodes[PL_NODES]; size_t count, members, files, bytes; };
int pl_plan(const unsigned char *, size_t, struct pl_plan *);
#ifdef __OpenBSD__
#include <sys/stat.h>
struct auth_result;
/* Expectations are separate trusted fixture observations, never archive claims. */
struct pl_expected { const char *path; int directory; size_t size; unsigned char digest[32]; };
enum pl_test { PL_REAL, PL_SETUP, PL_SHORT, PL_CLOSE, PL_SYNC, PL_SIGNAL,
 PL_LOSS, PL_STALL, PL_OUTPUT, PL_DIAGNOSTIC, PL_EXIST, PL_LINK,
 PL_REPLACE_DIR, PL_RENAME_ROOT, PL_AFTER_HOLD, PL_FILE_SWAP, PL_CONTROLLER_SIGNAL };
struct pl_result { size_t planned, submitted, observed; int launched, reaped, complete, uncertain; long peak_kib; };
struct pl_context {
 int rootfd, parentfd; struct stat identity; const struct pl_expected *expected;
 size_t count; enum pl_test test; struct pl_result result;
};
int pl_prerequisites(int, const struct stat *, long *);
int pl_phase(struct pl_context *, const unsigned char *, size_t, const char *, long long, struct auth_result *);
#endif
#endif
