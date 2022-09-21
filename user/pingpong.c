#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
    int fds[2];
    if (pipe(fds) < 0) {
        printf("pipe() failed\n");
        exit(1);
    }
    int pid = fork();
    int buf;

    if (pid < 0) {
        printf("fork() failed\n");
        exit(1);
    } else if (pid == 0) {
        read(fds[1], &buf, 1);
        printf("%d: received ping\n", getpid());
        write(fds[0], &buf, 1);
    } else {
        write(fds[0], &buf, 1);
        wait(0);
        read(fds[1], &buf, 1);
        printf("%d: received pong\n", getpid());
    }
    close(fds[0]);
    close(fds[1]);
    return 0;
}
