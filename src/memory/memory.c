#include "memory.h"

void* memset(void* pointer, int fill_char, size_t size)
{
    char* char_pointer = (char*) pointer;
    for(int i = 0; i < size; i++)
        char_pointer[i] = (char) fill_char;
    return pointer;
}