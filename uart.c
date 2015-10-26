
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
void uart_isr()
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
    
    // copy function argument to UART TX buffer
    // TODO
    
    // directly use argument as TX buffer
    uart_tx_buffer = buffer;
    uart_tx_buffer_length = length;
    uart_tx_buffer_cursor = 0;
    
    // setup the above interrupt service routine in the exception vector table
    set_uart_interrupt_handler(&uart_isr);

    // enable transmitter ready interrupt
    uart_interrupt_upon_TXDRDY_enable;

    // enable UART interrupt
    //  http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHBFEIB.html
    NVIC_ClearingPending(UART_INTERRUPT);
    NVIC_SetPriority(UART_INTERRUPT, 3);
    NVIC_EnableIRQ(UART_INTERRUPT);

    // enable transmission 
    uart_start_transmitter;
    
    // initiate transmission by writing the first byte to tranmitter buffer
    uart_write( *(uart_tx_buffer+(uart_tx_buffer_cursor++)) );
    delay_us(1000);
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
