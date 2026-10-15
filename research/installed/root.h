/* SPDX-License-Identifier: BSD-3-Clause */
/* Research; no wire API. */
#ifndef INSTALLED_ROOT_H
#define INSTALLED_ROOT_H
#include "validate.h"
struct ip_files{
uint64_t rootdev,rootino,parentino,caseino,mounttime,nodeino;
uint32_t nodemode,nodegid,nodelinks,nodeflags;
uint32_t flags,fsid[2];char spec[90];
int rootfd,rawfd,partfd,parentfd,casefd;
};
struct ip_bind{
struct storage_label label;unsigned char uid[8],types[16];
uint32_t labelflags,checksum;struct ip_files h;
};
enum storage_result ip_names(const char *,size_t,unsigned char [8]);
int ip_valid(const struct ip_bind *);
int ip_same(const struct ip_bind *,const struct ip_bind *);
#ifdef __OpenBSD__
struct pl_context;
struct rv_console;
struct ip_ctx{int rootfd,rawfd,partfd;
#ifdef AUTH_TEST
unsigned substitute;
#endif
};
int ip_open(struct ip_ctx *);
int ip_cap(const struct ip_ctx *,const struct pl_context *,struct ip_bind *);
int ip_begin(const struct ip_ctx *,const struct pl_context *,struct rv_console *,struct ip_bind *);
int ip_check(const struct ip_ctx *,const struct pl_context *,const struct ip_bind *);
int ip_close(struct ip_ctx *);
#endif
#endif
