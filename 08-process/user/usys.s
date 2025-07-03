#==============================================================================
# syscalls functions definition. Get in sync with kernel syscall.h numbers
#==============================================================================

# int exit(exit_code)
.global exit
exit:
    li a7, 0
    ecall
    ret

# int getpid()
.global getpid
getpid:
    li a7, 1
    ecall
    ret

# int console_puts(char* c)
.global console_puts
console_puts:
    li a7, 2
    ecall
    ret

# int console_putc(char c)
.global console_putc
console_putc:
    li a7, 3
    ecall
    ret

# int console_getc(void)
.global console_getc
console_getc:
    li a7, 4
    ecall
    ret

# int sleep(int ticks)
.global sleep
sleep:
    li a7, 5
    ecall
    ret