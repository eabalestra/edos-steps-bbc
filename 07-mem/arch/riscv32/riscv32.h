/*****************************************************************************
 * RISC-V: architecture abstraction layer
 *****************************************************************************/
#pragma once

#include "types.h"

/* symbols (memory addresses) defined in linker.ld */
extern char __kernel_start[], __text_end[], __bss[], __bss_end[], __stack0[],
            __kernel_end[], __mem_end[];

// constants
#define NCPU        4       // max cpus

// get mmio register address (byte)
#define mmio_reg_b(r) ((volatile uint8 *)(r))

// handled interrupt/exception/traps cause codes 
#define ILLEGAL_INSTRUCTION 0x00000002
#define TIMER_INTERRUPT     0x80000001
#define SYSCALL             0x0000000b
#define LOAD_PAGE_FAULT     0x0000000d
#define LOAD_ACCESS_FAULT   0x00000005
#define STORE_ACCESS_FAULT  0x00000007
#define STORE_PAGE_FAULT    0x0000000F

// core local interruptor (CLINT), which contains the timer.
#define CLINT 0x02000000L
// MTIME register: cycles since boot. Shared by all harts.
#define CLINT_MTIME (CLINT + 0xBFF8)
// MTIMECMP register (one per hart)
#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + 8*(hartid))

// UART controller registers
#define UART        0x10000000

// VIRTIO0 (disk) device controller registers
#define VIRTIO0 0x10001000

// PLIC device controller registers
#define PLIC      0x0c000000
#define PLIC_SIZE 0x400000

#define T_INTERVAL 5000000  // about 5/10th of second in QEMU

// Saved thread context (state before a context switch)
struct context {
    size_t ra;  // return address
    size_t sp;  // stack pointer
    size_t s0;  // RISC-V caller saved registers
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

// Schedule next timer interrupt, defined in arch.c
void next_timer_interrupt(int cpu_id);

// defined in arch.s
void context_switch(struct context* current, struct context* next);

// Layout of saved cpu state on stack on a trap.
// It should be in sync with low level trap handler (s_trap) in arch.s.
struct trap_frame {
    size_t ra;
    size_t gp;
    size_t t0;
    size_t t1;
    size_t t2;
    size_t t3;
    size_t t4;
    size_t t5;
    size_t t6;
    size_t a0;
    size_t a1;
    size_t a2;
    size_t a3;
    size_t a4;
    size_t a5;
    size_t a6;
    size_t a7;
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
    size_t sp;
} __attribute__((packed));

//=============================================================================
// Paging: RISCV32 uses two level page tables (page size of 4KB).
// Each node is an array of 1024 entries (ptes).
// Each pte is a 32 bits value: pte.ppn (22 bits) and pte.flags (12 bits).
// Virtual address (va) format: idx1: 10 bits, idx0: 10 bits, offset: 12 bits.
//=============================================================================
#define PAGE_SIZE   4096

// page table entry (pte). pte.ppn: 22 bits, pte.flags: 10 bits
typedef uint32 pte;

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

// get the page base address (round down)
#define page_base_addr(addr) (addr & 0xfffff000)

// is a address aligned to page size?
#define page_aligned(addr) (page_base_addr(addr) == addr)

// get virtual address offset (12 low order bits)
#define va_offset(va) (va & 0xfff)

// get index1 (10 higher order bits) on level 1 (root) node
#define va_index1(va) ((va >> 22) & 0x3ff)

// get index0 (10 mid order bits) on level 0 (leaf) node
#define va_index0(va) ((va >> 12) & 0x3ff)

// get physical page number from pte
#define pte_ppn(pte) ((pte >> 10) & 0x3ff)

// get physical page number (ppn) from physical address
#define pa2ppn(pa) ((pa / PAGE_SIZE) << 10)

// get physical address from pte
#define pte_pa(pte) ((pte >> 10) * PAGE_SIZE)

// kernel page table initialized by init_vm() in vm.c
extern pte* kernel_pgtbl;

// defined in arch.c
extern void enable_paging(void);

// map kernel memory layout
void map_kernel_memory(pte* pgtbl);

// map a page in a page table
void map_page(pte* pgtbl, vaddr va, paddr pa, uint flags);

// unmap a page in a page table
void unmap_page(pte* pgtbl, vaddr va, bool free);

//=============================================================================
// CPU registers low level functions
//=============================================================================

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
static inline bool irq_enabled(void)
{
    size_t v;
    __asm__ __volatile__ ("csrr %0, sstatus" : "=r" (v) );
    return (v & 0x2) != 0;
}

// skip instruction. Called on a recovered exception or syscall.
static inline void skip_instruction(struct trap_frame* tf)
{
    tf->ra += 4;
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
