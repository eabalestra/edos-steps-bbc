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
    printf("Processing value: %d\n", value);
}

int main(void)
{
    printf("Consumer process started with PID %d\n", getpid());

    // Just use the same IDs created by producer
    // (no need to call semcreate again, just semwait/semsignal)
    while (true)
    {
        semwait(EMPTY);

        // Enter critical section
        semwait(MUTEX);
        int value = get_from_buffer();
        semsignal(MUTEX);

        printf("Consumer read: %d\n", value);

        // Signal that buffer is empty again
        semsignal(FULL);

        // Small delay
        for (int i = 0; i < 2000000; i++)
            ;
    }

    return 0;
}