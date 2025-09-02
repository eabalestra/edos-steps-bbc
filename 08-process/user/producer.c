#include "edoslib.h"

#define EMPTY 1
#define FULL 2
#define MUTEX 3

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
    semwait(MUTEX);
    *SHARED_BUFFER = value;
    semsignal(MUTEX);
}

int main(void)
{
    printf("Producer process started with PID %d\n", getpid());

    // Create semaphores
    semcreate(EMPTY, 1); // empty semaphore: counts how many slots are available for producer to write (only one slot available in total).
    semcreate(FULL, 0);  // full semaphore: counts how many slots are filled for consumer to read.
    semcreate(MUTEX, 1); // mutex semaphore: ensures mutual exclusion when accessing the shared buffer.

    while (true)
    {
        int value = gen_value();

        // Enter critical section
        semwait(FULL);
        add_to_buffer(value);
        printf("Producer wrote: %d\n", value);
        semsignal(EMPTY);

        // Small delay
        for (int i = 0; i < 2000000; i++)
            ;
    }

    return 0;
}