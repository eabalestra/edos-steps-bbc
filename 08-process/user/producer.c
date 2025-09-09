#include "edoslib.h"

#define EMPTY 1
#define FULL 2
#define MUTEX 3

// Shared buffer address (mapped differently than consumer!)
#define SHARED_BUFFER ((volatile int *)0x40000000)

static int value_counter = 0;

int gen_value(void)
{
    value_counter++;
    return value_counter;
}

void add_to_buffer(int value)
{
    // Write value directly to shared memory address
    *SHARED_BUFFER = value;
}

int main(void)
{
    int N = 20;
    printf("Producer process started with PID %d\n", getpid());    

    // Create semaphores
    int emptySem = semcreate(EMPTY, 0); // empty semaphore: counts how many slots are available for producer to write (only one slot available in total).
    int fullSem = semcreate(FULL, N);   // full semaphore: counts how many slots are filled for consumer to read.
    int mutexSem = semcreate(MUTEX, 1); // mutex semaphore: ensures mutual exclusion when accessing the shared buffer.
    
    for (int i = 0; i < N; i++)
    {
        semwait(fullSem);
        
        semwait(mutexSem);
        int value = gen_value();
        add_to_buffer(value);
        printf("Producer wrote: %d.\n", value);
        semsignal(mutexSem);

        semsignal(emptySem);
    }

    return 0;
}