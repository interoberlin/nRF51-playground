
//#include <stdio.h>
//#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"

/*
 * Color LEDs
 */

#define PIN_LED_RED_BRIGHT  1
#define PIN_LED_GREEN       2
#define PIN_LED_YELLOW_1    3
#define PIN_LED_WHITE_1     4
#define PIN_LED_YELLOW_2    5
#define PIN_LED_BLUE        6
#define PIN_LED_RED         7
#define PIN_LED_WHITE_2     8

void setup_led_pins()
{
    nrf_gpio_pin_dir_set(PIN_LED_RED_BRIGHT,    NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_LED_GREEN,         NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_LED_YELLOW_1,      NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_LED_WHITE_1,       NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_LED_YELLOW_2,      NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_LED_BLUE,          NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_LED_RED,           NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(PIN_LED_WHITE_2,       NRF_GPIO_PIN_DIR_OUTPUT);
}

#define on(led)     nrf_gpio_pin_set(led)
#define off(led)    nrf_gpio_pin_clear(led)


int main()
{
    setup_led_pins();
    
    while(true)
    {
        off(PIN_LED_WHITE_2);
        on(PIN_LED_RED_BRIGHT);
        nrf_delay_ms(1000);

        off(PIN_LED_RED_BRIGHT);
        on(PIN_LED_GREEN);
        nrf_delay_ms(1000);
        
        off(PIN_LED_GREEN);
        on(PIN_LED_YELLOW_1);
        nrf_delay_ms(1000);
        
        off(PIN_LED_YELLOW_1);
        on(PIN_LED_WHITE_1);
        nrf_delay_ms(1000);
        
        off(PIN_LED_WHITE_1);
        on(PIN_LED_YELLOW_2);
        nrf_delay_ms(1000);
        
        off(PIN_LED_YELLOW_2);
        on(PIN_LED_BLUE);
        nrf_delay_ms(1000);
        
        off(PIN_LED_BLUE);
        on(PIN_LED_RED);
        nrf_delay_ms(1000);
        
        off(PIN_LED_RED);
        on(PIN_LED_WHITE_2);      
        nrf_delay_ms(1000);
    }

    return 0;
}
