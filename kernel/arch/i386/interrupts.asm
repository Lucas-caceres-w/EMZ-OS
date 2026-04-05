; --- Definiciones Externas (Funciones en C) ---
extern isr_handler
extern irq_handler

; --- Exportar Etiquetas para la IDT ---
global isr0
global isr10
global isr12
global isr13
global isr14
global isr_ignore
global irq0
global irq1

; =============================================================================
; TRAMPOLÍN COMÚN PARA EXCEPCIONES (ISRs)
; =============================================================================
isr_common_stub:
    pushad          ; Guarda EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    mov ax, ds      ; Guarda el segmento de datos actual
    push eax

    mov ax, 0x10    ; Carga el Kernel Data Segment (GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp        ; Empuja el puntero a la estructura registers_t
    call isr_handler
    add esp, 4      ; Limpia el puntero empujado

    pop eax         ; Restaura el segmento original
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popad           ; Restaura registros generales
    add esp, 8      ; Limpia el error_code e int_no del stack
    iret

; =============================================================================
; TRAMPOLÍN COMÚN PARA HARDWARE (IRQs)
; =============================================================================
irq_common_stub:
    pushad
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_handler
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popad
    add esp, 8
    iret

; =============================================================================
; DEFINICIÓN DE CADA INTERRUPCIÓN
; =============================================================================

; 0: Division By Zero (No empuja error code)
isr0:
    push byte 0
    push byte 0
    jmp isr_common_stub

; 10: Invalid TSS (SÍ empuja error code)
isr10:
    push byte 10
    jmp isr_common_stub

; 12: Stack Fault (SÍ empuja error code)
isr12:
    push byte 12
    jmp isr_common_stub

; 13: General Protection Fault (SÍ empuja error code)
isr13:
    push byte 13
    jmp isr_common_stub

; 14: PAGE FAULT (SÍ empuja error code - ¡VITAL PARA EL VMM!)
isr14:
    push byte 14
    jmp isr_common_stub

; Handler para ignorar (No empuja error code)
isr_ignore:
    push byte 0
    push byte 50     ; ID ficticio
    jmp isr_common_stub

; --- IRQs (Hardware) ---
irq0:
    push byte 0
    push byte 32
    jmp irq_common_stub

irq1:
    push byte 0
    push byte 33
    jmp irq_common_stub