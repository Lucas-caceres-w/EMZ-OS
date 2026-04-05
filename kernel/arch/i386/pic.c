#include "cpu/io.h"

void pic_init() {
  // ICW1 - Inicialización
  outb(0x20, 0x11);
  outb(0xA0, 0x11);

  // ICW2 - Remapear (IRQ 0-7 -> 32-39, IRQ 8-15 -> 40-47)
  outb(0x21, 0x20);
  outb(0xA1, 0x28);

  // ICW3 - Cascada
  outb(0x21, 0x04);
  outb(0xA1, 0x02);

  // ICW4 - Modo 8086
  outb(0x21, 0x01);
  outb(0xA1, 0x01);

  // MÁSCARAS: Habilitar Hardware
  outb(0x21, 0xFD); // 11111101 -> Habilita SOLO teclado (IRQ1)
  outb(0xA1, 0xFF); // Deshabilita todo el Slave PIC
}