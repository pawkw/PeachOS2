#include "status.h"
#include "io/io.h"
#include <stdint.h>

int disk_read_sector(int lba, int total_blocks_to_read, void* destination_buffer)
{
    // Select master drive and send bits 24-31 of address.
    outb(0x1f6, (unsigned char)(lba >> 24) | 0xe0);

    // Send total to read.
    outb(0x1f2, (unsigned char)total_blocks_to_read);

    // Send bits 0-7 of address.
    outb(0x1f3, (unsigned char)(lba & 0xff));

    // Send bits 8-15 of address.
    outb(0x1f4, (unsigned char)(lba >> 8));

    // Send bits 16-23 of address.
    outb(0x1f5, (unsigned char)(lba >> 16));

    // Send command 'read with retry'.
    outb(0x1f7, 0x20);

    uint16_t* current_location = (uint16_t*)destination_buffer;
    for(int i = 0; i < total_blocks_to_read; i++)
    {
        // Wait for disk controller to be ready.
        char c = insb(0x1f7);
        while(!(c & 0x08)) // Requires service flag.
        {
            c = insb(0x1f7);
        }

        for(int j = 0; j < 256; j++)
        {
            *current_location = insw(0x1f0);
            current_location++;
        }
    }
    return PEACHOS_ALL_OK;
}