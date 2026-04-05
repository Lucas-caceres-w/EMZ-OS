ENTRY(kernel_main)                /* El punto de entrada es la función kernel_main */
OUTPUT_FORMAT(binary)             /* Queremos un archivo .bin puro, sin cabeceras ELF */

SECTIONS
{
    . = 0x10000;                  /* Empezamos en la dirección que definiste en ASM */

    .text : {
        *(.text.entry)            /* Primero va lo que etiquetamos como entrada */
        *(.text)                  /* Luego el resto del código */
    }

    .rodata : { *(.rodata) }      /* Cadenas de texto (strings) */
    .data   : { *(.data) }        /* Variables inicializadas */
    .bss    : { 
                *(.bss)
                *(COMMON)
              }         /* Variables sin inicializar */
}