/* SPDX-License-Identifier: BSD-3-Clause */
/* Owned-PTY research supervisor; OpenBSD base libc/libutil only. */
#ifndef __OpenBSD__
#include <stdio.h>
int main(void) { fputs("OpenBSD native PTY checks required.\n", stderr); return 77; }
#else
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <util.h>

static long long now_ms(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t)) return -1;
    return (long long)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
static int same(const struct termios *a, const struct termios *b)
{
    return a->c_iflag == b->c_iflag && a->c_oflag == b->c_oflag &&
        a->c_cflag == b->c_cflag && a->c_lflag == b->c_lflag &&
        !memcmp(a->c_cc, b->c_cc, sizeof(a->c_cc)) &&
        cfgetispeed(a) == cfgetispeed(b) && cfgetospeed(a) == cfgetospeed(b);
}
static int run(const char *binary, const char *name, int rows, int cols, int action)
{
    int master=-1, slave=-1, p[2], status=0, reaped=0, sent=0, failed=0, pending;
    char output[65536]; size_t used=0; ssize_t n;
    pid_t child, result;
    struct termios before, during, after;
    struct winsize size;
    struct rusage usage;
    struct pollfd fd;
    long long start=now_ms(), deadline=start+15000;
    memset(&size,0,sizeof(size)); size.ws_row=rows; size.ws_col=cols;
    memset(&usage,0,sizeof(usage));
    if (start<0 || openpty(&master,&slave,NULL,NULL,&size) ||
        tcgetattr(slave,&before) || pipe(p)) return 1;
    if (action==9) {
        before.c_lflag|=PENDIN;
        if (tcsetattr(slave,TCSANOW,&before) || tcgetattr(slave,&before)) return 1;
        action=0;
    }
    child=fork();
    if (child<0) { close(master); close(slave); close(p[0]); close(p[1]); return 1; }
    if (!child) {
        close(master); close(p[0]);
        if (dup2(slave,STDIN_FILENO)<0 || dup2(p[1],STDOUT_FILENO)<0 ||
            dup2(p[1],STDERR_FILENO)<0) _exit(126);
        close(slave); close(p[1]); execl(binary,binary,(char *)NULL); _exit(127);
    }
    close(p[1]); fd.fd=p[0]; fd.events=POLLIN;
    output[0]='\0';
    while (!reaped && now_ms()<deadline) {
        int ready=poll(&fd,1,50);
        if (ready<0 && errno!=EINTR) { failed=1; break; }
        if (ready>0 && (fd.revents&(POLLIN|POLLHUP))) {
            n=read(p[0],output+used,sizeof(output)-used-1);
            if (n>0) { used+=(size_t)n; output[used]='\0'; }
            else if (n<0 && errno!=EINTR) { failed=1; break; }
            if (used>=sizeof(output)-1) { failed=1; break; }
        }
        if (!sent && strstr(output,"Label (ASCII")) {
            if (tcgetattr(slave,&during) || (during.c_lflag&ECHO)) { failed=1; break; }
            sent=1;
            if (action==0) {
                const char input[]="Example\nalpha\ncomplete\n";
                if (write(master,input,sizeof(input)-1)!=(ssize_t)sizeof(input)-1) failed=1;
            } else if (action==1) {
                const char input[]="\033spoof\nq\n";
                if (write(master,input,sizeof(input)-1)!=(ssize_t)sizeof(input)-1) failed=1;
            } else if (action==8) {
                char input[1029];
                memset(input,'A',1025); memcpy(input+1025,"\nq\n",3);
                if (write(master,input,1028)!=1028) failed=1;
            } else if (action>=3) {
                int signals[]={SIGTERM,SIGINT,SIGHUP,SIGWINCH,SIGKILL};
                if (action==6) {
                    size.ws_row=12; size.ws_col=40;
                    if (ioctl(slave,TIOCSWINSZ,&size)) failed=1;
                }
                if (kill(child,signals[action-3])) failed=1;
            }
        }
        result=wait4(child,&status,WNOHANG,&usage);
        if (result==child) reaped=1;
        else if (result<0 && errno!=EINTR) { failed=1; break; }
        if (failed) break;
    }
    if (!reaped) {
        failed=1; kill(child,SIGKILL);
        while (wait4(child,&status,0,&usage)<0 && errno==EINTR) {}
    }
    /* Child exit closes pipe; drain remaining bytes within the fixed buffer. */
    while ((n=read(p[0],output+used,sizeof(output)-used-1))>0) {
        used+=(size_t)n; output[used]='\0';
        if (used>=sizeof(output)-1) { failed=1; break; }
    }
    if (tcgetattr(slave,&after) || !sent || memchr(output,27,used)) failed=1;
    if (action==7) {
        if (!WIFSIGNALED(status) || WTERMSIG(status)!=SIGKILL || same(&before,&after)) failed=1;
        if (tcsetattr(slave,TCSANOW,&before) || tcflush(slave,TCIFLUSH) ||
            ioctl(slave,FIONREAD,&pending) ||
            tcsetattr(slave,TCSANOW,&before) ||
            tcgetattr(slave,&after) || !same(&before,&after)) failed=1;
    } else {
        if (!same(&before,&after) || !WIFEXITED(status) ||
            WEXITSTATUS(status)!=(action==0?0:2)) failed=1;
    }
    if (action==0) {
        if (!strstr(output,"SIMULATION COMPLETE") || !strstr(output,"Example") ||
            !strstr(output,"alpha")) failed=1;
    } else if (strstr(output,"SIMULATION COMPLETE")) failed=1;
    if ((action==1 || action==8) && !strstr(output,"Input rejected.")) failed=1;
    if (action==1 && strstr(output,"spoof")) failed=1;
    if (cols==0 && !strstr(output,"Dimensions unknown; fit unqualified.")) failed=1;
    if (action==2 && now_ms()-start<9000) failed=1;
    if (usage.ru_maxrss>32768) failed=1;
    if (failed) {
        printf("diagnostic %s status=%d reaped=%d sent=%d same=%d "
            "before_lflag=%lu after_lflag=%lu errno=%d\n",name,status,reaped,sent,
            same(&before,&after),(unsigned long)before.c_lflag,
            (unsigned long)after.c_lflag,errno);
    }
    /* Always restore the owned PTY out of band, including failed cases. */
    if (tcsetattr(slave,TCSANOW,&before) || tcflush(slave,TCIFLUSH) ||
        ioctl(slave,FIONREAD,&pending) || tcsetattr(slave,TCSANOW,&before)) failed=1;
    close(p[0]); close(master); close(slave);
    printf("%s %s rows=%d cols=%d rss_kib=%ld milliseconds=%lld\n",
        failed?"FAIL":"PASS",name,rows,cols,usage.ru_maxrss,now_ms()-start);
    return failed;
}
int main(int argc, char **argv)
{
    int failures=0;
    if (argc!=2 || argv[1][0]!='/') return 1;
    failures+=run(argv[1],"complete",24,80,0);
    failures+=run(argv[1],"initial-pendin-complete",24,80,9);
    failures+=run(argv[1],"small-complete",12,40,0);
    failures+=run(argv[1],"unknown-complete",0,0,0);
    failures+=run(argv[1],"overflow-cancel",24,80,8);
    failures+=run(argv[1],"control-cancel",24,80,1);
    failures+=run(argv[1],"idle-timeout",24,80,2);
    failures+=run(argv[1],"term",24,80,3);
    failures+=run(argv[1],"int",24,80,4);
    failures+=run(argv[1],"hup",24,80,5);
    failures+=run(argv[1],"resize",24,80,6);
    failures+=run(argv[1],"forced-kill-supervisor-restore",24,80,7);
    return failures?1:0;
}
#endif
