#include "paging.h"
#include "memory/heap/kernel_heap.h"
#include "status.h"

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

bool paging_is_aligned(void* address)
{
    return ((uint32_t)address % PAGING_PAGE_SIZE) == 0;
}

int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out)
{
    if(!paging_is_aligned(virtual_address))
        return -PEACHOS_INVALID_ARGUMENT;
    
    *directory_index_out = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);

    return PEACHOS_ALL_OK;
}

int paging_set(uint32_t* directory, void* virtual_address, uint32_t physical_address_with_flags)
{
    if(!paging_is_aligned(virtual_address))
        return -PEACHOS_INVALID_ARGUMENT;
    
    uint32_t directory_index = 0;
    uint32_t table_index = 0;

    int response = paging_get_indexes(virtual_address, &directory_index, &table_index);
    if(response < 0)
        return response;

    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & PAGING_ADDRESS_MASK);
    table[table_index] = physical_address_with_flags;

    return PEACHOS_ALL_OK;
}