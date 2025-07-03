###############################################################################
# risc-v architecture low level hardware abstraction layer (HAL)
###############################################################################

.section .text

#==============================================================================
# CPU boot firmware loads the kernel at address 0x80000000 and jump here
# (see kernel.ld). CPU starts running in M-mode
#==============================================================================
.global boot
boot:
    # save mhartid in tp register
    csrr tp, mhartid

    # set stack pointer for each cpu
    la sp, __stack0     # sp = __stack0
    li t0, 1024*4       # t0 = 4096
    addi t1, tp, 1      # t1 = mhartid + 1
    mul t0, t0, t1      # t0 = 4096 * (mhartid + 1)
    add sp, sp, t0      # sp = sp + (4096 * (mhartid + 1))

    # Disable virtual memory for now
    csrw    satp, x0

    # Set up PMP to allow S-mode and U-mode full access to memory
    li      t5, 0x1F               # NAPOT + R/W/X permissions
    csrw    pmpcfg0, t5            # Write to pmpcfg0
    li      t6, -1                 # All ones to cover entire address space
    csrw    pmpaddr0, t6           # Write to pmpaddr0

    # Set mstatus.MPP to Supervisor Mode
    csrr    t2, mstatus            # Read mstatus into t2
    li      t3, ~(0x3 << 11)       # Mask to clear MPP bits
    and     t2, t2, t3             # Clear MPP bits
    li      t4, (0x1 << 11)        # MPP = 01 (Supervisor Mode)
    or      t2, t2, t4             # Set MPP bits to 01
    csrw    mstatus, t2            # Write back to mstatus

    # Delegate interrupts and exceptions to S-mode
    li      t5, 0xffff
    csrs    medeleg, t5
    csrs    mideleg, t5

    # enable S-mode interrupts: Set sstatus.SIE bit
    csrsi    sstatus, 0x2
    
    # enable S-mode external, timer and software interrupts
    # by setting sie.SEIE, sie.STIE and sie.SSIE bits
    li      t5, (1 << 9) | (1 << 5) | (1 << 1)
    csrs    sie, t5

    # schedule next timer interrupt: next_timer_interrupt(hartid)
    mv      a0, tp
    call    next_timer_interrupt

    # Set mtvec with m_trap address to trap timer interrupts
    la      t5, m_trap
    csrw    mtvec, t5

    # enable M-mode interrupts: mstatus.MIE and mie.MTIE
    li     t5, (1 << 3)
    csrs   mstatus, t5
    # and mie.MTIE bit
    li     t5, (1 << 7)
    csrs   mie, t5

    # Return to S-mode to "supervisor" code (defined below)
    la      t0, supervisor
    csrw    mepc, t0

    mret

#==============================================================================
# supervisor entry point (after boot mret)
#==============================================================================
supervisor:
    # set S-mode trap handler
    la      t1, s_trap
    csrw    stvec, t1

    # call kernel_main() in supervisor mode
    call kernel_main

#==============================================================================
# context_switch(struct context *current_ctx, struct context *next_ctx)
# current_ctx in a0, next_ctx in a1
#==============================================================================
.global context_switch
context_switch:
    # save CPU caller-saved registers in current task context
    sw      ra,  0(a0)
    sw      sp,  4(a0)
    sw      s0,  8(a0)
    sw      s1,  12(a0)
    sw      s2,  16(a0)
    sw      s3,  20(a0)
    sw      s4,  24(a0)
    sw      s5,  28(a0)
    sw      s6,  32(a0)
    sw      s7,  36(a0)
    sw      s8,  40(a0)
    sw      s9,  44(a0)
    sw      s10, 48(a0)
    sw      s11, 52(a0)

    # restore register values from next thread context
    lw      ra,  0(a1)
    lw      sp,  4(a1)
    lw      s0,  8(a1)
    lw      s1,  12(a1)
    lw      s2,  16(a1)
    lw      s3,  20(a1)
    lw      s4,  24(a1)
    lw      s5,  28(a1)
    lw      s6,  32(a1)
    lw      s7,  36(a1)
    lw      s8,  40(a1)
    lw      s9,  44(a1)
    lw      s10, 48(a1)
    lw      s11, 52(a1)
    ret

