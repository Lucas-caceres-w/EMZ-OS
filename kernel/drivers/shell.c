#include "kernel/shell_cmd.h" // <--- IMPORTANTE: Aquí se define shell_command_t
#include "drivers/screen.h"
#include "libs/util.h"
#include "libs/types.h"

extern char key_buffer[];
extern int buffer_idx;
extern int cursor_idx;

// Tabla de comandos: Prolija y fácil de expandir
shell_command_t commands[] = {
    {"clear", kclear_screen},  {"help", shell_cmd_help},
    {"exit", shell_shutdown},  {"reboot", shell_reboot},
    {"memory", shell_cmd_mem}, {"heap", kheap_stats},
    {"read", shell_cmd_read}, {"write", shell_cmd_write}};

#define COMMAND_COUNT (sizeof(commands) / sizeof(shell_command_t))

int find_and_run_command(char *input) {
  for (int i = 0; i < (int)COMMAND_COUNT; i++) {
    int cmd_len = strlen(commands[i].name);

    // Caso 1: Comando exacto (ej: "clear")
    if (strcmp(input, commands[i].name) == 0) {
      commands[i].handler(""); // Pasamos argumento vacío
      return 1;
    }

    // Caso 2: Comando con espacio (ej: "read 100")
    if (strncmp(input, commands[i].name, cmd_len) == 0 &&
        input[cmd_len] == ' ') {
      char *arg = input + cmd_len + 1; // El argumento empieza tras el espacio
      commands[i].handler(arg);
      return 1;
    }
  }
  return 0;
}
