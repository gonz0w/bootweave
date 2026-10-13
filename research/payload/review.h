/* SPDX-License-Identifier: BSD-3-Clause */
/* Private compiled research binding; not a wire or installer interface. */
#ifndef PAYLOAD_REVIEW_H
#define PAYLOAD_REVIEW_H
#include <stdint.h>
#include "placement.h"
struct rv_binding {
 struct pl_plan plan;
 uint64_t length, device, inode, owner, group, mode;
 unsigned char payload[32], key[32], verifier[32], inspector[32];
};
struct rv_console { int in, out; char case_id[32]; uint64_t revision; };
int rv_valid(const struct rv_binding *);
int rv_same(const struct rv_binding *, const struct rv_binding *);
int rv_console(struct rv_console *, const struct rv_binding *, long long);
int rv_quiet(struct rv_console *);
long long rv_now(void);
#ifdef __OpenBSD__
struct auth_context;
struct auth_result;
int rv_phase(struct rv_console *, const struct auth_context *, unsigned char *, size_t, long long);
int rv_report(struct rv_console *, const struct pl_result *, const struct auth_result *);
#endif
#endif
