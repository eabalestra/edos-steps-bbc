#include "klib.h"
#include "arch.h"
#include "task.h"
#include "kalloc.h"
#include "vm.h"
#include "efs.h"

static volatile int ready = 0;

#define PRODUCER_VA 0x40000000
#define CONSUMER_VA 0x20000000

// Shared buffer physical page
static uint8 *shared_buffer_page = NULL;

// Map shared buffer to a process address space
static void map_shared_buffer_to_process(struct task *task, vaddr user_va)
{
    if (!shared_buffer_page)
    {
        // Allocate shared buffer page only once
        shared_buffer_page = (uint8 *)alloc_page();
        if (!shared_buffer_page)
        {
            panic("Cannot allocate shared buffer page");
        }
        // Initialize buffer to zeros (simple int at start of page)
        memset(shared_buffer_page, 0, PAGE_SIZE);

        printf("Shared buffer allocated at physical address: %p\n", shared_buffer_page);
    }

    // Map the same physical page to both processes at the same virtual address
    map_page(task->pgtbl, user_va, (paddr)shared_buffer_page,
             PAGE_R | PAGE_W | PAGE_U);

    printf("Shared buffer mapped to process '%s' at virtual address: 0x%x\n",
           task->name, user_va);
}


void kernel_main(void)
{
    int cpu_id = cpuid();
    if (cpu_id == 0)
    {
        init_kalloc();
        init_vm();

        create_process("init");
        // init_external_interrupts();


        struct task *producerProc = create_process("producer");
        if (!producerProc) panic("failed to create producer");

        struct task *consumerProc = create_process("consumer");
        if (!consumerProc) panic("failed to create consumer");

        // Map the same physical page at DIFFERENT virtual addresses
        map_shared_buffer_to_process(producerProc, (vaddr)PRODUCER_VA);
        map_shared_buffer_to_process(consumerProc, (vaddr)CONSUMER_VA);


        __sync_synchronize();
        ready = 1;
    }
    while (!ready)
        ;
    __sync_synchronize();

    // in each CPU
    // create_process("idle");
    enable_paging();
    // init_external_irqs_in_cpu(cpu_id);
    printf("Running scheduler on CPU %d\n", cpuid());
    scheduler();

    /*
    Implementar dos procesos de usuario que usen semáforos para acceder en forma
    sincronizada a un bloque de memoria compartida. Hacer el kernel asigne y
    mapee una página compartida entre ellos
    */
}