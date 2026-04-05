#ifndef VMM_H
#define VMM_H

#include "libs/types.h"

// Flags básicos de una página en x86
#define PAGE_PRESENT 0x1 // ¿Está en RAM?
#define PAGE_RW 0x2      // 0 = Solo lectura, 1 = Lectura/Escritura
#define PAGE_USER 0x4    // 0 = Solo Kernel, 1 = Usuario (Ring 3)

// Un directorio tiene 1024 entradas (4MB por tabla)
typedef struct {
  uint32_t entries[1024];
} page_directory_t;

// Una tabla tiene 1024 entradas (4KB por página)
typedef struct {
  uint32_t entries[1024];
} page_table_t;

void vmm_init();
void vmm_switch_directory(page_directory_t *dir);
void vmm_map_page(page_directory_t *dir, uint32_t virt, uint32_t phys,
                  uint32_t flags);

#endif