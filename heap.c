
#include "heap.h"

extern uint32_t heap_begin;
extern uint32_t heap_end;

uint8_t *heap_cursor = (uint8_t*) &heap_begin;

void* malloc(uint32_t size)
{
    // out of memory
    if ((uint32_t) heap_cursor+size >= heap_end)
        return NULL;

    // reserve memory and adjust heap cursor
    uint8_t *p = heap_cursor;
    heap_cursor += size;
    return p;
}

void free(void* ptr)
{
    // do nothing
}
