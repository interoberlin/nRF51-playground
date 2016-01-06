
/**
 * Delay routines library
 * for Cortex M0-based embedded systems running at 16 MHz
 *
 * Author:  Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#include "delay.h" 

void delay_us(uint32_t us)
{
    // CPU operating at 16 MHz
    asm volatile (
        "start: nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "sub %[cycles], #1\n\t"
        "bne start\n\t"
        : /* empty */ : [cycles] "l" (us)
    );
}

void delay_ms(uint32_t ms)
{
    for (uint32_t i=0; i<ms; i++)
        delay_us(999);
}
