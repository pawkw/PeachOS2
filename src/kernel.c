#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include "io/io.h"
#include "idt/idt.h"
#include "memory/heap/kernel_heap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"

int cursor_x = 0;
int cursor_y = 0;
uint16_t* video_memory = 0;
uint8_t g_foreground_colour = 0;
uint8_t g_background_colour = 0;

uint16_t terminal_make_char(char content, char colour)
{
    return (colour<<8)|content;
}

uint8_t terminal_make_colour(char foreground_colour, char background_colour)
{
    return background_colour << 4 | foreground_colour;
}

void terminal_put_char(int x, int y, char content, uint8_t foreground_colour, uint8_t background_colour)
{
    video_memory[y*VGA_WIDTH+x] = terminal_make_char(content, terminal_make_colour(foreground_colour, background_colour));
}

void update_cursor(void)
{
    uint16_t position = cursor_y * VGA_WIDTH + cursor_x;

    outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (position & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((position >> 8) & 0xFF));
}

void terminal_write_char(char content) 
{
    if(content != '\n')
        terminal_put_char(cursor_x++, cursor_y, content, g_foreground_colour, g_background_colour);
    else
    {
        cursor_y++;
        cursor_x = 0;
    }

    if(cursor_x >= VGA_WIDTH)
    {
        cursor_x = 0;
        cursor_y++;
    }
    if(cursor_y >= VGA_HEIGHT)
        cursor_y = 0;

    update_cursor();
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while(str[len++]);
    return --len;
}

void print(const char* str)
{
    size_t len = strlen(str);
    for(int i = 0; i < len; i++)
    {
        terminal_write_char(str[i]);
    }
}

void terminal_initialize(uint8_t foreground_colour, uint8_t background_colour) 
{
    g_foreground_colour = foreground_colour;
    g_background_colour = background_colour;
    video_memory = (uint16_t*)(0xb8000);
    uint16_t blank = terminal_make_char(' ', terminal_make_colour(g_foreground_colour, g_background_colour));
    for(int i = 0; i < (VGA_WIDTH*VGA_HEIGHT); i++)
        video_memory[i] = blank;
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

static struct paging_4GB_chunk* kernel_chunk = 0;

void kernel_main(void)
{
    terminal_initialize(0, 3);
    print("Peach OS\n2022.\n");

    // Initialize the heap.
    kernel_heap_init();

    // Initialize the interrupt descriptor table.
    idt_init();

    // Set up and enable paging.
    // TODO: This is a kernel page. It should not be accessible by all.
    kernel_chunk = paging_new_4GB(PAGING_IS_PRESENT | PAGING_IS_WRITABLE | PAGING_ACCESS_BY_ALL);
    if(kernel_chunk == 0)
    {
        print("Kernel chunk is zero.\n");
    }
    paging_switch(paging_4GB_chunk_get_directory(kernel_chunk));
    enable_paging();

    char buffer[512];
    disk_read_sector(0, 1, buffer);
    
    enable_interrupts();

}