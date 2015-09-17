
/**
* This example shows how to configure GPIO pins as outputs which can also be used to drive LEDs.
* Each LED is set on one at a time and each state lasts 100 milliseconds.
*/

#include <stdbool.h>
#include <stdint.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"

#define LED_PIN 21

void _start()
{
    uint8_t output_state = 0;
    
    nrf_gpio_pin_dir_set(LED_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
    
    while(true)
    {
        if (output_state == 1)
            nrf_gpio_pin_set(LED_PIN);
        else
            nrf_gpio_pin_clear(LED_PIN);
        output_state = (output_state + 1) & 1;
        nrf_delay_ms(100);
    }
}
