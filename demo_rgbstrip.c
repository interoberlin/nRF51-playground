
//#include <stdio.h>
//#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "delay.h"
#include "nrf_gpio.h"

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

int main()
{
    setup_led_pins();
    
    while(true)
    {
        on(PIN_LED_RED);
        delay_ms(1000);
        off(PIN_LED_RED);

        on(PIN_LED_GREEN);
        delay_ms(1000);
        off(PIN_LED_GREEN);
        
        on(PIN_LED_BLUE);
        delay_ms(1000);
        off(PIN_LED_BLUE);
    }

    return 0;
}
