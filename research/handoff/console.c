/* SPDX-License-Identifier: BSD-3-Clause */
/* Original bounded review console. This file cannot execute an installer. */
#include "snapshot.h"
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define LINE_CAP 1024
#define WORK_CAP 65536
#define OUTPUT_CAP 60000
static volatile sig_atomic_t stopped;
static struct termios saved;
static int terminal_saved, output_flags = -1, width = 40, skip_lf;
static size_t work, output;
static long long end_time, response_time;

static void
stop_handler(int signal_number)
{
    stopped = signal_number;
}

static long long
now_ms(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t) == -1)
        return -1;
    return (long long)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

static int
wait_fd(int fd, short events, long long deadline)
{
    struct pollfd p;
    long long now, left;
    int result;
    p.fd = fd;
    p.events = events;
    for (;;) {
        now = now_ms();
        if (stopped || now < 0 || now >= deadline || now >= end_time)
            return -1;
        left = deadline - now;
        if (left > end_time - now)
            left = end_time - now;
        p.revents = 0;
        result = poll(&p, 1, (int)left);
        if (result < 0 && errno == EINTR)
            continue;
        if (result <= 0 || (p.revents & (POLLERR | POLLNVAL)))
            return -1;
        if (p.revents & (events | POLLHUP))
            return 0;
    }
}

static int
emit_bytes(const char *text, size_t length)
{
    long long deadline = now_ms();
    ssize_t n;
    if (deadline < 0 || length > OUTPUT_CAP - output)
        return -1;
    deadline += 5000;
    if (deadline > response_time) deadline = response_time;
    while (length) {
        if (wait_fd(STDOUT_FILENO, POLLOUT, deadline) == -1)
            return -1;
        n = write(STDOUT_FILENO, text, length);
        if (n < 0 && (errno == EINTR || errno == EAGAIN))
            continue;
        if (n <= 0)
            return -1;
        output += (size_t)n;
        length -= (size_t)n;
        text += n;
    }
    return 0;
}

/* Trusted literals or previously validated ASCII only; wrap without clipping. */
static int
line(const char *text)
{
    size_t length = strlen(text), chunk;
    while (length) {
        chunk = length > (size_t)width ? (size_t)width : length;
        if (emit_bytes(text, chunk) || emit_bytes("\n", 1))
            return -1;
        text += chunk;
        length -= chunk;
    }
    return 0;
}

/* -1: stop/EOF; 0: accepted bytes; 1: whole line rejected. */
static int
read_line(char *buffer)
{
    size_t length = 0;
    int bad = 0;
    unsigned char c;
    ssize_t n;
    long long deadline = now_ms(), idle;
    if (deadline < 0)
        return -1;
    deadline += 10000;
    idle = deadline;
    for (;;) {
        if (wait_fd(STDIN_FILENO, POLLIN, idle < deadline ? idle : deadline))
            return -1;
        n = read(STDIN_FILENO, &c, 1);
        if (n < 0 && errno == EINTR)
            continue;
        if (n != 1 || ++work > WORK_CAP)
            return -1;
        idle = now_ms();
        if (idle < 0)
            return -1;
        idle += 10000;
        if (skip_lf) {
            skip_lf = 0;
            if (c == '\n')
                continue;
        }
        if (c == '\r' || c == '\n') {
            skip_lf = c == '\r';
            buffer[length] = '\0';
            response_time = now_ms();
            if (response_time < 0) return -1;
            response_time += 5000;
            return bad;
        }
        if (c < 32 || c > 126)
            bad = 1;
        if (length < LINE_CAP)
            buffer[length++] = (char)c;
        else
            bad = 1;
    }
}

static int
setup(void)
{
    struct sigaction action;
    struct termios t;
    struct winsize size;
    memset(&size, 0, sizeof(size));
    int signals[] = {SIGINT, SIGTERM, SIGHUP, SIGWINCH, SIGALRM};
    size_t i;
    memset(&action, 0, sizeof(action));
    action.sa_handler = stop_handler;
    if (sigemptyset(&action.sa_mask))
        return -1;
    for (i = 0; i < sizeof(signals) / sizeof(signals[0]); i++)
        if (sigaction(signals[i], &action, NULL))
            return -1;
    action.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &action, NULL))
        return -1;
    if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO)) {
        if (tcgetattr(STDIN_FILENO, &saved))
            return -1;
        terminal_saved = 1;
        t = saved;
        t.c_iflag = 0;
        t.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
        t.c_cc[VMIN] = 1;
        t.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &t))
            return -1;
        if (ioctl(STDIN_FILENO, TIOCGWINSZ, &size))
            memset(&size, 0, sizeof(size));
        if ((size.ws_col && (size.ws_col < 40 || size.ws_col > 240)) ||
            (size.ws_row && (size.ws_row < 12 || size.ws_row > 100)))
            return -1;
        if ((!size.ws_col || !size.ws_row) &&
            line("Dimensions unknown; fit unqualified.")) {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}


int
handoff_interrupted(void)
{
    return stopped != 0;
}

