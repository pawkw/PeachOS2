#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"

struct idt_desc idt_descriptors[PEACHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* pointer);
extern void int21h(void);
extern void no_interrupt(void);

void int21h_handler(void)
{
    print("Key pressed.\n");
    int_acknowledge;
}

void no_interrupt_handler(void)
{
    int_acknowledge;
}

void divide_by_zero()
{
    print("Divide by zero exception.\n");
    while(1);
}

void idt_set(int interrupt_number, void* address)
{
    struct idt_desc* descriptor = &idt_descriptors[interrupt_number];
    descriptor->offset_1 = (uint32_t) address & 0x0000ffff;
    descriptor->selector = KERNEL_CODE_SELECTOR;
    descriptor->zero = 0;
    // 32 bit interrupt gate with privilege level 3.
    descriptor->type_attributes = 0xee;
    descriptor->offset_2 = (uint32_t) address >> 16;
}

void idt_init(void)
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t) idt_descriptors;

    // Set temporary handler
    for(int i = 0; i < PEACHOS_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, no_interrupt);
    }

    idt_set(0, divide_by_zero);
    idt_set(0x21, int21h);

    // Load interrupt descriptor table.
    idt_load(&idtr_descriptor);
}