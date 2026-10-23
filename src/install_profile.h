/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef BOOTWEAVE_INSTALL_PROFILE_H
#define BOOTWEAVE_INSTALL_PROFILE_H
#include <stddef.h>

/* Separate from advisory metadata-v1. These are DATA bounds, not a root grant. */
#define BWI_PROFILE_MAX 65536
#define BWI_ARTIFACTS 32
#define BWI_PRINCIPALS 4
#define BWI_FILES 64
#define BWI_INPUTS 16
#define BWI_SERVICES 8
#define BWI_STEPS 128
#define BWI_PLAN_MAX 16384

struct bwi_artifact { char id[33], kind[9], file[129], sha256[65]; };
struct bwi_principal { char name[33]; };
struct bwi_file {
    char artifact[33], member[193], target[193], owner[33], group[33], mode[5];
};
struct bwi_input { char name[33], target[193], group[33], mode[5]; };
struct bwi_service { char name[33], entry[193], template[33]; };
struct bwi_policy { char management[33], ssh[33], network[33]; };
struct bwi_observability { char log[33], health[33]; };
struct bwi_profile {
    char project[33], release[4], architecture[6], firmware[5], handoff[21];
    size_t artifacts, principals, files, inputs, services;
    struct bwi_artifact artifact[BWI_ARTIFACTS];
    struct bwi_principal principal[BWI_PRINCIPALS];
    struct bwi_file file[BWI_FILES];
    struct bwi_input input[BWI_INPUTS];
    struct bwi_service service[BWI_SERVICES];
    struct bwi_policy policy;
    struct bwi_observability observability;
};

enum bwi_operation {
    BWI_SELECT_RELEASE = 1, BWI_CONFIRM_TARGET, BWI_INSTALL_STOCK,
    BWI_CREATE_PRINCIPAL, BWI_PLACE_VERIFIED, BWI_BIND_PRIVATE_INPUT,
    BWI_CONFIGURE_POLICY, BWI_CONFIGURE_OBSERVABILITY, BWI_REGISTER_SERVICE,
    BWI_OBSERVE_HANDOFF
};
struct bwi_step { enum bwi_operation operation; size_t index; };
struct bwi_plan { size_t count; struct bwi_step step[BWI_STEPS]; };

/* No file, authentication, target, resource, process or readiness assertions. */
int bwi_profile_parse(const unsigned char *, size_t, struct bwi_profile *);
int bwi_profile_validate(const struct bwi_profile *);
int bwi_plan_build(const struct bwi_profile *, struct bwi_plan *);
int bwi_plan_render(const struct bwi_profile *, const struct bwi_plan *,
    char *, size_t, size_t *);
#endif
