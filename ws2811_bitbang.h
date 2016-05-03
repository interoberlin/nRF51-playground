
/**
 * This is a library to control
 * RGB LEDs attached to a
 * WS2811S Shift Register PWM Controller
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPL v3
 */

#ifndef WS2811
#define WS2811

#include <stdint.h>
#include "delay.h"
#include "nrf_gpio.h"

// prepare pin
void ws2811_init(uint8_t pin);

// send 24 bits of data
void ws2811_send(uint8_t pin, uint32_t color);

// reset bus to apply sent data
void ws2811_reset(uint8_t pin);

#endif
