#==============================================================================
# Low-level trap handlers
#==============================================================================

#==============================================================================
# ISR for traps occurred in S-mode. Here, we are using a kernel mode stack.
# It calls to kernel_trap(struct trap_frame *tf) in trap.c
# save cpu registers in same order as defined in struct trap_frame
#==============================================================================
.global s_trap
.align 4
s_trap:
    # save CPU registers in current kernel stack
    addi    sp, sp, -29 * 4

    sw      ra, 0 * 4 (sp)
    sw      gp, 1 * 4 (sp)
    sw      t0, 2 * 4 (sp)
    sw      t1, 3 * 4 (sp)
    sw      t2, 4 * 4 (sp)
    sw      t3, 5 * 4 (sp)
    sw      t4, 6 * 4 (sp)
    sw      t5, 7 * 4 (sp)
    sw      t6, 8 * 4 (sp)
    sw      a0, 9 * 4 (sp)
    sw      a1, 10 * 4 (sp)
    sw      a2, 11 * 4 (sp)
    sw      a3, 12 * 4 (sp)
    sw      a4, 13 * 4 (sp)
    sw      a5, 14 * 4 (sp)
    sw      a6, 15 * 4 (sp)
    sw      a7, 16 * 4 (sp)
    sw      s0, 17 * 4 (sp)
    sw      s1, 18 * 4 (sp)
    sw      s2, 19 * 4 (sp)
    sw      s3, 20 * 4 (sp)
    sw      s4, 21 * 4 (sp)
    sw      s5, 22 * 4 (sp)
    sw      s6, 23 * 4 (sp)
    sw      s7, 24 * 4 (sp)
    sw      s8, 25 * 4 (sp)
    sw      s9, 26 * 4 (sp)
    sw      s10,27 * 4 (sp)
    sw      s11,28 * 4 (sp)

    call    kernel_trap

    # we can return in a different stack here (by a context switch)
    # restore registers
    lw      ra, 0 * 4 (sp)
    lw      gp, 1 * 4 (sp)
    lw      t0, 2 * 4 (sp)
    lw      t1, 3 * 4 (sp)
    lw      t2, 4 * 4 (sp)
    lw      t3, 5 * 4 (sp)
    lw      t4, 6 * 4 (sp)
    lw      t5, 7 * 4 (sp)
    lw      t6, 8 * 4 (sp)
    lw      a0, 9 * 4 (sp)
    lw      a1, 10 * 4 (sp)
    lw      a2, 11 * 4 (sp)
    lw      a3, 12 * 4 (sp)
    lw      a4, 13 * 4 (sp)
    lw      a5, 14 * 4 (sp)
    lw      a6, 15 * 4 (sp)
    lw      a7, 16 * 4 (sp)
    lw      s0, 17 * 4 (sp)
    lw      s1, 18 * 4 (sp)
    lw      s2, 19 * 4 (sp)
    lw      s3, 20 * 4 (sp)
    lw      s4, 21 * 4 (sp)
    lw      s5, 22 * 4 (sp)
    lw      s6, 23 * 4 (sp)
    lw      s7, 24 * 4 (sp)
    lw      s8, 25 * 4 (sp)
    lw      s9, 26 * 4 (sp)
    lw      s10,27 * 4 (sp)
    lw      s11,28 * 4 (sp)

    addi    sp, sp, 29 * 4

    sret

#==============================================================================
# ISR for user mode traps.
# sscratch points to process kernel mode stack, sp points to process user mode
# stack.
#==============================================================================
.global u_trap
.align 4
u_trap:
    # swap sp and sscratch
    csrrw   sp, sscratch, sp

    # now, sp points to process kernel stack, sscratch to process user stack

    # make room for the trap frame in stack (31 values of 4 bytes each)
    addi    sp, sp, -31 * 4

    # save CPU registers in process kernel mode stack
    sw      ra, 0 * 4 (sp)
    sw      gp, 1 * 4 (sp)
    sw      t0, 2 * 4 (sp)
    sw      t1, 3 * 4 (sp)
    sw      t2, 4 * 4 (sp)
    sw      t3, 5 * 4 (sp)
    sw      t4, 6 * 4 (sp)
    sw      t5, 7 * 4 (sp)
    sw      t6, 8 * 4 (sp)
    sw      a0, 9 * 4 (sp)
    sw      a1, 10 * 4 (sp)
    sw      a2, 11 * 4 (sp)
    sw      a3, 12 * 4 (sp)
    sw      a4, 13 * 4 (sp)
    sw      a5, 14 * 4 (sp)
    sw      a6, 15 * 4 (sp)
    sw      a7, 16 * 4 (sp)
    sw      s0, 17 * 4 (sp)
    sw      s1, 18 * 4 (sp)
    sw      s2, 19 * 4 (sp)
    sw      s3, 20 * 4 (sp)
    sw      s4, 21 * 4 (sp)
    sw      s5, 22 * 4 (sp)
    sw      s6, 23 * 4 (sp)
    sw      s7, 24 * 4 (sp)
    sw      s8, 25 * 4 (sp)
    sw      s9, 26 * 4 (sp)
    sw      s10,27 * 4 (sp)
    sw      s11,28 * 4 (sp)

    # save user-mode stack pointer (sp), now in sscratch
    csrr    a0, sscratch
    sw      a0, 29 * 4 (sp)     # tf->sp

    # save sepc (pc at trap point in user code)
    csrr    a0, sepc
    sw      a0, 30 * 4 (sp)     # tf->pc

    # call user_trap(). It will return to u_trap_ret.
    call    user_trap

