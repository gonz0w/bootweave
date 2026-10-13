/* SPDX-License-Identifier: BSD-3-Clause */
/* Original bounded native research. No payload file, extraction or shell. */
#include "auth_native.h"
#include <stdio.h>
#ifndef __OpenBSD__
struct auth_result auth_native(const struct auth_context *ctx,
 const unsigned char *sig, size_t length) {
 struct auth_result r = {0};
 (void)ctx; (void)sig; (void)length; r.failure = AUTH_CONTEXT; return r;
}
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sha2.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define HOME "/root/payload-auth/"
#ifndef AUTH_INSPECTOR
#define AUTH_INSPECTOR HOME "inspect"
#endif
static volatile sig_atomic_t stopped;
static void stop_signal(int s) { (void)s; stopped = 1; }
#ifdef AUTH_TEST
/* These fixed test hooks exist only in the explicitly compiled test build. */
extern void auth_test_hook(int, const char *, const unsigned char *, size_t);
extern int auth_test_child(int);
#define HOOK(n,p,b,l) auth_test_hook(n,p,b,l)
#else
#define HOOK(n,p,b,l) ((void)0)
#endif
long long auth_now(void) {
 struct timespec t;
 if (clock_gettime(CLOCK_MONOTONIC, &t)) return -1;
 return (long long)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
static int alive(long long deadline) {
 long long t = auth_now();
 return !stopped && t >= 0 && t < deadline;
}
static int flags(int fd) {
 int f = fcntl(fd, F_GETFL);
 return f < 0 || fcntl(fd, F_SETFL, f | O_NONBLOCK) ? -1 : f;
}
static int hash_fd(int fd, unsigned char digest[32], size_t ceiling) {
 unsigned char b[4096]; size_t total = 0; ssize_t n; SHA2_CTX ctx;
 SHA256Init(&ctx);
 while ((n = read(fd, b, sizeof(b))) != 0) {
  if (n < 0) { if (errno == EINTR) continue; return -1; }
  if ((size_t)n > ceiling - total) return -1;
  total += (size_t)n; SHA256Update(&ctx, b, (size_t)n);
 }
 SHA256Final(digest, &ctx); return 0;
}
int auth_hash_file(const char *path, unsigned char digest[32]) {
 int fd = open(path, O_RDONLY | O_NOFOLLOW | O_NONBLOCK), result;
 struct stat st;
 if (fd < 0) return -1;
 result = fstat(fd, &st) || !S_ISREG(st.st_mode) ? -1 : hash_fd(fd, digest, 1048576);
 if (close(fd)) result = -1;
 return result;
}
static int pinned(const char *path, const unsigned char expected[32]) {
 unsigned char digest[32]; struct stat st;
 int fd = open(path, O_RDONLY | O_NOFOLLOW | O_NONBLOCK), result = -1;
 if (fd < 0) return -1;
 if (!fstat(fd, &st) && S_ISREG(st.st_mode) && st.st_uid == getuid() &&
     (st.st_mode & 0111) && !(st.st_mode & 0022) && st.st_size > 0 &&
     st.st_size <= 1048576 && !hash_fd(fd, digest, 1048576) &&
     !memcmp(digest, expected, 32)) result = 0;
 if (close(fd)) result = -1;
 return result;
}
static int value(unsigned char c) {
 if (c >= 'A' && c <= 'Z') return c - 'A';
 if (c >= 'a' && c <= 'z') return c - 'a' + 26;
 if (c >= '0' && c <= '9') return c - '0' + 52;
 return c == '+' ? 62 : c == '/' ? 63 : -1;
}
static int record(const unsigned char *b, size_t n, size_t decoded) {
 static const char prefix[] = "untrusted comment: ";
 size_t i, line = 0, encoded = ((decoded + 2) / 3) * 4, pad = (3 - decoded % 3) % 3;
 if (!b || n > AUTH_RECORD || n <= sizeof(prefix) ||
     memcmp(b, prefix, sizeof(prefix) - 1) || memchr(b, 0, n)) return -1;
 while (line < n && b[line] != '\n') line++;
 if (line <= sizeof(prefix) - 1 || line - (sizeof(prefix) - 1) >= 1024 ||
     n != line + 1 + encoded + 1 || b[n - 1] != '\n') return -1;
 b += line + 1;
 for (i = 0; i < encoded - pad; i++) if (value(b[i]) < 0) return -1;
 for (; i < encoded; i++) if (b[i] != '=') return -1;
 if (pad && (value(b[encoded - pad - 1]) & (pad == 1 ? 3 : 15))) return -1;
 /* Only the algorithm marker is decoded here; signify owns cryptography. */
 if (((value(b[0]) << 2) | (value(b[1]) >> 4)) != 'E' ||
     (((value(b[1]) & 15) << 4) | (value(b[2]) >> 2)) != 'd') return -1;
 return 0;
}
static int bound(int resource, rlim_t n) {
 struct rlimit r = {n, n}; return setrlimit(resource, &r);
}
static void child_limits(void) {
 if (bound(RLIMIT_CORE, 0) || bound(RLIMIT_CPU, 15) ||
     bound(RLIMIT_FSIZE, AUTH_INPUT) || bound(RLIMIT_DATA, 32 * 1048576) ||
     bound(RLIMIT_RSS, 32 * 1048576)) _exit(125);
 umask(077);
}
static int drain(int *fd, unsigned char *b, size_t *used, size_t *total) {
 unsigned char chunk[512]; ssize_t n;
 while (*fd >= 0) {
  n = read(*fd, chunk, sizeof(chunk));
  if (n > 0) {
   if ((size_t)n > 4096 - *total) return -2;
   memcpy(b + *used, chunk, (size_t)n); *used += (size_t)n; *total += (size_t)n;
  } else if (!n) { close(*fd); *fd = -1; }
  else if (errno == EINTR) continue;
  else return errno == EAGAIN ? 0 : -1;
 }
 return 0;
}
int auth_run(char *const argv[], const unsigned char *input, size_t length,
 struct auth_child *r, long long deadline, int stage, struct auth_result *metrics) {
 int in[2] = {-1,-1}, out[2] = {-1,-1}, err[2] = {-1,-1};
 int failure = AUTH_IO, status = 0, done = 0, read_out = -1, read_err = -1;
 size_t sent = 0, total = 0; pid_t pid = -1, w; struct rusage usage;
 long long start = auth_now(), child_deadline;
 memset(r, 0, sizeof(*r));
 if (start < 0 || !alive(deadline)) return stopped ? AUTH_INTERRUPTED : AUTH_DEADLINE;
 child_deadline = start + 15000; if (child_deadline > deadline) child_deadline = deadline;
 if (pipe(in) || pipe(out) || pipe(err)) goto cleanup;
 HOOK(10 + stage, NULL, input, length);
 pid = fork(); if (pid < 0) goto cleanup;
 if (!pid) {
  char *const env[] = {"LC_ALL=C", "LANG=C", "PATH=", "TZ=UTC", NULL};
  struct sigaction sa;
  if (setsid() < 0 || dup2(in[0], 0) < 0 || dup2(out[1], 1) < 0 || dup2(err[1], 2) < 0) _exit(125);
  closefrom(3); child_limits();
  memset(&sa, 0, sizeof(sa)); sa.sa_handler = SIG_DFL; sigemptyset(&sa.sa_mask);
  if (sigaction(SIGINT, &sa, NULL) || sigaction(SIGTERM, &sa, NULL) ||
      sigaction(SIGHUP, &sa, NULL) || sigaction(SIGPIPE, &sa, NULL)) _exit(125);
#ifdef AUTH_TEST
  { int result = auth_test_child(stage); if (result >= 0) _exit(result); }
#else
  (void)stage;
#endif
  execve(argv[0], argv, env); _exit(125);
 }
 if (stage == 0) metrics->verifier_launches++;
 if (stage == 1) metrics->inspector_launches++;
 HOOK(20 + stage, NULL, (const unsigned char *)&pid, sizeof(pid));
 close(in[0]); in[0] = -1; close(out[1]); out[1] = -1; close(err[1]); err[1] = -1;
 read_out = out[0]; out[0] = -1; read_err = err[0]; err[0] = -1;
 if (flags(in[1]) < 0 || flags(read_out) < 0 || flags(read_err) < 0) goto cleanup;
 for (;;) {
  int d; struct pollfd f[3]; ssize_t n;
  if (!alive(child_deadline)) { failure = stopped ? AUTH_INTERRUPTED : AUTH_DEADLINE; goto cleanup; }
  if (in[1] >= 0) {
   if (sent == length) { close(in[1]); in[1] = -1; }
   else {
    size_t amount = length - sent; if (amount > 4096) amount = 4096;
    n = write(in[1], input + sent, amount);
    if (n > 0) sent += (size_t)n;
    else if (n < 0 && errno != EINTR && errno != EAGAIN) goto cleanup;
   }
  }
  d = drain(&read_out, r->out, &r->out_length, &total);
  if (!d) d = drain(&read_err, r->err, &r->err_length, &total);
  if (d) { failure = d == -2 ? AUTH_OUTPUT : AUTH_IO; goto cleanup; }
  if (!done) {
   w = wait4(pid, &status, WNOHANG, &usage);
   if (w == pid) { done = 1; metrics->reaped++; }
   else if (w < 0 && errno != EINTR) goto cleanup;
  }
  if (done && read_out < 0 && read_err < 0) break;
  f[0].fd = in[1]; f[0].events = POLLOUT; f[0].revents = 0;
  f[1].fd = read_out; f[1].events = POLLIN; f[1].revents = 0;
  f[2].fd = read_err; f[2].events = POLLIN; f[2].revents = 0;
  if (poll(f, 3, 10) < 0 && errno != EINTR) goto cleanup;
 }
 if (sent != length) goto cleanup;
 r->status = status; r->peak_kib = usage.ru_maxrss;
 if (r->peak_kib < 0 || r->peak_kib > 32768) { failure = AUTH_RESOURCE; goto cleanup; }
 if (r->peak_kib > metrics->child_peak_kib) metrics->child_peak_kib = r->peak_kib;
 failure = AUTH_OK;
cleanup:
 if (pid > 0 && (!done || failure)) {
  (void)kill(-pid, SIGKILL); (void)kill(pid, SIGKILL);
  if (!done) {
   do { w = wait4(pid, &status, 0, &usage); } while (w < 0 && errno == EINTR);
   if (w == pid) metrics->reaped++; else failure = AUTH_CLEANUP;
  }
 }
 if (read_out >= 0) close(read_out);
 if (read_err >= 0) close(read_err);
 { int i; for (i = 0; i < 2; i++) {
   if (in[i] >= 0) close(in[i]); if (out[i] >= 0) close(out[i]); if (err[i] >= 0) close(err[i]);
 } }
 return failure;
}
static int file(const char *path, const unsigned char *bytes, size_t length, struct stat *identity) {
 int fd = open(path, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600), failed = 0;
 size_t at = 0; ssize_t n;
 if (fd < 0) return -1;
 while (at < length) {
  n = write(fd, bytes + at, length - at);
  if (n > 0) at += (size_t)n;
  else if (n < 0 && errno == EINTR) continue;
  else { failed = 1; break; }
 }
 if (fstat(fd, identity)) failed = 1;
 if (close(fd)) failed = 1;
 return failed ? -1 : 0;
}
static int same(const struct stat *a, const struct stat *b, mode_t mode) {
 return S_ISDIR(a->st_mode) == S_ISDIR(b->st_mode) &&
     a->st_dev == b->st_dev && a->st_ino == b->st_ino &&
     a->st_uid == getuid() && b->st_uid == getuid() &&
     (a->st_mode & 07777) == mode && (b->st_mode & 07777) == mode;
}
static int checkfile(const char *path, const struct stat *identity,
 const unsigned char *data, size_t length) {
 unsigned char b[AUTH_RECORD + 1]; struct stat st;
 int fd = open(path, O_RDONLY | O_NOFOLLOW | O_NONBLOCK), failed = 0; ssize_t n;
 if (fd < 0) return -1;
 if (fstat(fd, &st) || !S_ISREG(st.st_mode) || !same(identity, &st, 0600) ||
     st.st_size != (off_t)length) failed = 1;
 n = read(fd, b, sizeof(b));
 if (n < 0 || (size_t)n != length || memcmp(b, data, length)) failed = 1;
 if (close(fd)) failed = 1;
 return failed ? -1 : 0;
}
static int post(const char *path, const struct stat *identity, const char *key,
 const struct stat *ki, const unsigned char *kb, size_t kl, const char *sig,
 const struct stat *si, const unsigned char *sb, size_t sl) {
 struct stat st; DIR *dir; struct dirent *e; int count = 0, failed = 0;
 if (lstat(path, &st) || !S_ISDIR(st.st_mode) || !same(identity, &st, 0700)) return -1;
 if (checkfile(key, ki, kb, kl) || checkfile(sig, si, sb, sl)) return -1;
 dir = opendir(path); if (!dir) return -1;
 while ((e = readdir(dir))) {
  if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
  if (strcmp(e->d_name, "selected.pub") && strcmp(e->d_name, "supplied.sig")) failed = 1;
  count++;
 }
 if (closedir(dir)) failed = 1;
 return failed || count != 2 ? -1 : 0;
}
static int capture(unsigned char *b, size_t *length, long long deadline) {
 int saved = flags(0), failure = AUTH_INPUT_BAD; size_t at = 0; ssize_t n;
 if (saved < 0) return AUTH_IO;
 while (alive(deadline)) {
  struct pollfd f = {0, POLLIN, 0};
  n = read(0, b + at, AUTH_INPUT + 1 - at);
  if (n > 0) { at += (size_t)n; if (at > AUTH_INPUT) break; }
  else if (!n) { *length = at; failure = at ? AUTH_OK : AUTH_INPUT_BAD; break; }
  else if (errno != EINTR && errno != EAGAIN) { failure = AUTH_IO; break; }
  if (poll(&f, 1, 10) < 0 && errno != EINTR) { failure = AUTH_IO; break; }
 }
 if (!alive(deadline)) failure = stopped ? AUTH_INTERRUPTED : AUTH_DEADLINE;
 if (fcntl(0, F_SETFL, saved)) failure = AUTH_CLEANUP;
 return failure;
}
static int summary(const unsigned char *b, size_t n, char out[128]) {
 unsigned long members, files, content; int used = 0, length;
 char text[128], canonical[128];
 if (n >= sizeof(text) || memchr(b, 0, n)) return -1;
 memcpy(text, b, n); text[n] = 0;
 if (sscanf(text, "Inspection passed: %lu members, %lu files, %lu content bytes.\n%n",
     &members, &files, &content, &used) != 3 || used != (int)n ||
     !members || members > 128 || !files || files > members || content > 524288) return -1;
 length = snprintf(canonical, sizeof(canonical), "Inspection passed: %lu members, %lu files, %lu content bytes.\n", members, files, content);
 if (length < 0 || (size_t)length != n || memcmp(canonical, b, n)) return -1;
 memcpy(out, canonical, n + 1); return 0;
}
struct auth_result auth_native(const struct auth_context *ctx,
 const unsigned char *sig, size_t sig_length) {
 struct auth_result r = {0}; struct auth_child child;
 unsigned char *snapshot = NULL, digest[32]; size_t length = 0;
 char dir[] = HOME "private-XXXXXXXX", key[128] = "", signature[128] = "";
 struct stat di, ki, si; struct sigaction sa, old[4]; struct rusage usage;
 const int signals[] = {SIGINT, SIGTERM, SIGHUP, SIGPIPE};
 int i, handlers = 0, have_dir = 0, have_key = 0, have_sig = 0;
 long long start = auth_now(), deadline = start + 45000;
 mode_t oldmask = umask(077);
 r.failure = AUTH_CONTEXT;
 if (start < 0 || !ctx || !ctx->length || ctx->length > AUTH_INPUT ||
     record(ctx->key, ctx->key_length, 42) || record(sig, sig_length, 74)) goto cleanup;
 stopped = 0; memset(&sa, 0, sizeof(sa)); sigemptyset(&sa.sa_mask);
 for (i = 0; i < 4; i++) {
  sa.sa_handler = signals[i] == SIGPIPE ? SIG_IGN : stop_signal;
  if (sigaction(signals[i], &sa, &old[i])) { r.failure = AUTH_IO; goto cleanup; }
  handlers++;
 }
 r.failure = AUTH_TOOLS;
 if (pinned("/usr/bin/signify", ctx->verifier_digest) || pinned(AUTH_INSPECTOR, ctx->inspector_digest)) goto cleanup;
 snapshot = malloc(AUTH_INPUT + 1); if (!snapshot) { r.failure = AUTH_RESOURCE; goto cleanup; }
 r.failure = capture(snapshot, &length, start + 10000); if (r.failure) goto cleanup;
 { SHA2_CTX hash; SHA256Init(&hash); SHA256Update(&hash, snapshot, length); SHA256Final(digest, &hash); }
 r.failure = AUTH_IDENTITY;
 if (length != ctx->length || memcmp(digest, ctx->digest, 32)) goto cleanup;
 HOOK(0, NULL, snapshot, length);
 r.failure = AUTH_IO;
 if (!mkdtemp(dir)) goto cleanup;
 have_dir = 1;
 if (lstat(dir, &di) || !S_ISDIR(di.st_mode) || di.st_uid != getuid() || (di.st_mode & 07777) != 0700) goto cleanup;
 if (snprintf(key, sizeof(key), "%s/selected.pub", dir) >= (int)sizeof(key) ||
     snprintf(signature, sizeof(signature), "%s/supplied.sig", dir) >= (int)sizeof(signature)) goto cleanup;
 have_key = 1; if (file(key, ctx->key, ctx->key_length, &ki)) goto cleanup;
 have_sig = 1; if (file(signature, sig, sig_length, &si)) goto cleanup;
 HOOK(1, dir, snapshot, length);
 if (post(dir, &di, key, &ki, ctx->key, ctx->key_length, signature, &si, sig, sig_length)) { r.failure = AUTH_CLEANUP; goto cleanup; }
 {
  char *const args[] = {"/usr/bin/signify", "-V", "-q", "-p", key, "-x", signature, "-m", "-", NULL};
  r.failure = auth_run(args, snapshot, length, &child, deadline, 0, &r);
 }
 if (r.failure) goto cleanup;
 if (!WIFEXITED(child.status) || WEXITSTATUS(child.status) || child.out_length || child.err_length) { r.failure = AUTH_SIGNATURE; goto cleanup; }
 HOOK(2, dir, snapshot, length);
 if (post(dir, &di, key, &ki, ctx->key, ctx->key_length, signature, &si, sig, sig_length)) { r.failure = AUTH_CLEANUP; goto cleanup; }
 r.verified = 1;
 {
  char *const args[] = {AUTH_INSPECTOR, NULL};
  r.failure = auth_run(args, snapshot, length, &child, deadline, 1, &r);
 }
 if (r.failure) goto cleanup;
 if (!WIFEXITED(child.status) || WEXITSTATUS(child.status) || child.err_length ||
     summary(child.out, child.out_length, r.summary)) { r.failure = AUTH_INSPECTION; goto cleanup; }
 if (post(dir, &di, key, &ki, ctx->key, ctx->key_length, signature, &si, sig, sig_length)) r.failure = AUTH_CLEANUP;
cleanup:
 if (have_sig && unlink(signature) && errno != ENOENT) r.failure = AUTH_CLEANUP;
 if (have_key && unlink(key) && errno != ENOENT) r.failure = AUTH_CLEANUP;
 if (have_dir && rmdir(dir)) r.failure = AUTH_CLEANUP;
 free(snapshot);
 if (!r.failure && (!alive(deadline))) r.failure = stopped ? AUTH_INTERRUPTED : AUTH_DEADLINE;
 if (!r.failure) {
  if (getrusage(RUSAGE_SELF, &usage) || usage.ru_maxrss < 0 || usage.ru_maxrss > 65536) r.failure = AUTH_RESOURCE;
  else r.controller_peak_kib = usage.ru_maxrss;
 }
 for (i = handlers - 1; i >= 0; i--) if (sigaction(signals[i], &old[i], NULL)) r.failure = AUTH_CLEANUP;
 umask(oldmask);
 if (!r.failure) r.completed = 1; else memset(r.summary, 0, sizeof(r.summary));
 return r;
}
#endif
#ifdef AUTH_STANDALONE
int main(int argc, char **argv) {
 (void)argv;
 fputs("Native authentication requires trusted research context.\n", stderr);
 return argc == 1 ? 77 : 2;
}
#endif
