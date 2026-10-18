/* SPDX-License-Identifier: BSD-3-Clause */
/* Trusted test supervisor; never linked into the planner. */
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#ifdef __linux__
#define _DEFAULT_SOURCE
#endif
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <poll.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#ifdef __OpenBSD__
#include <util.h>
#elif defined(__APPLE__)
#include <util.h>
#else
#include <pty.h>
#endif
#include "native_cases.h"

enum mode { NORMAL, ROOT_USER, PIPE_INPUT, FIFO_INPUT, SOCKET_INPUT,
    DEVICE_INPUT, TTY_INPUT, RDWR_INPUT, OFFSET_INPUT, CLOSED_INPUT,
    OUTPUT_ALIAS, ERROR_ALIAS, BROKEN_OUTPUT, BROKEN_ERROR, BLOCKED_OUTPUT,
    BLOCKED_ERROR, INTERRUPTED, FILE_OUTPUT, ACTUAL_CONSOLE, INVOKE_EXTRA };
static volatile sig_atomic_t active;
static unsigned int checked;
static long peak_kib;
static long long suite_start;
static char home[PATH_MAX];
static struct stat canary_stat, outside_stat;
static const char canary[] = "planner outside canary\n";
static int console_saved;
static struct winsize original_window;

static void
supervisor_deadline(int signal_number)
{
    int status;
    pid_t observed;
    (void)signal_number;
    if (active > 0) {
        do {
            observed = waitpid((pid_t)active, &status, WNOHANG);
        } while (observed == -1 && errno == EINTR);
        if (observed == 0) {
            (void)kill((pid_t)active, SIGKILL);
            while (waitpid((pid_t)active, &status, 0) == -1 && errno == EINTR)
                ;
        }
    }
    _exit(92);
}

static long long
now(void)
{
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1)
        exit(91);
    return (long long)time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

static void
fail(const char *reason)
{
    int status;
    if (active > 0) {
        (void)kill(active, SIGKILL);
        while (waitpid(active, &status, 0) == -1 && errno == EINTR)
            ;
    }
    if (console_saved)
        (void)ioctl(1, TIOCSWINSZ, &original_window);
    fprintf(stderr, "Planner native harness failed: %s; completed=%u\n", reason, checked);
    exit(1);
}

static int
metadata(const struct stat *a, const struct stat *b)
{
    if (a->st_dev != b->st_dev || a->st_ino != b->st_ino ||
        a->st_mode != b->st_mode || a->st_uid != b->st_uid ||
        a->st_gid != b->st_gid || a->st_size != b->st_size ||
        a->st_nlink != b->st_nlink)
        return 0;
#ifdef __APPLE__
    return a->st_mtimespec.tv_sec == b->st_mtimespec.tv_sec &&
        a->st_mtimespec.tv_nsec == b->st_mtimespec.tv_nsec &&
        a->st_ctimespec.tv_sec == b->st_ctimespec.tv_sec &&
        a->st_ctimespec.tv_nsec == b->st_ctimespec.tv_nsec;
#else
    return a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
        a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
        a->st_ctim.tv_sec == b->st_ctim.tv_sec &&
        a->st_ctim.tv_nsec == b->st_ctim.tv_nsec;
#endif
}

static void
write_all(int fd, const unsigned char *data, size_t length)
{
    ssize_t amount;
    while (length) {
        amount = write(fd, data, length);
        if (amount == -1 && errno == EINTR)
            continue;
        if (amount <= 0)
            fail("fixture write");
        data += amount;
        length -= (size_t)amount;
    }
}

static void
observe(const char *path, const unsigned char *bytes, size_t length,
    const struct stat *before)
{
    unsigned char buffer[16386];
    struct stat after;
    int fd = open(path, O_RDONLY | O_NOFOLLOW);
    size_t used = 0;
    ssize_t amount;
    if (fd < 0 || fstat(fd, &after) == -1 || !metadata(before, &after))
        fail("held fixture metadata changed");
    do {
        amount = read(fd, buffer + used, sizeof(buffer) - used);
        if (amount < 0 || used + (size_t)amount >= sizeof(buffer))
            fail("fixture read");
        used += (size_t)amount;
    } while (amount > 0);
    if (close(fd) == -1 || used != length || memcmp(buffer, bytes, length) ||
        lstat(path, &after) == -1 || !metadata(before, &after))
        fail("fixture content or named identity changed");
}

