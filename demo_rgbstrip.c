
//#include <stdio.h>
//#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "delay.h"
#include "nrf_gpio.h"

#include "random.h"

/*
 * Color LEDs
 */
#define PIN_LED_RED         7
#define PIN_LED_GREEN       5
#define PIN_LED_BLUE        6

void setup_led_pins()
{
    nrf_gpio_pin_dir_set(PIN_LED_GREEN,         NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_LED_BLUE,          NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_LED_RED,           NRF_GPIO_PIN_DIR_OUTPUT);
}

#define on(led)     nrf_gpio_pin_set(led)
#define off(led)    nrf_gpio_pin_clear(led)

typedef struct
{
    uint8_t red, green, blue;
} rgb_t;

/*
 * One "pulse": Switch LEDs on, make 256 steps
 */
void PWM(rgb_t *pwm)
{
    if (pwm->red > 0)
        on(PIN_LED_RED);
    if (pwm->green > 0)
        on(PIN_LED_GREEN);
    if (pwm->blue > 0)
        on(PIN_LED_BLUE);

    /*
     * PWM with 100Hz:
     * 256 steps in 10ms, 39us per step
     */
    uint32_t i;
    for (i=0; i<255; i++)
    {
        if (pwm->red == i)
            off(PIN_LED_RED);
        if (pwm->green == i)
            off(PIN_LED_GREEN);
        if (pwm->blue == i)
            off(PIN_LED_BLUE);
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

int main()
{
    setup_led_pins();

    // current color
    volatile rgb_t pwm;

    // next color
    volatile rgb_t color;
    
    // enable random number generator
    RNG_SHORTS = RNG_SHORTCUT_VALRDY_STOP;
    RNG_START = 1;

    while(true)
    {
        // fade towards color
        while (color.red != pwm.red || color.green != pwm.green || color.blue != pwm.blue)
        {
            PWM(&pwm);

//            for (uint8_t i=0; i<1; i++)
//            {
                if (color.red > pwm.red)
                    pwm.red++;
                if (color.red < pwm.red)
                    pwm.red--;

                if (color.green > pwm.green)
                    pwm.green++;
                if (color.green < pwm.green)
                    pwm.green--;

                if (color.blue > pwm.blue)
                    pwm.blue++;
                if (color.blue < pwm.blue)
                    pwm.blue--;
//            }
        }

        // display this color for a while
        for (uint8_t i=0; i<100; i++)
            PWM(&color);

        // choose new, random color
        color.red   = random();
        color.green = random();
        color.blue  = random();
    }

    return 0;
}
