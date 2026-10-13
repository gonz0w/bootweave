/* SPDX-License-Identifier: BSD-3-Clause */
/* Original fixed native supervisor/test doubles. Never extracts or invokes a shell. */
#include "auth_native.h"
#include <stdio.h>
#ifndef __OpenBSD__
int main(void) { fputs("Native authentication checks require OpenBSD.\n", stderr); return 77; }
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
#include <termios.h>
#include <time.h>
#include <unistd.h>
#define ROOT "/root/payload-auth/"
#ifndef AUTH_INSPECTOR
#define AUTH_INSPECTOR ROOT "inspect"
#endif
enum test_mode { REAL, MUTATE, REPLACE, BAD_DIRECTORY, BAD_KEY, BAD_INVENTORY,
 VERIFY_FAIL, VERIFY_DIAGNOSTIC, VERIFY_OUTPUT, VERIFY_COMBINED, VERIFY_SIGNAL,
 VERIFY_STALL, VERIFY_CLOSED, INSPECT_FAIL, INSPECT_BAD, INSPECT_DIAGNOSTIC };
struct observations { unsigned int phases; int bad; pid_t children[2]; };
struct packet { struct auth_result result; struct observations observations; };
struct event { int stage; pid_t pid; };
static enum test_mode mode;
static int event_fd = -1, tests;
static struct observations observed;
static long peak_controller, peak_child;
static pid_t active_worker;
static struct auth_result last_result;
static const char good_summary[] = "Inspection passed: 2 members, 1 files, 5 content bytes.\n";
static unsigned char *payloads[3], keys[2][AUTH_RECORD], signatures[3][AUTH_RECORD];
static size_t lengths[3], key_lengths[2], sig_lengths[3];
static struct auth_context contexts[3];
static void fail(const char *message) {
 if (active_worker > 0) { int status; (void)kill(-active_worker, SIGKILL); (void)kill(active_worker, SIGKILL); while (waitpid(active_worker, &status, 0) < 0 && errno == EINTR) {} }
 fprintf(stderr, "Native authentication check failed: %s; check %d mode %d; failure %d, reaped %d, child peak %ld KiB.\n",
     message, tests, (int)mode, (int)last_result.failure, last_result.reaped, last_result.child_peak_kib); exit(1);
}
static void require(int condition, const char *message) { if (!condition) fail(message); }
static void nonblock(int fd) { int f = fcntl(fd, F_GETFL); require(f >= 0 && !fcntl(fd, F_SETFL, f | O_NONBLOCK), "pipe flags"); }
static void write_all(int fd, const void *data, size_t length) {
 const unsigned char *b = data; ssize_t n;
 while (length) { n = write(fd, b, length); if (n > 0) { b += n; length -= (size_t)n; } else if (n < 0 && errno == EINTR) continue; else fail("owned write"); }
}
static size_t read_file(const char *path, unsigned char *b, size_t cap) {
 int fd = open(path, O_RDONLY | O_NOFOLLOW | O_NONBLOCK); struct stat st; size_t at = 0; ssize_t n;
 require(fd >= 0 && !fstat(fd, &st) && S_ISREG(st.st_mode) && st.st_size >= 0 && (size_t)st.st_size <= cap, "fixture open/bound");
 while (at < cap && (n = read(fd, b + at, cap - at)) != 0) { if (n > 0) at += (size_t)n; else if (errno != EINTR) fail("fixture read"); }
 require(at == (size_t)st.st_size && !close(fd), "fixture exact read"); return at;
}
static void hash(const unsigned char *b, size_t n, unsigned char digest[32]) { SHA2_CTX c; SHA256Init(&c); SHA256Update(&c, b, n); SHA256Final(digest, &c); }
static void owned_write(const char *path, const void *data, size_t length) {
 int fd = open(path, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600); require(fd >= 0, "exclusive fixture"); write_all(fd, data, length); require(!close(fd), "fixture close");
}
static int fixed_inventory(const char *path, const char *const allowed[], size_t n) {
 DIR *d = opendir(path); struct dirent *e; size_t count = 0, i; int ok = d != NULL;
 if (!d) return 0;
 while ((e = readdir(d))) {
  if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
  for (i = 0; i < n; i++) if (!strcmp(e->d_name, allowed[i])) break;
  if (i == n) ok = 0; count++;
 }
 if (closedir(d)) ok = 0;
 return ok && count == n;
}
static void canary(void) {
 unsigned char b[32]; static const char *const allowed[] = {"canary"};
 require(read_file(ROOT "work/canary", b, sizeof(b)) == 14 && !memcmp(b, "owned sentinel", 14), "canary unchanged");
 require(fixed_inventory(ROOT "work", allowed, 1), "no member effects");
}
void auth_test_hook(int phase, const char *directory, const unsigned char *b, size_t n) {
 struct stat st; char path[128];
 (void)b; (void)n;
 if (phase < 16) observed.phases |= 1U << phase;
 if (phase == 11 && !(observed.phases & (1U << 2))) observed.bad = 1;
 if (phase == 20 || phase == 21) {
  struct event event; event.stage = phase - 20;
  if (n != sizeof(pid_t)) { observed.bad = 1; return; }
  memcpy(&event.pid, b, sizeof(event.pid)); observed.children[event.stage] = event.pid;
  if (event_fd >= 0) write_all(event_fd, &event, sizeof(event));
 }
 if (phase == 1) {
  if (lstat(directory, &st) || !S_ISDIR(st.st_mode) || (st.st_mode & 07777) != 0700 || st.st_uid != getuid()) observed.bad = 1;
  snprintf(path, sizeof(path), "%s/selected.pub", directory);
  if (lstat(path, &st) || !S_ISREG(st.st_mode) || (st.st_mode & 07777) != 0600 || st.st_uid != getuid()) observed.bad = 1;
  snprintf(path, sizeof(path), "%s/supplied.sig", directory);
  if (lstat(path, &st) || !S_ISREG(st.st_mode) || (st.st_mode & 07777) != 0600 || st.st_uid != getuid()) observed.bad = 1;
 }
 if (phase == 2 && (mode == MUTATE || mode == REPLACE)) {
  if (mode == MUTATE) {
   int fd = open(ROOT "work/source", O_WRONLY | O_TRUNC | O_NOFOLLOW);
   if (fd < 0) { observed.bad = 1; return; }
   write_all(fd, "changed", 7); if (close(fd)) observed.bad = 1;
  } else {
   owned_write(ROOT "work/replacement", "replaced", 8);
   if (rename(ROOT "work/replacement", ROOT "work/source")) observed.bad = 1;
  }
 }
 if (phase == 2 && mode == BAD_DIRECTORY && chmod(directory, 0755)) observed.bad = 1;
 if (phase == 2 && mode == BAD_KEY) {
  snprintf(path, sizeof(path), "%s/selected.pub", directory); if (chmod(path, 0644)) observed.bad = 1;
 }
 if (phase == 2 && mode == BAD_INVENTORY) {
  snprintf(path, sizeof(path), "%s/unexpected", directory); owned_write(path, "owned test", 10);
 }
}
int auth_test_child(int stage) {
 unsigned char b[4096]; ssize_t n;
 if (stage == 0) {
  if (mode == VERIFY_FAIL) return 1;
  if (mode == VERIFY_DIAGNOSTIC) {
   size_t length = read_file(ROOT "keys/selected.sec", b, sizeof(b)); write_all(2, b, length); return 1;
  }
  if (mode == VERIFY_OUTPUT) { memset(b, 'X', sizeof(b)); write_all(1, b, sizeof(b)); write_all(1, b, 1); return 0; }
  if (mode == VERIFY_COMBINED) { memset(b, 'X', sizeof(b)); write_all(1, b, 2048); write_all(2, b, 2049); return 0; }
  if (mode == VERIFY_SIGNAL) { (void)raise(SIGTERM); return 1; }
  if (mode == VERIFY_CLOSED) { close(0); return 0; }
  if (mode == VERIFY_STALL) { for (;;) pause(); }
 }
 if (stage == 1) {
  if (mode == INSPECT_FAIL) return 1;
  if (mode == INSPECT_BAD) { while ((n = read(0, b, sizeof(b))) > 0) {} write_all(1, "Inspection passed: 129 members, 1 files, 0 content bytes.\n", 56); return 0; }
  if (mode == INSPECT_DIAGNOSTIC) { while ((n = read(0, b, sizeof(b))) > 0) {} write_all(2, "\033[2Juntrusted\n", 14); return 0; }
  if (mode == MUTATE || mode == REPLACE) {
   unsigned char digest[32]; SHA2_CTX c; size_t total = 0;
   SHA256Init(&c);
   while ((n = read(0, b, sizeof(b))) != 0) {
    if (n < 0 || (size_t)n > AUTH_INPUT - total) return 1;
    total += (size_t)n; SHA256Update(&c, b, (size_t)n);
   }
   SHA256Final(digest, &c); owned_write(ROOT "work/received.sha", digest, sizeof(digest));
   write_all(1, good_summary, sizeof(good_summary) - 1); return 0;
  }
 }
 return -1;
}
static void tool(char *const args[], const unsigned char *input, size_t n, int good) {
 struct auth_child child; struct auth_result metrics = {0};
 require(!auth_run(args, input, n, &child, auth_now() + 45000, 2, &metrics), "bounded base tool");
 require(WIFEXITED(child.status) && (good ? !WEXITSTATUS(child.status) : WEXITSTATUS(child.status) != 0), "base tool status");
 require(!child.out_length && (good ? !child.err_length : child.err_length <= 4096), "private base tool diagnostics");
 require(metrics.reaped == 1, "base tool reaped");
 if (metrics.child_peak_kib > peak_child) peak_child = metrics.child_peak_kib;
}
static void sign_bytes(const unsigned char *b, size_t n, const char *signature, int other) {
 char *const args[] = {"/usr/bin/signify", "-S", "-s", other ? ROOT "keys/other.sec" : ROOT "keys/selected.sec", "-m", "-", "-x", (char *)signature, NULL};
 tool(args, b, n, 1);
}
static void setup(void) {
 int i, fresh; struct stat st; unsigned char lifetime[128], previous[128];
 static const char *const paths[] = {ROOT "cases/case-00.tar", ROOT "cases/case-06.tar", ROOT "cases/case-08.tar"};
 char *const first[] = {"/usr/bin/signify", "-G", "-n", "-p", ROOT "keys/selected.pub", "-s", ROOT "keys/selected.sec", NULL};
 char *const second[] = {"/usr/bin/signify", "-G", "-n", "-p", ROOT "keys/other.pub", "-s", ROOT "keys/other.sec", NULL};
 fresh = mkdir(ROOT "keys", 0700) == 0;
 require(fresh || errno == EEXIST, "owned key directory");
 if (fresh) { tool(first, NULL, 0, 1); tool(second, NULL, 0, 1); }
 key_lengths[0] = read_file(ROOT "keys/selected.pub", keys[0], AUTH_RECORD);
 key_lengths[1] = read_file(ROOT "keys/other.pub", keys[1], AUTH_RECORD);
 require(!lstat(ROOT "keys", &st) && (st.st_mode & 07777) == 0700 && st.st_uid == getuid(), "key directory mode");
 { const char *const files[] = {"selected.pub", "selected.sec", "other.pub", "other.sec"}; char path[80];
  require(fixed_inventory(ROOT "keys", files, 4), "exact two-pair inventory");
  for (i = 0; i < 4; i++) { snprintf(path, sizeof(path), ROOT "keys/%s", files[i]); require(!lstat(path, &st) && S_ISREG(st.st_mode) && st.st_uid == getuid() && (st.st_mode & 07777) == 0600 && st.st_size > 0 && st.st_size <= AUTH_RECORD, "private key record mode"); require(!auth_hash_file(path, lifetime + i * 32), "private key lifetime identity"); }
 }
 if (fresh) owned_write(ROOT "key-lifetime.private", lifetime, sizeof(lifetime));
 else {
  require(!lstat(ROOT "key-lifetime.private", &st) && S_ISREG(st.st_mode) && st.st_uid == getuid() && (st.st_mode & 07777) == 0600, "private lifetime receipt mode");
  require(read_file(ROOT "key-lifetime.private", previous, sizeof(previous)) == sizeof(previous) && !memcmp(previous, lifetime, sizeof(previous)), "same approved two pairs across builds/consoles");
 }
 for (i = 0; i < 3; i++) {
  char path[80]; payloads[i] = malloc(AUTH_INPUT + 1); require(payloads[i] != NULL, "fixture memory");
  lengths[i] = read_file(paths[i], payloads[i], AUTH_INPUT + 1); contexts[i].length = lengths[i];
  contexts[i].key_length = key_lengths[0]; memcpy(contexts[i].key, keys[0], key_lengths[0]);
  hash(payloads[i], lengths[i], contexts[i].digest);
  require(!auth_hash_file("/usr/bin/signify", contexts[i].verifier_digest) && !auth_hash_file(AUTH_INSPECTOR, contexts[i].inspector_digest), "trusted tool identities");
  snprintf(path, sizeof(path), ROOT "keys/sig-%d", i); sign_bytes(payloads[i], lengths[i], path, 0);
  sig_lengths[i] = read_file(path, signatures[i], AUTH_RECORD);
 }
 /* Prerequisite: real base stdin verification, not a test double. */
 { char *const args[] = {"/usr/bin/signify", "-V", "-q", "-p", ROOT "keys/selected.pub", "-x", ROOT "keys/sig-0", "-m", "-", NULL};
  tool(args, payloads[0], lengths[0], 1); payloads[0][1025] ^= 1; tool(args, payloads[0], lengths[0], 0); payloads[0][1025] ^= 1;
 }
}
static void private_cleanup(void) {
 DIR *d = opendir(ROOT); struct dirent *e; char path[160], file[192];
 require(d != NULL, "private inventory");
 while ((e = readdir(d))) {
  if (strncmp(e->d_name, "private-", 8)) continue;
  require(strlen(e->d_name) < 64, "owned private name");
  snprintf(path, sizeof(path), ROOT "%s", e->d_name);
  /* Only the exact injected extra file may remain after a cleanup rejection. */
  require(mode == BAD_INVENTORY, "controller left private directory");
  { const char *const extra[] = {"unexpected"}; require(fixed_inventory(path, extra, 1), "cleanup failure inventory"); }
  snprintf(file, sizeof(file), "%s/unexpected", path); require(!unlink(file) && !rmdir(path), "dispose injected inventory");
 }
 require(!closedir(d), "private inventory close");
}
static struct auth_result run(const struct auth_context *context, const unsigned char *data, size_t n,
 const unsigned char *sig, size_t sn, enum test_mode selected, int signal_number, int unfinished) {
 int input[2], result[2], events[2], input_closed = 0, done = 0, status = 0, flags_before;
 size_t sent = 0, used = 0; struct packet packet; struct event event; struct rusage usage;
 pid_t worker, w; long long started = auth_now(); unsigned int launches = 0; int signalled = 0;
 unsigned char returned[sizeof(packet)], event_bytes[sizeof(event)]; size_t event_used = 0;
 mode = selected; memset(&packet, 0, sizeof(packet));
 if (mode == MUTATE || mode == REPLACE) owned_write(ROOT "work/source", data, n);
 require(!pipe(input) && !pipe(result) && !pipe(events), "worker pipes");
 flags_before = fcntl(input[0], F_GETFL); require(flags_before >= 0, "worker saved flags");
 worker = fork(); require(worker >= 0, "worker fork");
 if (!worker) {
  struct rlimit limit = {64 * 1048576, 64 * 1048576};
  close(input[1]); close(result[0]); close(events[0]); event_fd = events[1]; memset(&observed, 0, sizeof(observed));
  if (setsid() < 0 || dup2(input[0], 0) < 0 || setrlimit(RLIMIT_DATA, &limit) || setrlimit(RLIMIT_RSS, &limit)) _exit(125);
  close(input[0]); packet.result = auth_native(context, sig, sn); packet.observations = observed;
  write_all(result[1], &packet, sizeof(packet)); close(result[1]); close(events[1]); _exit(0);
 }
 active_worker = worker; close(result[1]); close(events[1]); nonblock(input[1]); nonblock(result[0]); nonblock(events[0]);
 for (;;) {
  struct pollfd f[3]; ssize_t count;
  require(auth_now() - started < 48000, "external workflow deadline");
  if (!input_closed && !unfinished) {
   if (sent < n) {
    size_t amount = n - sent; if (amount > 4096) amount = 4096;
    count = write(input[1], data + sent, amount);
    if (count > 0) sent += (size_t)count;
    else require(count < 0 && (errno == EAGAIN || errno == EINTR || errno == EPIPE), "worker input write");
   } else { close(input[1]); input_closed = 1; }
  }
  count = read(result[0], returned + used, sizeof(packet) - used);
  if (count > 0) used += (size_t)count;
  else require(count == 0 || errno == EINTR || errno == EAGAIN, "worker receipt read");
  for (;;) {
   count = read(events[0], event_bytes + event_used, sizeof(event) - event_used);
   if (count > 0) event_used += (size_t)count;
   else { require(count == 0 || errno == EINTR || errno == EAGAIN, "worker event read"); break; }
   if (event_used == sizeof(event)) {
    memcpy(&event, event_bytes, sizeof(event)); event_used = 0;
    require(event.stage == 0 || event.stage == 1, "fixed child stage"); launches |= 1U << event.stage;
    if (signal_number && !signalled) { require(event.stage == 0 && event.pid > 0 && !kill(worker, signal_number), "signal after actual child launch"); signalled = 1; }
   }
  }
  if (!done) { w = wait4(worker, &status, WNOHANG, &usage); require(w >= 0 || errno == EINTR, "worker wait4"); if (w == worker) { done = 1; active_worker = 0; } }
  if (done && used == sizeof(packet)) break;
  if (done) require(WIFEXITED(status) && !WEXITSTATUS(status), "worker terminated before receipt");
  f[0].fd = !input_closed && !unfinished ? input[1] : -1; f[0].events = POLLOUT; f[0].revents = 0;
  f[1].fd = used < sizeof(packet) ? result[0] : -1; f[1].events = POLLIN; f[1].revents = 0;
  f[2].fd = events[0]; f[2].events = POLLIN; f[2].revents = 0;
  require(poll(f, 3, 10) >= 0 || errno == EINTR, "worker supervision");
 }
 memcpy(&packet, returned, sizeof(packet));
 require(WIFEXITED(status) && !WEXITSTATUS(status) && usage.ru_maxrss >= 0 && usage.ru_maxrss <= 65536, "controller exit/RSS");
 require(fcntl(input[0], F_GETFL) == flags_before, "exact input flags restored");
 require(!packet.observations.bad && packet.result.reaped == packet.result.verifier_launches + packet.result.inspector_launches, "ordering and every child reaped");
 require((!!(launches & 1)) == !!packet.result.verifier_launches && (!!(launches & 2)) == !!packet.result.inspector_launches, "independent launch observations");
 { int i; for (i = 0; i < 2; i++) if (packet.observations.children[i] > 0) { errno = 0; require(kill(packet.observations.children[i], 0) == -1 && errno == ESRCH, "child absent after reap"); } }
 if (usage.ru_maxrss > peak_controller) peak_controller = usage.ru_maxrss;
 if (packet.result.child_peak_kib > peak_child) peak_child = packet.result.child_peak_kib;
 close(input[0]); if (!input_closed) close(input[1]); close(result[0]); close(events[0]);
 if (mode == MUTATE || mode == REPLACE) {
  unsigned char digest[32], changed[16]; size_t expected = mode == MUTATE ? 7 : 8;
  require(read_file(ROOT "work/received.sha", digest, sizeof(digest)) == 32 && !memcmp(digest, context->digest, 32), "actual retained bytes received through child pipe");
  require(read_file(ROOT "work/source", changed, sizeof(changed)) == expected && !memcmp(changed, mode == MUTATE ? "changed" : "replaced", expected), "original actually changed");
  require(!unlink(ROOT "work/received.sha") && !unlink(ROOT "work/source"), "mutation fixture disposal");
 }
 private_cleanup(); canary(); tests++; last_result = packet.result; return packet.result;
}
static void accept(struct auth_result r, const char *expected) { require(r.completed && r.verified && !r.failure && r.verifier_launches == 1 && r.inspector_launches == 1 && !strcmp(r.summary, expected), "authenticated acceptance"); }
static void reject(struct auth_result r, int verified, int inspected) { require(!r.completed && r.failure && !r.summary[0] && r.verified == verified && r.inspector_launches == inspected, "bounded rejection/no completion"); }
static void checks(void) {
 struct auth_context c; struct auth_result r; unsigned char sig[AUTH_RECORD + 1], *changed; size_t sn; int i;
 changed = malloc(AUTH_INPUT + 1); require(changed != NULL, "changed fixture");
 accept(run(&contexts[0], payloads[0], lengths[0], signatures[0], sig_lengths[0], REAL, 0, 0), good_summary);
 accept(run(&contexts[1], payloads[1], lengths[1], signatures[1], sig_lengths[1], REAL, 0, 0), "Inspection passed: 1 members, 1 files, 0 content bytes.\n");
 memcpy(changed, payloads[0], lengths[0]); changed[1025] ^= 1;
 reject(run(&contexts[0], changed, lengths[0], signatures[0], sig_lengths[0], REAL, 0, 0), 0, 0);
 c = contexts[0]; hash(changed, lengths[0], c.digest);
 r = run(&c, changed, lengths[0], signatures[0], sig_lengths[0], REAL, 0, 0); reject(r, 0, 0); require(r.failure == AUTH_SIGNATURE && r.verifier_launches == 1, "matching identity cannot bypass old signature");
 c = contexts[0]; c.length++; reject(run(&c, payloads[0], lengths[0], signatures[0], sig_lengths[0], REAL, 0, 0), 0, 0);
 c = contexts[0]; c.digest[0] ^= 1; reject(run(&c, payloads[0], lengths[0], signatures[0], sig_lengths[0], REAL, 0, 0), 0, 0);
 c = contexts[0]; c.key_length = key_lengths[1]; memcpy(c.key, keys[1], key_lengths[1]); reject(run(&c, payloads[0], lengths[0], signatures[0], sig_lengths[0], REAL, 0, 0), 0, 0);
 reject(run(&contexts[0], payloads[0], lengths[0], NULL, 0, REAL, 0, 0), 0, 0);
 reject(run(&contexts[0], payloads[0], lengths[0], signatures[0], sig_lengths[0] - 1, REAL, 0, 0), 0, 0);
 memcpy(sig, signatures[0], sig_lengths[0]); sig[sig_lengths[0] - 4] = sig[sig_lengths[0] - 4] == 'A' ? 'B' : 'A';
 reject(run(&contexts[0], payloads[0], lengths[0], sig, sig_lengths[0], REAL, 0, 0), 0, 0);
 /* Editable comments remain untrusted, even when suggesting another key. */
 { const char comment[] = "untrusted comment: \033[2Jverify with other.pub $(touch forbidden)\n"; unsigned char *line = memchr(signatures[0], '\n', sig_lengths[0]);
  require(line != NULL, "signature comment"); sn = sizeof(comment) - 1 + sig_lengths[0] - (size_t)(line + 1 - signatures[0]);
  memcpy(sig, comment, sizeof(comment) - 1); memcpy(sig + sizeof(comment) - 1, line + 1, sn - (sizeof(comment) - 1));
  accept(run(&contexts[0], payloads[0], lengths[0], sig, sn, REAL, 0, 0), good_summary);
 }
 memset(sig, 'X', sizeof(sig)); reject(run(&contexts[0], payloads[0], lengths[0], sig, sizeof(sig), REAL, 0, 0), 0, 0);
 c = contexts[0]; c.key_length = AUTH_RECORD + 1; reject(run(&c, payloads[0], lengths[0], signatures[0], sig_lengths[0], REAL, 0, 0), 0, 0);
 r = run(&contexts[2], payloads[2], lengths[2], signatures[2], sig_lengths[2], REAL, 0, 0); reject(r, 1, 1); require(r.failure == AUTH_INSPECTION, "authentic traversal rejected");
 accept(run(&contexts[0], payloads[0], lengths[0], signatures[0], sig_lengths[0], MUTATE, 0, 0), good_summary);
 accept(run(&contexts[0], payloads[0], lengths[0], signatures[0], sig_lengths[0], REPLACE, 0, 0), good_summary);
 reject(run(&contexts[0], NULL, 0, signatures[0], sig_lengths[0], REAL, 0, 0), 0, 0);
 memcpy(changed, payloads[1], lengths[1]); changed[AUTH_INPUT] = 0;
 reject(run(&contexts[1], changed, AUTH_INPUT + 1, signatures[1], sig_lengths[1], REAL, 0, 0), 0, 0);
 c = contexts[0]; c.inspector_digest[0] ^= 1; reject(run(&c, payloads[0], lengths[0], signatures[0], sig_lengths[0], REAL, 0, 0), 0, 0);
 c = contexts[0]; c.verifier_digest[0] ^= 1; reject(run(&c, payloads[0], lengths[0], signatures[0], sig_lengths[0], REAL, 0, 0), 0, 0);
 require(!setenv("PATH", "/untrusted", 1) && !setenv("SIGNIFY_OPTIONS", "$(touch forbidden)", 1) && !setenv("LC_ALL", "untrusted", 1), "injected caller environment");
 accept(run(&contexts[0], payloads[0], lengths[0], signatures[0], sig_lengths[0], REAL, 0, 0), good_summary);
 for (i = VERIFY_FAIL; i <= VERIFY_SIGNAL; i++) reject(run(&contexts[0], payloads[0], lengths[0], signatures[0], sig_lengths[0], (enum test_mode)i, 0, 0), 0, 0);
 reject(run(&contexts[1], payloads[1], lengths[1], signatures[1], sig_lengths[1], VERIFY_CLOSED, 0, 0), 0, 0);
 r = run(&contexts[1], payloads[1], lengths[1], signatures[1], sig_lengths[1], VERIFY_STALL, 0, 0); reject(r, 0, 0); require(r.failure == AUTH_DEADLINE, "full input pipe deadline");
 for (i = INSPECT_FAIL; i <= INSPECT_DIAGNOSTIC; i++) reject(run(&contexts[0], payloads[0], lengths[0], signatures[0], sig_lengths[0], (enum test_mode)i, 0, 0), 1, 1);
 for (i = BAD_DIRECTORY; i <= BAD_INVENTORY; i++) { r = run(&contexts[0], payloads[0], lengths[0], signatures[0], sig_lengths[0], (enum test_mode)i, 0, 0); reject(r, 0, 0); require(r.failure == AUTH_CLEANUP, "postconditions reject before completion"); }
 { const int signals[] = {SIGINT, SIGTERM, SIGHUP}; for (i = 0; i < 3; i++) { r = run(&contexts[0], payloads[0], lengths[0], signatures[0], sig_lengths[0], VERIFY_STALL, signals[i], 0); reject(r, 0, 0); require(r.failure == AUTH_INTERRUPTED, "signal cleanup"); } }
 { long long before = auth_now(); r = run(&contexts[0], NULL, 0, signatures[0], sig_lengths[0], REAL, 0, 1); reject(r, 0, 0); require(r.failure == AUTH_DEADLINE && auth_now() - before >= 9000 && auth_now() - before <= 13000, "unfinished input deadline"); }
 free(changed);
}
static void aliases(void) {
 int i; unsigned char *b = malloc(AUTH_INPUT + 1), sig[AUTH_RECORD]; char path[80], signature[80]; struct auth_context c;
 require(b != NULL, "alias memory");
 for (i = 0; i < 26; i++) {
  size_t n, sn; struct auth_result r; int accepted = i < 2 || (i >= 6 && i < 10);
  snprintf(path, sizeof(path), ROOT "aliases/alias-%02d.tar", i); n = read_file(path, b, AUTH_INPUT + 1);
  snprintf(signature, sizeof(signature), ROOT "keys/alias-%02d.sig", i); sign_bytes(b, n, signature, 0); sn = read_file(signature, sig, AUTH_RECORD);
  c = contexts[0]; c.length = n; hash(b, n, c.digest);
  r = run(&c, b, n, sig, sn, REAL, 0, 0);
  if (accepted) accept(r, good_summary); else reject(r, 1, 1);
 }
 free(b);
}
static void dispose_signatures(void) {
 char path[80]; int i; static const char *const names[] = {"selected.pub", "selected.sec", "other.pub", "other.sec"};
 for (i = 0; i < 3; i++) { snprintf(path, sizeof(path), ROOT "keys/sig-%d", i); require(!unlink(path), "signature disposal"); }
 for (i = 0; i < 26; i++) { snprintf(path, sizeof(path), ROOT "keys/alias-%02d.sig", i); require(!unlink(path), "alias signature disposal"); }
 require(fixed_inventory(ROOT "keys", names, 4), "only approved two pairs retained privately");
 for (i = 0; i < 3; i++) free(payloads[i]);
}
int main(int argc, char **argv) {
 struct sigaction sa; struct rlimit core = {0,0}; struct termios before, after; int tty;
 (void)argv; if (argc != 1) return 2;
 umask(077); memset(&sa, 0, sizeof(sa)); sa.sa_handler = SIG_IGN; sigemptyset(&sa.sa_mask);
 require(!sigaction(SIGPIPE, &sa, NULL) && !setrlimit(RLIMIT_CORE, &core), "supervisor signal/core setup");
 tty = isatty(0); if (tty) require(!tcgetattr(0, &before), "saved TTY");
 canary(); setup(); checks(); aliases(); dispose_signatures(); canary();
 if (tty) { require(!tcgetattr(0, &after) && before.c_iflag == after.c_iflag && before.c_oflag == after.c_oflag && before.c_cflag == after.c_cflag && before.c_lflag == after.c_lflag && !memcmp(before.c_cc, after.c_cc, sizeof(before.c_cc)) && cfgetispeed(&before) == cfgetispeed(&after) && cfgetospeed(&before) == cfgetospeed(&after), "TTY unchanged"); }
 printf("Native authentication checks passed: %d cases; controller peak %ld KiB; child peak %ld KiB; retained pipe bytes observed; same two pairs retained privately; canary unchanged; TTY %s.\n", tests, peak_controller, peak_child, tty ? "unchanged" : "unavailable");
 return 0;
}
#endif
