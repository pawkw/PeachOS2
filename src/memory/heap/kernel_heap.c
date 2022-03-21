#include "kernel_heap.h"
#include "heap.h"
#include "config.h"
#include "kernel.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kernel_heap_init(void)
{
    int total_table_entries = PEACHOS_HEAP_SIZE_BYTES / PEACHOS_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)(PEACHOS_HEAP_TABLE_ADDRESS);
    kernel_heap_table.total_entries = total_table_entries;

    void* end_address = (void*)(PEACHOS_HEAP_ADDRESS + PEACHOS_HEAP_SIZE_BYTES);
    int response = heap_create(&kernel_heap, (void*)(PEACHOS_HEAP_ADDRESS), end_address, &kernel_heap_table);
    if(response < 0)
    {
        print("Error: Did not create heap.\n");
        // TODO: Kernel panic.
        while(1);
    }
}

void* kernel_malloc(size_t size)
{
    return heap_malloc(&kernel_heap, size);
}

void kernel_free(void* address)
{
    if(heap_free(&kernel_heap, address) < 0)
    {
        print("Kernel free memory error.\n");
        // TODO: Kernel panic.
        while(1);
    }
}