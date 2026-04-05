#include "cpu/io.h"
#include "libs/util.h"

void ata_read_sector(uint32_t lba, uint16_t *buffer);
void ata_write_sector(uint32_t lba, uint16_t *buffer);