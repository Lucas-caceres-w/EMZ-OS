#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "cpu/idt.h" // Para la estructura registers
#include "drivers/keycodes.h"

// Prototipos de funciones del buffer y manejo
void init_keyboard();
void keyboard_handler(struct registers *r);
void dispatch_key_action(keycode_t key);
void shell_readline(char *buffer_salida, int max_len);

#endif