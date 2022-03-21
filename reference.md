## List of CPU exception interrupts
wiki.osdev.org/exceptions

## BIOS interrupt list
http://www.ctyme.com/rbrown.htm

## Enter protected mode
https://wiki.osdev.org/Protected_Mode

## Build a cross compiler
https://wiki.osdev.org/GCC_Cross-Compiler

## Read ATA sectors without BIOS
https://wiki.osdev.org/ATA_read/write_sectors

# GDB commands

gdb
add-symbol-file ./build/kernelfull.o 0x100000
target remote | qemu-system-i386 -hda ./bin/os.bin -S -gdb stdio