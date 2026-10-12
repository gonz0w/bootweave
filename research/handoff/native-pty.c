/* SPDX-License-Identifier: BSD-3-Clause */
/* Owned native PTYs and a fixed fixture-only executable; no installer backend. */
#ifndef __OpenBSD__
#include <stdio.h>
int main(void) { puts("Native OpenBSD PTY research required."); return 77; }
#else
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
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
        !memcmp(a->c_cc,b->c_cc,sizeof(a->c_cc)) &&
        cfgetispeed(a) == cfgetispeed(b) && cfgetospeed(a) == cfgetospeed(b);
}
static int send_line(int fd, const char *s)
{
    size_t n = strlen(s);
    return write(fd,s,n) != (ssize_t)n;
}
static int run(const char *name, int rows, int cols, int action)
{
    int master=-1, slave=-1, flags, status=0, reaped=0, stage=0, failed=0, pending;
    char output[65536]; size_t used=0; ssize_t n;
    pid_t child, result;
    struct termios before, during, after;
    struct winsize size;
    struct rusage usage;
    struct pollfd fd;
    long long start=now_ms(), deadline=start+15000;
    memset(&size,0,sizeof(size)); size.ws_row=rows; size.ws_col=cols;
    memset(&usage,0,sizeof(usage)); output[0]='\0';
    if (start<0 || openpty(&master,&slave,NULL,NULL,&size) ||
        tcgetattr(slave,&before) || (flags=fcntl(slave,F_GETFL))<0) return 1;
    if (action==9) {
        before.c_lflag |= PENDIN;
        if (tcsetattr(slave,TCSANOW,&before) || tcgetattr(slave,&before)) return 1;
    }
    child=fork();
    if (child<0) { close(master); close(slave); return 1; }
    if (!child) {
        close(master);
        if (dup2(slave,0)<0 || dup2(slave,1)<0 || dup2(slave,2)<0) _exit(126);
        closefrom(3);
        if (action==10)
            execl("/root/handoff-check/driver","driver","changed",(char *)NULL);
        else if (action==15)
            execl("/root/handoff-check/driver","driver","late-input",(char *)NULL);
        else if (action==16)
            execl("/root/handoff-check/driver","driver","unavailable",(char *)NULL);
        else execl("/root/handoff-check/driver","driver",(char *)NULL);
        _exit(127);
    }
    fd.fd=master; fd.events=POLLIN;
    while (!reaped && now_ms()<deadline) {
        int ready=poll(&fd,1,50);
        if (ready<0 && errno!=EINTR) { failed=1; break; }
        if (ready>0 && (fd.revents&(POLLIN|POLLHUP))) {
            n=read(master,output+used,sizeof(output)-used-1);
            if (n>0) { used+=(size_t)n; output[used]='\0'; }
            else if (n<0 && errno!=EINTR && errno!=EIO) { failed=1; break; }
            if (used>=sizeof(output)-1) { failed=1; break; }
        }
        if (stage==0 && strstr(output,"Type next to review effects.")) {
            if (tcgetattr(slave,&during) || (during.c_lflag&ECHO)) { failed=1; break; }
            stage=1;
            if (action==0 || action==9 || action==10 || action==15 || action==16)
                failed=send_line(master,"next\n");
            else if (action==1) failed=send_line(master,"\033spoof\n");
            else if (action==2) { /* deliberate idle timeout */ }
            else if (action>=3 && action<=7) {
                int signals[]={SIGTERM,SIGINT,SIGHUP,SIGWINCH,SIGKILL};
                if (action==6) {
                    size.ws_row=12; size.ws_col=40;
                    if (ioctl(slave,TIOCSWINSZ,&size)) failed=1;
                }
                if (kill(child,signals[action-3])) failed=1;
            } else if (action==8) {
                char input[1027]; memset(input,'A',1025); input[1025]='\n'; input[1026]='\0';
                failed=send_line(master,input);
            } else if (action==11) failed=send_line(master,"next\nnext\nerase 0123456789abcdef\n");
            else if (action==12) failed=send_line(master,"\n");
            else if (action==14) { close(master); master=-1; fd.fd=-1; }
            else failed=send_line(master,"q\n");
        } else if (stage==1 && (action==0 || action==9 || action==10 ||
            action==15 || action==16) &&
            strstr(output,"Type next to confirm.")) {
            stage=2; failed=send_line(master,"next\n");
        } else if (stage==2 && strstr(output,"erase 0123456789abcdef")) {
            stage=3; failed=send_line(master,"erase 0123456789abcdef\n");
        } else if (stage==3 && action==15 && strstr(output,"FIXTURE OBSERVING")) {
            stage=4; failed=send_line(master,"q\n");
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
    while (poll(&fd,1,0)>0 && (fd.revents&POLLIN)) {
        n=read(master,output+used,sizeof(output)-used-1);
        if (n<=0) break;
        used+=(size_t)n; output[used]='\0';
        if (used>=sizeof(output)-1) { failed=1; break; }
    }
    if (!stage || memchr(output,27,used)) failed=1;
    if (action==14) {
        /* Closing the only master produces real TTY EOF/hangup. The dead
         * terminal cannot provide a restoration readback; dispose both ends. */
        if (!WIFEXITED(status) || WEXITSTATUS(status)<2 || WEXITSTATUS(status)>3)
            failed=1;
    } else if (tcgetattr(slave,&after)) failed=1;
    else if (action==7) {
        if (!WIFSIGNALED(status) || WTERMSIG(status)!=SIGKILL || same(&before,&after)) failed=1;
    } else {
        int expected=(action==0 || action==9)?0:2;
        if (!same(&before,&after) || fcntl(slave,F_GETFL)!=flags ||
            !WIFEXITED(status) || WEXITSTATUS(status)!=expected) failed=1;
    }
    if ((action==0 || action==9) != (strstr(output,"FIXTURE CONFIRMED")!=NULL)) failed=1;
    if (action==1 && strstr(output,"spoof")) failed=1;
    if (!cols && !strstr(output,"Dimensions unknown; fit unqualified.")) failed=1;
    if (action==2 && now_ms()-start<9000) failed=1;
    if (usage.ru_maxrss>32768) failed=1;
    /* Forced death cannot restore itself; the owned supervisor restores out of band. */
    if (action!=14 && (tcsetattr(slave,TCSANOW,&before) || tcflush(slave,TCIFLUSH) ||
        ioctl(slave,FIONREAD,&pending) || tcsetattr(slave,TCSANOW,&before) ||
        tcgetattr(slave,&after) || !same(&before,&after) || fcntl(slave,F_SETFL,flags))) failed=1;
    if (master>=0) close(master);
    close(slave);
    printf("%s %s rss_kib=%ld milliseconds=%lld\n",failed?"FAIL":"PASS",name,
        usage.ru_maxrss,now_ms()-start);
    return failed;
}
int main(int argc, char **argv)
{
    int failures=0;
    (void)argv;
    if (argc!=1) return 2;
    failures+=run("fresh-confirmation",24,80,0);
    failures+=run("initial-pendin",24,80,9);
    failures+=run("small-confirmation",12,40,0);
    failures+=run("unknown-confirmation",0,0,0);
    failures+=run("changed-observation",24,80,10);
    failures+=run("queued-multiline",24,80,11);
    failures+=run("empty-rejected",24,80,12);
    failures+=run("cancel",24,80,13);
    failures+=run("control-rejected",24,80,1);
    failures+=run("overflow-rejected",24,80,8);
    failures+=run("idle-timeout",24,80,2);
    failures+=run("term",24,80,3);
    failures+=run("int",24,80,4);
    failures+=run("hup",24,80,5);
    failures+=run("resize",24,80,6);
    failures+=run("forced-kill-supervisor-restore",24,80,7);
    failures+=run("lost-owned-terminal-eof",24,80,14);
    failures+=run("input-during-observation",24,80,15);
    failures+=run("unavailable-observation",24,80,16);
    return failures?1:0;
}
#endif
