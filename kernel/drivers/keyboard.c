#include "drivers/keyboard.h"
#include "cpu/idt.h"
#include "cpu/io.h"
#include "drivers/keycodes.h"
#include "drivers/screen.h"
#include "kernel/actions.h"
#include "libs/util.h"

// En keyboard.c
static unsigned char layout_us[128] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '};

static unsigned char layout_us_shift[128] = {
    0,   27,  '!',  '@',  '#',  '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t', 'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{',  '}',  '\n', 0,   'A', 'S', 'D', 'F', 'G', 'H',
    'J', 'K', 'L',  ':',  '\"', '~', 0,   '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M',  '<',  '>',  '?', 0,   '*', 0,   ' '};


// Al principio de keyboard.c (o en un header)
extern char key_buffer[]; // Referencia al array de actions.c
volatile int any_key;
extern void reset_buffer(); // <--- Esto permite usarla

void dispatch_key_action(keycode_t key) {
  switch (key) {
  case KEY_ENTER:
    any_key = 1;
    execute_line();
    break;
  case KEY_BACKSPACE:
    delete_char(-1);
    break;
  case KEY_DELETE:
    delete_char(1);
    break;
  case KEY_LEFT:
    move_cursor(KEY_LEFT);
    break;
  case KEY_RIGHT:
    move_cursor(KEY_RIGHT);
    break;
  case KEY_HOME:
    go_to_line_limit(KEY_HOME);
    break;
  case KEY_END:
    go_to_line_limit(KEY_END);
    break;
  case KEY_F12:
    clear_terminal();
    break; // Atajo rápido

  default:
    // Si es un caracter visible, lo insertamos
    if (key >= 32 && key <= 126) {
      insert_char((char)key);
    }
    break;
  }
}

void keyboard_handler(struct registers *r) {
  unsigned char scancode = inb(0x60);
  static int shift_active = 0;

  // 1. Detectar Presionar/Soltar Shift
  if (scancode == 0x2A || scancode == 0x36) {
    shift_active = 1;
    return;
  }
  if (scancode == 0xAA || scancode == 0xB6) {
    shift_active = 0;
    return;
  }

  // 2. Ignorar "Key Up" del resto de teclas
  if (scancode & 0x80)
    return;

  // 3. Traducir Scancodes Especiales (E0) o Flechas
  keycode_t final_key = KEY_NULL;

  if (scancode == 0x4B)
    final_key = KEY_LEFT;
  else if (scancode == 0x4D)
    final_key = KEY_RIGHT;
  else if (scancode == 0x47)
    final_key = KEY_HOME;
  else if (scancode == 0x4F)
    final_key = KEY_END;
  else if (scancode == 0x53)
    final_key = KEY_DELETE;
  else if (scancode == 0x3B)
    final_key = KEY_F1;
  // ... más traducciones ...
  else {
    // Traducción ASCII normal usando las tablas duales
    final_key = shift_active ? layout_us_shift[scancode] : layout_us[scancode];
  }

  // 4. Enviar a la función de acciones
  if (final_key != KEY_NULL) {
    dispatch_key_action(final_key);
  }
}

void shell_readline(char *buffer_salida, int max_len) {
  // Usamos tu función de actions.c para limpiar antes de empezar
  reset_buffer();

  // BLOQUEO: Se queda aquí hasta que execute_line ponga enter_presionado = 1
  while (!any_key) {
    __asm__ volatile("hlt");
  }

  // COPIA: Pasamos lo que tus funciones metieron en key_buffer al instalador
  // Como no tienes strncpy, usamos un bucle:
  int i = 0;
  while (i < max_len - 1 && key_buffer[i] != '\0') {
    buffer_salida[i] = key_buffer[i];
    i++;
  }
  buffer_salida[i] = '\0';

  // Ahora que ya copiamos el nombre, podemos limpiar el buffer para la shell
  reset_buffer();
  any_key = 0;
}