#ifndef KHEAP_H
#define KHEAP_H

#include "libs/types.h"
#include "mem/kheap.h" // Importante para kmalloc

// Los metadatos de cada bloque
typedef struct header {
  uint32_t size;   // Tamaño del bloque útil
  uint32_t magic;  // Número mágico para verificar integridad (ej: 0x12345678)
  uint8_t is_free; // 1 si está libre, 0 si está ocupado
  struct header *next; // Siguiente bloque
}__attribute__((packed)) header_t;

extern header_t *heap_start;
// Prototipos
void kheap_init(uint32_t start_addr, uint32_t initial_size);
void *kmalloc(uint32_t size);
void kfree(void *ptr);
void init_shell_buffer();

#endif