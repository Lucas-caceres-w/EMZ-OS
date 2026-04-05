#ifndef ACTIONS_H
#define ACTIONS_H

#include "drivers/keycodes.h"

// Acciones de edición de texto (Usadas por Backspace, Delete, etc.)
void delete_char(int direction); // -1 para atrás, 1 para adelante
void insert_char(char c);

// Acciones de navegación
void move_cursor(keycode_t direction);  // KEY_LEFT, KEY_RIGHT, etc.
void go_to_line_limit(keycode_t limit); // KEY_HOME, KEY_END

// Acciones de sistema
void execute_line(); // Lo que antes era execute_command
void clear_terminal();
void redraw_line();
void shutdown();

#endif