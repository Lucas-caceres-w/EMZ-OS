#include "cpu/idt.h"
#include "kernel/klog.h"

// Registrar los dos puentes de ASM
extern void irq0();
extern void irq1();
extern void isr0();
extern void isr10();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr_ignore();

struct idt_entry {
  unsigned short base_low;  // Dirección baja de la función (bits 0-15)
  unsigned short sel;       // Selector de segmento de código (0x08)
  unsigned char always0;    // Siempre 0
  unsigned char flags;      // Atributos (Presente, Nivel de privilegio, etc.)
  unsigned short base_high; // Dirección alta de la función (bits 16-31)
} __attribute__((packed));

struct idt_ptr {
  unsigned short limit; // Tamaño de la tabla - 1
  unsigned int base;    // Dirección donde empieza la tabla
} __attribute__((packed));

struct idt_entry idt[256] __attribute__((aligned(16)));
struct idt_ptr idtp;

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel,
                  unsigned char flags) {
  idt[num].base_low = (base & 0xFFFF);
  idt[num].base_high = (base >> 16) & 0xFFFF;
  idt[num].sel = sel;
  idt[num].always0 = 0;
  idt[num].flags = flags;
}

void idt_init() {
  idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
  idtp.base = (unsigned int)&idt;
  klog("IDT Base: %x | isr0: %x\n", idtp.base, (unsigned int)isr0);
  // LIMPIEZA: Ponemos todas las entradas en 0 para que no apunten a basura
  for (int i = 0; i < 256; i++) {
    idt_set_gate(i, (unsigned int)isr_ignore, 0x08, 0x8E);
  }

  idt_set_gate(0, (unsigned int)isr0, 0x08, 0x8E);   // División por cero
  idt_set_gate(10, (unsigned int)isr10, 0x08, 0x8E); // General Protection Fault
  idt_set_gate(12, (unsigned int)isr12, 0x08, 0x8E); // General Protection Fault
  idt_set_gate(13, (unsigned int)isr13, 0x08, 0x8E); // General Protection Fault
  idt_set_gate(14, (unsigned int)isr14, 0x08, 0x8E); // PAGE FAULT

  idt_set_gate(32, (unsigned int)irq0, 0x08, 0x8E); // Timer
  idt_set_gate(33, (unsigned int)irq1, 0x08, 0x8E); // Keyboard

  __asm__ volatile("lidt %0" : : "m"(idtp));
}