#include "paging.h"
#include "memory/heap/kernel_heap.h"

static uint32_t* current_directory = 0;

void paging_load_directory(uint32_t* directory);

struct paging_4GB_chunk* paging_new_4GB(uint8_t flags)
{
    uint32_t* directory = kernel_zalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_DIRECTORY);
    int offset = 0;
    for(int i = 0; i < PAGING_TOTAL_ENTRIES_PER_DIRECTORY; i++)
    {
        uint32_t* directory_entry = kernel_zalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for(int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++)
        {
            directory_entry[j] = (offset + (j * PAGING_PAGE_SIZE)) | flags;
        }
        offset += PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE;
        directory[i] = (uint32_t)directory_entry | flags | PAGING_IS_WRITABLE;
    }

    struct paging_4GB_chunk* chunk_4GB = kernel_zalloc(sizeof(struct paging_4GB_chunk));
    chunk_4GB->directory_entry = directory;
    return chunk_4GB;
}

void paging_switch(uint32_t* directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

uint32_t* paging_4GB_chunk_get_directory(struct paging_4GB_chunk* chunk)
{
    return chunk->directory_entry;
}