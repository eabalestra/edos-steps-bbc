//=============================================================================
// Page allocator
//=============================================================================
#include "arch.h"
#include "klib.h"

struct link {
    struct link *next;
};

static struct link *free_list = NULL;
static unsigned int free_pages = 0;

// allocate a page.
// Return its base physical address or null if there is no a free page.
void* alloc_page(void)
{
    if (!free_list)
        return 0;
    struct link *n = free_list;
    free_list = n->next;
    memset(n, 0, PAGE_SIZE);
    return n;
}

// deallocate a page pointed by pa
void free_page(void* pa)
{
    struct link *n = (struct link*) pa;

    if (!page_aligned((address)n))
        panic("Physical address %x not aligned!", pa);

    if ((char*) pa < __kernel_end || (char*) pa > __mem_end)
        panic("Physical address %x out of RAM!", pa);

    n->next = free_list;
    free_list = n;
    free_pages++;
}

// build free pages linked list
void init_kalloc(void)
{
    printf("Initializing kernel block allocator...\n");
    for (char* p = __mem_end - PAGE_SIZE; p >= __kernel_end; p -= PAGE_SIZE) {
        free_page(p);
    }
    printf("free pages=%d, free_list=%x, kend=%x, mem_end=%x\n",
           free_pages, free_list, __kernel_end, __mem_end);
}
