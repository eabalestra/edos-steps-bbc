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
        semaphores[i].ref_count = 0;
        semaphores[i].used = 0;
        semaphores[i].lock = (spinlock){0};
    }
}

// Check if process has access to semaphore
bool proc_has_semaphore(struct task *task, int sem_id)
{
    for (int i = 0; i < NSEM_PROC; i++)
    {
        if (task->current_sems[i].used && task->current_sems[i].sem_id == sem_id)
        {
            return true;
        }
    }
    return false;
}

// Add process semaphore reference
int add_proc_semaphore(struct task *task, int sem_id)
{
    // Find free slot in process semaphore table
    for (int i = 0; i < NSEM_PROC; i++)
    {
        if (!task->current_sems[i].used)
        {
            task->current_sems[i].sem_id = sem_id;
            task->current_sems[i].used = true;
            return 0;
        }
    }
    return -1; // No free slots
}

// Remove process semaphore reference
void remove_proc_semaphore(struct task *task, int sem_id)
{
    for (int i = 0; i < NSEM_PROC; i++)
    {
        if (task->current_sems[i].used && task->current_sems[i].sem_id == sem_id)
        {
            task->current_sems[i].used = false;
            task->current_sems[i].sem_id = -1;
            return;
        }
    }
}

//=============================================================================
// Functions for syscalls implementation
//=============================================================================

// Allocate a new semaphore
int semcreate(int id, int init_value)
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
            return 0; // Success
        }
    }
    release(&sem_table_lock);
    return -1; // No free slots
}

// Find semaphore by ID
struct semaphore *semget(int id)
{
    acquire(&sem_table_lock);

    for (int i = 0; i < NSEM; i++)
    {
        if (semaphores[i].used && semaphores[i].id == id)
        {
            release(&sem_table_lock);
            return &semaphores[i];
        }
    }

    release(&sem_table_lock);
    return NULL;
}

// If the semaphore's value is zero, the process is supsended.
// Ohterwise, the value of the semaphore is decreased.
int semwait(int id)
{
    struct semaphore *sem = semget(id);

    acquire(&sem->lock);

    if (sem->value <= 0)
    {
        suspend(&sem->id, &sem->lock);
    }

    // TODO: posible bug (Chino)
    sem->value--;

    // PARA MI ESTO SE TIENE QUE HACER
    // struct task *task = current_task();
    // task->current_sems[task->sem_count] = &sem->id;
    // task->sem_count++;

    release(&sem->lock);

    return 0;
}

// Increments the value of the semaphore and wakes up the sleeping processes.
int semsignal(int id)
{
    struct semaphore *sem = semget(id);

    acquire(&sem->lock);

    sem->value++;

    wakeup(&sem->id);

    // PARA MI ESTO SE TIENE QUE HACER
    // struct task *task = current_task();
    // task->sem_count--;

    release(&sem->lock);

    return 0;
}

// Free a semaphore
int semclose(int id)
{
    acquire(&sem_table_lock);
    struct semaphore *sem = semget(id);
    if (sem && sem->ref_count > 0)
    {
        sem->ref_count--;
        if (sem->ref_count == 0)
        {
            sem->used = false;
            sem->id = -1;
            sem->value = 0;
        }
        release(&sem_table_lock);
        return 0; // Success
    }
    release(&sem_table_lock);
    return -1; // Semaphore not found or already freed
}
