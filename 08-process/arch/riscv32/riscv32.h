/*****************************************************************************
 * RISC-V: architecture abstraction layer
 *****************************************************************************/
#pragma once

#include "types.h"

/* symbols (memory addresses) defined in linker.ld */
extern char __kernel_start[], __text_end[], __bss[], __bss_end[], __stack0[],
            __kernel_end[], __mem_end[];

// constants
#define NCPU    4   // max cpus supported

//=============================================================================
// MMIO devices
//=============================================================================

// core local interruptor (CLINT), which contains the timer.
#define CLINT                   0x02000000
// MTIME register: cycles since boot. Shared by all harts.
#define CLINT_MTIME             (CLINT + 0xBFF8)
// MTIMECMP register (one per hart)
#define CLINT_MTIMECMP(hartid)  (CLINT + 0x4000 + 8*(hartid))

// Timer interrupt interval, about 5/10th of second in QEMU
#define T_INTERVAL              5000000

// UART controller registers base address and IRQ number
#define UART                    0x10000000
#define UART_IRQ                10

// VIRTIO0 (disk) device controller registers base address
#define VIRTIO0                 0x10001000

// PLIC external interrupts device controller registers
#define PLIC                    0x0c000000
#define PLIC_SIZE               0x400000
#define PLIC_SCLAIM(hart)       (PLIC + 0x201004 + (hart)*0x2000)
#define PLIC_SENABLE(hart)      (PLIC + 0x2080   + (hart)*0x100)
#define PLIC_SPRIORITY(hart)    (PLIC + 0x201000 + (hart)*0x2000)

//=============================================================================
// Saved thread context (CPU state) in a context_switch
//=============================================================================
struct context {
    size_t ra;
    size_t sp;
    size_t s0;
    size_t s1;
    size_t s2;
    size_t s3;
    size_t s4;
    size_t s5;
    size_t s6;
    size_t s7;
    size_t s8;
    size_t s9;
    size_t s10;
    size_t s11;
} __attribute__((packed));

// defined in arch.s
void context_switch(struct context* current, struct context* next);

//=============================================================================
// Interrupts and exceptions
//=============================================================================

// handled interrupt/exception/traps cause codes 
#define ILLEGAL_INSTRUCTION     0x00000002
#define TIMER_INTERRUPT         0x80000001 // delegated from m-mode
#define SYSCALL                 0x00000008
#define EXTERNAL_INTERRUPT      0x80000009
#define LOAD_PAGE_FAULT         0x0000000d
#define LOAD_ACCESS_FAULT       0x00000005
#define STORE_ACCESS_FAULT      0x00000007
#define STORE_PAGE_FAULT        0x0000000f
#define INSTRUCTION_PAGE_FAULT  0x0000000c

// get irq number from PLIC (claim)
static inline int get_irq_number(int cpu_id)
{
    return *(int *)PLIC_SCLAIM(cpu_id);
}

// Ack interrupt to PLIC (irq complete)
static inline void irq_ack(int cpu_id, int irq)
{
    *(int *)PLIC_SCLAIM(cpu_id) = irq;
}

// defined in arch.c
// Schedule next timer interrupt, defined in arch.c
void next_timer_interrupt(int cpu_id);
void init_external_interrupts(void);
void init_external_irqs_in_cpu(int cpu_id);

// saved cpu state in stack by trap interrupts handlers (s_trap and u_trap)
struct trap_frame {
    size_t ra;      // offset 0 * 4
    size_t gp;      // offset 1 * 4
    size_t t0;      // offset 2 * 4
    size_t t1;      // offset 3 * 4
    size_t t2;      // offset 4 * 4
    size_t t3;      // offset 5 * 4
    size_t t4;      // offset 6 * 4
    size_t t5;      // offset 7 * 4
    size_t t6;      // offset 8 * 4
    size_t a0;      // offset 9 * 4
    size_t a1;      // offset 10 * 4
    size_t a2;      // offset 11 * 4
    size_t a3;      // offset 12 * 4
    size_t a4;      // offset 13 * 4
    size_t a5;      // offset 14 * 4
    size_t a6;      // offset 15 * 4
    size_t a7;      // offset 16 * 4
    size_t s0;      // offset 17 * 4
    size_t s1;      // offset 18 * 4
    size_t s2;      // offset 19 * 4
    size_t s3;      // offset 20 * 4
    size_t s4;      // offset 21 * 4
    size_t s5;      // offset 22 * 4
    size_t s6;      // offset 23 * 4
    size_t s7;      // offset 24 * 4
    size_t s8;      // offset 25 * 4
    size_t s9;      // offset 26 * 4
    size_t s10;     // offset 27 * 4
    size_t s11;     // offset 28 * 4
    size_t sp;      // offset 29 * 4
    size_t pc;      // offset 30 * 4
} __attribute__((packed));

