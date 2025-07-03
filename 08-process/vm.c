#include "arch.h"
#include "klib.h"
#include "kalloc.h"

// Build the kernel page table. Called from kernel_main().
void init_vm(void)
{
    extern pte* kernel_pgtbl;
    kernel_pgtbl = (pte*) alloc_page(); // root node
    map_kernel_memory(kernel_pgtbl);
}

// map region starting at va to [pa_start, pa_end]
void map_region(pte* pgtbl, vaddr va, paddr start, uint size, uint flags)
{
    if (!page_aligned(va))
        panic("Not aligned virtual address: %x", va);
    if (!page_aligned(start))
        panic("Not aligned physical address:%x", start);

    for (paddr pa = start; pa < start + size; pa += PAGE_SIZE) {
        map_page(pgtbl, va, pa, flags);
        va += PAGE_SIZE;
    }
}

// unmap virtual address space given
void unmap_region(pte* pgtbl, vaddr va, uint size, bool free)
{
    if (!page_aligned(va))
        panic("Not aligned virtual address: %x", va);

    for (paddr a = va; a < va + size; a += PAGE_SIZE) {
        unmap_page(pgtbl, a, free);
    }
}
