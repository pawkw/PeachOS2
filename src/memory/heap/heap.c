#include "heap.h"
#include "kernel.h"
#include "status.h"
#include "memory/memory.h"
#include <stdint.h>
#include <stdbool.h>

static bool validate_heap_alignment(void* heap_pointer)
{
    return ((uint32_t)heap_pointer % PEACHOS_HEAP_BLOCK_SIZE) == 0;
}

static bool validate_heap_table(void* start_of_heap, void* end_of_heap, struct heap_table* table)
{
    size_t table_size = (size_t)(end_of_heap - start_of_heap);
    size_t total_blocks = table_size / PEACHOS_HEAP_BLOCK_SIZE;

    return table->total_entries == total_blocks;
}

int heap_create(struct heap* heap, void* start_of_heap, void* end_of_heap, struct heap_table* table)
{
    if(!validate_heap_alignment(start_of_heap) || !validate_heap_alignment(end_of_heap))
    {
        print("Heap not aligned.");
        return -PEACHOS_INVALID_ARGUMENT;
    }

    memset(heap, 0, sizeof(struct heap));
    heap->start_address = start_of_heap;
    heap->table = table;

    if(!validate_heap_table(start_of_heap, end_of_heap, table))
    {
        print("Heap table invalid.");
        return -PEACHOS_INVALID_ARGUMENT;
    }
    
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total_entries;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

    return PEACHOS_ALL_OK;
}

static uint32_t round_up_to_block_size(uint32_t value)
{
    if((value % PEACHOS_HEAP_BLOCK_SIZE) == 0)
        return value;
    return (value / PEACHOS_HEAP_BLOCK_SIZE + 1) * PEACHOS_HEAP_BLOCK_SIZE;
}

static int get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0x0f; // Lower nibble.
}

int heap_get_start_block(struct heap* heap, uint32_t number_of_blocks)
{
    struct heap_table* table = heap->table;

    int block_count = 0;
    int start_block = -1;
    
    for(size_t i = 0; i < table->total_entries; i++)
    {
        if(get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE)
        {
            // Does not fit: reset.
            block_count = 0;
            start_block = -1;
            continue;
        }

        if(start_block == -1)
        {
            start_block = i;
        }

        block_count++;

        if(block_count == number_of_blocks)
        {
            // Does fit: exit.
            break;
        }
    }

    if(start_block == -1)
        return PEACHOS_NO_MEMORY;
    
    return start_block;
}

void heap_mark_blocks_taken(struct heap* heap, int start_block, uint32_t number_of_blocks)
{
    uint32_t end_block = start_block + number_of_blocks - 1;
    // TODO: Assert that the block is in bounds.

    for(uint32_t i = start_block; i <= end_block; i++)
    {
        HEAP_BLOCK_TABLE_ENTRY is_first = (i == start_block) ? HEAP_BLOCK_IS_FIRST : 0;
        HEAP_BLOCK_TABLE_ENTRY has_next = (i < end_block) ? HEAP_BLOCK_HAS_NEXT : 0;
        heap->table->entries[i] = is_first | has_next | HEAP_BLOCK_TABLE_ENTRY_TAKEN;
    }
}

void* heap_block_to_address(struct heap* heap, int block)
{
    return (block * PEACHOS_HEAP_BLOCK_SIZE) + heap->start_address;
}

int heap_address_to_block(struct heap* heap, void* address)
{
    return (address - heap->start_address) / PEACHOS_HEAP_BLOCK_SIZE;
}

void* heap_malloc_blocks(struct heap* heap, uint32_t number_of_blocks)
{
    void* address = 0;

    int start_block = heap_get_start_block(heap, number_of_blocks);
    if(start_block < 0)
        return address;
    
    address = heap_block_to_address(heap, start_block);

    // Mark blocks as taken.
    heap_mark_blocks_taken(heap, start_block, number_of_blocks);

    return address;
}

void* heap_malloc(struct heap *heap, size_t size)
{
    size_t aligned_size = round_up_to_block_size((uint32_t) size);
    uint32_t total_blocks = aligned_size / PEACHOS_HEAP_BLOCK_SIZE;

    return heap_malloc_blocks(heap, total_blocks);
}

int heap_mark_blocks_free(struct heap* heap, int start_block)
{
    if((heap->table->entries[start_block] & HEAP_BLOCK_IS_FIRST) == 0)
    {
        print("Invalid start block in heap_mark_blocks_free.\n");
        return -PEACHOS_INVALID_ARGUMENT;
    }

    int current_block = start_block;
    while((heap->table->entries[current_block] & HEAP_BLOCK_HAS_NEXT) != 0)
    {
        heap->table->entries[current_block++] = HEAP_BLOCK_TABLE_ENTRY_FREE;
    }
    heap->table->entries[current_block] = HEAP_BLOCK_TABLE_ENTRY_FREE;

    return PEACHOS_ALL_OK;
}

int heap_free(struct heap* heap, void* memory_address)
{
    // TODO: Assert block is in bounds.
    return heap_mark_blocks_free(heap, heap_address_to_block(heap, memory_address));
}