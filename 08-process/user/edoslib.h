//=============================================================================
// EDOS library
//=============================================================================
typedef int             bool;
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;
typedef unsigned int    uint;
typedef unsigned int    address;
typedef unsigned long   uint64;
typedef uint32          size_t;
typedef uint32          paddr;
typedef uint32          vaddr;
typedef uint32          reg_size;   // register size

#define false (0)
#define true  (1)
#define NULL  ((void *) 0)

//=============================================================================
// EDOS syscalls, ordered by syscall number, in sync with kernel syscall.c
//=============================================================================
extern int exit(int exit_code);
extern int getpid(void);

extern int console_puts(char *c);
extern int console_putc(char c);
extern int console_getc(void);

extern int sleep(int ticks);

// Semaphore system calls
extern int semcreate(int id, int init_value);
extern int semget(int id);  // TODO: check if this will be change in the future
extern int semwait(int id);
extern int semsignal(int id);
extern int semclose(int id);

//=============================================================================
// Utility functions
//=============================================================================
static inline void putchar(char c)
{
    console_putc(c); // syscall
}

extern char* console_read_line(void);
void *memset(void *buf, char c, unsigned int n);
void *memcpy(void *dst, const void *src, size_t n);
int strlen(const char *str);
char *strcpy(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
void printf(const char *fmt, ...);
