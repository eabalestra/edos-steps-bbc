#pragma once
#include "types.h"
#include "spinlock.h"

#define NSEM 16 // Maximum number of semaphores in system

// Semaphore structure
struct semaphore
{
    int id;        // Semaphore ID
    int value;     // Current semaphore value
    int ref_count; // Number of processes using this semaphore
    bool used;     // Is this semaphore slot in use?
    spinlock lock; // Lock for atomic operations
};

struct proc_sem
{
    int sem_id; // Semaphore ID
    int used;   // Is this slot used?
};

// Global semaphore table
extern struct semaphore semaphores[NSEM];
extern spinlock sem_table_lock;

// Initialize semaphore system
void init_semaphores(void);

// 
int *semcreate(int id, int init_value);
struct semaphore *semget(int id);
int sem_wait(int id);
int sem_signal(int id);
int sem_close(int id);

// Semaphore management functions
bool proc_has_semaphore(struct task *task, int sem_id);