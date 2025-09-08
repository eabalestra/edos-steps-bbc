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

    int fullSem = semopen(FULL);
    int emptySem = semopen(EMPTY);
    int mutexSem = semopen(MUTEX);

    printf("Semaphores open in CONSUMER: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem);
    printf("Semaphores open in CONSUMER: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem);
    printf("Semaphores open in CONSUMER: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem);
    printf("Semaphores open in CONSUMER: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem);
    printf("Semaphores open in CONSUMER: EMPTY=%d, FULL=%d, MUTEX=%d\n", emptySem, fullSem, mutexSem);

    // Just use the same IDs created by producer
    // (no need to call semcreate again, just semwait/semsignal)
    while (true)
    {
        semwait(emptySem);
        
        printf("ESTO SE DEBERIA ejecutarrrrrrrrrr\n");
        printf("ESTO SE DEBERIA ejecutarrrrrrrrrr\n");
        printf("ESTO SE DEBERIA ejecutarrrrrrrrrr\n");
        printf("ESTO SE DEBERIA ejecutarrrrrrrrrr\n");
        // Enter critical section
        semwait(mutexSem);
        int value = get_from_buffer();
        semsignal(mutexSem);

        printf("Consumer read: %d\n", value);
        printf("Consumer read: %d\n", value);
        printf("Consumer read: %d\n", value);
        printf("Consumer read: %d\n", value);

        // Signal that buffer is empty again
        semsignal(fullSem);

        // Small delay
        for (int i = 0; i < 2000000; i++)
            ;
    }

    return 0;
}