#ifndef HEAP_H
#define HEAP_H

#include "config.h"
#include <stdint.h>
#include <stddef.h>

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 1
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST 0b01000000

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table
{
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t total_entries;
};

struct heap
{
    struct heap_table* table;
    void* start_address;
};

int heap_create(struct heap* heap, void* data_pool, void* end_of_heap, struct heap_table* table);
void* heap_malloc(struct heap* heap, size_t size);
int heap_free(struct heap* heap, void* memory_address);

#endif