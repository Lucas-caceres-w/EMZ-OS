#ifndef SHELL_H
#define SHELL_H
#define MAX_BUFFER_SIZE 256

// Definimos el tipo de la función (un puntero a función)
typedef void (*command_handler_t)(char *arg);

// Definimos la estructura
typedef struct {
    char *name;
    command_handler_t handler;
} shell_command_t;


// Declaramos las funciones para que otros archivos las vean
void shell_cmd_help(char *arg); // Aunque no use 'arg', debe recibirlo
void shell_reboot(char *arg);
void shell_shutdown(char *arg);
void shell_cmd_mem(char *arg);
void kheap_stats(char *arg);
void shell_cmd_read(char *arg); // Esta sí lo usará
void shell_cmd_write(char *arg);
int find_and_run_command(char *input); // Prototipo

#endif