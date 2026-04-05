#include "drivers/ata.h"
#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "fs/config.h"
#include "kernel/klog.h"
#include "libs/util.h"
#include "mem/kheap.h"

// Debe coincidir con el nombre usado en keyboard.c
extern int any_key;
extern int prompt_limit_offset;

void wait_for_any_key() {
  any_key = 0;
  while (any_key == 0) {
    __asm__ volatile("hlt");
  }
  any_key = 0;
  prompt_limit_offset = get_cursor_offset();
}

void collect_user_name(char *out_name) {
  klog("Introduce tu nombre de usuario: ");
  // Esta función ahora despertará cuando des ENTER
  prompt_limit_offset = get_cursor_offset();

  shell_readline(out_name, 31);
}

void run_first_time_setup() {
  kclear_screen(0);
  klog("==========================================\n");
  klog("      ASISTENTE DE INSTALACION EMZ-OS     \n");
  klog("==========================================\n\n");

  char nombre[32];
  for (int i = 0; i < 32; i++)
    nombre[i] = 0; // Sustituto de memset si falla

  collect_user_name(nombre);

  // En lugar de emz_config_t new_conf;
  emz_config_t *new_conf = (emz_config_t *)kmalloc(512);
  memset(new_conf, 0, 512);

  new_conf->magic = EMZ_MAGIC_FS;
  new_conf->installed = 1;

  // Copia el nombre
  for (int i = 0; i < 31 && nombre[i] != '\0'; i++) {
    new_conf->user_name[i] = nombre[i];
  }

  ata_write_sector(CONFIG_SECTOR, (uint16_t *)new_conf);
  kfree(new_conf); // Limpieza

  klog("\n[OK] Instalacion finalizada.\n");
  klog("Presiona cualquier tecla para entrar al sistema...");

  wait_for_any_key();
}