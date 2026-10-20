/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_PRIVATE_CONTENT_FORMAT_H
#define BOOTWEAVE_PRIVATE_CONTENT_FORMAT_H
#include "content_review.h"
#include <stdio.h>

/* Only the two closed entries select these fixed literals at compile time. */
static int
content_format(char *text, size_t capacity, const struct bw_plan *plan,
    const struct bw_payload_summary *summary)
{
    return snprintf(text, capacity,
        BW_REPORT_TITLE "\n"
        "Compiled policy: matched\n"
        "Definition SHA-256: matched compiled pin\n"
        "Payload SHA-256: matched definition\n"
        "Archive structure: complete restricted ustar\n"
        BW_REPORT_SIGNATURE "\n"
        "Project ID (policy-bound): %s\n"
        "Project name (supplied): %s\n"
        "Requested release (policy-bound): %s\n"
        "Requested architecture (policy-bound): %s\n"
        "Payload file (supplied label): %s\n"
        "Archive members: %zu\n"
        "Regular files: %zu\n"
        "Content bytes: %zu\n"
        "Target: unselected; no device identity collected\n"
        "Operations authorized: none\n"
        "Operations performed: none\n"
        "Operations independently observed: none\n",
        plan->id, plan->name, plan->release, plan->architecture, plan->file,
        summary->members, summary->files, summary->content_bytes);
}
#endif
