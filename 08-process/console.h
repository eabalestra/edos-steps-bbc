#pragma once
#include "spinlock.h"

extern spinlock console_lock;

int  console_putc(char ch);
void console_puts(const char *s);
char console_read_char(void);
char console_interrupt_handler(void);
