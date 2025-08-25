#include "edoslib.h"

int empty, full, mutex;       // semaphore descriptors

int main(void)
{
    printf("Consumer process started with PID %d\n", getpid());

    mutex = semget(1);
    empty = semget(2);
    full  = semget(3);


    while (true) {
        sem_wait(&empty);
        int value = get_from_buffer();
        sem_signal(&full);
        process(value);
    }

    return 0;
}

int get_from_buffer() {
    sem_wait(&mutex);
    // extract item from buffer
    sem_signal(&mutex);
}


void process(int value) {
     printf("Consumer [%d] processed item: %d\n", getpid(), value);
}