
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
#define PIN_UART_RXD 18

void uart_setup()
{
    uart_disable();

    // configure transmission
    nrf_gpio_pin_dir_set(PIN_UART_TXD, NRF_GPIO_PIN_DIR_OUTPUT);
    uart_select_pin_as_TXD(PIN_UART_TXD);
    uart_select_pin_as_RTS(UART_PIN_DISABLE);
    uart_select_pin_as_CTS(UART_PIN_DISABLE);
    uart_set_baud(UART_BAUD_9600);
    uart_set_parity_exclude();
    uart_flow_control_enable();

    // configure reception
    uart_select_pin_as_RXD(PIN_UART_RXD);
    uart_interrupt_upon_RXDRDY_enable();
    uart_interrupt_upon_RXTO_enable();
    uart_interrupt_upon_ERROR_enable();
    uart_start_receiver();

    uart_enable();
}

int main()
{
    uart_setup();
    uart_send_bytes("\xD\xAnRF51822:~$ ", 14);
    
    while (true)
    {
        asm("wfi");
        //delay_us(1000000);
    }

    return 0;
}
