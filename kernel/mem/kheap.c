#include "mem/kheap.h"
#include "kernel/klog.h"
#include "libs/types.h"
#include "libs/util.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include "kernel/shell_cmd.h"

extern page_directory_t *kernel_directory;
header_t *heap_start = NULL;

extern char *key_buffer; // Ya no es 
extern int buffer_idx;
extern int cursor_idx;

// Esta función debe llamarse UNA VEZ en kernel_main
void init_shell_buffer() {
  key_buffer = (char *)kmalloc(MAX_BUFFER_SIZE);
  memset(key_buffer, 0, MAX_BUFFER_SIZE);
  buffer_idx = 0;
  cursor_idx = 0;
}

void kheap_init(uint32_t start_addr, uint32_t initial_size) {
  // 1. Mapear la memoria necesaria para el heap inicial (VMM)
  // Nota: Por ahora mapeamos una página, pero podrías mapear 'initial_size'
  uint32_t phys = (uint32_t)pmm_alloc_block();
  vmm_map_page(kernel_directory, start_addr, phys, 3);

  // 2. Configurar el primer bloque gigante
  heap_start = (header_t *)start_addr;
  heap_start->size = initial_size - sizeof(header_t);
  heap_start->magic = 0x12345678;
  heap_start->is_free = 1;
  heap_start->next = NULL;

  klog("Heap listo en %x (%d bytes)\n", start_addr, initial_size);
}