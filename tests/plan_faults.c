/* SPDX-License-Identifier: BSD-3-Clause */
#define _POSIX_C_SOURCE 200809L
#ifdef __APPLE__
#define _DARWIN_C_SOURCE
#endif
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Linked only to a separately compiled test IO object, never production. */
static int
mode(const char *name)
{
    const char *value = getenv("BW_TEST_FAULT");
    return value != NULL && strcmp(value, name) == 0;
}

ssize_t
bw_test_read(int fd, void *buffer, size_t length)
{
    static unsigned int calls;
    calls++;
    if (mode("read-error")) {
        errno = EIO;
        return -1;
    }
    if (mode("short")) {
        if (calls == 1) {
            errno = EINTR;
            return -1;
        }
        if (length > 7)
            length = 7;
    }
    return read(fd, buffer, length);
}

ssize_t
bw_test_write(int fd, const void *buffer, size_t length)
{
    static unsigned int calls;
    calls++;
    if (fd == 1) {
        /* Output occurs only after the held input was closed. */
        errno = 0;
        if (fcntl(0, F_GETFD) != -1 || errno != EBADF)
            _exit(99);
        if (mode("write-zero"))
            return 0;
        if (mode("write-error")) {
            errno = EAGAIN;
            return -1;
        }
        if (mode("short")) {
            if (calls == 1) {
                errno = EINTR;
                return -1;
            }
            if (length > 3)
                length = 3;
        }
    }
    return write(fd, buffer, length);
}

int
bw_test_fstat(int fd, struct stat *result)
{
    static unsigned int input_calls;
    int status = fstat(fd, result);
    if (fd == 0 && status == 0 && ++input_calls == 2 && mode("changed"))
        result->st_size++;
    return status;
}
