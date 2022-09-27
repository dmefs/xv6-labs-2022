#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

void
xargs(char *argv[])
{
    int pid;

    if ((pid = fork()) < 0) {
        printf("xargs: fork() failed\n");
        exit(1);
    } else if (pid == 0) {
        close(0);
        exec(argv[0], argv);
    }
    wait(0);
}

int main(int argc, char *argv[])
{
    char buf[128];
    char *argvs[MAXARG];
    char c;
    char *p = buf;

    memset(argvs, 0, sizeof(argvs));

    int i;
    for (i = 1; i < argc; i++) {
        argvs[i - 1] = argv[i];
    }
    argvs[i-1] = buf;
    while (read(0, &c, sizeof(c))) {
        if (c != '\n' && p < (buf + MAXARG)) {
            *p++ = c;
        } else {
            *p = '\0';
            xargs(argvs);
            p = buf;
        }
    }
    return 0;
}
