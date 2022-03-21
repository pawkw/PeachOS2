[BITS 32]
section .asm

global paging_load_directory
global enable_paging

paging_load_directory:
    push ebp
    mov ebp, esp

    ; Load cr3 with new directory address.
    mov eax, [ebp + 8]
    mov cr3, eax

    pop ebp
    ret

enable_paging:
    push ebp
    mov ebp, esp

    ; Set paging bit on cr0.
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    pop ebp
    ret