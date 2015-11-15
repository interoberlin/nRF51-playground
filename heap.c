
#include "heap.h"

//extern uint32_t heap_begin;
extern uint32_t heap_end;

uint8_t* heap_cursor = (uint8_t*) 0x20007000; //(uint8_t*) heap_begin;

/*
 * The malloc function allocates size number of bytes and
 * returns a pointer to the allocated memory.
 * The memory is not initialized.
 * If size is 0, then NULL is returned.
 */
void* malloc(uint32_t size)
{
    // allocate no memory?
    if (size == 0)
        return NULL;

    // out of memory
    if (((uint32_t) heap_cursor)+size >= (uint32_t) &heap_end)
        return NULL;

    // reserve memory and adjust heap cursor
    uint8_t *p = heap_cursor;
    heap_cursor += size;
    return p;
}

/*
 * The free function frees memory previously claimed using malloc.
 *
 * Memory management would however require more code,
 * so for now this function does nothing.
 */
void free(void* ptr)
{
    // do nothing
}

/*
 * The memset() function fills the first n bytes of the memory area
 * pointed to by s with the constant byte c.
 * Function returns a pointer to the memory area s.
 */
void* memset(void* s, uint8_t c, uint32_t n)
{
    for (uint32_t i=0; i<n; i++)
        *((uint8_t*) (s+i)) = c;
    return s;
}

/*
 * The  memcpy function copies n bytes from memory area src to memory area dest.  The memory areas must not overlap.
 * Function returns a pointer to dest.
 */
void* memcpy(void* dest, void* src, uint32_t n)
{
    uint8_t* s = (uint8_t*) src;
    uint8_t* d = (uint8_t*) dest;
    for (uint32_t i=0; i<n; i++)
        *d++ = *s++;
    return dest;
}
