#ifndef IDT_H
#define IDT_H
#include <stdint.h>

struct idt_desc
{
    uint16_t offset_1; // Lower 16 bits.
    uint16_t selector; // Selector in GDT.
    uint8_t zero;      // Reserved.
    uint8_t type_attributes;      // Descriptor type and attributes.
    uint16_t offset_2; // Upper 16 bits.
} __attribute__((packed));

struct idtr_desc
{
    uint16_t limit;    // Size of table - 1.
    uint32_t base;     // Base address of the table.
} __attribute__((packed));

void idt_init(void);
#endif