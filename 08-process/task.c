#include "arch.h"
#include "klib.h"
#include "task.h"
#include "spinlock.h"
#include "kalloc.h"
#include "vm.h"
#include "efs.h"

// kernel ticks (incremented in each timer interrupt)
volatile unsigned int ticks = 0;
spinlock ticks_lock = 0;

// CPUs state
struct cpu_state cpus_state[NCPU];

// tasks/processes table
static struct task tasks[TASK_MAX];

// for task and process id assignment
static uint last_tid = 0, last_pid = 0;

// The kernel page table
extern pte* kernel_pgtbl;

// defined in trap.c
extern void return_to_user_mode(void); 

void init_tasks(void)
{
    for (int i=0; i < NCPU; i++) {
        cpus_state[i].task = NULL;
    }
    for (int i=0; i < TASK_MAX; i++) {
        tasks[i].state = UNUSED;
        tasks[i].lock = 0;
    }
}

// Kernel tasks must call it at start
void init_task(void)
{
    release(&current_task()->lock);
    enable_interrupts();
}

// return current_task in this cpu
struct task* current_task(void)
{
    return cpus_state[cpuid()].task;
}

// create task with 'pc' as initial program counter
struct task* create_task(char* name, task_function f) {
    // Find an unused tasks table slot
    struct task *task = NULL;

    for (int i = 0; i < TASK_MAX && !task; i++) {
        acquire(&tasks[i].lock);
        if (tasks[i].state == UNUSED) {
            task = &(tasks[i]);
            memset(&task->ctx, 0, sizeof(struct context));
            task->ctx.ra = (address) f;
            task->kstack = alloc_page();
            task->ctx.sp = (address) (task->kstack + PAGE_SIZE);
            task->tid    = ++last_tid;
            task->pid    = 0;
            task->cpu_id = -1;
            task->killed = false;
            task->pgtbl  = kernel_pgtbl;
            task->wait_condition = 0;
            if (strlen(name) > TASK_NAME_LEN)
                name[TASK_NAME_LEN - 1] = '\0';
            strcpy(task->name, name);
            task->state = CREATED;
        }
        release(&tasks[i].lock);
    }
    return task;
}

// free task resources. Called from scheduler.
void free_resources(struct task* t)
{
    printf("Freeing task %s resources...\n", t->name);
    free_page(t->kstack);
    t->state = UNUSED;
}

// select a new task to give the CPU
void scheduler(void)
{
    int cpu_id = cpuid();
    cpus_state[cpu_id].task = NULL;
    while (1) {
        enable_interrupts();
        for (int i=0; i<TASK_MAX; i++) {
            acquire(&tasks[i].lock);
            if (tasks[i].state == ZOMBIE) {
                free_resources(&tasks[i]);
            } else if (tasks[i].state == RUNNABLE) {
                struct task* next_task = &tasks[i];

                next_task->state = RUNNING;
                next_task->cpu_id = cpu_id;
                next_task->ticks = QUANTUM;
                cpus_state[cpu_id].task = next_task;

                // switch to next_task (continue task execution)
                context_switch(&cpus_state[cpu_id].ctx, &next_task->ctx);

                // task give up this cpu, again in scheduler thread context
                next_task->cpu_id = -1;
                cpus_state[cpu_id].task = NULL;
            }
            release(&tasks[i].lock);
        }
    }
}

// Switch context to scheduler.
// Preconditions:
// 1. current->lock aquired.
// 2. cpus_state[cpu_id].noff == 1
// 3. interrupts enabled
// 4. current->state == RUNNING
static void sched(struct task* current)
{
    int cpu_id = cpuid();
    int irq_status = cpus_state[cpu_id].irq_enabled;

    // switch to scheduler context
    context_switch(&current->ctx, &cpus_state[cpu_id].ctx);

    // later, scheduler will jump here (maybe on a different CPU).
    // Set previous irq state.
    cpus_state[cpuid()].irq_enabled = irq_status;
}

