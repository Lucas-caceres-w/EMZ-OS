[org 0x7E00]
bits 16

start:

    mov si, msg
print_loop:
    lodsb
    cmp al, 0
    je load_kernel
    mov ah, 0x0E
    int 0x10
    jmp print_loop

; ======================
; CARGAR KERNEL EN 0x100000
; ======================
load_kernel:
    mov ax, 0x1000  ; Usamos segmento 0
    mov es, ax
    xor bx, bx  ; Offset 0x1000. Dirección física: 0x1000
    
    mov ah, 0x02
    mov al, 50      ; Lee más sectores (50), por si el kernel creció
    mov ch, 0
    mov cl, 3      ; <--- Prueba con el sector 2 (justo tras el bootloader)
    mov dh, 0
    mov dl, 0x80
    int 0x13
    jc disk_error

    mov ax, 0x2401
    int 0x15
    jc a20_error
; ======================
; ACTIVAR PROTECTED MODE
; ======================
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEG:protected_mode

disk_error:
    mov si, err_msg
print_err:
    lodsb
    cmp al, 0
    je halt
    mov ah, 0x0E
    int 0x10
    jmp print_err

halt:
    jmp $

; ======================
; 32 BIT MODE
; ======================
bits 32
protected_mode:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Stack bien ubicado (fuera de donde cargaste el kernel)
    mov esp, 0x90000
    mov ebp, esp
    
    ; Saltar al Kernel (0x10000 es donde cargó ES:BX)
    jmp CODE_SEG:0x10000

hang:
    jmp hang

; ======================
; DATOS AL FINAL
; ======================
msg db "Load kernel emz...",0
err_msg db "Disk read error!",0
a20_error db "Error al cargar",0

gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF       ; Código
    dq 0x00CF92000000FFFF      ; datos
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ 0x08
DATA_SEG equ 0x10