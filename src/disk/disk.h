#ifndef DISK_H
#define DISK_H

typedef unsigned int PEACHOS_DISK_TYPE;

// A real physical hard disk.
#define PEACHOS_DISK_TYPE_REAL 0

struct disk
{
    PEACHOS_DISK_TYPE disk_type;
    int sector_size;
};

void disk_search_and_init(void);
struct disk* disk_get(int disk_number);
int disk_read_block(struct disk* disk_to_read, unsigned int lba, int total_blocks_to_read, void* destination_buffer);

#endif