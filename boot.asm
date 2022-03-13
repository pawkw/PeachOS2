ORG 0
BITS 16

_start:
    ; BIOS parameter block.
    jmp short start
    nop
    times 33 db 0

start:
    jmp 0x7c0:cont

int_0:
    ; Divide by zero exception handler.
    mov ah, 0eh
    mov al, '!'
    mov bx, 00
    int 0x10
    iret

cont:
    cli
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0
    mov ss, ax
    mov sp, 0x7c00
    sti

    ; Set interrupt vector table
    mov word[ss:00], int_0
    mov word[ss:02], 0x7c0

    mov si, message
    call print

    mov ax, 0
    div ax

    jmp $

print:
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done

    call print_char
    jmp .loop

.done:
    ret
    
print_char:
    mov ah, 0eh
    int 0x10
    ret

message: db 'Hello from Peach OS', 0
times 510 - ($ - $$) db 0
dw 0xaa55
