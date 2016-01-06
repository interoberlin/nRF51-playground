
#include "strings.h"

/*
 * Count the number of characters in the string until the null terminator.
 * The null terminator must occur, otherwise string length is counted until the end of heap.
 */
uint32_t strlen(char* s)
{
    uint32_t len = 0;
    extern uint32_t heap_end;

    // keep counting until end of heap is reached
    // or reached character is a null terminator
    while (((uint32_t) s)+len < (uint32_t) &heap_end && *(s+len) != 0)
        len++;

    // heap end?
    if (((uint32_t) s)+len >= (uint32_t) &heap_end)
        return 0;

    return len;
}

/*
 * Writes the one-character hexadecimal representation
 * of a 4-bit character (nibble) to a string
 */
void nibble2hex(char *s, uint8_t n)
{
    if (n < 10)
        *s = 0x30+n;
    else if (n < 16)
        *s = 0x57+n;
    else
        *s = 0x20;
}

/*
 * Writes the two-character hexadecimal representation
 * of an 8-bit character to a string
 */
void char2hex(char *s, char *c)
{
    // higher nibble
    nibble2hex( s,  ((*c) & 0xF0) >> 4 );
    // lower nibble
    nibble2hex( s+1, (*c) & 0x0F );
}
