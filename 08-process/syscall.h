#pragma once

#define SYS_EXIT                0
#define SYS_GETPID              1
#define SYS_CONSOLE_PUTS        2
#define SYS_CONSOLE_PUTC        3
#define SYS_CONSOLE_GETC        4
#define SYS_SLEEP               5

// Semaphore system calls
#define SYS_SEMCREATE           6
#define SYS_SEMGET              7
#define SYS_SEMWAIT             8
#define SYS_SEMSIGNAL           9
#define SYS_SEMCLOSE            10

#define SYSCALLS                11
