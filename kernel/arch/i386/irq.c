#include "cpu/io.h"
#include "cpu/idt.h"
#include "drivers/screen.h"

extern void keyboard_handler(struct registers *r);

void irq_handler(struct registers *r) {
    
    if (r->int_no == 33) {
        // Opción A: Usar tu driver de teclado (si ya es estable)
        keyboard_handler(r);
    }
    
    // Si la IRQ vino del Slave PIC (IRQ 8-15, o sea, int_no 40-47)
    if (r->int_no >= 40) {
        outb(0xA0, 0x20); // EOI al Slave
    }
    
    // Siempre enviamos EOI al Master PIC
    outb(0x20, 0x20);
}