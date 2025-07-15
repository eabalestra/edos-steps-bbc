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

# int semcreate(int id, int init_value)
.global semcreate
semcreate:
    li a7, 6
    ecall
    ret

# int semget(int id)
.global semget
semget:
    li a7, 7
    ecall
    ret

# int semwait(int id)
.global semwait
semwait:
    li a7, 8
    ecall
    ret

# int semsignal(int id)
.global semsignal
semsignal:
    li a7, 9
    ecall
    ret

# int semclose(int id)
.global semclose
semclose:
    li a7, 10
    ecall
    ret
