#ifndef KMALLOC_H
#define KMALLOC_H

#include "libs/types.h"

void *kmalloc(uint32_t size);
void kfree(void *ptr);

#endif