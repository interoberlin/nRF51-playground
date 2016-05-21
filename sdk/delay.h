
/**
 * Delay routines library
 * for Cortex M0-based embedded systems running at 16 MHz
 *
 * Author:  Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

void delay_us(uint32_t us);

void delay_ms(uint32_t ms);

#endif
