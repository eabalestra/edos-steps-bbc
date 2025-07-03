#include "arch.h"
#include "spinlock.h"
#include "task.h"


#define UART_HR       (uint8*)(UART+0x00)   // transmit/receive holding register
#define UART_LSR      (uint8*)(UART+0x05)   // line control register
#define UART_TX_IDLE  (1 << 5)              // status flag bit in UART_LSR
#define UART_RX_READY (1)                   // status flag bit in UART_LSR
#define IER 1                               // interrupt enable register
#define IER_RX_ENABLE (0x1)                 // interrupt receive enable bit
#define IER_TX_ENABLE (0x2)                 // interrupt transmit enable bit

#define R(addr)     ((uint8*)addr)

// last key entered
static char kbd_last_char = 0;
spinlock console_lock = 0;

void console_init(void)
{
    // enable receive interrupts
    *R(IER) = IER_RX_ENABLE;
}

int console_putc(char ch) {
    // wait for UART transmitter register empty
	while ((*R(UART_LSR) & UART_TX_IDLE) == 0)
        ;
    // write character to UART THR to start transmission
	return *R(UART_HR) = ch;
}

// write string to console with sync
void console_puts(const char *s) 
{
    acquire(&console_lock);
	while (*s)
        console_putc(*s++);
    release(&console_lock);
}

// Read a character from keyboard and put in buffer.
// Called from syscall
// to do: manage interrupts and suspend task
char console_read_char(void) {
    char c = 0;
    acquire(&console_lock);
    if ((*R(UART_LSR) & UART_RX_READY) != 0) {
        c = *R(UART_HR);
    }
    release(&console_lock);
    return c;
}
