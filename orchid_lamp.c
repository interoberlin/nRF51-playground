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

typedef struct
{
    uint8_t red, green, blue, white;
} color_t;

/*
 * One "pulse": Switch LEDs on, make 256 steps
 */
void PWM(color_t *pwm)
{
    if (pwm->red > 0)
        on(PIN_RED);
    if (pwm->green > 0)
        on(PIN_GREEN);
    if (pwm->blue > 0)
        on(PIN_BLUE);
    if (pwm->white > 0)
        on(PIN_WARMWHITE);

    /*
     * PWM with 100Hz:
     * 256 steps in 10ms, 39us per step
     */
    uint32_t i;
    for (i=0; i<255; i++)
    {
        if (pwm->red == i)
            off(PIN_RED);
        if (pwm->green == i)
            off(PIN_GREEN);
        if (pwm->blue == i)
            off(PIN_BLUE);
        if (pwm->white == i)
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

void fade_towards(color_t* color, color_t* pwm)
{
    while  (color->red   != pwm->red   ||
            color->green != pwm->green ||
            color->blue  != pwm->blue  ||
            color->white != pwm->white)
    {
        PWM(pwm);

        // update color
        if (color->red > pwm->red)
            pwm->red++;
        if (color->red < pwm->red)
            pwm->red--;

        if (color->green > pwm->green)
            pwm->green++;
        if (color->green < pwm->green)
            pwm->green--;

        if (color->blue > pwm->blue)
            pwm->blue++;
        if (color->blue < pwm->blue)
            pwm->blue--;

        if (color->white > pwm->white)
            pwm->white++;
        if (color->white < pwm->white)
            pwm->white--;
    }
}

int main()
{
    setup_led_pins();

    // current color
    color_t pwm;

    // next color
    color_t color_next;
    
    // enable random number generator
    RNG_SHORTS = RNG_SHORTCUT_VALRDY_STOP;
    RNG_START = 1;

    // forever
    while(true)
    {
        // fade towards color
        fade_towards(&color_next, &pwm);

        // display this color for a while
        for (uint8_t i=0; i<200; i++)
            PWM(&color_next);

        // choose new, random color
        color_next.red   = random();
        color_next.green = random();
        color_next.blue  = random();
        color_next.white  = random();
    }

    return 0;
}
