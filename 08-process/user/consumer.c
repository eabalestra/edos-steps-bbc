#include "edoslib.h"

#define EMPTY 1
#define FULL 2
#define MUTEX 3

// Shared buffer address (mapped differently than producer!)
#define SHARED_BUFFER ((volatile int *)0x20000000)

int get_from_buffer()
{
    // Read value directly from shared memory address
    return *SHARED_BUFFER;
}

void process(int value)
{
    // Simulate processing
    printf("Processing value: %d.\n", value);
}

int main(void)
{
    printf("Consumer process started with PID %d\n", getpid());

    int emptySem = semopen(EMPTY);
    int fullSem = semopen(FULL);
    int mutexSem = semopen(MUTEX);

    // Just use the same IDs created by producer
    // (no need to call semcreate again, just semwait/semsignal)

    for (int i = 0; i < 12; i++)
    {
        semwait(emptySem);

        // Enter critical section
        semwait(mutexSem);
        int value = get_from_buffer();
        printf("Consumer read: %d.\n", value);
        semsignal(mutexSem);

        // Signal that buffer is empty again
        semsignal(fullSem);
    }

    return 0;
}