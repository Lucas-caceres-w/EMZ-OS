#include "drivers/screen.h"
#include "cpu/idt.h"

// Mensajes para las primeras 32 excepciones de x86
char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Fault de Punto Flotante",
    "Alignment Check",
    "Machine Check",
    "Reserved"
};

/* Esta función será llamada desde interrupts.asm */
void fault_handler(struct registers *r) {
    if (r->int_no < 32) {
        kclear_screen(0);
        kprint_at(" !!! KERNEL PANIC !!! ", 30, 10, RED_ON_WHITE);
        
        if (r->int_no < 20) {
            kprint_at(exception_messages[r->int_no], 30, 12, WHITE_ON_BLACK);
        } else {
            kprint_at("Reserved Exception", 30, 12, WHITE_ON_BLACK);
        }
        
        while(1) __asm__("hlt");
    }
}