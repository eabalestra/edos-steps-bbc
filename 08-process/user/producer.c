#include "edoslib.h"

// Shared buffer address
#define SHARED_BUFFER ((volatile int *)0x40000000)

void add_to_buffer(int value)
{
    // Write value directly to shared memory address
    *SHARED_BUFFER = value;
}

static int value_counter = 0;
int gen_value(void)
{
    return value_counter++;
}

int main(void)
{
    printf("Producer process started with PID %d\n", getpid());

    while (true)
    {
        int value = gen_value();
        add_to_buffer(value);

        printf("Producer wrote: %d\n", value);

        // Small delay to see the output
        for (int i = 0; i < 2000000; i++)
            ;
    }

    return 0;
}