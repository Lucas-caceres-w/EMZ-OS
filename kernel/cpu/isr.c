#include "cpu/isr.h"
#include "drivers/screen.h"
#include "kernel/klog.h"
#include "mem/pmm.h"
#include "mem/vmm.h"

// Array de 256 punteros a funciones (uno por cada interrupción posible)
isr_t interrupt_handlers[256];
extern page_directory_t *kernel_directory;

// Función para "conectar" un handler a un número de interrupción
void register_interrupt_handler(uint8_t n, isr_t handler) {
  interrupt_handlers[n] = handler;
}

// --- CAMBIA ESTO ---
void page_fault_handler(registers_t *r) {
  uint32_t fault_addr;
  __asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));

  // Si el usuario quiere escribir en 5MB (0x500000)
  if (fault_addr >= 0x400000 && fault_addr < 0x800000) {
    klog("Asignando memoria fisica para direccion virtual %x...\n", fault_addr);

    // Obtenemos un bloque físico real del PMM
    uint32_t phys_frame = (uint32_t)pmm_alloc_block();

    // Hacemos el mapeo: Virtual (5MB) -> Fisico (lo que de el PMM)
    vmm_map_page(kernel_directory, fault_addr, phys_frame, 3);

    return;
  }

  klog("PANIC: Acceso ilegal a %x\n", fault_addr);
  while (1)
    ;
}

void division_by_zero_handler(registers_t *r) {
  klog("\n[!] EXCEPCION: Division por cero.");
  while (1)
    __asm__("hlt");
}

void key_handler(registers_t *r) {
  // Por ahora lo dejamos vacío para que compile
  (void)r;
}

void isr_handler(registers_t *r) {
  klog("INT: %d | ERR: %x | EIP: %x\n", r->int_no, r->err_code, r->eip);
  if (interrupt_handlers[r->int_no] != 0) {
    isr_t handler = interrupt_handlers[r->int_no];
    handler(r);
  } else {
    klog("INT detectada: %d\n", r->int_no);
  }
}

// --- INSTALACIÓN ---

void isr_install() {
  for (int i = 0; i < 256; i++)
    interrupt_handlers[i] = 0;

  register_interrupt_handler(0,
                             division_by_zero_handler); // Para el test manual
  register_interrupt_handler(14, page_fault_handler);   // Para el VMM
}