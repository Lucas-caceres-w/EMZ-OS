# Librerias para la emulación del OS-EMZ

## Compilación y Ensamblado

- gcc: El compilador de C.
 
- nasm: El ensamblador para los archivos .asm.
 
- make: Para automatizar la compilación con un solo comando.
 
- binutils: Contiene ld (el enlazador) y objdump (para ver el código binario).

## Manejo de Imágenes de Disco (.img)

- mtools: Para copiar archivos dentro del .img sin ser root.

- dd: Herramienta de terminal para pegar el kernel en sectores específicos del disco.

- dosfstools: Para darle formato FAT al disco si lo necesitas.

## Emulación y Depuración

- qemu-system-i386: El emulador donde corre tu sistema.

- gdb: El depurador para encontrar errores en el código C mientras corre en QEMU.

## Dependencias del Sistema (Linux/Ubuntu)

- build-essential: Paquete base que trae gcc, make y librerías básicas.

- libsdl2-dev: Para que la ventana de QEMU funcione bien con el teclado.

- libc6-dev-i386: Librerías de 32 bits si estás en un Linux de 64 bits.

## Herramientas de Inspección

- hexedit o bless: Para abrir el archivo .img y ver byte por byte si tu usuario se guardó en el sector 100.