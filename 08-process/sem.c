#include "sem.h"
#include "task.h"
#include "klib.h"
#include "spinlock.h"

// Global semaphore table
struct semaphore semaphores[NSEM];
spinlock sem_table_lock = 0;
struct task *currentTask = NULL;

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

    currentTask = current_task();
    int semaphoreDescriptor = findFreeSemaphoreDescriptor(currentTask);

    if (semaphoreDescriptor != -1 && freeSemaphoreSlot != -1)
    {
        sem = &(semaphores[freeSemaphoreSlot]);
        sem->id = id;
        sem->value = init_value;
        sem->used = true;
        sem->ref_count = 1;
        sem->lock = (spinlock){0};

        // TODO: AGUSTIN
        currentTask->current_sems[semaphoreDescriptor] = sem;
    }
    else
    {
        release(&sem_table_lock);
        return -1;
    }

    release(&sem_table_lock);

    return semaphoreDescriptor; // Return the descriptor (index) of the process semaphore table
}

// Returns the index of the first available slot in the task's semaphore table.
// Returns -1 if no free slot is found.
int findFreeSemaphoreDescriptor(struct task *task)
{
    for (int i = 0; i < NSEM_PROC; i++)
    {
        if (task->current_sems[i] == NULL)
        {
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

// Given the semaphore's descriptor, if the semaphore's value is zero,
// the process is supsended. Ohterwise, the value of the semaphore is decreased.
// id is the descriptor of the semaphore
int semwait(int id)
{
    currentTask = current_task();

    if (id < 0 || id >= NSEM_PROC) return -1;

    if (currentTask->current_sems[id] == NULL) {
        return -1; // Process hasn't opened the semaphore yet
    }

    struct semaphore *sem = currentTask->current_sems[id];

    acquire(&sem->lock);

    while (sem->value <= 0)
    {
        suspend(&sem->id, &sem->lock);
    }

    sem->value--;

    release(&sem->lock);
    return 0;
}

// Increments (signals) the value of a semaphore and wakes up any process
// that might be waiting on it.
// id is the descriptor of the semaphore
int semsignal(int id)
{  
    currentTask = current_task();

    if (id < 0 || id >= NSEM_PROC) return -1;

    if (currentTask->current_sems[id] == NULL) {
        return -1; // Process hasn't opened the semaphore yet
    }

    struct semaphore *sem = currentTask->current_sems[id];
    
    acquire(&sem->lock);
    sem->value++; 

    wakeup(&sem->id);

    release(&sem->lock);

    return 0;
}

int semclose(int id)
{
    return 0;
}

// Opens an existing semaphore with the given ID for the current task.
// If the task doesn't have the semaphore, it finds a free slot in the task's semaphore table,
// and adds it to it.
int semopen(int id)
{
    int sem_index = semget(id);
    if (sem_index == -1) {
        return -1;  // sempahore does not exist
    }

    if (processHasSemaphore(sem_index) == 0) {
        return -1;  // the process has already opened that semaphore
    }

    acquire(&sem_table_lock);
    struct semaphore *sem = &(semaphores[sem_index]);

    currentTask = current_task();
    int semaphoreDescriptor = findFreeSemaphoreDescriptor(currentTask);

    if (semaphoreDescriptor != -1)
    {
        sem->ref_count++;
        currentTask->current_sems[semaphoreDescriptor] = sem;
    }
    else
    {
        release(&sem_table_lock);
        return -1;
    }

    release(&sem_table_lock);

    return semaphoreDescriptor;
}


// Checks if the current process has a semaphore with index sem_index
// Se fija si el proceso tiene un semaforo en su tabla local. Siendo este semaforo
// el que se encuentra en el indice sem_index en la tabla global semaphores.
int processHasSemaphore(int sem_index)
{
    if (sem_index < 0 || sem_index >= NSEM) {
        return -1; // invalid index
    }

    currentTask = current_task();

    acquire(&sem_table_lock);
    for (int i = 0; i < NSEM_PROC; i++)
    {
        if (currentTask->current_sems[i] == &semaphores[sem_index])
        {
            release(&sem_table_lock);
            return 0;
        }
    }
    release(&sem_table_lock);
    return -1;
}