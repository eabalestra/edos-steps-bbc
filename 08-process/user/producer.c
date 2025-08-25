#include "edoslib.h"

void add_to_buffer(int value)
{
    sys_semwait(&mutex);

    // add value to buffer

    sys_semsignal(&mutex);
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
        sem_wait(&full);
        add_to_buffer(value);
        sem_notify(&empty);
    }

    return 0;
}