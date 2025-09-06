#include "sem.h"
#include "task.h"
#include "klib.h"
#include "spinlock.h"

// Global semaphore table
struct semaphore semaphores[NSEM];
spinlock sem_table_lock = 0;

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

//=============================================================================
// Functions for syscalls implementation
//=============================================================================

// Returns the index of the first unused semaphore in the global semaphore table.
int findFreeSlotSemaphore()
{
    for (int i = 0; i < NSEM; i++)
    {
        if (!semaphores[i].used)
        {
            return i;
        }
    }
    return -1;
}

int semcreate(int id, int init_value)
{
    // Check if semaphore with this ID already exists
    if (semget(id) != -1)
    {
        return -1; // Semaphore already exists
    }

    acquire(&sem_table_lock);

    struct semaphore *sem = NULL;

    int freeSemaphoreSlot = findFreeSlotSemaphore();
    
    struct task *task = current_task();
    int semaphoreDescriptor = findFreeSemaphoreDescriptor(task);

    if (semaphoreDescriptor != -1 && freeSemaphoreSlot != -1) {
        sem = &(semaphores[freeSemaphoreSlot]);
        sem->id = id;
        sem->value = init_value;
        sem->used = true;
        sem->ref_count = 1;
        sem->lock = (spinlock){0};
   
        task->current_sems[semaphoreDescriptor] = &sem;
    } else {
        release(&sem_table_lock);
        return -1; 
    }

    release(&sem_table_lock);

    return semaphoreDescriptor; // Return the file descriptor (index) of the process semaphore table
}

// Returns the index of the first available slot in the task's semaphore table.
// Returns -1 if no free slot is found.
int findFreeSemaphoreDescriptor(struct task *task) {
    for (int i = 0; i < NSEM_PROC; i++) {
        if (task->current_sems[i] == NULL) {
            return i;
        }
    }
    return -1;
}

// Find semaphore by ID
// Returns the index in the semaphore table or NULL if not found
int semget(int id)
{
    acquire(&sem_table_lock);

    for (int i = 0; i < NSEM; i++)
    {
        if (semaphores[i].used && semaphores[i].id == id)
        {
            release(&sem_table_lock);
            return i; // Found
        }
    }

    release(&sem_table_lock);
    return -1; // Not found
}

int semwait(id)
{
    return 0;
}

int semclose(int id)
{
    return 0;
}

int semsignal(int id)
{
    return 0;
}