// trapframe helper functions (used by exec and syscall)

// get syscall number from the trap frame
static inline int syscall_number(struct trap_frame *tf)
{
    return tf->a7;
}

// get syscall argument number (defined in arch.c)
static inline size_t syscall_arg(struct trap_frame *tf, int n)
{
    return *(&tf->a0 + n);
}

// put an argument in corresponding saved register in trap frame
static inline void put_argument(struct trap_frame *tf, size_t value, int n)
{
    *(&tf->a0 + n) = value;
}

// put the syscall return value in trap frame
static inline void syscall_put_result(struct trap_frame *tf, int value)
{
    tf->a0 = value;
}

//=============================================================================
// Paging
// Virtual address format: [idx1(10 bits)][idx0(10 bits)][offset(12 bits)]
//=============================================================================
#define PAGE_SIZE   4096

// page table entry (pte): [ppn(22 bits)][flags(10 bits)]
typedef uint32 pte;

// kernel page table initialized by init_vm() in vm.c
extern pte* kernel_pgtbl;

// page table entry flags
#define PAGE_V    (0x1)  // "Valid" bit (entry is enabled)
#define PAGE_R    (0x2)  // Readable
#define PAGE_W    (0x4)  // Writable
#define PAGE_X    (0x8)  // Executable
#define PAGE_U    (0x10) // User mode

// set pte flags
#define set_pte_flags(entry, flags) (*entry |= flags)

// clear pte flags
#define clear_pte_flags(entry, flags) (*entry &= ~flags)

// is a address aligned to PAGE-SIZE?
#define page_aligned(addr) ((addr & 0xfffffc00) == addr)

// get the page base address
#define page_base_addr(addr) (addr & 0xfffffc00)

// get virtual address offset
#define va_offset(va) (va & 0xfff)

// get index1 (10 higher order bits) on level 1 (root) node
#define va_index1(va) ((va >> 22) & 0x3ff)

// get index0 (10 mid order bits) on level 0 (leaf) node
#define va_index0(va) ((va >> 12) & 0x3ff)

// get physical page number from pte
#define pte_ppn(pte) (pte >> 10)

// get physical page number (ppn) from physical address
#define pa2ppn(pa) ((pa / PAGE_SIZE) << 10)

// get physical address from pte
#define pte_pa(pte) ((pte >> 10) * PAGE_SIZE)

// physical page table address to SATP register
#define pa2satp(pa) (0x80000000 | (pa >> 12))

// defined in arch.c
extern void enable_paging(void);

// map kernel memory layout
void map_kernel_memory(pte* pgtbl);

// map a page in a page table
void map_page(pte* pgtbl, vaddr va, paddr pa, uint flags);

// unmap a page in a page table
void unmap_page(pte* pgtbl, paddr va, bool free);

// virtual to kernel address
paddr va2kernel_address(pte *pgtbl, vaddr va);

//=============================================================================
// CPU get/set registers and other low level functions
//=============================================================================

// set stvec to user low level ISR u_trap
static inline void set_u_trap_handler(void)
{
    extern void u_trap(void); // defined in arch.s
    __asm__ __volatile__ ("csrw stvec, %0" : : "r" (u_trap));
}

// set stvec to supervisor (kernel) mode low level ISR u_trap
static inline void set_k_trap_handler(void)
{
    extern void s_trap(void); // defined in arch.s
    __asm__ __volatile__ ("csrw stvec, %0" : : "r" (s_trap));
}

