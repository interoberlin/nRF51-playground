
//#include <stdio.h>
//#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "nrf_gpio.h"
#include "delay.h"
#include "uart.h"


/*
 * RS-232 serial port (UART)
 */

#define PIN_UART_TXD 20

void setup_uart()
{
    uart_disable;
    nrf_gpio_pin_dir_set(PIN_UART_TXD, NRF_GPIO_PIN_DIR_OUTPUT);
    uart_select_pin_as_TXD(PIN_UART_TXD);
    uart_select_pin_as_RXD(UART_PIN_DISABLE);
    uart_select_pin_as_RTS(UART_PIN_DISABLE);
    uart_select_pin_as_CTS(UART_PIN_DISABLE);
    uart_set_baud(baud_115200);
    uart_set_parity_exclude;
    uart_disable_flow_control;
    uart_enable;
}


int main()
{
    setup_uart();
    
    while (true)
    {
        delay_us(500000);
        uart_write(0x65);
        uart_start_transmitter;
    }

    return 0;
}
