#include "edoslib.h"

// Shared buffer address
#define SHARED_BUFFER ((volatile int *)0x40000000)

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

    while (true)
    {
        int value = get_from_buffer();
        process(value);
        
        printf("Consumer read: %d\n", value);

        // Small delay to see the output
        for (int i = 0; i < 2000000; i++)
            ;
    }

    return 0;
}