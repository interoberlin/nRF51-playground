
/*
 * UART functions
 */
 
#include "uart.h"

/*
 * Transmitting data
 */

#ifdef UART_SEND_USING_INTERRUPTS

// TODO: FIFOs...

volatile char*      uart_tx_buffer = NULL;
volatile uint32_t   uart_tx_buffer_length = 0;
volatile uint32_t   uart_tx_buffer_cursor = 0;

// TODO: something about the malloc is apparently not working yet...
volatile char*      uart_rx_buffer = "\0\0\0\0\0\0\0\0";
volatile uint32_t   uart_rx_buffer_length = 8;
volatile uint32_t   uart_rx_buffer_cursor = 0;

/*
 * UART Interrupt Service Routine
 * attached in nrf51_startup.c
 * to be invoked by NVIC
 */
void UART0_Handler()
{
    uint32_t transmitted = uart_event_TXDRDY;
    uint32_t received = uart_event_RXDRDY;
    uint32_t timeout = uart_event_RXTO;
    uint32_t error = uart_event_ERROR;

    // is the transmitter circuit ready for another byte?
    if (transmitted != 0)
    {
        if (uart_tx_buffer_cursor >= uart_tx_buffer_length-1)
            // disable future interrupts on a TXDRDY event
            uart_interrupt_upon_TXDRDY_disable;

        if (uart_tx_buffer_cursor < uart_tx_buffer_length)
            // output one byte to UART
            uart_write( *(uart_tx_buffer+(uart_tx_buffer_cursor++)) );

        clear_event(uart_event_TXDRDY);
    }

    // did the receiver circuit receive a byte?
    if (received != 0)
    {
        // must be cleared before reading RX, see nRF Series Reference manual p.153
        clear_event(uart_event_RXDRDY);

        // allocate memory for rx buffer
/*
        TODO: buffering...
        if (uart_rx_buffer == NULL)
        {
            uart_rx_buffer = malloc(uart_rx_buffer_length);
            if (uart_rx_buffer == NULL)
                return; // malloc failed
        }
*/
        uint8_t incoming = uart_read;

        // echo read byte back to sender
        uart_write(incoming);
        
        // do we have room for another byte in the buffer?
        if (uart_rx_buffer_cursor < uart_rx_buffer_length-1)
        {
            uart_rx_buffer[uart_rx_buffer_cursor++] = incoming;
        }
    }
    
    if (timeout != 0)
    {
        clear_event(uart_event_RXTO);
    }

    if (error != 0)
    {
        clear_event(uart_event_ERROR);
    }
}

/*
 * Send a string to the configured UART
 * by providing a pointer to the buffer
 * and the length of bytes to send
 */
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

/*
 * Receive bytes from configured UART
 * by providing a pointer to malloc'ed memory
 * and the maximum number of bytes that may
 * be written to it.
 *
 * Returns the actual number of bytes written to the buffer.  
 */
uint8_t uart_receive(char* buffer, uint8_t max)
{
    return 0;
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
