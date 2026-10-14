/* SPDX-License-Identifier: BSD-3-Clause */
/* Research; no wire API. */
#ifndef INSTALLED_ROOT_H
#define INSTALLED_ROOT_H
#include "validate.h"
struct ip_files{
 uint64_t root_device,root_inode,parent_inode,case_inode,mount_time;
 uint32_t flags,fsid[2];char spec[90];
 int rootfd,rawfd,blockfd,parentfd,casefd;
};
struct ip_binding{
 struct storage_label label;unsigned char uid[8],types[16];
 uint32_t label_flags,checksum;struct ip_files held;
};
enum storage_result ip_names(const char *,size_t,unsigned char [8]);
int ip_valid(const struct ip_binding *);
int ip_same(const struct ip_binding *,const struct ip_binding *);
#ifdef __OpenBSD__
struct pl_context;
struct rv_console;
struct ip_context{int rootfd,rawfd,blockfd;
#ifdef AUTH_TEST
 unsigned substitute;
#endif
};
int ip_open(struct ip_context *);
int ip_capture(const struct ip_context *,const struct pl_context *,struct ip_binding *);
int ip_begin(const struct ip_context *,const struct pl_context *,struct rv_console *,struct ip_binding *);
int ip_check(const struct ip_context *,const struct pl_context *,const struct ip_binding *);
int ip_close(struct ip_context *);
#endif
#endif
