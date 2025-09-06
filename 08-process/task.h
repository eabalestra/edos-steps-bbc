#pragma once
#include "sem.h"
#include "arch.h"
#include "spinlock.h"

#define QUANTUM         2
#define TASK_NAME_LEN  81
#define TASK_MAX       16    // Maximum number of tasks

// task state
#define UNUSED     0    // Unused task/process control structure
#define CREATED    1    // Task created, not yet RUNNABLE
#define RUNNABLE   2    // Runnable task
#define RUNNING    3    // CPU is running it
#define WAITING    4    // task is in some waiting queue
#define TERMINATED 5    // finished task (but yet exist in tasks table)
#define ZOMBIE     6    // finished tasks that stay in tasks table yet

// Process virtual address space. User mode stack is mapped to PROC_MAX_VA
#define PROC_MIN_VA   (0)
#define PROC_MAX_VA   (0x80000000)

#define NSEM_PROC 8 // Maximum number of semaphores per process


// task/process control block
struct task {
    uint                tid;                 // Task id
    uint                pid;                 // Process id
    char                name[TASK_NAME_LEN]; // task name
    int                 state;               // Task state
    struct task         *parent;             // Parent task (null for init)
    bool                killed;              // Task was killed?
    int                 exit_code;           // exit code for terminated tasks
    struct context      ctx;                 // Saved context
    int                 ticks;               // last cpu burst
    uint8*              kstack;              // Kernel-mode stack
    uint64              sleep_ticks;         // Waiting for ticks
    int                 cpu_id;              // CPU in which this task is running
    void*               wait_condition;      // Waiting condition
    pte*                pgtbl;               // Page table
    spinlock            lock;                // lock for task
    struct semaphore*   current_sems[NSEM_PROC];  // current semaphores used by this process
};

// CPU state
struct cpu_state {
    int    noff;        // disable_interrupts() nested calls
    bool   irq_enabled; // interrupts enabled/disabled
    struct task* task;  // current task
    struct context ctx; // saved main thread (scheduler) context
};

// export cpus_state (for spinlock.c and others)
extern struct cpu_state cpus_state[NCPU];

// get and increment ticks value
unsigned int get_ticks(void);
void         inc_ticks(void);

// defined in task.c
extern volatile unsigned int ticks;
extern spinlock ticks_lock;

// initialize tasks descriptors table
void init_tasks(void);

// task is running in this CPU?
#define running_in_cpu(cpu, task)   \
    (task && task->state == RUNNING && task == cpus_state[cpu].task)

// Is task t a process?
#define is_process(t) (t && t->pid > 0 && t->pgtbl != kernel_pgtbl)

// get task trap frame
#define task_trap_frame_address(t)      \
    (struct trap_frame *)(t->kstack + PAGE_SIZE - sizeof(struct trap_frame))

// type task_function
typedef void (*task_function)(void);

// create a RUNNABLE task
struct task* create_task(char* name, task_function pc);

// get current task. If kernel is in scheduler context, it return NULL.
struct task* current_task(void);

// current task yield CPU by doing a contexto switch to scheduler.
void         yield(void);

// select next task to run and do context switch.
void         scheduler(void);

// sleep for a number of ticks
void         sleep(uint ticks);

// Suspend current task for given condition
void         suspend(void* condition, spinlock* lk);

// Make RUNNABLE all tasks waiting for condition
void         wakeup(void* condition);

// Wait until a given task exit. Return finished task exit_code.
int          wait_for_task(struct task* t);

// Current task end. Just mark it as killed.
void         terminate(void);

// Exit current task.
void exit(void);

// create a process (user task)
struct task* create_process(char *path);

// exec program in task t
int exec(struct task* t, char* path, char* args[]);
