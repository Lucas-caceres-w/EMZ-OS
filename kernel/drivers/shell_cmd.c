#include "cpu/io.h"
#include "drivers/ata.h"
#include "drivers/screen.h"
#include "kernel/klog.h"
#include "libs/types.h"
#include "libs/util.h"
#include "mem/kheap.h"
#include "mem/pmm.h"
#include "fs/config.h"

extern char current_user[32];

void shell_cmd_help() {
  screenlog("Comandos: help | clear | memory | heap | read <sector (100/200)>| reboot | exit\n");
}

void shell_shutdown() {
  screenlog("\n[!] Apagando sistema...\n");

  // 1. Intento por puerto de QEMU (ACPI/Sleep control)
  // Esto cierra la ventana de QEMU inmediatamente
  outw(0x604, 0x2000);

  // 2. Intento por VirtualBox / Bochs
  outw(0xB004, 0x2000);

  // 3. Si falla (Hardware real), hacemos un Halt seguro
  screenlog("Error: El hardware no soporta auto-apagado.\n");
  screenlog("Halt del procesador...");
  __asm__ volatile("cli");
  while (1)
    __asm__ volatile("hlt");
}

void shell_reboot() {
  screenlog("\n[#] Reiniciando...");

  uint8_t temp;
  // Esperar a que el controlador de teclado esté listo (vaciar buffer)
  do {
    temp = inb(0x64);
  } while (temp & 0x02);

  // Enviar el comando 0xFE (Reset CPU) al puerto de control 0x64
  outb(0x64, 0xFE);

  // Si el hardware es muy viejo y falla, forzamos un "Triple Fault"
  // Cargando una IDT vacía y llamando a una interrupción
  static uint16_t idt_vacia[3] = {0, 0, 0};
  __asm__ volatile("lidt (%0)" : : "r"(idt_vacia));
  __asm__ volatile("int $3");
}

void clear_terminal() {
  kclear_screen(0);
  print_prompt(current_user);
}

void shell_cmd_mem() {
  uint32_t free_blocks = pmm_get_free_block_count();

  // 1. Convertimos a la unidad base: Kilobytes (cada bloque son 4KB)
  uint32_t free_kb = free_blocks * 4;
  uint32_t total_kb = TOTAL_BLOCKS * 4;

  klog("\n--- Estado de Memoria Fisica ---\n");

  // 2. Lógica de auto-escala para la memoria libre
  if (free_kb >= 1024 * 1024) {
    // Si es mayor o igual a 1024 MB, mostramos en GB
    // Usamos divisiones para obtener la parte entera y decimal simple
    uint32_t gb = free_kb / (1024 * 1024);
    uint32_t mb_rem = (free_kb % (1024 * 1024)) / 1024;
    klog("Libre: %d.%d GB\n", gb, mb_rem);
  } else if (free_kb >= 1024) {
    // Si es mayor a 1024 KB, mostramos en MB
    uint32_t mb = free_kb / 1024;
    klog("Libre: %d MB\n", mb);
  } else {
    // Si es muy poca, mostramos en KB
    klog("Libre: %d KB\n", free_kb);
  }

  // 3. Mostrar el total (opcional, con la misma lógica si quieres)
  klog("Total: %d MB\n", total_kb / 1024);
  klog("Uso: %d%%\n", 100 - (free_kb * 100 / total_kb));
}

void kheap_stats() {
  header_t *curr = heap_start;
  uint32_t free_mem = 0;
  uint32_t used_mem = 0;

  while (curr) {
    if (curr->is_free)
      free_mem += curr->size;
    else
      used_mem += curr->size;
    curr = curr->next;
  }

  klog("HEAP STATS:\n");
  klog("  Usada: %d bytes\n", used_mem);
  klog("  Libre: %d bytes\n", free_mem);
  klog("  Total: %d bytes\n", used_mem + free_mem);
}

void shell_cmd_read(char *arg) {
  int sector = atoi(arg); // Necesitas una función para convertir string a int
  uint16_t *buffer = (uint16_t *)kmalloc(512);

  if (sector < 100) {
    klog("ERROR: El sector %d esta reservado para el sistema (Kernel/Boot).\n",
         sector);
    klog("Usa sectores >= 100 para pruebas.\n");
    return;
  }
  // Verificación de seguridad
  if (buffer == NULL) {
    klog("ERROR: No hay memoria suficiente para leer el disco.\n");
    return;
  }
  klog("Leyendo Sector %d...\n", sector);
  ata_read_sector(sector, buffer);
  // Volcado hexadecimal/texto simple
  unsigned char *ptr = (unsigned char *)buffer;
  for (int i = 0; i < 64; i++) { // Mostramos los primeros 64 bytes
    if (ptr[i] >= 32 && ptr[i] <= 126)
      klog("%c", ptr[i]);
    else
      klog(".");
  }
  klog("\n");

  kfree(buffer);
}

void shell_cmd_write(char *arg) {
  if (*arg == '\0') {
    klog("Uso: write <sector> <texto>\n");
    return;
  }
  // 1. Extraer el sector
  int sector = atoi(arg);
  // 2. Buscar dónde empieza el texto (después del primer espacio)
  char *texto = arg;
  while (*texto != ' ' && *texto != '\0')
    texto++;
  if (*texto == ' ')
    texto++; // Saltamos el espacio

  if (*texto == '\0') {
    klog("Error: Falta el texto a escribir.\n");
    return;
  }
  // 3. Preparar el buffer de 512 bytes (el disco solo escribe bloques
  // completos)
  uint16_t *buffer = (uint16_t *)kmalloc(512);
  memset(buffer, 0, 512); // Limpiamos con ceros
  // Copiamos el texto del usuario al buffer
  char *dest = (char *)buffer;
  for (int i = 0; i < 510 && texto[i] != '\0'; i++) {
    dest[i] = texto[i];
  }
  // 4. ¡AL DISCO!
  klog("Escribiendo en sector %d...\n", sector);
  ata_write_sector(sector, buffer);
  klog("Hecho.\n");

  kfree(buffer);
}

void shell_cmd_setup(char *arg) {
  // Si no pasas nombre, damos error
  if (*arg == '\0') {
    klog("Uso: setup <tu_nombre>\n");
    return;
  }

  // 1. Creamos la estructura en memoria (usando el Heap que ya dominas)
  emz_config_t *conf = (emz_config_t *)kmalloc(sizeof(emz_config_t));
  memset(conf, 0, sizeof(emz_config_t));

  // 2. Llenamos los datos
  conf->magic = EMZ_MAGIC_FS;
  conf->shell_color = 0x0F; // Blanco por defecto
  conf->installed = 1;

  // Copiamos el nombre (máximo 31 caracteres)
  for (int i = 0; i < 31 && arg[i] != '\0'; i++) {
    conf->user_name[i] = arg[i];
  }

  // 3. ¡A grabar en el disco!
  klog("Instalando EMZ-OS para %s...\n", conf->user_name);
  ata_write_sector(CONFIG_SECTOR, (uint16_t *)conf);

  klog("Sistema configurado. Reinicia para ver los cambios.\n");

  kfree(conf);
}