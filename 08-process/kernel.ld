ENTRY(boot)

SECTIONS {
    . = 0x80000000;

    __kernel_start = .;
    .text :{
        KEEP(*(.text.boot));
        *(.text .text.*);
        . = ALIGN(0x1000);
    }
    __text_end = .;

    .rodata : ALIGN(4) {
        *(.rodata .rodata.*);
    }

    .data : ALIGN(4) {
        *(.data .data.*);
    }

    .bss : ALIGN(4) {
        __bss = .;
        *(.bss .bss.* .sbss .sbss.*);
        __bss_end = .;
    }

    /* initial CPU stacks (align to page boundary) */
    . = ALIGN(0x1000);
    __stack0 = .;
    . += 4 * 4096;
    __kernel_end = .;

    /* memory end physical address: 0x80000000 + 128MB */
    . = 0x80000000 + 128 * 1024 * 1024;
    __mem_end = .;
}

