
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
#include "ws2811s.h"

// to which pin is the WS2811S attached?
#define PIN_WS2811S   10

void setup()
{
    nrf_gpio_pin_dir_set(PIN_WS2811S, NRF_GPIO_PIN_DIR_OUTPUT);
    ws2811s_reset(PIN_WS2811S);

    init_random();
}

void loop()
{
    //uint32_t color = (random() << 16) | (random() << 8) | random();
    uint32_t color = 0x00ff00;

    ws2811s_send(PIN_WS2811S, color);
    ws2811s_reset(PIN_WS2811S);

//    delay_ms(10);
}

int main()
{
    setup();
    while (true)
        loop();
    return 0;
}
