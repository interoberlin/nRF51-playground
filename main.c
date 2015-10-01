
/**
* This example shows how to configure GPIO pins as outputs which can also be used to drive LEDs.
* Each LED is set on one at a time and each state lasts 100 milliseconds.
*/

//#include <stdio.h>
//#include <string.h>
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
    
    // setup UART
    uart_disable;
    nrf_gpio_pin_dir_set(30, NRF_GPIO_PIN_DIR_OUTPUT);
    uart_select_pin_as_TXD(30);
    uart_select_pin_as_RXD(UART_PIN_DISABLE);
    uart_select_pin_as_RTS(UART_PIN_DISABLE);
    uart_select_pin_as_CTS(UART_PIN_DISABLE);
    uart_set_baud(baud_115200);
    uart_set_parity_exclude;
    uart_disable_flow_control;
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
        
        uart_send("Test-123 Hallo, Welt!\n", 22);
    }

    return 0;
}
