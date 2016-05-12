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
 * Revision 1: 2x 6 warm white SMD LEDs
 * Revision 2: SMD RGB LEDs
 */
#define PIN_STAMEN_RED   8
#define PIN_STAMEN_GREEN 6
#define PIN_STAMEN_BLUE  10

/*
 * Calyx lighting:
 * Revision 1: 12 RGB SMD LEDs
 * Revision 2: SMD RGB LEDs
 */
#define PIN_CALYX_RED    7
#define PIN_CALYX_GREEN  9
#define PIN_CALYX_BLUE   5

// short makros to switch LEDs on and off
#define on(led)     nrf_gpio_pin_set(led)
#define off(led)    nrf_gpio_pin_clear(led)

/*
 * Setup LEDs: All pins have direction OUTPUT
 */
void setup_led_pins()
{
    nrf_gpio_pin_dir_set(PIN_STAMEN_RED,    NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_STAMEN_GREEN,  NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_STAMEN_BLUE,   NRF_GPIO_PIN_DIR_OUTPUT);

    off(PIN_STAMEN_RED);
    off(PIN_STAMEN_GREEN);
    off(PIN_STAMEN_BLUE);

    nrf_gpio_pin_dir_set(PIN_CALYX_RED,     NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_CALYX_GREEN,   NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_CALYX_BLUE,    NRF_GPIO_PIN_DIR_OUTPUT);

    off(PIN_CALYX_RED);
    off(PIN_CALYX_GREEN);
    off(PIN_CALYX_BLUE);
}

typedef uint8_t color_t[6];

/*
 * One "pulse": Switch LEDs on, make 256 steps
 */
void PWM(color_t color_current)
{
    // Only switch on, if on time > 0
    if (color_current[0] > 0)
        on(PIN_STAMEN_RED);
    if (color_current[1] > 0)
        on(PIN_STAMEN_GREEN);
    if (color_current[2] > 0)
        on(PIN_STAMEN_BLUE);

    if (color_current[3] > 0)
        on(PIN_CALYX_RED);
    if (color_current[4] > 0)
        on(PIN_CALYX_GREEN);
    if (color_current[5] > 0)
        on(PIN_CALYX_BLUE);

    /*
     * PWM with 100Hz:
     * 256 steps in 10ms, 39us per step
     */
    uint32_t i;
    for (i=0; i<255; i++)
    {
        if (color_current[0] == i)
            off(PIN_STAMEN_RED);
        if (color_current[1] == i)
            off(PIN_STAMEN_GREEN);
        if (color_current[2] == i)
            off(PIN_STAMEN_BLUE);

        if (color_current[3] == i)
            off(PIN_CALYX_RED);
        if (color_current[4] == i)
            off(PIN_CALYX_GREEN);
        if (color_current[5] == i)
            off(PIN_CALYX_BLUE);

        delay_us(20); //39
        // 5: LEDs flicker
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
    while  (
            color_next[0] != color_current[0] ||
            color_next[1] != color_current[1] ||
            color_next[2] != color_current[2] ||
            color_next[3] != color_current[3] ||
            color_next[4] != color_current[4] ||
            color_next[5] != color_current[5]
            )
    {
        // output pulse-width modulation
        uint8_t i;

        for (i=0; i<2; i++)
            PWM(color_current);

        // update PWM timing to new color
        for (i=0; i<6; i++)
        {
            if (color_next[i] > color_current[i])
                color_current[i]++;
            if (color_next[i] < color_current[i])
                color_current[i]--;
        }
    }
}

const color_t color_blood = {255, 0, 0};
const color_t color_lemon = {255, 64, 0};
const color_t color_sunset = {255, 40, 0};
const color_t color_peach = {255, 32, 0};
const color_t color_radioactive = {128, 255, 32};
const color_t color_blacklight = {0, 0, 255};
const color_t color_flamingo = {255, 32, 128};
const color_t color_violet = {255, 0, 255};
const color_t color_blueberry = {80, 0, 255};
const color_t color_arctic = {80, 128, 128};
const color_t color_grass = {255, 180, 0};

void randomize_color(color_t color)
{
    color[0] = 255;
    color[1] = 40;
    color[2] = 0;
    color[3] = 0;
    color[4] = 0;
    color[5] = 0;
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
        for (uint8_t i=0; i<250; i++)
            PWM(color_next);

        // choose new, random color
        randomize_color(color_next);
    }

    return 0;
}
