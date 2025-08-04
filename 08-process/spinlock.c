// spinlock.c: extracted/adapted from xv6.
#include "spinlock.h"
#include "arch.h"
#include "task.h"

// Disable interrupts and count nested calls
void push_irq_off(void)
{
    bool old = irq_enabled();
    int cpu_id = cpuid();

    disable_interrupts();
    if (cpus_state[cpu_id].noff == 0) {
        cpus_state[cpu_id].irq_enabled = old;
    }
    cpus_state[cpu_id].noff++;
}

void pop_irq_off(void)
{
    int cpu_id = cpuid();
    if (--cpus_state[cpu_id].noff == 0 && cpus_state[cpu_id].irq_enabled) {
        enable_interrupts();
    }
}

void acquire(spinlock *lk)
{
    // disable interrupts to avoid deadlock.
    push_irq_off();

    // On RISC-V, sync_lock_test_and_set turns into an atomic swap:
    //   a5 = 1
    //   s1 = &lk->locked
    //   amoswap.w.aq a5, a5, (s1)
    while(__sync_lock_test_and_set(lk, 1) != 0)
        ;

    // Tell the C compiler and the processor to not move loads or stores
    // past this point, to ensure that the critical section's memory
    // references happen strictly after the lock is acquired.
    // On RISC-V, this emits a fence instruction.
    __sync_synchronize();
}

void release(spinlock *lk)
{
    // Tell the C compiler and the CPU to not move loads or stores
    // past this point, to ensure that all the stores in the critical
    // section are visible to other CPUs before the lock is released,
    // and that loads in the critical section occur strictly before
    // the lock is released.
    // On RISC-V, this emits a fence instruction.
    __sync_synchronize();

    // Release the lock, equivalent to lk->locked = 0.
    // This code doesn't use a C assignment, since the C standard
    // implies that an assignment might be implemented with
    // multiple store instructions.
    // On RISC-V, sync_lock_release turns into an atomic swap:
    //   s1 = &lk->locked
    //   amoswap.w zero, zero, (s1)
    __sync_lock_release(lk);

    pop_irq_off();
}
