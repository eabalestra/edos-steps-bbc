#include <stdarg.h>
#include "edoslib.h"

extern int main();

// console read buffer
#define CONSOLE_LINE_LEN 91
static char input[CONSOLE_LINE_LEN];

// some ascii codes
#define KBD_ENTER   (13)

// process entry point
void start(void)
{
    exit(main());
}

char* console_read_line(void)
{
    int c = 0, i = 0;
    while (i < CONSOLE_LINE_LEN) {
        c = console_getc();
        if (c == KBD_ENTER) {
            input[i] = 0;
            break;
        }
        if (c >= 31 && c <= 127) {
            input[i] = (char) c;
            console_puts(input + i);
            i++;
        }
    }
    return input;
}

void *memset(void *buf, char c, unsigned int n)
{
    unsigned char *p = (unsigned char *) buf;
    while (n--)
        *p++ = c;
    return buf;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *) dst;
    const unsigned char *s = (const unsigned char *) src;
    while (n--)
        *d++ = *s++;
    return dst;
}

int strlen(const char *str)
{
    int r = 0;
    while (str[r])
        r++;
    return r;
}

char *strcpy(char *dst, const char *src)
{
    char *d = dst;
    while (*src)
        *d++ = *src++;
    *d = '\0';
    return dst;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s2) {
        if (*s1 != *s2)
            break;
        s1++;
        s2++;
    }

    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

void printf(const char *fmt, ...)
{
    va_list vargs;
    char *hex_digits = "0123456789abcdef";

    va_start(vargs, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case '\0':
                    putchar('%');
                    goto end;
                case '%':
                    putchar('%');
                    break;
                case 's': {
                    const char *s = va_arg(vargs, const char *);
                    while (*s) {
                        putchar(*s);
                        s++;
                    }
                    break;
                }
                case 'd': {
                    int value = va_arg(vargs, int);
                                     
                    if (value < 0) {
                        putchar('-');
                        value = -value;
                    }

                    long int divisor = 1;
                    while (value / divisor > 9)
                        divisor *= 10;

                    while (divisor > 0) {
                        putchar('0' + value / divisor);
                        value %= divisor;
                        divisor /= 10;
                    }
                    break;
                }
                case 'x': {
                    int value = va_arg(vargs, int);
                    for (int i = 7; i >= 0; i--) {
                        int nibble = (value >> (i * 4)) & 0xf;
                        putchar(hex_digits[nibble]);
                    }
                }
            }
        } else {
            putchar(*fmt);
        }

        fmt++;
    }

end:
    va_end(vargs);
}
