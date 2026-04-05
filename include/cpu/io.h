#ifndef IO_H
#define IO_H

#include "libs/types.h"

/**
 * Envía un byte a un puerto (Output Byte)
 * Usado para configurar el PIC, el PIT o mover el cursor.
 */
static inline void outb(unsigned short port, unsigned char val) {
  __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * Lee un byte desde un puerto (Input Byte)
 * Usado para leer el teclado (puerto 0x60) o el estado del disco.
 */
static inline unsigned char inb(unsigned short port) {
  unsigned char ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

/**
 * Pequeña pausa de I/O para hardware antiguo
 */
static inline void io_wait(void) { outb(0x80, 0); }

static inline uint16_t inw(uint16_t port) {
  uint16_t result;
  __asm__("inw %1, %0" : "=a"(result) : "dN"(port));
  return result;
}
/**
 * Lee un word (16 bits) desde el puerto especificado.
 */
static inline void outw(unsigned short port, unsigned short val) {
  __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

#endif