/* Require a quiet queue; CRLF's remaining LF is only a delimiter. */
static int
quiet(void)
{
    struct pollfd p = { STDIN_FILENO, POLLIN, 0 };
    unsigned char c;
    long long deadline = now_ms();
    int n;
    if (deadline < 0) return -1;
    deadline += 50;
    for (;;) {
        long long now = now_ms();
        if (stopped || now < 0 || now >= end_time) return -1;
        if (now >= deadline) return 0;
        n = poll(&p, 1, (int)(deadline - now));
        if (n < 0 && errno == EINTR) continue;
        if (n < 0 || (p.revents & (POLLERR | POLLHUP | POLLNVAL))) return -1;
        if (n == 0) return 0;
        if (!skip_lf || read(STDIN_FILENO, &c, 1) != 1 ||
            c != '\n' || ++work > WORK_CAP) return -1;
        skip_lf = 0;
    }
}

int
handoff_console(const struct handoff_snapshot *reviewed,
    int (*observe)(struct handoff_snapshot *))
{
    char input[LINE_CAP + 1], uid[17], command[23], number[64];
    struct handoff_snapshot current;
    struct termios restored;
    int state = 0, result = 2, pending, n;
    size_t i;
    long long start = now_ms();
    uint64_t bytes;
    if (start < 0 || storage_label_check(&reviewed->label, &bytes) != STORAGE_VALID)
        return 3;
    end_time = start + 60000;
    response_time = start + 5000;
    for (i = 0; i < 8; i++) {
        static const char hex[] = "0123456789abcdef";
        uid[i * 2] = hex[reviewed->uid[i] >> 4];
        uid[i * 2 + 1] = hex[reviewed->uid[i] & 15];
    }
    uid[16] = '\0';
    memcpy(command, "erase ", 6); memcpy(command + 6, uid, 17);
    output_flags = fcntl(STDOUT_FILENO, F_GETFL);
    if (output_flags < 0 ||
        fcntl(STDOUT_FILENO, F_SETFL, output_flags | O_NONBLOCK)) return 3;
    if (setup()) { result = 3; goto cleanup; }
    for (;;) {
        if (quiet() || line("Bootweave confirmed-install research") ||
            line("h help | b back | q cancel")) break;
        if (state == 0) {
            n = snprintf(number, sizeof(number), "Bytes: %llu", (unsigned long long)bytes);
            if (n < 0 || (size_t)n >= sizeof(number) || line("Reviewed fixture: sd0") || line(number)) break;
            n = snprintf(number, sizeof(number), "Sector bytes: %lu", (unsigned long)reviewed->label.sector_bytes);
            if (n < 0 || (size_t)n >= sizeof(number) || line(number) ||
                line("DUID:") || line(uid) ||
                line("Metadata proves neither emptiness") ||
                line("nor host identity.") ||
                line("Type next to review effects.")) break;
        } else if (state == 1) {
            if (line("Reviewed effects on this target:") ||
                line("Erase: whole-disk MBR, auto FFS/swap.") ||
                line("Verified stock OS sets and bootblocks.") ||
                line("Supervised install and offline boot.") ||
                line("Existing contents will be destroyed.") ||
                line("Partial installation has no rollback.") ||
                line("Type next to confirm.")) break;
        } else {
            if (line("Final confirmation for sd0:") ||
                line("8589934592 bytes; whole-disk erase.") ||
                line("Type this exact acknowledgement:") ||
                line(command)) break;
        }
        if (read_line(input) != 0 || stopped) break;
        if (!strcmp(input, "q")) break;
        if (!strcmp(input, "h")) {
            if (line("Research only; stock prompts follow.") ||
                line("No project payload or readiness claim.")) break;
            continue;
        }
        if (!strcmp(input, "b")) { if (state > 0) state--; continue; }
        if (state < 2 && !strcmp(input, "next")) { state++; continue; }
        if (state == 2 && !strcmp(input, command) && !quiet()) {
            alarm(4);
            n = observe(&current);
            alarm(0);
            if (!stopped && n == STORAGE_VALID && handoff_same(reviewed, &current) && !quiet())
                result = 0;
            else (void)line("Observation changed or unavailable.");
            break;
        }
        (void)line("Input rejected; no handoff.");
        break;
    }
cleanup:
    if (terminal_saved) {
        if (tcsetattr(STDIN_FILENO, TCSANOW, &saved) ||
            tcflush(STDIN_FILENO, TCIFLUSH) ||
            ioctl(STDIN_FILENO, FIONREAD, &pending) ||
            tcsetattr(STDIN_FILENO, TCSANOW, &saved) ||
            tcgetattr(STDIN_FILENO, &restored)) result = 3;
        else if (restored.c_iflag != saved.c_iflag ||
            restored.c_oflag != saved.c_oflag || restored.c_cflag != saved.c_cflag ||
            restored.c_lflag != saved.c_lflag ||
            memcmp(restored.c_cc, saved.c_cc, sizeof(saved.c_cc)) ||
            cfgetispeed(&restored) != cfgetispeed(&saved) ||
            cfgetospeed(&restored) != cfgetospeed(&saved)) result = 3;
    }
    if (result == 0 && (stopped || line("Confirmation checked; handoff requested."))) result = 2;
    if (output_flags >= 0 && fcntl(STDOUT_FILENO, F_SETFL, output_flags)) result = 3;
#ifdef __OpenBSD__
    if (output_flags >= 0 && fcntl(STDOUT_FILENO, F_GETFL) != output_flags) result = 3;
#endif
    return result;
}
