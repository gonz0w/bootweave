/* SPDX-License-Identifier: BSD-3-Clause */
#include "plan.h"
#ifndef __OpenBSD__
int main(void) { return 77; }
#else
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int
stack_probe(unsigned int depth)
{
    volatile unsigned char bytes[8192];
    int result;
    size_t i;
    for (i = 0; i < sizeof(bytes); i++)
        bytes[i] = (unsigned char)(depth + i);
    result = depth == 0 ? 0 : stack_probe(depth - 1);
    for (i = 0; i < sizeof(bytes); i++)
        result += bytes[i];
    return result;
}

int
main(int argc, char **argv)
{
    struct rlimit bounds;
    const int resources[] = {RLIMIT_DATA, RLIMIT_STACK, RLIMIT_NOFILE,
        RLIMIT_CORE, RLIMIT_FSIZE, RLIMIT_CPU};
    const rlim_t ceilings[] = {16777216, 2097152, 8, 0, 8192, 2};
    char *const exec_args[] = {"echo", "unsafe-exec", NULL};
    char *const environment[] = {NULL};
    char bytes[4096];
    volatile unsigned long counter = 0;
    size_t i;
    int fd, result = -1, count = 0;
    pid_t child;
    void *memory;

    if (argc != 2 || !bw_start() || !bw_confine())
        return 4;
    if (strcmp(argv[1], "limits") == 0) {
        for (i = 0; i < sizeof(resources) / sizeof(resources[0]); i++)
            if (getrlimit(resources[i], &bounds) == -1 ||
                bounds.rlim_cur != ceilings[i] || bounds.rlim_max != ceilings[i])
                return 90;
        return bw_write(1, "limits verified\n", 16) ? 0 : 5;
    }
    if (strcmp(argv[1], "fd") == 0) {
        if (fcntl(9, F_GETFD) != -1 || errno != EBADF)
            return 90;
        while ((fd = dup(0)) != -1) {
            if (++count > 5 || fd >= 8)
                return 90;
        }
        return count == 5 && errno == EMFILE ? 0 : 90;
    }
    if (strcmp(argv[1], "memory") == 0) {
        memory = malloc(17 * 1024 * 1024);
        if (memory != NULL) {
            free(memory);
            return 90;
        }
        return errno == ENOMEM ? 0 : 90;
    }
    if (strcmp(argv[1], "stack") == 0)
        return stack_probe(512) == -1 ? 0 : 90;
    if (strcmp(argv[1], "cpu") == 0)
        for (;;)
            counter++;
    if (strcmp(argv[1], "file") == 0) {
        memset(bytes, 'X', sizeof(bytes));
        for (i = 0; i < 3; i++)
            if (!bw_write(1, bytes, sizeof(bytes)))
                return 5;
        return 90;
    }
    if (!bw_write(1, "confined\n", 9))
        return 5;
    if (strcmp(argv[1], "open") == 0 || strcmp(argv[1], "create") == 0) {
        fd = strcmp(argv[1], "open") == 0 ? open("canary", O_RDONLY) :
            open("unexpected", O_CREAT | O_EXCL | O_WRONLY, 0600);
        if (fd != -1) {
            close(fd);
            return 90;
        }
    } else if (strcmp(argv[1], "exec") == 0) {
        result = execve("/bin/echo", exec_args, environment);
    } else if (strcmp(argv[1], "fork") == 0) {
        child = fork();
        if (child == 0)
            _exit(90);
        if (child > 0) {
            while (waitpid(child, &result, 0) == -1 && errno == EINTR)
                ;
            return 90;
        }
    } else if (strcmp(argv[1], "inet4") == 0 || strcmp(argv[1], "inet6") == 0) {
        fd = socket(strcmp(argv[1], "inet4") == 0 ? AF_INET : AF_INET6, SOCK_STREAM, 0);
        if (fd != -1) {
            close(fd);
            return 90;
        }
    } else if (strcmp(argv[1], "unveil") == 0) {
        result = unveil("/", "r");
        if (result == 0)
            return 90;
    } else {
        return 2;
    }
    if (errno != EACCES && errno != EPERM && errno != ENOENT)
        return 90;
    return bw_write(1, "denied\n", 7) ? 0 : 5;
}
#endif