static void
inventory(void)
{
    DIR *directory = opendir(".");
    struct dirent *entry;
    int count = 0;
    if (directory == NULL)
        fail("work inventory");
    while ((entry = readdir(directory)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if (strcmp(entry->d_name, "input") && strcmp(entry->d_name, "canary"))
            fail("unexpected work path");
        count++;
    }
    if (closedir(directory) == -1 || count != 2)
        fail("work inventory count");
    observe("canary", (const unsigned char *)canary, sizeof(canary) - 1, &canary_stat);
    observe("../outside-canary", (const unsigned char *)canary,
        sizeof(canary) - 1, &outside_stat);
}

static void
nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        fail("pipe flags");
}

static void
collect(int fd, char *buffer, size_t *length, size_t capacity)
{
    ssize_t amount;
    for (;;) {
        if (*length == capacity)
            fail("capture ceiling");
        amount = read(fd, buffer + *length, capacity - *length);
        if (amount == -1 && (errno == EAGAIN || errno == EINTR))
            return;
        if (amount < 0)
            fail("capture read");
        if (amount == 0)
            return;
        *length += (size_t)amount;
    }
}

static int
diagnostic(const char *bytes, size_t length)
{
    const char *const fixed[] = {"bootweave: input rejected\n",
        "bootweave: runtime refused\n", "bootweave: invalid invocation; use --help\n"};
    const char prefix[] = "bootweave: definition rejected at byte ";
    size_t i, value = 0;
    for (i = 0; i < sizeof(fixed) / sizeof(fixed[0]); i++)
        if (length == strlen(fixed[i]) && !memcmp(bytes, fixed[i], length))
            return 1;
    if (length <= sizeof(prefix) || memcmp(bytes, prefix, sizeof(prefix) - 1) ||
        bytes[length - 1] != '\n')
        return 0;
    for (i = sizeof(prefix) - 1; i + 1 < length; i++) {
        if (bytes[i] < '0' || bytes[i] > '9' || value > 16384)
            return 0;
        value = value * 10 + (unsigned int)(bytes[i] - '0');
    }
    return value <= 16384;
}

static int
same_terminal(const struct termios *a, const struct termios *b)
{
    return a->c_iflag == b->c_iflag && a->c_oflag == b->c_oflag &&
        a->c_cflag == b->c_cflag && a->c_lflag == b->c_lflag &&
        !memcmp(a->c_cc, b->c_cc, sizeof(a->c_cc)) &&
        cfgetispeed(a) == cfgetispeed(b) && cfgetospeed(a) == cfgetospeed(b);
}

