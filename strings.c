
#include "strings.h"

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
