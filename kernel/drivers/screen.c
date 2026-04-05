#include "drivers/screen.h"
#include "cpu/idt.h"
#include "cpu/io.h"
#include "libs/util.h"

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

int prompt_limit_offset = 0;

int get_cursor_offset() {
  outb(REG_SCREEN_CTRL, 14);
  int offset = inb(REG_SCREEN_DATA) << 8;
  outb(REG_SCREEN_CTRL, 15);
  offset += inb(REG_SCREEN_DATA);
  return offset * 2;
}

void set_cursor_offset(int offset) {
  offset /= 2;
  outb(REG_SCREEN_CTRL, 14);
  outb(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
  outb(REG_SCREEN_CTRL, 15);
  outb(REG_SCREEN_DATA, (unsigned char)(offset & 0xFF));
}

// En drivers/screen.h y screen.c
void print_prompt(char *name) {
  screenlog("[");
  screenlog(name);
  screenlog("@emz-os]> ");

  // Capturamos el offset justo después de imprimir el nombre del usuario
  prompt_limit_offset = get_cursor_offset();
}

int get_screen_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }

int scroll_screen() {
  unsigned char *vidmem = (unsigned char *)VIDEO_ADDRESS;
  int row_size = MAX_COLS * 2;

  // 1. Mover filas hacia arriba
  for (int i = 1; i < MAX_ROWS; i++) {
    for (int j = 0; j < row_size; j++) {
      vidmem[(i - 1) * row_size + j] = vidmem[i * row_size + j];
    }
  }

  // 2. Limpiar la última fila
  int last_row_offset = (MAX_ROWS - 1) * row_size;
  for (int i = 0; i < row_size; i += 2) {
    vidmem[last_row_offset + i] = ' ';
    vidmem[last_row_offset + i + 1] = WHITE_ON_BLACK;
  }

  // 3. El nuevo offset es el inicio de la última fila para seguir escribiendo
  return last_row_offset;
}

void kclear_screen(char *message) {
  (void)message;
  int screen_size = MAX_ROWS * MAX_COLS;
  unsigned char *screen = (unsigned char *)VIDEO_ADDRESS;

  for (int i = 0; i < screen_size; i++) {
    screen[i * 2] = ' ';
    screen[i * 2 + 1] = WHITE_ON_BLACK;
  }
  // RESET del cursor al inicio después de limpiar
  set_cursor_offset(0);
}

int move_offset_to_new_line(int offset) {
  return ((offset / (MAX_COLS * 2)) + 1) * (MAX_COLS * 2);
}

void kprint_at(char *message, int col, int row, char attribute_byte) {
  int offset;
  // Si pasamos coordenadas positivas, vamos ahí. Si no, usamos el cursor
  // actual.
  if (col >= 0 && row >= 0) {
    offset = get_screen_offset(col, row);
  } else {
    offset = get_cursor_offset();
  }

  int i = 0;
  while (message[i] != 0) {
    // VERIFICAR SCROLL: Si el offset llega al final de la memoria visible
    if (offset >= MAX_ROWS * MAX_COLS * 2) {
      offset = scroll_screen();
    }

    if (message[i] == '\n') {
      offset = move_offset_to_new_line(offset);
    } else {
      unsigned char *vidmem = (unsigned char *)VIDEO_ADDRESS;
      vidmem[offset] = message[i];
      vidmem[offset + 1] = attribute_byte;
      offset += 2;
    }
    i++;
  }
  // Actualizamos el cursor físico al final de la impresión
  set_cursor_offset(offset);
}

void screenlog(char *message) { kprint_at(message, -1, -1, WHITE_ON_BLACK); }

// Corregimos backspace para que sea coherente
void kprint_backspace() {
  int offset = get_cursor_offset();
  if (offset <= prompt_limit_offset)
    return;

  offset -= 2;
  unsigned char *vidmem = (unsigned char *)VIDEO_ADDRESS;
  vidmem[offset] = ' ';
  vidmem[offset + 1] = WHITE_ON_BLACK;
  set_cursor_offset(offset);
}