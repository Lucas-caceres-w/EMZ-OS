#include "cpu/idt.h"
#include "cpu/isr.h"
#include "cpu/pic.h"
#include "drivers/ata.h"
#include "drivers/screen.h"
#include "fs/config.h"
#include "kernel/klog.h"
#include "kernel/setup_wizard.h"
#include "libs/types.h"
#include "libs/util.h"
#include "mem/kheap.h"
#include "mem/pmm.h"
#include "mem/vmm.h"

int sistema_ya_instalado = 0; // Por defecto asumimos que no está instalado
extern int prompt_limit_offset;
char current_user[32] =
    "invitado"; // Global: vive mientras el PC esté encendido

void kmain() {
  emz_config_t *boot_conf = (emz_config_t *)kmalloc(512);
  ata_read_sector(CONFIG_SECTOR, (uint16_t *)boot_conf);

  if (boot_conf->magic != EMZ_MAGIC_FS) {
    kfree(boot_conf);
    run_first_time_setup();

    // IMPORTANTE: Después del setup, sistema_ya_instalado ya es 1
    // Re-leemos los datos para llenar el current_user
    boot_conf = (emz_config_t *)kmalloc(512);
    ata_read_sector(CONFIG_SECTOR, (uint16_t *)boot_conf);
  }

  // Sincronizamos la variable global current_user
  for (int i = 0; i < 31 && boot_conf->user_name[i] != '\0'; i++) {
    current_user[i] = boot_conf->user_name[i];
    current_user[i + 1] = '\0';
  }

  sistema_ya_instalado = 1;
  kclear_screen(0);
  klog("Bienvenido de nuevo, %s\n\n", current_user);

  // print_prompt ya llama internamente a get_cursor_offset()
  print_prompt(current_user);

  kfree(boot_conf);
}

void __attribute__((section(".text.entry"))) kernel_main() {
  // 1. Hardware & Memoria (Silencioso)
  idt_init();

  isr_install();

  pic_init();

  init_pmm();

  vmm_init();

  kheap_init(0x1000000, 4096);

  kclear_screen(0);

  __asm__ volatile("sti");

  init_shell_buffer();

  kmain();

  // El sistema se queda esperando interrupciones de teclado
  while (1) {
    __asm__ volatile("hlt");
  }
}