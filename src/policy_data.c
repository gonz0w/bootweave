/* SPDX-License-Identifier: BSD-3-Clause */
/* Inert synthetic anchor only: no private key or production publisher exists.
 * Pins tests/fixtures/policy-{definition.json,payload.txt}. No CLI links this.
 */
#include "policy_binding.h"
const struct bw_policy_definition bw_compiled_policy = {
    BW_POLICY_READONLY_V1,
    sizeof("untrusted comment: Bootweave inert policy fixture\n"
        "RWQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n") - 1,
    "untrusted comment: Bootweave inert policy fixture\n"
        "RWQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n",
    {0x02,0x82,0x17,0xbc,0x0c,0xc1,0x66,0x83,0xfb,0x54,0x1c,0xe2,0xa5,0x68,0xc8,0x24,
     0x88,0x40,0x42,0x97,0xa9,0xdf,0x9c,0xe2,0x0b,0xe5,0x81,0xa6,0x26,0x94,0xfb,0xb4},
    "offline-viewer", "7.9", "amd64"
};
