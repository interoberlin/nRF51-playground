/**
 * Orchid lamp
 * Firmware for Elena Wuest's first major Orchid lamp
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPL v3
 */

#include <stdbool.h>
#include <stdint.h>

#include "delay.h"
#include "nrf_gpio.h"

#include "random.h"

/*
 * Stamen lighting:
 * 2x 6 warm white SMD LEDs
 */
#define PIN_WARMWHITE       8

/*
 * Calyx lighting:
 * 12 RGB SMD LEDs
 */
#define PIN_GREEN           7
#define PIN_RED             5
#define PIN_BLUE            6

/*
 * For future purpose:
 * Fibre lighting:
 * WS2811S
 */
#define PIN_WS2811_DATA     10

void setup_led_pins()
{
    nrf_gpio_pin_dir_set(PIN_WARMWHITE,     NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_GREEN,         NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_BLUE,          NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_RED,           NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_WS2811_DATA,   NRF_GPIO_PIN_DIR_OUTPUT);
}

#define on(led)     nrf_gpio_pin_set(led)
#define off(led)    nrf_gpio_pin_clear(led)

typedef uint8_t color_t[4];

/*
 * One "pulse": Switch LEDs on, make 256 steps
 */
void PWM(color_t color_current)
{
    if (color_current[0] > 0)
        on(PIN_RED);
    if (color_current[1] > 0)
        on(PIN_GREEN);
    if (color_current[2] > 0)
        on(PIN_BLUE);
    if (color_current[3] > 0)
        on(PIN_WARMWHITE);

    /*
     * PWM with 100Hz:
     * 256 steps in 10ms, 39us per step
     */
    uint32_t i;
    for (i=0; i<255; i++)
    {
        if (color_current[0] == i)
            off(PIN_RED);
        if (color_current[1] == i)
            off(PIN_GREEN);
        if (color_current[2] == i)
            off(PIN_BLUE);
        if (color_current[3] == i)
            off(PIN_WARMWHITE);
        delay_us(15); //39
    }
}

#define abs(val)    ((val) < 0 ? (-(val)) : (val))

/*
 * Returns a random number between 0 and 255
 */
uint8_t random()
{
    // wait until a random number is available
    while (RNG_EVENT_VALRDY == 0)
        delay_us(10);

    // read random number
    volatile uint8_t result = RNG_VALUE;

    // restart random number generator
    RNG_EVENT_VALRDY = 0;
    RNG_START = 1;

    return result;
}

void fade_towards(color_t color_current, color_t color_next)
{
    while  (color_next[0] != color_current[0] ||
            color_next[1] != color_current[1] ||
            color_next[2] != color_current[2] ||
            color_next[3] != color_current[3])
    {
        // output pulse-width modulation
        PWM(color_current);

        // update PWM timing to new color
        for (uint8_t i=0; i<4; i++)
        {
            if (color_next[i] > color_current[i])
                color_current[i]++;
            if (color_next[i] < color_current[i])
                color_current[i]--;
        }
    }
}

int main()
{
    setup_led_pins();

    // current color
    color_t color_current;

    // next color
    color_t color_next;
    
    // enable random number generator
    RNG_SHORTS = RNG_SHORTCUT_VALRDY_STOP;
    RNG_START = 1;

    // forever
    while(true)
    {
        // fade towards color
        fade_towards(color_current, color_next);

        // display this color for a while
        for (uint8_t i=0; i<200; i++)
            PWM(color_next);

        // choose new, random color
        color_next[0] = random();
        color_next[1] = random();
        color_next[2] = random();
        color_next[3] = random();
    }

    return 0;
}
