#define CONFIG_SECTOR 100
#define EMZ_MAGIC_FS 0x454D5A19 // "EMZ" + un número de versión

#include "libs/types.h"

typedef struct {
    uint32_t magic;           // Para verificar si el sector tiene datos de EMZ-OS
    char user_name[32];       // Nombre del dueño del sistema
    uint8_t shell_color;      // Color preferido (0-15)
    uint8_t installed;        // 1 si ya se corrió el setup
    char padding[470];        // Rellenamos para completar los 512 bytes del sector
} __attribute__((packed)) emz_config_t;