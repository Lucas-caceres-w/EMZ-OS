#include "kernel/actions.h"
#include "cpu/io.h"
#include "drivers/ata.h"
#include "drivers/keycodes.h"
#include "drivers/screen.h"
#include "kernel/klog.h"
#include "kernel/shell_cmd.h"
#include "libs/util.h"
#include "mem/kmalloc.h"

// Variables de estado (pueden ser extern si están definidas en keyboard.c)
char key_buffer[MAX_BUFFER_SIZE];
int buffer_idx = 0;
int cursor_idx = 0;

extern char current_user[32]; // Una global que llenas en kmain
extern int any_key;
extern int prompt_limit_offset;
extern int sistema_ya_instalado; // Le avisamos que la variable está en main.c

// Función auxiliar para no repetir código (La "prolija")
void reset_buffer() {
  buffer_idx = 0;
  cursor_idx = 0;
  key_buffer[0] = '\0';
}
/**
 * Redibuja la línea actual desde el prompt.
 */
void redraw_line() {
  set_cursor_offset(prompt_limit_offset);

  screenlog(key_buffer);

  screenlog(" "); // Limpia el rastro de caracteres borrados
  // Reposiciona el cursor físico basado en cursor_idx
  set_cursor_offset(prompt_limit_offset + (cursor_idx * 2));
}
/**
 * Inserta un carácter en la posición actual del cursor.
 */
void insert_char(char c) {
  if (buffer_idx < MAX_BUFFER_SIZE - 1) {
    // Desplazar caracteres a la derecha para hacer espacio
    for (int i = buffer_idx; i > cursor_idx; i--) {
      key_buffer[i] = key_buffer[i - 1];
    }
    key_buffer[cursor_idx] = c;
    cursor_idx++;
    buffer_idx++;
    key_buffer[buffer_idx] = '\0';

    redraw_line();
  }
}
/**
 * Borra un carácter (direction -1 = Backspace, 1 = Delete).
 */
void delete_char(int direction) {
  if (direction == -1) { // BACKSPACE
    if (cursor_idx > 0) {
      for (int i = cursor_idx - 1; i < buffer_idx; i++) {
        key_buffer[i] = key_buffer[i + 1];
      }
      buffer_idx--;
      cursor_idx--;
      redraw_line();
    }
  } else if (direction == 1) { // DELETE
    if (cursor_idx < buffer_idx) {
      for (int i = cursor_idx; i < buffer_idx; i++) {
        key_buffer[i] = key_buffer[i + 1];
      }
      buffer_idx--;
      // No movemos cursor_idx porque el texto "viene" hacia el cursor
      redraw_line();
    }
  }
}
/**
 * Mueve el cursor a la izquierda o derecha.
 */
void move_cursor(keycode_t direction) {
  if (direction == KEY_LEFT && cursor_idx > 0) {
    cursor_idx--;
  } else if (direction == KEY_RIGHT && cursor_idx < buffer_idx) {
    cursor_idx++;
  }
  // Actualizamos el cursor físico
  set_cursor_offset(prompt_limit_offset + (cursor_idx * 2));
}
/**
 * Mueve el cursor al inicio o al final de la línea actual.
 */
void go_to_line_limit(keycode_t limit) {
  if (limit == KEY_HOME) {
    cursor_idx = 0;
  } else if (limit == KEY_END) {
    cursor_idx = buffer_idx;
  }
  set_cursor_offset(prompt_limit_offset + (cursor_idx * 2));
}

void read_disk() {
  // Pedimos 512 bytes al Heap para el buffer del disco
  uint16_t *disk_buffer = (uint16_t *)kmalloc(512);

  klog("Leyendo Sector 0...\n");
  ata_read_sector(0, disk_buffer);

  // Los últimos dos bytes de un sector de arranque válido son 0x55 y 0xAA
  uint16_t signature = disk_buffer[255];

  if (signature == 0xAA55) { // Por el orden de bytes (endianness)
    klog("¡Éxito! Firma de arranque detectada: 0x%x\n", signature);
  } else {
    klog("Sector leído, pero no parece un bootsector (Firma: %x)\n", signature);
  }

  kfree(disk_buffer); // ¡No olvides liberar la memoria!
}
/**
 * Ejecuta el comando actual en el buffer.
 */
void execute_line() {
  screenlog("\n");

  if (sistema_ya_instalado) {
    if (buffer_idx > 0) {
      // 1. Intentamos ejecutar. find_and_run_command ya imprime el resultado si
      // tiene éxito.
      if (!find_and_run_command(key_buffer)) {
        // 2. SOLO si falla, imprimimos el error.
        screenlog("Comando desconocido: \n Escribe help para ver los comandos disponibles");
        //screenlog(key_buffer);
        screenlog("\n");
      }
    }

    // 3. Limpiamos y mostramos el nuevo prompt
    reset_buffer();
    print_prompt(current_user);

  } else {
    // 4. Si estamos en el asistente de instalación (Wizard)
    any_key = 1;
  }
}
