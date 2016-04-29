
/*
 * This example shall demonstrate
 * how to control RGB LEDs attached to a
 * WS2811S Shift Register PWM Controller
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPL v3
 */

#include <stdbool.h>
#include <stdint.h>

#include "delay.h"
#include "random.h"
#include "ws2811.h"

// to which pin is the WS2811S attached?
#define PIN_WS2811S   10

void setup()
{
    ws2811_init(PIN_WS2811S);

    init_random();
}

void loop()
{
    uint32_t color = (random() << 16) | (random() << 8) | random();

    ws2811_send(PIN_WS2811S, color);
    ws2811_reset(PIN_WS2811S);

//    delay_ms(10);
}

int main()
{
    setup();
    while (true)
        loop();
    return 0;
}
