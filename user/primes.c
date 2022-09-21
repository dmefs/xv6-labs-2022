#include "kernel/types.h"
#include "user/user.h"

void csp()
{
    int p;

    // get number from right side
    if (read(0, &p, sizeof(p))) {
        printf("prime %d\n", p);
        int fds[2];
        if (pipe(fds) < 0) {
            printf("pipe() failed in csp()\n");
            exit(1);
        }
        
        int pid;
        if ((pid = fork()) < 0) {
            printf("fork() failed\n");
            exit(1);
        } else if (pid == 0) {
            close(0);
            dup(fds[0]);
            close(fds[0]);
            close(fds[1]);
            csp();
        } else {
            close(1);
            dup(fds[1]);
            close(fds[0]);
            close(fds[1]);
            int n;
            while (read(0, &n, sizeof(n))) {
                if (n % p)
                    write(1, &n, sizeof(n));
            }
        }
    }
    close(0);
    close(1);
    wait(0);
}

int main(int argc, char const *argv[])
{
    int fds[2];
    if (pipe(fds) < 0) {
        printf("pipe() failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        printf("pid() failed\n");
        exit(1);
    } else if (pid == 0) {
        // use pseudo code https://swtch.com/~rsc/thread/
        close(0);
        dup(fds[0]);
        close(fds[0]);
        close(fds[1]);
        csp();
    } else {
        close(1);
        dup(fds[1]);
        close(fds[0]);
        close(fds[1]);
        for (int i = 2; i <= 35; i++) {
            write(1, &i, sizeof(i));
        }
        // close pipe so child process will return from read().
        close(1);
        if (wait(0) < 0) {
            fprintf(2, "wait() failed\n");
            exit(1);
        }
    }
    return 0;
}
