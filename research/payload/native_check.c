/* SPDX-License-Identifier: BSD-3-Clause */
/* Original fixed-path OpenBSD research supervisor. Never extracts archives. */
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#ifndef __OpenBSD__
int main(void) { fputs("Native payload checks require OpenBSD.\n", stderr); return 77; }
#else
#define ROOT "/root/payload/"
#define CAP 4096
static long peak_kib;
static int tests;
static const char *binaries[] = { ROOT "inspect", ROOT "inspect-static" };
static const char *summaries[] = {
 "Inspection passed: 2 members, 1 files, 5 content bytes.\n",
 "Inspection passed: 2 members, 1 files, 5 content bytes.\n",
 "Inspection passed: 1 members, 1 files, 0 content bytes.\n",
 "Inspection passed: 128 members, 128 files, 0 content bytes.\n",
 "Inspection passed: 2 members, 2 files, 524288 content bytes.\n",
 "Inspection passed: 1 members, 1 files, 0 content bytes.\n",
 "Inspection passed: 1 members, 1 files, 0 content bytes.\n",
 "Inspection passed: 1 members, 1 files, 0 content bytes.\n"
};
enum mode { NORMAL, PARTIAL, TRUNCATED, EXCESS, STALL, INTERRUPT, BROKEN, FULL, ARGUMENT };
static void fail(const char *message) { fprintf(stderr, "Native check failed: %s.\n", message); exit(1); }
static long long now(void) {
 struct timespec t;
 if (clock_gettime(CLOCK_MONOTONIC, &t)) fail("clock");
 return (long long)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
static void nonblock(int fd) {
 int flags = fcntl(fd, F_GETFL);
 if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK)) fail("pipe flags");
}
static void limit(int resource, rlim_t value) {
 struct rlimit bound = { value, value };
 if (setrlimit(resource, &bound)) _exit(125);
}
static void canary(void) {
 char data[32];
 int fd = open("canary", O_RDONLY | O_NOFOLLOW);
 ssize_t n;
 DIR *d; struct dirent *e; int count = 0;
 if (fd < 0) fail("canary open");
 n = read(fd, data, sizeof(data)); close(fd);
 if (n != 14 || memcmp(data, "owned sentinel", 14)) fail("canary changed");
 d = opendir("."); if (!d) fail("work inventory");
 while ((e = readdir(d))) {
  if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
  if (strcmp(e->d_name, "canary")) fail("unexpected member effect");
  count++;
 }
 closedir(d); if (count != 1) fail("work inventory changed");
}
static void collect(int fd, char *data, size_t *used) {
 ssize_t n;
 for (;;) {
  if (*used >= CAP) fail("capture budget");
  n = read(fd, data + *used, CAP - *used);
  if (n > 0) { *used += (size_t)n; continue; }
  if (n < 0 && errno != EAGAIN && errno != EINTR) fail("capture read");
  break;
 }
}
static void check(int binary, int index, enum mode mode, int signal_number) {
 int in[2], out[2], err[2], source, saved[3], status = 0, done = 0;
 int sent_signal = 0, input_closed = 0;
 pid_t child, result;
 char path[80], output[CAP], errors[CAP], chunk[4096];
 size_t used = 0, error_used = 0, pending = 0, at = 0, supplied = 0, filled = 0;
 ssize_t n;
 long long start, elapsed; struct rusage usage;
 if (snprintf(path, sizeof(path), ROOT "cases/case-%02d.tar", index) >= (int)sizeof(path)) fail("fixed path");
 source = open(path, O_RDONLY | O_NOFOLLOW); if (source < 0) fail("fixture open");
 if (pipe(in) || pipe(out) || pipe(err)) fail("pipe creation");
 saved[0] = fcntl(in[0], F_GETFL); saved[1] = fcntl(out[1], F_GETFL); saved[2] = fcntl(err[1], F_GETFL);
 if (saved[0] < 0 || saved[1] < 0 || saved[2] < 0) fail("saved flags");
 if (mode == FULL) {
  memset(chunk, 'X', sizeof(chunk)); nonblock(out[1]);
  for (;;) {
   n = write(out[1], chunk, sizeof(chunk));
   if (n < 0 && errno == EAGAIN) break;
   if (n <= 0 || (filled += (size_t)n) > 1048576) fail("pipe fill");
  }
  while ((n = write(out[1], chunk, 1)) == 1) if (++filled > 1048576) fail("pipe fill budget");
  if (n != -1 || errno != EAGAIN || fcntl(out[1], F_SETFL, saved[1])) fail("full pipe setup");
 }
 if (mode == BROKEN) close(out[0]);
 start = now(); child = fork(); if (child < 0) fail("fork");
 if (!child) {
  char * const argv[] = { (char *)binaries[binary], NULL };
  char * const badargv[] = { (char *)binaries[binary], "$(touch unexpected-effect)", NULL };
  char * const env[] = { "PATH=/bin:/usr/bin", "LC_ALL=C", "TZ=UTC", NULL };
  if (dup2(in[0], 0) < 0 || dup2(out[1], 1) < 0 || dup2(err[1], 2) < 0) _exit(125);
  closefrom(3);
  limit(RLIMIT_DATA, 32 * 1024 * 1024); limit(RLIMIT_RSS, 32 * 1024 * 1024);
  limit(RLIMIT_CPU, 15); limit(RLIMIT_FSIZE, 1048576); limit(RLIMIT_CORE, 0);
  execve(binaries[binary], mode == ARGUMENT ? badargv : argv, env); _exit(125);
 }
 nonblock(in[1]); if (mode != BROKEN) nonblock(out[0]); nonblock(err[0]);
 for (;;) {
  elapsed = now() - start;
  if (elapsed >= 15000) { kill(child, SIGKILL); waitpid(child, &status, 0); fail("supervisor deadline"); }
  if (!input_closed && mode != STALL && mode != INTERRUPT) {
   size_t ceiling = mode == TRUNCATED ? 511 : mode == EXCESS ? 512 : 1048577;
   if (at == pending && supplied < ceiling) {
    size_t amount = mode == PARTIAL ? 17 : sizeof(chunk);
    if (amount > ceiling - supplied) amount = ceiling - supplied;
    n = read(source, chunk, amount); if (n < 0) fail("fixture read");
    pending = (size_t)n; at = 0;
   }
   if (at < pending) {
    n = write(in[1], chunk + at, pending - at);
    if (n > 0) { at += (size_t)n; supplied += (size_t)n; }
    else if (n < 0 && errno != EAGAIN && errno != EPIPE && errno != EINTR) fail("input write");
   } else if (mode != EXCESS) { close(in[1]); input_closed = 1; }
  }
  if (mode == INTERRUPT && !sent_signal && elapsed >= 100) {
   if (kill(child, signal_number)) fail("signal delivery");
   sent_signal = 1;
  }
  if (mode != FULL && mode != BROKEN) collect(out[0], output, &used);
  collect(err[0], errors, &error_used);
  result = wait4(child, &status, WNOHANG, &usage);
  if (result < 0) fail("wait4");
  if (result == child) { done = 1; break; }
  (void)poll(NULL, 0, mode == PARTIAL ? 1 : 2);
 }
 elapsed = now() - start;
 if (!done || !WIFEXITED(status)) fail("child completion");
 if (mode != FULL && mode != BROKEN) collect(out[0], output, &used);
 collect(err[0], errors, &error_used);
 if (fcntl(in[0], F_GETFL) != saved[0] || fcntl(out[1], F_GETFL) != saved[1] || fcntl(err[1], F_GETFL) != saved[2]) fail("exact descriptor flags");
 if (usage.ru_maxrss < 0 || usage.ru_maxrss > 32768) fail("RSS budget");
 if (usage.ru_maxrss > peak_kib) peak_kib = usage.ru_maxrss;
 if (used + error_used > 4096 || memchr(output, 27, used) || memchr(errors, 27, error_used)) fail("diagnostic budget");
 if ((mode == NORMAL && index < 8) || mode == PARTIAL) {
  const char *expected = summaries[index];
  if (WEXITSTATUS(status) || error_used || used != strlen(expected) || memcmp(output, expected, used)) fail("accepted summary");
 } else if (mode == BROKEN || mode == FULL) {
  if (WEXITSTATUS(status) != 3 || error_used || used) fail("output failure status");
 } else {
  static const char *allowed[] = { "input", "input limit", "header", "path", "content limit", "tree conflict", "trailer or empty archive", "trailing data", "member limit", "content input", "content padding", "arguments" };
  size_t i; int matched = 0; char expected[96];
  if (!WEXITSTATUS(status) || used) fail("rejection status");
  for (i = 0; i < sizeof(allowed) / sizeof(allowed[0]); i++) {
   int length = snprintf(expected, sizeof(expected), "Inspection rejected: %s.\n", allowed[i]);
   if (length > 0 && error_used == (size_t)length && !memcmp(errors, expected, error_used)) matched = 1;
  }
  if (mode != INTERRUPT && !matched) fail("fixed rejection diagnostic");
  if (mode == INTERRUPT && error_used) fail("signal diagnostic");
 }
 if (mode == EXCESS && elapsed > 3000) fail("excess body wait");
 if (mode == STALL && (elapsed < 9000 || elapsed > 13000)) fail("input deadline");
 if (mode == FULL && (elapsed < 4000 || elapsed > 7000)) fail("output deadline");
 if (mode == FULL) {
  size_t drained = 0;
  for (;;) {
   n = read(out[0], chunk, sizeof(chunk));
   if (n < 0 && errno == EAGAIN) break;
   if (n <= 0) fail("full pipe readback");
   for (at = 0; at < (size_t)n; at++) if (chunk[at] != 'X') fail("false output completion");
   drained += (size_t)n;
  }
  if (drained != filled) fail("full pipe changed");
 }
 close(source); close(in[0]); if (!input_closed) close(in[1]);
 if (mode != BROKEN) close(out[0]);
 close(out[1]); close(err[0]); close(err[1]);
 canary(); tests++;
}
int main(int argc, char **argv) {
 int binary, index, have_tty;
 struct sigaction action; struct termios before, after;
 (void)argv; if (argc != 1) fail("arguments");
 memset(&action, 0, sizeof(action)); action.sa_handler = SIG_IGN;
 if (sigemptyset(&action.sa_mask) || sigaction(SIGPIPE, &action, NULL)) fail("SIGPIPE setup");
 if (chdir(ROOT "work")) fail("owned work directory");
 have_tty = isatty(0); if (have_tty && tcgetattr(0, &before)) fail("saved TTY");
 canary();
 for (binary = 0; binary < 2; binary++) {
  for (index = 0; index < 35; index++) check(binary, index, NORMAL, 0);
  check(binary, 0, PARTIAL, 0); check(binary, 0, TRUNCATED, 0);
  check(binary, 31, EXCESS, 0); check(binary, 0, STALL, 0);
  check(binary, 0, INTERRUPT, SIGINT); check(binary, 0, INTERRUPT, SIGTERM);
  check(binary, 0, INTERRUPT, SIGHUP); check(binary, 2, BROKEN, 0);
  check(binary, 2, FULL, 0); check(binary, 0, ARGUMENT, 0);
 }
 if (have_tty && (tcgetattr(0, &after) || before.c_iflag != after.c_iflag || before.c_oflag != after.c_oflag || before.c_cflag != after.c_cflag || before.c_lflag != after.c_lflag || memcmp(before.c_cc, after.c_cc, sizeof(before.c_cc)) || cfgetispeed(&before) != cfgetispeed(&after) || cfgetospeed(&before) != cfgetospeed(&after))) fail("TTY attributes changed");
 printf("Native payload checks passed: %d cases; peak RSS %ld KiB; exact flags restored; canary unchanged; TTY %s.\n", tests, peak_kib, have_tty ? "unchanged" : "unavailable");
 return 0;
}
#endif