// current task leaves CPU
void yield(void)
{
    int cpu_id = cpuid();
    struct task *current_task = cpus_state[cpu_id].task;

    if (!current_task)
        panic("yield");

    // printf("yield task %s in CPU %d\n", current_task->name, cpu_id);

    acquire(&current_task->lock);
    current_task->state = RUNNABLE;

    // go to scheduler thread
    sched(current_task);

    // later, scheduler switch here (with task locked)
    release(&current_task->lock);
}

// Suspend (sleep) current task. Lock lk is related to waiting condition or
// resource.
// Invariant: cause != NULL && lk acquired
void suspend(void* condition, spinlock* lk)
{
    struct task* task = current_task();
    acquire(&task->lock);

    // release lk to avoid deadlock
    release(lk);

    task->wait_condition = condition;
    task->state = WAITING;

    // yield CPU (jump to scheduler thread)
    sched(task);

    // later, task resume execution here
    task->wait_condition = 0;
    release(&task->lock);

    // acquire lock again
    acquire(lk);
}

// Make RUNNABLE tasks waiting for condition
void wakeup(void* condition)
{
    struct task* current = current_task();
    for (int i = 0; i < TASK_MAX; i++) {
        struct task* t = &tasks[i];
        if (t != current) {
            acquire(&t->lock);
            if (t->state == WAITING && t->wait_condition == condition) {
                printf("pid %d awake!\n", t->pid);
                t->state = RUNNABLE;
            }
            release(&t->lock);
        }
    }
}

// current task sleep for n ticks
void sleep(uint n)
{
    struct task *t = current_task();
    if (!t)
        panic("sleep: No current task!");
    acquire(&ticks_lock);
    t->sleep_ticks = n;
    suspend((void*) &ticks, &ticks_lock);
    release(&ticks_lock);
}

// Wait until a given task exit. Return finished task exit_code.
int wait_for_task(struct task* t)
{
    int exit_code = 0;
    acquire(&t->lock);
    if (t->state != UNUSED && t->state != ZOMBIE) {
        suspend(t, &t->lock);
    }
    if (t->state == ZOMBIE) {
        exit_code = t->exit_code;
        t->state = UNUSED;
    }
    release(&t->lock);
    return exit_code;
}

// Set int as parent of childs of task
static void reparent(struct task *task)
{

}

// current task exits
void terminate(void)
{
    struct task *t = current_task();
    if (!t)
        panic("exit: No current task!");
    
    t->state = ZOMBIE;

    // to do: close files

    // give any children to init
    reparent(t);

    // wakeup tasks waiting for this task
    wakeup(t);

    // we'll jump to return of context_switch in scheduler from sched.
    // There task was locked.
    acquire(&t->lock);
    sched(t);

    panic("return from sched() in terminate!");
}

// get ticks value
unsigned int get_ticks(void)
{
    unsigned int result;
    acquire(&ticks_lock);
    result = ticks;
    release(&ticks_lock);
    return result;
}

// increment ticks. Called from trap() in trap.c
void inc_ticks(void)
{
    acquire(&ticks_lock);
    ticks++;
    release(&ticks_lock);
    // wakeup tasks waiting for sleep(n)
    for (int i=0; i<TASK_MAX; i++) {
        acquire(&tasks[i].lock);
        if (tasks[i].state == WAITING && tasks[i].wait_condition == &ticks) {
            if (--tasks[i].sleep_ticks == 0) {
                printf("Task pid=%d awake.\n", tasks[i].pid);
                tasks[i].state = RUNNABLE;
            }
        }
        release(&tasks[i].lock);
    }
}

// load and map program sections from the embedded file system
static bool load_program(pte* pgtbl, char *path)
{
    struct file *file = efs_file(path);
    unsigned int count = 0;
    int flags = PAGE_R | PAGE_W | PAGE_X | PAGE_U;

    if (!file || file->type != EFS_FILE_PROGRAM)
        return false;

    for (vaddr va = PROC_MIN_VA; count < file->length; va += PAGE_SIZE) {
        paddr pa = (paddr) alloc_page();
        if (!pa) {
            unmap_region(pgtbl, 0, PROC_MAX_VA, true);
            return false;
        }
        int n = min(file->length - count, PAGE_SIZE);
        memcpy((char*)pa, file->data + count, n);
        printf("mapping user code va=%x to pa=%x\n", va, pa);
        map_page(pgtbl, va, pa, flags);
        count += n;
    }
    return true;
}

