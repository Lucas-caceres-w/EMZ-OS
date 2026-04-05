#include "kernel/klog.h"
#include "libs/types.h"

#define BLOCK_SIZE 4096
#define RAM_SIZE (128 * 1024 * 1024)
#define TOTAL_BLOCKS (RAM_SIZE / BLOCK_SIZE)

// Usamos uint32_t porque es la palabra natural del CPU i386
uint32_t bitmap[TOTAL_BLOCKS / 32];

// Función interna para marcar un bit
void mmap_set(int bit) { bitmap[bit / 32] |= (1 << (bit % 32)); }

// Verifica si un bloque está ocupado (Corregida para usar uint32_t)
int pmm_is_block_occupied(uint32_t block_number) {
  return (bitmap[block_number / 32] & (1 << (block_number % 32)));
}

void pmm_lock_region(uint32_t start_addr, uint32_t size) {
  uint32_t start_block = start_addr / BLOCK_SIZE;
  uint32_t num_blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

  for (uint32_t i = 0; i < num_blocks; i++) {
    mmap_set(start_block + i);
  }
}

void *pmm_alloc_block() {
  // Empezamos a buscar desde el bloque 512 (512 * 4096 = 2MB)
  // Así protegemos el primer megabyte del Kernel y el hardware.
  for (uint32_t i = 512; i < TOTAL_BLOCKS; i++) {
    if (!pmm_is_block_occupied(i)) {
      mmap_set(i);
      uint32_t addr = i * BLOCK_SIZE;
      return (void *)addr;
    }
  }
  klog("PMM ERROR: ¡No queda memoria fisica!");
  return 0;
}

uint32_t pmm_get_free_block_count() {
  uint32_t free_count = 0;

  for (uint32_t i = 0; i < TOTAL_BLOCKS; i++) {
    // Reutilizamos tu función de verificación
    if (!pmm_is_block_occupied(i)) {
      free_count++;
    }
  }
  return free_count;
}

void init_pmm() {
  // 1. Limpiar todo el bitmap
  for (int i = 0; i < (TOTAL_BLOCKS / 32); i++) {
    bitmap[i] = 0;
  }

  // 2. Protegemos el primer MB (BIOS + Kernel + Stack + Video)
  pmm_lock_region(0x0, 0x100000);

  // 3. (Opcional) Protegemos el área del Bitmap mismo para que no se
  // sobreescriba pmm_lock_region((uint32_t)bitmap, sizeof(bitmap));

  klog("PMM: Gestor iniciado. 128MB listos.");
}