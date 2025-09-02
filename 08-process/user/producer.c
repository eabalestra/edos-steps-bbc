#include "edoslib.h"

// Shared buffer address (mapped differently than consumer!)
#define SHARED_BUFFER ((volatile int *)0x40000000)

static int value_counter = 0;

int gen_value(void)
{
    return value_counter++;
}

void add_to_buffer(int value)
{
    // Write value directly to shared memory address
    *SHARED_BUFFER = value;
}

int main(void)
{
    printf("Producer process started with PID %d\n", getpid());

    // Create semaphores
    semcreate(1, 1);   // empty semaphore: counts how many slots are available for producer to write (only one slot available in total).
    semcreate(2,0);   // full semaphore: counts how many slots are filled for consumer to read.
    semcreate(3,1);   // mutex semaphore: ensures mutual exclusion when accessing the shared buffer.

    while (true)
    {
        int value = gen_value();

        // Wait until buffer has space
        semwait(1);   // empty

        // Enter critical section
        semwait(3);   // mutex
        add_to_buffer(value);
        semsignal(3); // mutex

        printf("Producer wrote: %d\n", value);

        // Signal that buffer is full
        semsignal(2); // full

        // Small delay
        for (int i = 0; i < 2000000; i++);
    }

    return 0;
}