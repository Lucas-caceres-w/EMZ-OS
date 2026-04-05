#include "kernel/klog.h"
#include "mem/vmm.h"
#include "mem/pmm.h"
#include "libs/util.h"

// Directorio maestro (1024 entradas de 4 bytes = 4KB)
// __attribute__((aligned(4096))) es vital aquí.
page_directory_t *kernel_directory __attribute__((aligned(4096)));

// Una tabla de páginas para cubrir los primeros 4MB de RAM
page_table_t *first_page_table __attribute__((aligned(4096)));

void vmm_enable_paging(page_directory_t *dir) {
  __asm__ volatile("mov %0, %%cr3" : : "r"(dir));
  uint32_t cr0;
  __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
  klog("CR0 antes: %x\n", cr0);
  cr0 |= 0x80000000;
  __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));

  uint32_t cr0_after;
  __asm__ volatile("mov %%cr0, %0" : "=r"(cr0_after));
  klog("CR0 despues: %x\n", cr0_after);
}

void vmm_map_page(page_directory_t *dir, uint32_t virt, uint32_t phys,
                  uint32_t flags) {
  uint32_t pd_idx = virt >> 22;           // Índice del Directorio (bits 22-31)
  uint32_t pt_idx = (virt >> 12) & 0x3FF; // Índice de la Tabla (bits 12-21)

  // 1. ¿Existe la tabla de páginas para este rango?
  if (!(dir->entries[pd_idx] & 0x1)) {
    // No existe: hay que crearla dinámicamente
    uint32_t new_table = (uint32_t)pmm_alloc_block();

    // Limpiamos la nueva tabla (importante: poner todo en 0)
    uint32_t *ptr = (uint32_t *)new_table;
    for (int i = 0; i < 1024; i++)
      ptr[i] = 0;

    // La conectamos al directorio
    dir->entries[pd_idx] = new_table | 3; // Presente + RW
  }

  // 2. Obtenemos la dirección de la tabla (quitando los flags)
  uint32_t *table = (uint32_t *)(dir->entries[pd_idx] & 0xFFFFF000);

  // 3. Mapeamos la dirección física
  table[pt_idx] = phys | flags;

  // 4. TLB Flush: Avisar al CPU que la tabla cambió
  __asm__ volatile("invlpg (%0)" ::"r"(virt) : "memory");
}

void vmm_init() {
  // 1. Pedimos bloques físicos al PMM para nuestras tablas
  // Forzamos a que cada uno pida un bloque de 4KB real
  kernel_directory = (page_directory_t *)pmm_alloc_block();
  first_page_table = (page_table_t *)pmm_alloc_block();

  // Limpieza CRÍTICA: Si no limpias con 0, el CPU ve "fantasmas"
  for (int i = 0; i < 1024; i++) {
    kernel_directory->entries[i] = 0x00000002; // No presente, RW
    first_page_table->entries[i] = 0x00000000;
  }

  // Mapeo de identidad (0 a 4MB)
  for (unsigned int i = 0; i < 1024; i++) {
    first_page_table->entries[i] = (i * 4096) | 3; // Presente + RW
  }

  kernel_directory->entries[0] = ((uint32_t)first_page_table) | 3;

  vmm_enable_paging(kernel_directory);
}
