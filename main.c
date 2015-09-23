
/**
* This example shows how to configure GPIO pins as outputs which can also be used to drive LEDs.
* Each LED is set on one at a time and each state lasts 100 milliseconds.
*/

#include <stdbool.h>
#include <stdint.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "uart.h"

#define LED_PIN 21

int main()
{
    uint8_t output_state = 0;
    
    nrf_gpio_pin_dir_set(LED_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
    
    uart_disable;
    nrf_gpio_pin_dir_set(30, NRF_GPIO_PIN_DIR_OUTPUT);
    select_pin_as_TXD(30);
    select_pin_as_RXD(PIN_DISABLE);
    select_pin_as_RTS(PIN_DISABLE);
    select_pin_as_CTS(PIN_DISABLE);
    set_baud(baud_115200);
    set_parity_exclude;
    disable_flow_control;
    uart_enable;
    
    while(true)
    {
        for (int i=0; i<10; i++)
        {
            if (output_state == 1)
                nrf_gpio_pin_set(LED_PIN);
            else
                nrf_gpio_pin_clear(LED_PIN);
            output_state = (output_state + 1) & 1;
            nrf_delay_ms(100);
        }
        
        write_TXD(0x65);
        start_transmitter;
    }

    return 0;
}