// push argument values on stack and update the trap frame
// return user stack top virtual address
// User mode stack main function arguments layout:
// +---------------+ <--- sp
// |     argc      |
// +---------------+ ---+ <--- char* argv[] argument (pointer) points here
// |   argv[0](*)  |    |
// +---------------|    |
// |      ...      |    | char* argv[argc]: array of pointers to arg_i strings
// +---------------+    |
// | args[n-1](**) |    |
// +---------------+ ---+
// |     arg_0     | (*) arg0: null-terminated string
// |      ...      |
// +---------------|
// |      ...      |
// +---------------+
// |    arg_n-1    | (**) arg_n-1: null-terminated string
// |      ...      |
// +---------------+
vaddr setup_main_args(struct task *task, uint8 *sp, char* args[])
{
    struct trap_frame *tf = task_trap_frame_address(task);
    int offset = 0, argc;
    vaddr argv[50]; // arguments virtual addresses

    // push args strings and build argv[] array
    for (argc = 0; args && args[argc] && argc < 100; argc++) {
        offset -= strlen(args[argc]) + 1;
        sp -= offset;
        strcpy((char*)sp, args[argc]);
        // we need to set virtual address in argv[]
        argv[argc] = PROC_MAX_VA - offset;
    }

    // push char* argv[] array
    for (int i=0; i<argc; i++) {
        offset -= sizeof(char*);
        sp -= offset;
        *((char*)sp) = argv[i];
    }

    // set arguments argc and argv in trap frame
    put_argument(tf, argc, 0);
    put_argument(tf, PROC_MAX_VA - offset, 1);

    // return virtual address of top of user mode stack
    return PROC_MAX_VA - offset;
}

// a new process start here from scheduler
static void start_process(void)
{
    struct task *task = current_task();
    // switch context from scheduler holds task lock
    release(&task->lock);
    return_to_user_mode();
}

// exec(task, program)
// replace image memory of task by loading program sections
bool exec(struct task* task, char* path, char* args[])
{
    struct trap_frame *tf = task_trap_frame_address(task);

    // allocate for a new page table
    pte* new_pgtbl = (pte*) alloc_page();
    if (!new_pgtbl)
        goto error;

    // copy kernel page table
    memcpy(new_pgtbl, kernel_pgtbl, PAGE_SIZE);
    
    // allocate, map and load text and data from program
    if (!load_program(new_pgtbl, path))
        goto error;
    
    // allocate and map the user mode stack at end of process address space
    uint8 *ustack = (uint8 *) alloc_page();
    if (!ustack)
        goto error;
    map_page(new_pgtbl, PROC_MAX_VA - PAGE_SIZE, (paddr) ustack,
             PAGE_R | PAGE_W | PAGE_U);

    // push command line arguments for main(int argc, char* argv[])
    vaddr sp = setup_main_args(task, ustack + PAGE_SIZE, args);

    // setup the trap frame for returning to user mode
    tf->sp = sp; // set user stack top virtual address
    tf->pc = 0;  // set program entry point

    // if current task is a process, free old memory
    if (is_process(task)) {
        unmap_region(task->pgtbl, 0, PROC_MAX_VA, true);
        free_page(task->pgtbl);
    }
    task->pgtbl = new_pgtbl;

    // set context to use the kernel mode stack
    task->ctx.sp = (size_t) tf;
    
    return sp;

error:
    free_page(new_pgtbl);
    return false;
}

// create and setup a new process (user mode task)
bool create_process(char *path)
{
    struct task* t = create_task(path, start_process);

    if (!t)
        panic("Create process: No tasks free slot!");
    
    t->pid = ++last_pid;

    if (!exec(t, path, 0)) {
        release(&t->lock);
        return false;
    }

    t->state = RUNNABLE;

    return true;
}