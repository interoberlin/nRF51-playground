
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
#include "spi_master.h"
#include "fifo.h"

// prepare pin
void ws2811_init(uint8_t pin, uint32_t spi_device_to_use);

// append 24 bits of data to buffer
void ws2811_write(uint32_t color);

// begin sending the data in the buffer
void ws2811_send();

#endif
