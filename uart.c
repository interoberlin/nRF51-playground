
/**
 * UART library
 * for the Nordic Semiconductor nRF51 series
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */
 
#include "uart.h"

/*
 * Transmitting data
 */

#ifdef UART_SEND_USING_INTERRUPTS

// reserve memory for incoming and outgoing bytes
struct fifo_s uart_inbuffer, uart_outbuffer;

// work with a pointer to this memory
struct fifo_s *uart_tx_fifo;
struct fifo_s *uart_rx_fifo;

/*
 * Initialize FIFO buffers
 */
void uart_init_fifo()
{
    uart_rx_fifo = &uart_inbuffer;
    fifo_init(uart_rx_fifo);

    uart_tx_fifo = &uart_outbuffer;
    fifo_init(uart_tx_fifo);
}

/*
 * UART Interrupt Service Routine
 * attached in nrf51_startup.c
 * to be invoked by NVIC
 */
void UART0_Handler()
{
    // cache event stati for processing
    uint32_t transmitted    = uart_event_TXDRDY;
    uint32_t received       = uart_event_RXDRDY;
    uint32_t timeout        = uart_event_RXTO;
    uint32_t error          = uart_event_ERROR;

    // is the transmitter circuit ready for another byte?
    if (transmitted)
    {
        // send another byte
        if (fifo_available(uart_tx_fifo) > 0)
        {
            // one byte
            char outgoing;
            fifo_read(uart_tx_fifo, &outgoing);

            // to UART
            uart_write(outgoing);
        }

        // TX buffer empty
        if (fifo_available(uart_tx_fifo) <= 0)
            // disable future interrupts on a TXDRDY event
            uart_interrupt_upon_TXDRDY_disable;

        // clear before or after sending next byte from TX buffer?
        clear_event(uart_event_TXDRDY);
    }

    // did the receiver circuit receive a byte?
    if (received)
    {
        if (!fifo_full(uart_rx_fifo))
        {
            // must be cleared before reading RX, see nRF Series Reference manual p.153
            clear_event(uart_event_RXDRDY);

            // receive one byte
            char incoming = uart_read;

            // echo read byte back to sender
            uart_write(incoming);

            // push received byte to buffer
            fifo_write(uart_rx_fifo, &incoming);
        }
    }
    
    // unhandled, but must be cleared
    if (timeout)
    {
        clear_event(uart_event_RXTO);
    }

    // unhandled, but must be cleared
    if (error)
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
    uint8_t timeout = 10;
    for (uint8_t i=0; i<length; i++)
    {
        // FIFO is full? -> wait until timeout
        while (fifo_full(uart_tx_fifo) && (timeout--) > 0)
            delay_us(100);
        // timeout
        if (timeout <= 0)
            break;
        // push current buffer char to FIFO
        if (*(buffer+i) == '\n')
            fifo_write(uart_tx_fifo, (char*) &"\r");
        fifo_write(uart_tx_fifo, buffer+i);
    }

    // enable transmitter ready event interrupt
    uart_interrupt_upon_TXDRDY_enable;

    // enable UART interrupt
    uart_interrupt_enable;

    // enable transmission 
    uart_start_transmitter;
    
    // initiate transmission cycle by writing the first byte to tranmitter buffer
    char outgoing;
    fifo_read(uart_tx_fifo, &outgoing);
    uart_write(outgoing);
}

/*
 * Count the number of characters in the string until the null terminator.
 * The null terminator must occur, otherwise string length is counted until the end of heap.
 */
uint32_t strlen(char* s)
{
    uint32_t len = 0;
    extern uint32_t heap_end;

    // keep counting until end of heap is reached
    // or reached character is a null terminator
    while (((uint32_t) s)+len < (uint32_t) &heap_end && *(s+len) != 0)
        len++;

    // heap end?
    if (((uint32_t) s)+len >= (uint32_t) &heap_end)
        return 0;

    return len;
}

/*
 * Send a string: automatically determine the number of characters to send
 */
void uart_send_string(char* s)
{
    uart_send(s, strlen(s));
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
