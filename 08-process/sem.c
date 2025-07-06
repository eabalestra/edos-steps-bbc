#include "sem.h"
#include "task.h"
#include "klib.h"
#include "spinlock.h"

// Global semaphore table
struct semaphore semaphores[NSEM];
spinlock sem_table_lock = {0};

// Initialize semaphore system
void init_semaphores(void)
{
    for (int i = 0; i < NSEM; i++)
    {
        semaphores[i].id = -1;
        semaphores[i].value = 0;
        semaphores[i].used = 0;
        semaphores[i].lock = (spinlock){0};
    }
}

// Find semaphore by ID
struct semaphore *semget(int id)
{
    acquire(&sem_table_lock);

    for (int i = 0; i < NSEM; i++)
    {
        if (semaphores[i].used && semaphores[i].id == id)
        {
            return &semaphores[i];
        }
    }

    release(&sem_table_lock);
    return NULL;
}

// Allocate a new semaphore
struct semaphore *semcreate(int id, int init_value)
{
    acquire(&sem_table_lock);

    struct semaphore *sem = NULL;

    // Check if semaphore with this ID already exists
    if (semget(id) != NULL)
    {
        release(&sem_table_lock);
        return -1; // Semaphore already exists
    }

    // Find free slot
    for (int i = 0; i < NSEM; i++)
    {
        if (!semaphores[i].used)
        {
            sem = &(semaphores[i]);
            sem->id = id;
            sem->value = init_value;
            sem->used = 1;
            sem->ref_count = 0;
            sem->lock = (spinlock){0};
            release(&sem_table_lock);
            return sem; // Success
        }
    }
    release(&sem_table_lock);
    return sem; // No free slots
}

// If the semaphore's value is zero, the process is supsended. Ohterwise, the value of the semaphore is decreased.
int sem_wait(int id)
{
    struct semaphore *sem = semget(id);

    acquire(&sem->lock);

    if (sem->value <= 0)
    {
        suspend(&sem->id, &sem->lock);
    }

    sem->value--;
    struct task *task = current_task();
    task->currentSemaphores[task->sem_count] = &sem->id;
    task->sem_count++;

    release(&sem->lock);

    return 0;
}

// Increments the value of the semaphore and wakes up the sleeping processes.
int sem_signal(int id)
{
    struct semaphore *sem = semget(id);

    acquire(&sem->lock);

    sem->value++;

    wakeup(&sem->id);

    struct task *task = current_task();
    task->sem_count--;

    release(&sem->lock);

    return 0;
}

int sem_close(int id)
{
    struct semaphore *sem = semget(id);

    acquire(&sem->lock);

    sem->id = -1;
    sem->value = 0;
    sem->used = 0;
    sem->lock = (spinlock){0};

    deleteTasksSemaphore(id);

    release(&sem_close);
}