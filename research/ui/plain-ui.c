/* SPDX-License-Identifier: BSD-3-Clause */
/* Research simulation only. No installer, file, network or privilege backend. */
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
static long long end_time;

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
valid_label(const char *s)
{
    size_t i, length = strlen(s);
    unsigned char c;
    if (!length || length > 64)
        return 0;
    for (i = 0; i < length; i++) {
        c = (unsigned char)s[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == ' ' || c == '_' || c == '-'))
            return 0;
    }
    return 1;
}

static int
setup(void)
{
    struct sigaction action;
    struct termios t;
    struct winsize size;
    int signals[] = {SIGINT, SIGTERM, SIGHUP, SIGWINCH};
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
    if (isatty(STDIN_FILENO)) {
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
        if (!ioctl(STDIN_FILENO, TIOCGWINSZ, &size) &&
            size.ws_col >= 10 && size.ws_col <= 240 &&
            size.ws_row >= 8 && size.ws_row <= 100) {
            width = size.ws_col;
        } else if (line("Dimensions unknown; fit unqualified.")) {
            return -1;
        }
    } else if (line("Dimensions unknown; fit unqualified.")) {
        return -1;
    }
    return 0;
}

int
main(void)
{
    char input[LINE_CAP + 1], label[65] = "", choice[6] = "";
    struct termios restored;
    int state = 0, result = 2, read_result, pending;
    long long start = now_ms();
    if (start < 0)
        return 3;
    end_time = start + 60000;
    output_flags = fcntl(STDOUT_FILENO, F_GETFL);
    if (output_flags < 0 ||
        fcntl(STDOUT_FILENO, F_SETFL, output_flags | O_NONBLOCK))
        return 3;
    if (setup()) {
        result = 3;
        goto cleanup;
    }
    for (;;) {
        if (stopped || line("SIMULATION: Example project") ||
            line("h help | b back | q cancel"))
            break;
        if (state == 0) {
            if (label[0] && (line("Saved label:") || line(label) ||
                line("keep retains the saved label.")))
                break;
            if (line("Label (ASCII, 1-64 bytes):"))
                break;
        } else if (state == 1) {
            if (choice[0] && (line("Saved choice:") || line(choice) ||
                line("keep retains the saved choice.")))
                break;
            if (line("Choice: alpha or beta"))
                break;
        } else {
            if (line("Review label:") || line(label) ||
                line("Review choice:") || line(choice) ||
                line("Type complete to finish simulation."))
                break;
        }
        read_result = read_line(input);
        if (read_result < 0 || stopped)
            break;
        if (read_result == 1) {
            if (line("Input rejected."))
                break;
            continue;
        }
        if (!strcmp(input, "q"))
            break;
        if (!strcmp(input, "h")) {
            if (line("Synthetic choices only; no installer."))
                break;
            continue;
        }
        if (!strcmp(input, "b")) {
            if (state > 0)
                state--;
            continue;
        }
        if (!strcmp(input, "keep") &&
            ((state == 0 && label[0]) || (state == 1 && choice[0]))) {
            state++;
        } else if (state == 0 && valid_label(input)) {
            memcpy(label, input, strlen(input) + 1);
            state = 1;
        } else if (state == 1 &&
            (!strcmp(input, "alpha") || !strcmp(input, "beta"))) {
            memcpy(choice, input, strlen(input) + 1);
            state = 2;
        } else if (state == 2 && !strcmp(input, "complete")) {
            result = 0;
            break;
        } else if (line("Input rejected.")) {
            break;
        }
    }
cleanup:
    if (terminal_saved) {
        /* Discard owned queued input. FIONREAD processes OpenBSD PENDIN
         * on the emptied queue without blocking. Reapply the original
         * flags afterwards, including any initially saved PENDIN state. */
        if (tcsetattr(STDIN_FILENO, TCSANOW, &saved) ||
            tcflush(STDIN_FILENO, TCIFLUSH) ||
            ioctl(STDIN_FILENO, FIONREAD, &pending) ||
            tcsetattr(STDIN_FILENO, TCSANOW, &saved) ||
            tcgetattr(STDIN_FILENO, &restored)) {
            result = 3;
        } else if (restored.c_iflag != saved.c_iflag ||
            restored.c_oflag != saved.c_oflag ||
            restored.c_cflag != saved.c_cflag ||
            restored.c_lflag != saved.c_lflag ||
            memcmp(restored.c_cc, saved.c_cc, sizeof(saved.c_cc)) ||
            cfgetispeed(&restored) != cfgetispeed(&saved) ||
            cfgetospeed(&restored) != cfgetospeed(&saved)) {
            result = 3;
        }
    }
    /* Completion is reported only after terminal postconditions hold. */
    if (result == 0 && (stopped || line("SIMULATION COMPLETE")))
        result = 2;
    if (output_flags >= 0 &&
        fcntl(STDOUT_FILENO, F_SETFL, output_flags))
        result = 3;
    return result;
}
