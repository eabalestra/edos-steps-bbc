#include "klib.h"
#include "task.h"
#include "syscall.h"
#include "console.h"

// exit(exit_code)
int sys_exit(struct task *task)
{
    struct trap_frame *tf = task_trap_frame_address(task);
    task->killed = true;
    task->exit_code = (int) syscall_arg(tf, 0);
    return 0;
}

// getpid()
int sys_getpid(struct task *task)
{
    return task->pid;
}

// console write string
int sys_console_puts(struct task *task)
{
    struct trap_frame *tf = task_trap_frame_address(task);
    size_t str = syscall_arg(tf, 0);
    char *kaddr = (char *) va2kernel_address(task->pgtbl, str);
    printf("%s", kaddr);
    return 0;
}

// console write character
int sys_console_putc(struct task *task)
{
    struct trap_frame *tf = task_trap_frame_address(task);
    char c = syscall_arg(tf, 0);
    console_putc(c);
    return 0;
}

// console read character
int sys_console_getc(struct task *task)
{
    return console_read_char();
}

// sleep(int ticks)
int sys_sleep(struct task *task)
{
    struct trap_frame *tf = task_trap_frame_address(task);
    unsigned int n = syscall_arg(tf, 0);
    printf("pid %d going to sleep.\n", task->pid);
    sleep(n);
    return 0;
}

// struct semaphore *semcreate(int id, int init_value)
int sys_semcreate(struct task *task)
{
    struct trap_frame *tf = task_trap_frame_address(task);
    
    int id = (int) syscall_arg(tf, 0);
    int init_value = (int) syscall_arg(tf, 1);

    if (init_value < 0) {
        return -1; // Invalid initial value
    }

    
}

//=============================================================================
// syscall dispatcher
//=============================================================================

typedef int (*syscall_f)(struct task *);

// dispatching table ordered by syscall numbers
static syscall_f syscalls_table[SYSCALLS] = {
    sys_exit,
    sys_getpid,
    sys_console_puts,
    sys_console_putc,
    sys_console_getc,
    sys_sleep
};

// Syscall dispatcher. Call the syscall and store result in trap frame
void syscall(struct task *task)
{
    struct trap_frame *tf = task_trap_frame_address(task);
    uint n = syscall_number(tf);
    // printf("syscall %d from pid %d\n", n, task->pid);
    if (n < SYSCALLS && syscalls_table[n]) {
        int result = syscalls_table[n](task);
        syscall_put_result(tf, result);
    } else {
        syscall_put_result(tf, -1);
    }
}
