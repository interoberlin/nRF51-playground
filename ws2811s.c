
/**
 * This is a library to control
 * RGB LEDs attached to a
 * WS2811S Shift Register PWM Controller
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPL v3
 */

#include "ws2811s.h"

//#define LOW_SPEED

uint32_t mask = 0;

// using defines instead of functions drastically increases switching speed

#define pin_set()    NRF_GPIO->OUTSET = mask;
#define pin_clear()  NRF_GPIO->OUTCLR = mask;

// 1.18us
#define ONE_HIGH() \
{ \
    pin_set(); \
    asm volatile("nop;nop;nop;nop;nop;nop;nop;nop;nop;"); \
}

// 1.30us
#define ONE_LOW() \
{ \
    pin_clear(); \
    asm volatile("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;"); \
}

// 616ns
#define ZERO_HIGH() pin_set();

// 2.040us
#define ZERO_LOW() \
{ \
    pin_clear(); \
    asm volatile("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;"); \
}

#define ONE()  ONE_HIGH();  ONE_LOW();
#define ZERO() ZERO_HIGH(); ZERO_LOW();

void T0H()
{
    #ifdef LOW_SPEED
        // 0.5us = 8 instructions
        asm volatile("nop; nop; nop; nop; nop; nop; nop; nop;");
    #else
        asm volatile("nop; nop; nop; nop;");
    #endif
}

void T1H()
{
    #ifdef LOW_SPEED
        // 1.2us = 19.2 instructions
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop;"
        );
    #else
        // 10 instr. => 5.2us?
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
        );
    #endif
}

void T0L()
{
    #ifdef LOW_SPEED
        // 2.0us = 32 instructions
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop;"
        );
    #else
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop;"
        );
    #endif
}

void T1L()
{
    #ifdef LOW_SPEED
        // 1.3us = 20.8 instructions
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop;"
        );
    #else
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
        );
    #endif
}

void RES()
{
    // >50us = >800 instructions
    delay_us(50); // results in 76us
    //delay_us(25); // oscilloscope says 48us
    //delay_us(40); // 54.8us
}

void on()
{
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();

    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();

    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();

    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();

    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
    ONE_HIGH();
    ONE_LOW();
}

void off()
{
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();

    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();

    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();

    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();

    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
    ZERO_HIGH();
    ZERO_LOW();
}

// send 24 bits of data
void ws2811s_send(uint8_t pin, uint32_t color)
{
    mask = 1 << pin;

    on();

    delay_ms(1000);

    off();

    delay_ms(1000);
}

// reset bus to apply sent data
void ws2811s_reset(uint8_t pin)
{
    pin_clear();
    RES();
}
