
/**
 * Simple string manipulation library
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef STRING_H
#define STRING_H

#include <stdint.h>

void nibble2hex(char *s, uint8_t n);
void char2hex(char *s, char *c);

#endif
