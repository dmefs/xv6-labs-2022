#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        printf("Use: ./sleep second.\n");
        exit(1);
    }
    if (sleep(atoi(argv[1]))) {
        printf("sleep: error.\n");
        exit(1);
    }
    return 0;
}
