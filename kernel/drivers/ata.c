#include "drivers/ata.h"
#include "cpu/io.h"
#include "kernel/klog.h"
#include "libs/util.h"

// Puertos del Bus Primario (Master)
#define ATA_PRIMARY_DATA 0x1F0
#define ATA_PRIMARY_ERR 0x1F1
#define ATA_PRIMARY_SECCOUNT 0x1F2
#define ATA_PRIMARY_LBA_LO 0x1F3
#define ATA_PRIMARY_LBA_MID 0x1F4
#define ATA_PRIMARY_LBA_HI 0x1F5
#define ATA_PRIMARY_DRIVE_SEL 0x1F6
#define ATA_PRIMARY_COMMAND 0x1F7
#define ATA_PRIMARY_STATUS 0x1F7
/**
 * Lee un sector (512 bytes) del disco duro usando LBA de 28 bits.
 */
void ata_read_sector(uint32_t lba, uint16_t *buffer) {
  // 1. Seleccionar unidad (0xE0 es Master) y enviar los 4 bits más altos del
  // LBA
  outb(ATA_PRIMARY_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));

  // 2. Enviar cantidad de sectores (1) y el resto del LBA
  outb(ATA_PRIMARY_SECCOUNT, 1);
  outb(ATA_PRIMARY_LBA_LO, (uint8_t)lba);
  outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));
  outb(ATA_PRIMARY_LBA_HI, (uint8_t)(lba >> 16));

  // 3. Enviar comando READ SECTORS (0x20)
  outb(ATA_PRIMARY_COMMAND, 0x20);

  // 4. Polling: Esperar a que el bit BSY se limpie y el bit DRQ se active
  // Esto indica que el disco tiene los datos listos en su buffer interno
  while (inb(ATA_PRIMARY_STATUS) & 0x80)
    ; // Esperar si está ocupado (BSY)
  while (!(inb(ATA_PRIMARY_STATUS) & 0x08))
    ; // Esperar hasta que esté listo (DRQ)

  for (int i = 0; i < 256; i++) { // 256 * 2 bytes = 512 bytes exactos
    buffer[i] = inw(ATA_PRIMARY_DATA);
  }
}

void ata_write_sector(uint32_t lba, uint16_t *buffer) {
  // 1. Enviar comando de selección de unidad y bits altos de LBA
  // 0xE0 indica Drive Master + modo LBA
  outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
  // 2. Enviar conteo de sectores (1 sector)
  outb(0x1F2, 1);
  // 3. Enviar los bits restantes de la dirección LBA
  outb(0x1F3, (uint8_t)lba);         // LBA Low
  outb(0x1F4, (uint8_t)(lba >> 8));  // LBA Mid
  outb(0x1F5, (uint8_t)(lba >> 16)); // LBA High
  // 4. Enviar comando de ESCRITURA (0x30 es 'Write Sectors')
  outb(0x1F7, 0x30);
  // 5. Esperar a que el disco esté listo para recibir datos
  // Leemos el registro de estado (0x1F7)
  // Buscamos que BSY (Busy) sea 0 y DRQ (Data Request) sea 1
  while (!(inb(0x1F7) & 0x08))
    ;
  // 6. Transferir los datos al puerto de datos (0x1F0)
  // Transferimos 256 palabras de 16 bits cada una (= 512 bytes)
  for (int i = 0; i < 256; i++) {
    outw(0x1F0, buffer[i]);
  }
  // 7. Esperar a que el disco termine de escribir en el plato físico
  // (Cache flush simplificado para PIO)
  while (inb(0x1F7) & 0x80)
    ;
}