
/**
 * Random Number Generator (RNG) library
 * for the Nordic Semiconductor nRF51 series
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 *
 * License: GNU GPLv3
 */

#ifndef RNG_H
#define RNG_H

#include "stdint.h"
#include "delay.h"

#define RNG_BASE                0x4000D000

#define RNG_START               (*(volatile uint32_t*)   (RNG_BASE+0x000))

#define RNG_EVENT_VALRDY        (*(volatile uint32_t*)   (RNG_BASE+0x100))

//...
#define RNG_SHORTS              (*(volatile uint32_t*)   (RNG_BASE+0x200))

#define RNG_SHORTCUT_VALRDY_STOP 1

#define RNG_VALUE               (*(volatile uint32_t*)   (RNG_BASE+0x508))

// Initialize random number generator
void init_random();

// Returns a random number between 0 and 255
uint8_t random();

#endif
