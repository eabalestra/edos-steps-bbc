/*============================================================================*
 * Virtual memory                                                             *
 *============================================================================*/
#pragma once

// init create kernel page table
void init_vm(void);

// map an address (contiguous) space to a given starting physical address
void map_region(pte* pgtbl, vaddr va, paddr start, uint size, uint flags);

// unmap an address (contiguous) virtual space.
// if free is true, unused page table leaves are deallocated (no root node)
void unmap_region(pte* pgtbl, vaddr va, uint size, bool free);

// copy data from user space to kernel space
void* copy_from_user(pte* pgtbl, paddr dst, vaddr src, int count);

// copy data from kernel space to user space
void* copy_to_user(pte* pgtbl, vaddr dst, paddr src, int count);
