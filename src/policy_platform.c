/* SPDX-License-Identifier: BSD-3-Clause */
/* Native base-libc hashing only; never a configurable tool or algorithm. */
#include "policy_binding.h"
#include "auth_input.h"
#ifdef __OpenBSD__
#include <sha2.h>
typedef char native_hash_workspace[(sizeof(SHA2_CTX) + BW_POLICY_COMMON_WORKSPACE <= 2048) ? 1 : -1];
#endif

int
bw_policy_sha256(const unsigned char *data, size_t length, unsigned char out[32])
{
#ifdef __OpenBSD__
    SHA2_CTX context;
    if (!data || !out || length > BW_AUTH_PAYLOAD_MAX) return 0;
    SHA256Init(&context);
    SHA256Update(&context, data, length);
    SHA256Final(out, &context);
    return 1;
#else
    (void)data; (void)length; (void)out;
    return 0;
#endif
}
