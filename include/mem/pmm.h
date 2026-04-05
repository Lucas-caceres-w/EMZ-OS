#ifndef PMM_H
#define PMM_H

#include "libs/types.h"

// Configuraciones de tu mapa de memoria
#define BLOCK_SIZE 4096                  // 4KB por bloque (estándar x86)
#define RAM_SIZE   (128 * 1024 * 1024)   // Asumimos 128MB por ahora
#define TOTAL_BLOCKS (RAM_SIZE / BLOCK_SIZE)

// Funciones principales
void init_pmm();                         // Inicia el bitmap
void *pmm_alloc_block();                 // Reserva 4KB y devuelve la dirección
void pmm_free_block(void *addr);         // Libera un bloque
int pmm_is_block_occupied(uint32_t block_number);
void pmm_lock_region(uint32_t start_addre, uint32_t size);
uint32_t pmm_get_free_block_count();
#endif