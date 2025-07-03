#include "arch.h"
#include "kalloc.h"
#include "klib.h"

//=============================================================================
// External interrupt configuration functions
//=============================================================================

// Set CLINT mtimecmp register to set next timer interrupt.
// Called from m_trap in trap.s. 
void next_timer_interrupt(int cpu_id)
{
    // mtimecmp[hartid] = mtime + interval
    *(uint64*)CLINT_MTIMECMP(cpu_id) = *(uint64*)CLINT_MTIME + T_INTERVAL;
    enable_timer_interrupts();
}

// PLIC: external interrupts controller initialization
void init_external_interrupts(void)
{
    // set priority (0=disabled)
    *(uint32*)(PLIC + UART_IRQ*4) = 1;
}

// enable external interrupts in hart
void init_external_irqs_in_cpu(int cpu_id)
{
    // PLIC accept and forward UART interrupts to hart
    *(uint32*)PLIC_SENABLE(cpu_id) = (1 << UART_IRQ);
    // set hart's priority threshold
    *(uint32*)PLIC_SPRIORITY(cpu_id) = 0;
}

//=============================================================================
// Paging
//=============================================================================

// kernel page table initialized by init_vm() in vm.c
pte* kernel_pgtbl;

// set satp register pointing to the kernel page table.
void enable_paging(void)
{
    set_page_table(kernel_pgtbl);
}

// get the page table entry (pte) from a virtual address
static pte* get_pte(pte* pgtbl, vaddr va, bool create_leaf)
{
    uint i = va_index1(va);
    if ((pgtbl[i] & PAGE_V) == 0) {
        if (create_leaf) {
            // Create the non-existent leaf page table node
            paddr pa = (paddr) alloc_page();
            if (!pa)
                panic("alloc_page() failed!");
            // set the pte pointing to leaf node
            pgtbl[i] = pa2ppn(pa) | PAGE_V;
        } else
            return 0;
    }
    // get leaf node physical address
    pte* pg0 = (pte*) pte_pa(pgtbl[i]);
    // return pte address
    return pg0 ? &pg0[va_index0(va)] : 0;
}

// map the virtual address va to physical address in a page table
void map_page(pte* pgtbl, vaddr va, paddr pa, uint flags)
{
    pte* entry = get_pte(pgtbl, va, true);
    *entry = pa2ppn(pa) | flags | PAGE_V;
}

// unmap a page in a page table
// if free is true deallocate page frame
// root page table node is not deallocated
void unmap_page(pte* pgtbl, vaddr va, bool free)
{
    pte* entry = get_pte(pgtbl, va, false);
    if (entry && *entry & PAGE_V) {
        paddr pa = pte_pa(*entry);
        *entry = 0;
        if (free) {
            free_page((void *)pa);
        }
    }
}

// virtual to kernel address
paddr va2kernel_address(pte *pgtbl, vaddr va)
{
    pte *entry = get_pte(pgtbl, va, false);
    if (entry)
        return pte_pa((size_t) *entry) + va_offset(va);
    panic("PTE entry invalid!");
}

// map kernel memory layout. 1:1 mappings
void map_kernel_memory(pte* pgtbl)
{
    extern void map_region(pte*, vaddr, paddr, uint, uint);
    address ktext_start = (address) __kernel_start;
    uint    ktext_size  = (uint)(__text_end - __kernel_start);
    address kdata_start = (address) (__text_end);
    uint    kdata_size  = (uint) (__mem_end - __text_end);

    printf("ktext_start: %x\n", ktext_start);
    printf("kdata_start: %x\n", kdata_start);

    //                va           pa           size        flags
    // MMIO devices
    map_region(pgtbl, UART,        UART,        PAGE_SIZE,  PAGE_R | PAGE_W);
    map_region(pgtbl, VIRTIO0,     VIRTIO0,     PAGE_SIZE,  PAGE_R | PAGE_W);
    map_region(pgtbl, PLIC,        PLIC,        PLIC_SIZE,  PAGE_R | PAGE_W);
    // Kernel memory (all RAM)
    map_region(pgtbl, ktext_start, ktext_start, ktext_size, PAGE_R | PAGE_X);
    map_region(pgtbl, kdata_start, kdata_start, kdata_size, PAGE_R | PAGE_W);
}
