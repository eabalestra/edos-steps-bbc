#pragma once

#include "types.h"

void  init_kalloc(void);
void* alloc_page(void);
void  free_page(void* pa);
