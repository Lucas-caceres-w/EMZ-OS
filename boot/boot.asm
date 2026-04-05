[org 0x7C00]
bits 16

start:

    mov [BOOT_DRIVE], dl

    xor ax, ax
    mov ds, ax
    mov es, ax

    mov sp, 0x7C00

    ; leer stage2 (sector 2)
    mov ah, 0x02
    mov al, 1
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [BOOT_DRIVE]

    mov bx, 0x7E00

    int 0x13

    jc disk_error

    jmp 0x0000:0x7E00

disk_error:
    mov si, msg
print:
    lodsb
    cmp al,0
    je halt
    mov ah,0x0E
    int 0x10
    jmp print

halt:
    jmp $

msg db "Disk error",0
BOOT_DRIVE db 0

times 510-($-$$) db 0
dw 0xAA55