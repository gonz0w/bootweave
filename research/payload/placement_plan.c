/* SPDX-License-Identifier: BSD-3-Clause */
/* Original complete-buffer planner. No I/O, effects or selected executables. */
#include "placement.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
static int zeros(const unsigned char *b, size_t n) {
 size_t i; for (i = 0; i < n; i++) if (b[i]) return 0; return 1;
}
static int number(const unsigned char *b, size_t n, uint64_t *v) {
 size_t i = 0; *v = 0;
 while (i < n && b[i] >= '0' && b[i] <= '7') {
  if (*v > (UINT64_MAX - (b[i] - '0')) / 8) return -1;
  *v = *v * 8 + b[i++] - '0';
 }
 if (!i || i == n) return -1;
 for (; i < n; i++) if (b[i] && b[i] != ' ') return -1;
 return 0;
}
static int text(const unsigned char *b, size_t cap, size_t *n) {
 *n = 0; while (*n < cap && b[*n]) (*n)++;
 return zeros(b + *n, cap - *n) ? 0 : -1;
}
static int name(const unsigned char *b, int dir, char out[256]) {
 size_t n, p, i, start = 0, depth = 0, len;
 if (text(b, 100, &n) || !n || text(b + 345, 155, &p)) return -1;
 len = n + (p ? p + 1 : 0); if (len > 255) return -1;
 if (p) { memcpy(out, b + 345, p); out[p] = '/'; }
 memcpy(out + (p ? p + 1 : 0), b, n);
 if (dir && out[len - 1] == '/') len--;
 out[len] = 0; if (!len) return -1;
 for (i = 0; i <= len; i++) {
  unsigned char c = (unsigned char)out[i];
  if (!c || c == '/') {
   size_t part = i - start;
   if (!part || (part == 1 && out[start] == '.') ||
       (part == 2 && out[start] == '.' && out[start + 1] == '.') || ++depth > 16) return -1;
   start = i + 1;
  } else if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
      (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-')) return -1;
 }
 return 0;
}
static int find(const struct pl_plan *p, const char *path) {
 size_t i; for (i = 0; i < p->count; i++) if (!strcmp(p->nodes[i].path, path)) return (int)i;
 return -1;
}
static int add(struct pl_plan *p, const char *path, int dir, int explicit_member,
 size_t offset, size_t size) {
 int i = find(p, path); struct pl_node *n;
 if (i >= 0) {
  n = &p->nodes[i];
  if (!dir || !n->directory || (explicit_member && n->explicit_member)) return -1;
  if (explicit_member) n->explicit_member = 1;
  return 0;
 }
 if (p->count == PL_NODES) return -1;
 n = &p->nodes[p->count++]; strcpy(n->path, path); n->directory = dir;
 n->explicit_member = explicit_member; n->offset = offset; n->size = size; return 0;
}
static int compare(const void *a, const void *b) {
 return strcmp(((const struct pl_node *)a)->path, ((const struct pl_node *)b)->path);
}
int pl_plan(const unsigned char *b, size_t length, struct pl_plan *p) {
 size_t at = 0, trailer = 0, i; uint64_t mode, uid, gid, size, stamp, check, dev;
 char path[256], prefix[256];
 memset(p, 0, sizeof(*p));
 if (!b || !length || length > 1048576 || length % 512) return -1;
 while (at < length) {
  const unsigned char *h = b + at; unsigned long sum = 0; int dir;
  if (zeros(h, 512)) { trailer++; at += 512; continue; }
  if (trailer || ++p->members > 128) return -1;
  for (i = 0; i < 512; i++) sum += i >= 148 && i < 156 ? 32 : h[i];
  if (memcmp(h + 257, "ustar\0", 6) || memcmp(h + 263, "00", 2) ||
      number(h + 148, 8, &check) || check != sum || !zeros(h + 500, 12) ||
      (h[156] && h[156] != '0' && h[156] != '5')) return -1;
  dir = h[156] == '5';
  if (number(h + 100, 8, &mode) || number(h + 108, 8, &uid) ||
      number(h + 116, 8, &gid) || number(h + 124, 12, &size) ||
      number(h + 136, 12, &stamp) || uid || gid || mode != (dir ? 0755U : 0644U) ||
      (dir && size) || size > 262144 || size > 524288 - p->bytes ||
      !zeros(h + 157, 100) || !zeros(h + 265, 64)) return -1;
  for (i = 329; i <= 337; i += 8)
   if (!zeros(h + i, 8) && (number(h + i, 8, &dev) || dev)) return -1;
  if (name(h, dir, path)) return -1;
  at += 512;
  { size_t padded = ((size_t)size + 511) / 512 * 512;
   if (padded > length - at || !zeros(b + at + (size_t)size, padded - (size_t)size)) return -1;
   strcpy(prefix, path);
   for (i = 0; prefix[i]; i++) if (prefix[i] == '/') {
    prefix[i] = 0; if (add(p, prefix, 1, 0, 0, 0)) return -1; prefix[i] = '/';
   }
   if (add(p, path, dir, 1, at, (size_t)size)) return -1;
   at += padded;
  }
  p->bytes += (size_t)size; if (!dir) p->files++;
 }
 if (trailer < 2 || !p->files) return -1;
 qsort(p->nodes, p->count, sizeof(p->nodes[0]), compare);
 for (i = 0; i < p->count; i++) {
  char *slash; strcpy(prefix, p->nodes[i].path); slash = strrchr(prefix, '/');
  p->nodes[i].parent = -1;
  if (slash) {
   *slash = 0; p->nodes[i].parent = find(p, prefix);
   if (p->nodes[i].parent < 0 || (size_t)p->nodes[i].parent >= i ||
       !p->nodes[p->nodes[i].parent].directory) return -1;
  }
 }
 return 0;
}