static inline size_t r_sscratch(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, sscratch" : "=r" (v));
    return v;
}

// remember process kernel-mode stack address in sscratch
static inline void set_kstack(paddr stack_addr)
{
    __asm__ __volatile__ ("csrw sscratch, %0" : : "r" (stack_addr));
}

// get stack pointer value
static inline size_t get_sp(void)
{
    size_t v;
    __asm__ __volatile__ ("mv %0, sp" : "=r" (v));
    return v;
}

// invalid (un-implemented) instruction
static inline void invalid_instruction(void)
{
    __asm__ __volatile__("unimp");
}

// get supervisor mode trap cause (scause)
static inline size_t trap_cause(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, scause" : "=r" (v) );
    return v;
}

// get supervisor mode faulting address (stval)
static inline size_t fault_address(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, stval" : "=r" (v) );
    return v;
}

// get supervisor mode trap pc (sepc)
static inline size_t trap_pc(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, sepc" : "=r" (v) );
    return v;
}

// set supervisor mode trap pc (sepc)
static inline void set_trap_pc(size_t sepc)
{
    __asm__ __volatile__ ("csrw sepc, %0" : : "r" (sepc));
}

// get hart id. It was saved in tp register on boot in arch.s
static inline int cpuid(void)
{
    size_t v;
    __asm__ __volatile__ ("mv %0, tp" : "=r" (v) );
    return v;
}

// disable S-Mode interruputs: clear sstatus.SIE
static inline void disable_interrupts(void)
{
    __asm__ __volatile__ ("csrci sstatus, 0x2");
}

// enable S-Mode interruputs: set status.SIE
static inline void enable_interrupts(void)
{
    __asm__ __volatile__ ("csrsi sstatus, 0x2");
}

// are S-mode interrupts enabled?
static inline bool irq_status(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, sstatus" : "=r" (v) );
    return (v & 0x2) != 0;
}

// skip instruction. Called on a syscall.
static inline void skip_trap_instruction(struct trap_frame* tf)
{
    tf->pc += 4;
}

// ack timer interrupt from S-mode by clearing sip.SSIP
static inline void ack_timer_interrupt(void)
{
    __asm__ __volatile__ ("csrci sip, 2");
}

// get sstatus CSR
static inline size_t cpu_status(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, sstatus" : "=r" (v) );
    return v;
}

// set sstatus CSR
static inline void set_cpu_status(size_t status)
{
    __asm__ __volatile__ ("csrw sstatus, %0" : : "r" (status));
}

// set satp
static inline void w_satp(size_t v) {
    __asm__ __volatile__ ("csrw satp, %0" : : "r" (v) );
}

// get sie register
static inline size_t r_sie(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, sie" : "=r" (v) );
    return v;
}

// get sip register
static inline size_t r_sip(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, sip" : "=r" (v) );
    return v;
}

// enable M-mode interrupts: set mstatus.MPIE
static inline void enable_timer_interrupts(void)
{
    size_t v = 1 << 7;
    __asm__ __volatile__ ("csrs mstatus, %0" : : "r" (v));
}

// get mstatus register (only in machine mode)
static inline size_t r_mstatus(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, mstatus" : "=r" (v) );
    return v;
}

// get mie register (only in machine mode)
static inline size_t r_mie(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, mie" : "=r" (v) );
    return v;
}

// get mip register (only in machine mode)
static inline size_t r_mip(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, mip" : "=r" (v) );
    return v;
}

// setup CPU to return to u-mode from s-mode
static inline void set_u_previous_mode(void)
{
    size_t status = cpu_status();
    status &= ~(1 << 8);    // clear sstatus.SPP
    status |= (1 << 5);     // enable interrupts in user mode (sstatus.SPIE=1)
    set_cpu_status(status);
}

// set task page table in CPU
static inline void set_page_table(pte* pgtbl)
{
    __asm__ __volatile__("sfence.vma");
    w_satp(pa2satp((size_t) pgtbl));
    __asm__ __volatile__("sfence.vma");
}
