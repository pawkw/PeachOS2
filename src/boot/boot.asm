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
    jmp CODE_SEG:load_32
    ; jmp $

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

[BITS 32]
load_32:
    mov eax, 1   ; Sector #.
    mov ecx, 100 ; Number of sectors.
    mov edi, 0x0100000 ; Destination
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax ; Back up lba sector.
    mov dx, 0x1f6
    shr eax, 24
    or eax, 0xe0 ; Select master drive.
    out dx, al   ; Send bits 24-31 of sector # to lba.

    mov eax, ecx
    mov dx, 0x1f2
    out dx, al   ; Send total sectors to read.

    mov eax, ebx
    mov dx, 0x1f3
    out dx, al   ; Send bits 0-7 of sector # to lba.

    mov eax, ebx
    mov dx, 0x1f4
    shr eax, 8
    out dx, al    ; Send bits 8-15 of sector # to lba.

    mov eax, ebx
    mov dx, 0x1f5
    shr eax, 16
    out dx, al    ; Send bits 16-23 of sector # to lba.

    mov al, 0x20
    mov dx, 0x1f7
    out dx, al

; Read sectors.
.next_sector:
    push ecx

.try_read:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_read  ; Try until drive is ready.

    ; Read a sector.
    mov ecx, 256
    mov dx, 0x1f0
    rep insw

    pop ecx
    loop .next_sector

    ret

times 510 - ($ - $$) db 0
dw 0xaa55
