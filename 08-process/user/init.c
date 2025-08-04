#include "edoslib.h"

char* msg = "init awake!\n";

int main(void)
{
    printf("Welcome to EDOS userland init process with pid %d!\n", getpid());
    printf("Init: msg va=%x\n", msg);
    sleep(4);
    console_puts(msg);
    return 0;
}
