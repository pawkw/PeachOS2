#include "kernel.h"
#include <stddef.h>
#include <stdint.h>

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
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while(str[len++]);
    return len;
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
    g_foreground_colour = 0;
    g_background_colour = 3;
    video_memory = (uint16_t*)(0xb8000);
    uint16_t blank = terminal_make_char(' ', terminal_make_colour(g_foreground_colour, g_background_colour));
    for(int i = 0; i < (VGA_WIDTH*VGA_HEIGHT); i++)
        video_memory[i] = blank;
    cursor_x = 0;
    cursor_y = 0;
}

void kernel_main(void)
{
    terminal_initialize(0, 3);
    print("Peach OS\n2022.");
}