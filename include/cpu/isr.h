#ifndef ISR_H
#define ISR_H

#include "libs/types.h"

// Estructura que representa el estado del CPU tras una interrupción
// El orden es CRITICO: coincide con el pushad de ensamblador
typedef struct {
    uint32_t ds;                                     // Segmento de datos
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed por 'pusha'
    uint32_t int_no, err_code;                       // ID de interrupción y error (si existe)
    uint32_t eip, cs, eflags, useresp, ss;           // Pushed automáticamente por el CPU
} registers_t;

// Tipo de dato para un "Manejador de Interrupción" (puntero a función)
typedef void (*isr_t)(registers_t *);

// Prototipos de las funciones de control
void isr_install();
void register_interrupt_handler(uint8_t n, isr_t handler);

#endif