#==============================================================================
# u_trap_ret(struct trap_frame*):
# Return to user mode. Called from return_to_user_mode().
# return_to_user_mode() set stvec=u_trap, satp=task->pgtbl,
# sscratch=task->kstack + PAGE_SIZE, sstatus.SPP=0 (U-mode) and
# sstatus.SPIE=1 (U-mode interrupts enabled).
# a0 points to trap frame.
#==============================================================================
.global u_trap_ret
.align 4
u_trap_ret:
    mv      sp, a0

    # restore sepc to return to process code: sepc <-- tf->pc
    lw      a0, 30 * 4 (sp) 
    csrw    sepc, a0

    # restore registers
    lw      ra, 0 * 4 (sp)
    lw      gp, 1 * 4 (sp)
    lw      t0, 2 * 4 (sp)
    lw      t1, 3 * 4 (sp)
    lw      t2, 4 * 4 (sp)
    lw      t3, 5 * 4 (sp)
    lw      t4, 6 * 4 (sp)
    lw      t5, 7 * 4 (sp)
    lw      t6, 8 * 4 (sp)
    lw      a0, 9 * 4 (sp)
    lw      a1, 10 * 4 (sp)
    lw      a2, 11 * 4 (sp)
    lw      a3, 12 * 4 (sp)
    lw      a4, 13 * 4 (sp)
    lw      a5, 14 * 4 (sp)
    lw      a6, 15 * 4 (sp)
    lw      a7, 16 * 4 (sp)
    lw      s0, 17 * 4 (sp)
    lw      s1, 18 * 4 (sp)
    lw      s2, 19 * 4 (sp)
    lw      s3, 20 * 4 (sp)
    lw      s4, 21 * 4 (sp)
    lw      s5, 22 * 4 (sp)
    lw      s6, 23 * 4 (sp)
    lw      s7, 24 * 4 (sp)
    lw      s8, 25 * 4 (sp)
    lw      s9, 26 * 4 (sp)
    lw      s10,27 * 4 (sp)
    lw      s11,28 * 4 (sp)
    lw      sp, 29 * 4 (sp)     # sp = tf->sp

    # return to user mode: pc <-- sepc
    sret

#==============================================================================
# machine-mode (timer) interrupts handler
# It save some used registers in current stack
#==============================================================================
.global m_trap
.align 4
m_trap:
    # save a0-a7 registers: some are used by next_timer_interrupt()
    addi    sp, sp, -8 * 4
    sw      a0, 0 * 4 (sp)
    sw      a1, 1 * 4 (sp)
    sw      a2, 2 * 4 (sp)
    sw      a3, 3 * 4 (sp)
    sw      a4, 4 * 4 (sp)
    sw      a5, 5 * 4 (sp)
    sw      a6, 6 * 4 (sp)
    sw      a7, 7 * 4 (sp)

    # save in a7 the value of ra (it will be changed in next call)
    mv      a7, ra

    # schedule the next timer interrupt: call next_timer_interrupt(cpu_id)
    mv      a0, tp
    call    next_timer_interrupt

    # restore ra
    mv      ra, a7

    # restore saved registers and put 0 in current stack to avoid filtering
    # kernel data to user process
    lw      a0, 0 * 4 (sp)
    sw      x0, 0 * 4 (sp)
    lw      a1, 1 * 4 (sp)
    sw      x0, 1 * 4 (sp)
    lw      a2, 2 * 4 (sp)
    sw      x0, 2 * 4 (sp)
    lw      a3, 3 * 4 (sp)
    sw      x0, 3 * 4 (sp)
    lw      a4, 4 * 4 (sp)
    sw      x0, 4 * 4 (sp)
    lw      a5, 5 * 4 (sp)
    sw      x0, 5 * 4 (sp)
    lw      a6, 6 * 4 (sp)
    sw      x0, 6 * 4 (sp)
    lw      a7, 7 * 4 (sp)
    sw      x0, 7 * 4 (sp)
    addi    sp, sp, 8 * 4

    # set timer interrupt pending bit in sip.
    # This will delegate interrupt to the S-mode trap handler
    li a1, 2
    csrw sip, a1

    # jump (delegate) to supervisor trap handler as a software interrupt
    # (scause = 0x80000001)
    
    mret
