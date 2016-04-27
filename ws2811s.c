
/**
 * This is a library to control
 * RGB LEDs attached to a
 * WS2811S Shift Register PWM Controller
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPL v3
 */

#include "ws2811s.h"

#define LOW_SPEED 1

void HIGH(uint8_t pin)
{
    nrf_gpio_pin_set(pin);
}

void LOW(uint8_t pin)
{
    nrf_gpio_pin_clear(pin);
}

void T0H()
{
    if (LOW_SPEED)
        // 0.5us = 8 instructions
        asm volatile("nop; nop; nop; nop; nop; nop; nop; nop;");
    else
        asm volatile("nop; nop; nop; nop;");
}

void T1H()
{
    if (LOW_SPEED)
        // 1.2us = 19.2 instructions
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop;"
        );
    else
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
        );
}

void T0L()
{
    if (LOW_SPEED)
        // 2.0us = 32 instructions
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop;"
        );
    else
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop;"
        );
}

void T1L()
{
    if (LOW_SPEED)
        // 1.3us = 20.8 instructions
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
                "nop;"
        );
    else
        asm volatile
        (
                "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"
        );

}

void RES()
{
    // >50us = >800 instructions
    delay_us(50);
}

void send_one(uint8_t pin)
{
    HIGH(pin);
    T1H();
    LOW(pin);
    T1L();
}

void send_zero(uint8_t pin)
{
    HIGH(pin);
    T0H();
    LOW(pin);
    T0L();
}

// send 24 bits of data
void ws2811s_send(uint8_t pin, uint32_t color)
{
    for (uint8_t i=0; i<24; i++)
    {
        if (color & (1 << 23))
            send_one(pin);
        else
            send_zero(pin);
        color = color << 1;
    }
}

// reset bus to apply sent data
void ws2811s_reset(uint8_t pin)
{
    LOW(pin);
    RES();
}
