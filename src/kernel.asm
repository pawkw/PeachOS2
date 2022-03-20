[BITS 32]
global _start
extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; Enable A20 line.
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Remap the master PIC.
    mov al, 00010001b
    out 0x20, al   ; Set PIC to init mode.

    mov al, 0x20   ; Set master PIC IRQs to start at int 0x20.
    out 0x21, al

    mov al, 00000001b
    out 0x21, al   ; Put PIC in x86 mode.

    call kernel_main

    jmp $

; Pad the file so that the C code is aligned.
times 512 - ($ - $$) db 0