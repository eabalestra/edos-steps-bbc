#pragma once
#include "types.h"
#include "spinlock.h"

#define NSEM 16     // Maximum number of semaphores in system
#define NSEM_PROC 8 // Maximum number of semaphores per process

// Forward declaration
struct task;

// Semaphore structure
struct semaphore
{
    int id;                        // Semaphore ID
    int value;                     // Current semaphore value
    int used;                      // Is this semaphore slot in use?
    spinlock lock;                 // Lock for atomic operations
    struct wait_queue *wait_queue; // Wait queue for blocked processes
};

struct wait_queue
{
    struct task *task;
    struct wait_queue *next;
    spinlock lock;
};

// Global semaphore table
extern struct semaphore semaphores[NSEM];
extern spinlock sem_table_lock;

// Initialize semaphore system
void init_semaphores(void);

// Semaphore management functions
struct semaphore *semcreate(int id, int init_value);
struct semaphore *semget(int id);
int sem_wait(int id);
int sem_signal(int id);
int sem_close(int id);
