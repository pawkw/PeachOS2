section .asm

global idt_load

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8] ; Get the address.
    lidt [ebx]         ; Load interrupt descriptor table.

    pop ebp
    ret