static void
check(const char *binary, const struct native_case *fixture, enum mode mode,
    int expected_status, const char *argument, const char *environment_value)
{
    char executable[PATH_MAX], output[8193], errors[257], fill[4096];
    char fault_environment[96];
    char *arguments[4], *environment[6];
    int source, out[2], err[2], extra[2] = {-1, -1}, status, input_fd;
    int out_flags, err_flags, done = 0, sent_signal = 0, output_file = -1;
    pid_t result;
    size_t used = 0, error_used = 0, filled = 0;
    ssize_t amount;
    long long started, elapsed;
    long rss;
    struct rusage usage;
    struct stat before, observed;
    struct termios tty_before, tty_after;
    sigset_t mask;

    if (now() - suite_start > 900000)
        fail("suite deadline");
    if (snprintf(executable, sizeof(executable), "%s/%s", home, binary) >= (int)sizeof(executable))
        fail("executable path");
    source = open("input", O_RDWR | O_NOFOLLOW);
    if (source < 0 || fstat(source, &before) == -1 || !S_ISREG(before.st_mode) ||
        before.st_nlink != 1 || ftruncate(source, 0) == -1)
        fail("input preparation");
    write_all(source, fixture->data, fixture->length);
    close(source);
    source = open("input", mode == RDWR_INPUT ? O_RDWR : O_RDONLY);
    if (source < 0 || fstat(source, &before) == -1 || pipe(out) || pipe(err))
        fail("case descriptors");
    /* Establish kernel first-write state before the exact flag baseline. */
    if (write(out[1], "X", 1) != 1 || read(out[0], fill, 1) != 1 ||
        write(err[1], "X", 1) != 1 || read(err[0], fill, 1) != 1)
        fail("owned pipe staging");
    input_fd = source;
    if (mode == OFFSET_INPUT && lseek(source, 1, SEEK_SET) != 1)
        fail("offset preparation");
    if (mode == PIPE_INPUT && pipe(extra))
        fail("input pipe");
    if (mode == SOCKET_INPUT && socketpair(AF_UNIX, SOCK_STREAM, 0, extra))
        fail("input socket");
    if (mode == TTY_INPUT) {
        if (openpty(&extra[0], &extra[1], NULL, NULL, NULL) || tcgetattr(extra[1], &tty_before))
            fail("input PTY");
        input_fd = extra[1];
    } else if (mode == FIFO_INPUT) {
        if (mkfifo("fifo", 0600) || (extra[0] = open("fifo", O_RDONLY | O_NONBLOCK)) < 0)
            fail("input FIFO");
        input_fd = extra[0];
    } else if (mode == DEVICE_INPUT) {
        extra[0] = open("/dev/null", O_RDONLY);
        if (extra[0] < 0)
            fail("input device");
        input_fd = extra[0];
    } else if (mode == PIPE_INPUT || mode == SOCKET_INPUT) {
        input_fd = extra[0];
    }
    out_flags = fcntl(out[1], F_GETFL);
    err_flags = fcntl(err[1], F_GETFL);
    if (out_flags < 0 || err_flags < 0)
        fail("saved descriptor flags");
    if (mode == BLOCKED_OUTPUT || mode == INTERRUPTED || mode == BLOCKED_ERROR) {
        int fd = mode == BLOCKED_ERROR ? err[1] : out[1];
        memset(fill, 'X', sizeof(fill));
        nonblock(fd);
        for (;;) {
            amount = write(fd, fill, sizeof(fill));
            if (amount == -1 && errno == EAGAIN)
                break;
            if (amount <= 0 || (filled += (size_t)amount) > 1048576)
                fail("pipe fill ceiling");
        }
        while ((amount = write(fd, fill, 1)) == 1)
            if (++filled > 1048576)
                fail("pipe fill ceiling");
        if (amount != -1 || errno != EAGAIN ||
            fcntl(fd, F_SETFL, mode == BLOCKED_ERROR ? err_flags : out_flags))
            fail("full pipe preparation");
    }
    if (mode == BROKEN_OUTPUT)
        close(out[0]);
    if (mode == BROKEN_ERROR)
        close(err[0]);
    if (mode == OUTPUT_ALIAS || mode == ERROR_ALIAS)
        output_file = open("input", O_WRONLY | O_APPEND);
    if (mode == FILE_OUTPUT)
        output_file = open("../file-output", O_RDWR | O_CREAT | O_EXCL, 0600);
    if ((mode == OUTPUT_ALIAS || mode == ERROR_ALIAS || mode == FILE_OUTPUT) && output_file < 0)
        fail("output fixture");
    if (mode == ACTUAL_CONSOLE && (!isatty(1) || tcgetattr(1, &tty_before)))
        fail("actual console prerequisite");
    arguments[0] = executable;
    arguments[1] = (char *)argument;
    arguments[2] = NULL;
    arguments[3] = NULL;
    if (mode == INVOKE_EXTRA)
        arguments[2] = "extra";
    environment[0] = "PATH=/bin:/usr/bin";
    environment[1] = "LC_ALL=C";
    environment[2] = "TZ=UTC";
    environment[3] = NULL;
    environment[4] = NULL;
    environment[5] = NULL;
    if (environment_value != NULL) {
        if (strlen(environment_value) >= sizeof(fault_environment))
            fail("test environment bound");
        strcpy(fault_environment, environment_value);
        environment[3] = fault_environment;
    }
    started = now();
    active = fork();
    if (active < 0)
        fail("fork");
    if (active == 0) {
        if (dup2(input_fd, 0) < 0 ||
            dup2(mode == ACTUAL_CONSOLE ? 1 :
                (mode == OUTPUT_ALIAS || mode == FILE_OUTPUT ? output_file : out[1]), 1) < 0 ||
            dup2(mode == ERROR_ALIAS ? output_file : err[1], 2) < 0)
            _exit(125);
        if (mode == CLOSED_INPUT)
            close(0);
        if (dup2(source, 9) < 0)
            _exit(125);
        sigemptyset(&mask);
        sigaddset(&mask, SIGALRM);
        if (sigprocmask(SIG_BLOCK, &mask, NULL))
            _exit(125);
#ifdef __OpenBSD__
        if (mode != ROOT_USER) {
            gid_t group = 32767;
            if (setgroups(1, &group) || setresgid(group, group, group) ||
                setresuid(32767, 32767, 32767))
                _exit(125);
        }
        /* Preserve fd 9 as an adversarial inherited capability. */
        {
            int fd;
            for (fd = 3; fd < 9; fd++)
                close(fd);
            closefrom(10);
        }
#else
        {
            int fd;
            for (fd = 3; fd < 256; fd++)
                if (fd != 9)
                    close(fd);
        }
#endif
        execve(executable, arguments, environment);
        _exit(125);
    }
    if (mode != BROKEN_OUTPUT)
        nonblock(out[0]);
    if (mode != BROKEN_ERROR)
        nonblock(err[0]);
    for (;;) {
        elapsed = now() - started;
        if (elapsed > 7000)
            fail("child deadline");
        if (mode == INTERRUPTED && !sent_signal && elapsed >= 100) {
            if (kill(active, SIGTERM))
                fail("interrupt delivery");
            sent_signal = 1;
        }
        if (mode != BLOCKED_OUTPUT && mode != INTERRUPTED && mode != BROKEN_OUTPUT &&
            mode != OUTPUT_ALIAS && mode != FILE_OUTPUT && mode != ACTUAL_CONSOLE)
            collect(out[0], output, &used, 4096);
        if (mode != BLOCKED_ERROR && mode != BROKEN_ERROR && mode != ERROR_ALIAS)
            collect(err[0], errors, &error_used, 256);
        result = wait4(active, &status, WNOHANG, &usage);
        if (result == -1)
            fail("wait4");
        if (result == active) {
            done = 1;
            break;
        }
        (void)poll(NULL, 0, 2);
    }
    result = active;
    active = 0;
    elapsed = now() - started;
    if (!done)
        fail("completion");
    if (mode != BLOCKED_OUTPUT && mode != INTERRUPTED && mode != BROKEN_OUTPUT &&
        mode != OUTPUT_ALIAS && mode != FILE_OUTPUT && mode != ACTUAL_CONSOLE)
        collect(out[0], output, &used, 4096);
    if (mode != BLOCKED_ERROR && mode != BROKEN_ERROR && mode != ERROR_ALIAS)
        collect(err[0], errors, &error_used, 256);
    if (fcntl(out[1], F_GETFL) != out_flags || fcntl(err[1], F_GETFL) != err_flags) {
        fprintf(stderr, "Descriptor flags stdout=%d/%d stderr=%d/%d\n",
            out_flags, fcntl(out[1], F_GETFL), err_flags, fcntl(err[1], F_GETFL));
        fail("shared descriptor flags changed");
    }
    if (mode == TTY_INPUT && (tcgetattr(extra[1], &tty_after) ||
        !same_terminal(&tty_before, &tty_after)))
        fail("PTY attributes changed");
    if (mode == ACTUAL_CONSOLE && (tcgetattr(1, &tty_after) ||
        !same_terminal(&tty_before, &tty_after)))
        fail("actual console attributes changed");
    if (mode == FILE_OUTPUT) {
        if (fstat(output_file, &observed) || observed.st_size != 8192 ||
            lseek(output_file, 0, SEEK_SET) != 0)
            fail("file size limit");
        amount = read(output_file, output, sizeof(output));
        if (amount != 8192)
            fail("file output readback");
        for (used = 0; used < 8192; used++)
            if (output[used] != 'X')
                fail("file output content");
        if (unlink("../file-output"))
            fail("file output disposal");
        used = 0;
    }
    if (expected_status == -1000) {
        if (!(WIFEXITED(status) && WEXITSTATUS(status) == 4) &&
            !(WIFSIGNALED(status) && (WTERMSIG(status) == SIGKILL || WTERMSIG(status) == SIGXCPU)))
            fail("CPU resource termination");
        if (used || error_used || elapsed < 500)
            fail("CPU failure oracle");
    } else if (expected_status < 0) {
        if (expected_status == -SIGABRT && WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            if (used != 16 || memcmp(output, "confined\ndenied\n", 16))
                fail("explicit denial oracle");
        } else if (!WIFSIGNALED(status) || WTERMSIG(status) != -expected_status) {
            fail("expected signal");
        }
        if (expected_status == -SIGABRT && WIFSIGNALED(status) &&
            (used != 9 || memcmp(output, "confined\n", 9) || error_used))
            fail("fatal denial oracle");
        if (expected_status != -SIGABRT && (used || error_used))
            fail("fatal output oracle");
    } else if (!WIFEXITED(status) || WEXITSTATUS(status) != expected_status) {
        fprintf(stderr, "Observed wait status=%d binary=%s mode=%d\n", status, binary, mode);
        fail("exit oracle");
    }
    if (expected_status == 0 && mode != ACTUAL_CONSOLE &&
        (used != fixture->output_length || memcmp(output, fixture->output, used) || error_used))
        fail("complete output oracle");
    if (mode == ACTUAL_CONSOLE && error_used)
        fail("actual console diagnostic oracle");
    if (expected_status > 0 && (used ||
        (error_used && !diagnostic(errors, error_used))))
        fail("rejection output oracle");
    if ((mode == BLOCKED_OUTPUT || mode == BLOCKED_ERROR) && elapsed < 4500)
        fail("deadline returned early");
    rss = usage.ru_maxrss;
#ifdef __APPLE__
    rss = (rss + 1023) / 1024;
#endif
    if (rss < 0 || rss > 16384)
        fail("child RSS ceiling");
    if (rss > peak_kib)
        peak_kib = rss;
    if ((long long)(usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 1000000 +
        usage.ru_utime.tv_usec + usage.ru_stime.tv_usec > 2100000) {
        fprintf(stderr, "CPU ceiling observation: wait=%d wall_ms=%lld user_us=%lld system_us=%lld rss_kib=%ld\n",
            status, elapsed,
            (long long)usage.ru_utime.tv_sec * 1000000 + usage.ru_utime.tv_usec,
            (long long)usage.ru_stime.tv_sec * 1000000 + usage.ru_stime.tv_usec, rss);
        fail("CPU ceiling");
    }
    if (mode == ACTUAL_CONSOLE) {
        printf("console status=%d wall_ms=%lld\n", status, elapsed);
        printf("cpu_us=%lld rss_kib=%ld\n",
            (long long)(usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 1000000 +
            usage.ru_utime.tv_usec + usage.ru_stime.tv_usec, rss);
    } else {
        printf("case=%u pid=%ld status=%d wall_ms=%lld user_us=%ld system_us=%ld rss_kib=%ld output=%zu diagnostic=%zu\n",
            checked, (long)result, status, elapsed,
            (long)usage.ru_utime.tv_sec * 1000000 + usage.ru_utime.tv_usec,
            (long)usage.ru_stime.tv_sec * 1000000 + usage.ru_stime.tv_usec,
            rss, used, error_used);
    }
    if (mode == FIFO_INPUT && unlink("fifo"))
        fail("FIFO disposal");
    if (mode != BROKEN_OUTPUT)
        close(out[0]);
    if (mode != BROKEN_ERROR)
        close(err[0]);
    close(out[1]); close(err[1]); close(source);
    if (extra[0] >= 0) close(extra[0]);
    if (extra[1] >= 0) close(extra[1]);
    if (output_file >= 0) close(output_file);
    observe("input", fixture->data, fixture->length, &before);
    inventory();
    checked++;
}

int
main(int argc, char **argv)
{
    static const char *const binaries[] = {"bootweave", "bootweave-static"};
    static const char *const io_faults[] = {"short", "changed", "read-error", "write-zero", "write-error",
        "timer-real", "timer-cpu", "sigprof"};
    static const char *const gates[] = {"data", "stack", "nofile", "core", "fsize", "cpu", "unveil", "lock", "pledge", "ugid"};
    static const char *const denied[] = {"open", "create", "exec", "fork", "inet4", "inet6", "unveil"};
    const struct native_case empty = {native_cases[0].data, 1, 3, "", 0};
    struct native_case probe_case = native_cases[0];
    struct winsize window;
    struct stat work;
    char name[64], environment[96];
    size_t binary, i;
    struct sigaction action;
    sigset_t timer_mask;
    memset(&action, 0, sizeof(action));
    sigemptyset(&action.sa_mask);
    action.sa_handler = supervisor_deadline;
    if (sigaction(SIGALRM, &action, NULL))
        fail("supervisor timer setup");
    action.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &action, NULL))
        fail("supervisor SIGPIPE setup");
    sigemptyset(&timer_mask);
    sigaddset(&timer_mask, SIGALRM);
    if (sigprocmask(SIG_UNBLOCK, &timer_mask, NULL))
        fail("supervisor signal mask");
    alarm(910);
#ifdef __OpenBSD__
    struct passwd *account;
    const int resources[] = {RLIMIT_DATA, RLIMIT_STACK, RLIMIT_NOFILE,
        RLIMIT_CORE, RLIMIT_FSIZE, RLIMIT_CPU};
    const rlim_t ceilings[] = {67108864, 8388608, 64, 0, 8388608, 180};
    struct rlimit bounds;
    for (i = 0; i < sizeof(resources) / sizeof(resources[0]); i++) {
        bounds.rlim_cur = bounds.rlim_max = ceilings[i];
        if (setrlimit(resources[i], &bounds))
            fail("supervisor resource setup");
    }
    if (getuid() != 0 || strcmp((account = getpwnam("nobody")) == NULL ? "" : account->pw_name, "nobody") ||
        account->pw_uid != 32767 || account->pw_gid != 32767)
        fail("root/nobody prerequisite");
#endif
    if (argc != 2 || getcwd(home, sizeof(home)) == NULL || chdir("work") ||
        lstat(".", &work) || !S_ISDIR(work.st_mode) ||
        lstat("canary", &canary_stat) || lstat("../outside-canary", &outside_stat))
        fail("owned work prerequisite");
    inventory();
    suite_start = now();
    if (!strcmp(argv[1], "--corpus") || !strcmp(argv[1], "--tracecase")) {
        for (binary = 0; binary < 2; binary++)
            for (i = 0; i < NATIVE_CASE_COUNT; i++) {
                if (!strcmp(argv[1], "--tracecase") && i != NATIVE_TRACE_CASE)
                    continue;
                check(binaries[binary], &native_cases[i], NORMAL, native_cases[i].status, "plan", NULL);
            }
    } else if (!strcmp(argv[1], "--edges")) {
        for (binary = 0; binary < 2; binary++) {
            for (i = PIPE_INPUT; i <= ERROR_ALIAS; i++)
                check(binaries[binary], &native_cases[0], (enum mode)i, 3, "plan", NULL);
            check(binaries[binary], &native_cases[0], BROKEN_OUTPUT, 5, "plan", NULL);
            check(binaries[binary], &empty, BROKEN_ERROR, 5, "plan", NULL);
            check(binaries[binary], &native_cases[0], BLOCKED_OUTPUT, 4, "plan", NULL);
            check(binaries[binary], &empty, BLOCKED_ERROR, 4, "plan", NULL);
            check(binaries[binary], &native_cases[0], INTERRUPTED, -SIGTERM, "plan", NULL);
#ifdef __OpenBSD__
            probe_case.output = "Usage: bootweave plan < project.json\n"
                "       bootweave --help | --version\n"
                "Read-only unverified planner; OpenBSD only.\n";
            probe_case.output_length = strlen(probe_case.output);
            check(binaries[binary], &probe_case, NORMAL, 0, "--help", NULL);
            probe_case.output = "bootweave planner 0.1 (definition version 1)\n";
            probe_case.output_length = strlen(probe_case.output);
            check(binaries[binary], &probe_case, NORMAL, 0, "--version", NULL);
            check(binaries[binary], &native_cases[0], NORMAL, 2, NULL, NULL);
            check(binaries[binary], &native_cases[0], NORMAL, 2, "unknown", NULL);
            check(binaries[binary], &native_cases[0], INVOKE_EXTRA, 2, "plan", NULL);
            check(binaries[binary], &native_cases[0], ROOT_USER, 4, "plan", NULL);
            snprintf(name, sizeof(name), "bootweave-suid-%zu", binary);
            check(name, &native_cases[0], NORMAL, 4, "plan", NULL);
            snprintf(name, sizeof(name), "bootweave-sgid-%zu", binary);
            check(name, &native_cases[0], NORMAL, 4, "plan", NULL);
#endif
        }
    } else if (!strcmp(argv[1], "--faults")) {
        for (binary = 0; binary < 2; binary++) {
            snprintf(name, sizeof(name), "io-fault-%zu", binary);
            for (i = 0; i < sizeof(io_faults) / sizeof(io_faults[0]); i++) {
                snprintf(environment, sizeof(environment), "BW_TEST_FAULT=%s", io_faults[i]);
                check(name, &native_cases[0], NORMAL,
                    i == 0 ? 0 : i < 3 ? 3 : i < 5 ? 5 : 4, "plan", environment);
            }
            snprintf(name, sizeof(name), "gate-fault-%zu", binary);
            for (i = 0; i < sizeof(gates) / sizeof(gates[0]); i++) {
                snprintf(environment, sizeof(environment), "BW_TEST_GATE=%s", gates[i]);
                check(name, &native_cases[0], NORMAL, 4, "plan", environment);
            }
        }
    } else if (!strcmp(argv[1], "--probes")) {
        probe_case.output = ""; probe_case.output_length = 0;
        for (binary = 0; binary < 2; binary++) {
            snprintf(name, sizeof(name), "probe-%zu", binary);
            for (i = 0; i < sizeof(denied) / sizeof(denied[0]); i++)
                check(name, &probe_case, NORMAL, -SIGABRT, denied[i], NULL);
            probe_case.output = "limits verified\n"; probe_case.output_length = 16;
            check(name, &probe_case, NORMAL, 0, "limits", NULL);
            probe_case.output = ""; probe_case.output_length = 0;
            check(name, &probe_case, NORMAL, 0, "fd", NULL);
            check(name, &probe_case, NORMAL, 0, "memory", NULL);
            check(name, &probe_case, NORMAL, -SIGSEGV, "stack", NULL);
            check(name, &probe_case, NORMAL, -1000, "cpu", NULL);
            check(name, &probe_case, FILE_OUTPUT, 5, "file", NULL);
        }
    } else if (!strcmp(argv[1], "--unit")) {
        probe_case.output = ""; probe_case.output_length = 0;
        for (binary = 0; binary < 2; binary++) {
            snprintf(name, sizeof(name), "unit-%zu", binary);
            check(name, &probe_case, NORMAL, 0, NULL, NULL);
        }
    } else if (!strcmp(argv[1], "--console80-dynamic") || !strcmp(argv[1], "--console80-static") ||
        !strcmp(argv[1], "--console40-dynamic") || !strcmp(argv[1], "--console40-static")) {
        if (ioctl(1, TIOCGWINSZ, &original_window))
            fail("console window prerequisite");
        console_saved = 1;
        window = original_window;
        window.ws_col = !strncmp(argv[1], "--console80-", 12) ? 80 : 40;
        window.ws_row = 25;
        if (ioctl(1, TIOCSWINSZ, &window))
            fail("console window setup");
        binary = strstr(argv[1], "-dynamic") != NULL ? 0 : 1;
        printf("Console: %ux25 %s\n", window.ws_col, binary == 0 ? "dynamic" : "static");
        check(binaries[binary], &native_cases[0], ACTUAL_CONSOLE, 0, "plan", NULL);
        if (ioctl(1, TIOCSWINSZ, &original_window))
            fail("console window restoration");
        console_saved = 0;
    } else {
        fail("unknown harness mode");
    }
    if (!strncmp(argv[1], "--console", 9))
        puts("Console check passed; TTY restored.");
    else
        printf("Planner harness passed: %u checks; peak child RSS %ld KiB; elapsed %lld ms.\n",
            checked, peak_kib, now() - suite_start);
    if (fflush(stdout) || ferror(stdout))
        fail("supervisor receipt output");
    return 0;
}
