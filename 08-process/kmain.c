#include "klib.h"
#include "arch.h"
#include "task.h"
#include "kalloc.h"
#include "vm.h"
#include "efs.h"

static volatile int ready = 0;

void kernel_main(void) {
    int cpu_id = cpuid();
    if (cpu_id == 0) {
        init_kalloc();
        init_vm();

        create_process("init");
        // init_external_interrupts();

        create_process("producer");
        create_process("consumer");

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
