#pragma once
#include "types.h"
#include "spinlock.h"

#define NSEM 16 // Maximum number of semaphores in system

// Forward declaration to avoid circular dependency
struct task;

// Semaphore structure
struct semaphore
{
    int id;        // Semaphore ID
    int value;     // Current semaphore value
    int ref_count; // Number of processes using this semaphore
    bool used;     // Is this semaphore slot in use?
    spinlock lock; // Lock for atomic operations
};

// Global semaphore table
extern struct semaphore semaphores[NSEM];
extern spinlock sem_table_lock;

// Initialize semaphore system
void init_semaphores(void);

// Semaphore management functions
bool proc_has_semaphore(struct task *task, int sem_id);
void remove_proc_semaphore(struct task *task, int sem_id);
int add_proc_semaphore(struct task *task, int sem_id);

// System call interface for semaphore operations
int semcreate(int id, int init_value);
int semget(int id);
int semwait(int id);
int semsignal(int id);
int semclose(int id);