#pragma once

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