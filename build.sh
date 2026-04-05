#!/bin/bash
set -e

# --- CONFIGURACIÓN ---
BUILD_DIR="build"
INCLUDE_DIR="include"
# Banderas para un kernel limpio
CFLAGS="-m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector -I$INCLUDE_DIR -nostdinc -fno-builtin -g -mpreferred-stack-boundary=2"
LDFLAGS="-m elf_i386 -T linker.ld"

echo "--- Iniciando Build con Estructura Espejo ---"

# 1. Limpieza inicial (solo la carpeta build)
rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR

# 2. Compilar Bootloader (Salida directa a build/)
echo "[1/4] Compilando Bootloader..."
nasm boot/boot.asm -f bin -o $BUILD_DIR/boot.bin
nasm boot/stage2.asm -f bin -o $BUILD_DIR/stage2.bin

# 3. COMPILACIÓN RECURSIVA
OBJECTS=""

echo "[2/4] Procesando fuentes en /kernel..."

# Función para compilar y mantener estructura
compile_file() {
    local file=$1
    local type=$2 # "c" o "asm"
    
    # Obtener la ruta relativa eliminando "kernel/" (ej: arch/i386/idt.c)
    local rel_path=${file#kernel/}
    # Definir la ruta del objeto en build/ (ej: build/arch/i386/idt.o)
    local obj_path="$BUILD_DIR/objs/${rel_path%.*}.o"
    # Crear el subdirectorio necesario dentro de build/
    mkdir -p "$(dirname "$obj_path")"

    if [ "$type" == "c" ]; then
        echo "  CC  $file"
        gcc $CFLAGS -c "$file" -o "$obj_path"
    else
        echo "  AS  $file"
        nasm -f elf32 "$file" -o "$obj_path"
    fi
    
    # Guardar la ruta del objeto para el linker
    OBJECTS="$OBJECTS $obj_path"
}

# Buscar y procesar archivos .c
for f in $(find kernel -name "*.c"); do
    compile_file "$f" "c"
done

# Buscar y procesar archivos .asm (excluyendo boot/ si estuviera dentro)
for f in $(find kernel -name "*.asm"); do
    compile_file "$f" "asm"
done

# 4. LINKEO
echo "[3/4] Enlazando objetos"
ld $LDFLAGS $OBJECTS -o $BUILD_DIR/kernel.bin

# 5. IMAGEN DE DISCO
echo "[4/4] Generando os.img..."
truncate -s 512 $BUILD_DIR/stage2.bin
truncate -s %512 $BUILD_DIR/kernel.bin
cat $BUILD_DIR/boot.bin $BUILD_DIR/stage2.bin $BUILD_DIR/kernel.bin > $BUILD_DIR/os_full.bin

# Crear imagen de floppy de 1.44MB estándar
dd if=/dev/zero of=os.img bs=512 count=2880 status=none

dd if=$BUILD_DIR/os_full.bin of=os.img conv=notrunc status=none

echo "SISTEMA DE ARCHIVOS EMZ-OS LISTO" | dd of=os.img bs=512 seek=100 conv=notrunc status=none

echo "-------------------------------------------"
echo "BUILD EXITOSO."