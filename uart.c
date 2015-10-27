
/*
 * UART functions
 */
 
#include "uart.h"

/*
 * Transmitting data
 */

#ifdef UART_SEND_USING_INTERRUPTS

volatile char*      uart_tx_buffer = 0;
volatile uint32_t   uart_tx_buffer_length = 0;
volatile uint32_t   uart_tx_buffer_cursor = 0;

/*
 * UART Interrupt Service Routine
 * attached in nrf51_startup.c
 * to be invoked by NVIC
 */
void UART0_Handler()
{
    if (uart_event_TXDRDY != 0)
    {
        clear_event(uart_event_TXDRDY);
        
        if (uart_tx_buffer_cursor >= uart_tx_buffer_length-1)
            // disable future interrupts on a TXDRDY event
            uart_interrupt_upon_TXDRDY_disable;

        if (uart_tx_buffer_cursor < uart_tx_buffer_length)
            // output one byte to UART
            uart_write( *(uart_tx_buffer+(uart_tx_buffer_cursor++)) );
    }
}

void uart_send(char* buffer, uint8_t length)
{
    uart_stop_transmitter;
    
    // copy data to internal buffer to prevent buffer changes during transmission 
    // TODO
    
    // meanwhile directly use argument as TX buffer
    uart_tx_buffer = buffer;
    uart_tx_buffer_length = length;
    uart_tx_buffer_cursor = 0;

    // enable transmitter ready event interrupt
    uart_interrupt_upon_TXDRDY_enable;

    // enable UART interrupt
    uart_interrupt_enable;

    // enable transmission 
    uart_start_transmitter;
    
    // initiate transmission by writing the first byte to tranmitter buffer
    uart_write( *(uart_tx_buffer+(uart_tx_buffer_cursor++)) );
}

#else // #ifdef UART_SEND_USING_INTERRUPTS

void uart_send(char* buffer, uint8_t length)
{
    // write byte by byte to UART output
    for (int i=0; i<length; i++)
    {
        uart_write( *(buffer+i) );
        // enable transmission 
        uart_start_transmitter;
        // wait for transmission to complete
        delay_us(300);
    }
}

#endif // #ifdef UART_SEND_USING_INTERRUPTS
