#include "edoslib.h"

int main(void)
{
    printf("Welcome to EDOS userland init process with pid %d!\n", getpid());
    sleep(4);
    printf("init awake!\n");
    return 0;
}
