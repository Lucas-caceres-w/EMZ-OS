#include "mem/kmalloc.h"
#include "kernel/klog.h"
#include "libs/types.h"
#include "mem/kheap.h"

void *kmalloc(uint32_t size) {
  header_t *current = heap_start;

  while (current) {
    if (current->is_free && current->size >= size) {
      // ¿El bloque es mucho más grande de lo pedido? (ej. sobran > 32 bytes)
      if (current->size > size + sizeof(header_t) + 16) {
        // Creamos un nuevo header en el espacio sobrante
        header_t *next_block =
            (header_t *)((uint32_t)current + sizeof(header_t) + size);
        next_block->size = current->size - size - sizeof(header_t);
        next_block->is_free = 1;
        next_block->next = current->next;

        // Ajustamos el bloque actual
        current->size = size;
        current->next = next_block;
      }

      current->is_free = 0;
      return (void *)(current + 1);
    }
    current = current->next;
  }
  return NULL; // Aquí llamaríamos a vmm_map_page si quisiéramos expandir
}

void kfree(void *ptr) {
  if (!ptr)
    return;

  header_t *h = (header_t *)((uint32_t)ptr - sizeof(header_t));
  if (h->magic != 0x12345678) {
    if (h->magic != 0x0) {
      klog("[WARN] kfree: sector de memoria no marcado (%x)\n", h->magic);
    }
    return;
  }

  h->is_free = 1;

  if (h->next && h->next->is_free) {
    h->size += sizeof(header_t) + h->next->size;
    h->next = h->next->next;
  }
}