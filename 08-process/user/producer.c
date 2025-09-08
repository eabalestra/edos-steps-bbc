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
    *SHARED_BUFFER = value;
}

int main(void)
{
    printf("Producer process started with PID %d\n", getpid());

    // Create semaphores

    int emptySem = semcreate(EMPTY, 0); // empty semaphore: counts how many slots are available for producer to write (only one slot available in total).
    int fullSem = semcreate(FULL, 1);   // full semaphore: counts how many slots are filled for consumer to read.
    int mutexSem = semcreate(MUTEX, 1); // mutex semaphore: ensures mutual exclusion when accessing the shared buffer.

   /*  printf("Semaphores created: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem);
    printf("Semaphores created: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem);
    printf("Semaphores created: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem);
    printf("Semaphores created: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem);
    printf("Semaphores created: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem);
    printf("Semaphores created: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem); */


    while (true)
    {
        int value = gen_value();

        // Enter critical section
        semwait(fullSem);

        semwait(mutexSem);
        add_to_buffer(value);
        semsignal(mutexSem);

        printf("Producer wrote: %d\n", value);
        semsignal(emptySem);

        // Small delay
        for (int i = 0; i < 2000000; i++)
            ;
    }

    return 0;
}