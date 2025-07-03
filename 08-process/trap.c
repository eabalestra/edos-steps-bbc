#include "klib.h"
#include "arch.h"
#include "task.h"
#include "spinlock.h"
#include "console.h"

// call to device interrupt handler
void handle_device_interrupt(int cpu_id)
{
    int irq = get_irq_number(cpu_id);
    switch (irq) {
        /*
        case UART_IRQ:
            console_interrupt_handler();
            irq_ack(cpu_id, irq);
            break;
        */
        default:
            printf("Unsupported device interrupt!\n");
    }
}

//=============================================================================
// High level trap handler.
// Traps in kernel mode come here.
// CPU is using a kernel-mode stack.
// Current task can be a kernel-mode task or the scheduler thread.
//=============================================================================
void kernel_trap(void)
{
    int     cpu_id     = cpuid();
    size_t  cause      = trap_cause();
    size_t  pc         = trap_pc();
    size_t  status     = cpu_status();
    struct  task* task = current_task();
    char    *task_name = task ? task->name : "kernel";

    /*
    printf("K-trap: ticks=%d, cpu=%d, cause=%x, sepc=%x, status=%x, task=%s\n",
           get_ticks(), cpu_id, cause, pc, status, task_name);
    */
    
    switch (cause) {
        case TIMER_INTERRUPT:
            if (cpu_id == 0) {
                inc_ticks();
            }
            ack_timer_interrupt();
            if (task && --task->ticks == 0) {
                yield();
            }
            break;

        case EXTERNAL_INTERRUPT:
            handle_device_interrupt(cpu_id);
            break;

        default:
            panic("Unexpected trap in kernel mode!\n");
    }

    // yield() may have cause some traps to occur. Restore trap address and
    // cpu status to continue this kernel thread
    set_trap_pc(pc);
    set_cpu_status(status);
}

//=============================================================================
// Return to the low-level ISR u_trap_ret
//=============================================================================
void return_to_user_mode(void)
{
    extern void u_trap_ret(struct trap_frame* tf); // defined in arch.s
    struct task *task = current_task();

    disable_interrupts();
    
    // set cpu to user mode trap handler
    
    set_u_trap_handler();

    // save process kernel mode stack somewhere to use in next trap
    set_kstack((paddr)(task->kstack + PAGE_SIZE));

    // set CPU status to return to user mode
    set_u_previous_mode();
    
    /*
    uint pc = task->tf->pc;
    uint sp = task->tf->sp;
    printf("Returning to user mode: pc=%x, sp=%x, sscratch=%x\n", pc, sp, 
           r_sscratch());
    */

    // set current task page table
    set_page_table(task->pgtbl);

    // return to user mode
    u_trap_ret(task_trap_frame_address(task));
}

//=============================================================================
// Traps in user mode comes here (via u_trap in arch.s).
// Here, we are in a process context.
//=============================================================================
void user_trap(void)
{
    int     cpu_id        = cpuid();
    size_t  cause         = trap_cause();
    size_t  pc            = trap_pc();
    size_t  status        = cpu_status();
    address fault_addr    = fault_address();
    struct  task* task    = current_task();
    extern void syscall(struct task *task);

    /*
    printf("U-trap: Task %s in CPU %d, cause=%x, sepc=%x, stval=%x\n",
           task->name, cpu_id, cause, pc, fault_addr);
    */

    if (!is_process(task))
        panic("user trap: No user process!");

    // change to kernel page table
    set_page_table(kernel_pgtbl);

    // set supervisor (kernel) mode trap ISR
    set_k_trap_handler();

    switch (cause) {
        case TIMER_INTERRUPT:
            /*
printf("timer trap: ticks=%d, cpu=%d, cause=%x, sepc=%x, status=%x,task=%s\n",
get_ticks(), cpu_id, cause, pc, status, task->name);
            */
            if (cpu_id == 0) {
                inc_ticks();
            }
            ack_timer_interrupt();
            
            if (--task->ticks == 0) {
                yield();
            }
            break;

        case EXTERNAL_INTERRUPT:
            handle_device_interrupt(cpu_id);
            break;

        case SYSCALL:
            // printf("syscall %d at pc=%x\n", tf->a7, tf->pc);
            enable_interrupts();
            syscall(task);
            skip_trap_instruction(task_trap_frame_address(task));
            // printf("syscall return at at pc=%x\n", tf->pc);
            break;

        case LOAD_PAGE_FAULT:
        case LOAD_ACCESS_FAULT:
        case INSTRUCTION_PAGE_FAULT:
        case STORE_ACCESS_FAULT:
        case STORE_PAGE_FAULT:
            task->exit_code = 1; task->killed = true; 
            break;

        case ILLEGAL_INSTRUCTION:
            printf("Task %s in CPU %d illegal instruction at sepc=%x\n",
                    task->name, cpu_id, pc);
            task->exit_code = 1; task->killed = true;
            break;

        default:
            printf("user trap: ticks=%d, cpu=%d, cause=%x, sepc=%x, status=%x,task=%s\n", get_ticks(), cpu_id, cause, pc, status, task->name);
            panic("Unsupported trap\n");
    }

    if (task->killed) {
        terminate();
    }

    return_to_user_mode();
    panic("Return return_to_user_mode()");
}
