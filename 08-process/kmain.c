#include "klib.h"
#include "arch.h"
#include "task.h"
#include "kalloc.h"
#include "vm.h"
#include "efs.h"

static volatile int ready = 0;

// Shared buffer physical page - allocated once and mapped to both processes
static uint8 *shared_buffer_page = NULL;

// Map shared buffer to a process address space
void map_shared_buffer_to_process(struct task *task)
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
    map_page(task->pgtbl, SHARED_BUFFER_VA, (paddr)shared_buffer_page,
             PAGE_R | PAGE_W | PAGE_U);

    printf("Shared buffer mapped to process '%s' at virtual address: 0x%x\n",
           task->name, SHARED_BUFFER_VA);
}

// External function to map shared buffer
extern void map_shared_buffer_to_process(struct task *task);

void kernel_main(void)
{
    int cpu_id = cpuid();
    if (cpu_id == 0)
    {
        init_kalloc();
        init_vm();

        create_process("init");
        // init_external_interrupts();

        create_process("producer");
        create_process("consumer");

        // Map shared buffer for producer and consumer processes
        if (strcmp(path, "producer") == 0 || strcmp(path, "consumer") == 0)
        {
            map_shared_buffer_to_process(t);
        }

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
