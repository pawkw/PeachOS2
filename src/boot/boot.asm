ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
    ; BIOS parameter block.
    jmp short start
    nop
    times 33 db 0

start:
    jmp 0:cont

cont:
    cli
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti

load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ; jmp CODE_SEG:load_32
    jmp $

gdt_start:
gdt_null:
    dd 0
    dd 0

; Offset 0x8.
gdt_code:     ; CS should point here.
    dw 0xffff ; Segment limit 0-15 bits.
    dw 0      ; Base address 0-15 bits.
    db 0      ; Base address 16-23 bits.
    db 0x9a   ; Access byte.
    db 11001111b ; High and low 4 bit flags.
    db 0      ; Base 24-31 bits.

; Offset 0x10.
gdt_data:     ; DS, ES, SS, FS, GS should point here.
    dw 0xffff ; Segment limit 0-15 bits.
    dw 0      ; Base address 0-15 bits.
    db 0      ; Base address 16-23 bits.
    db 0x92   ; Access byte.
    db 11001111b ; High and low 4 bit flags.
    db 0      ; Base 24-31 bits.
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Size.
    dd gdt_start ; Offset.

times 510 - ($ - $$) db 0
dw 0xaa55
