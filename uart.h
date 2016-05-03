
/**
 * UART library
 * for the Nordic Semiconductor nRF51 series
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

#include "cortex_m0.h"

#include "strings.h"
#include "fifo.h"

/*
 * Constants
 */

#define UART_BASE   0x40002000

// Peripheral tasks (see PPI)
#define STARTRX     0x000   // Start UART receiver
#define STOPRX      0x004   // Stop UART receiver
#define STARTTX     0x008   // Start UART transmitter
#define STOPTX      0x00C   // Stop UART transmitter
#define SUSPEND     0x01C   // Suspend UART

// Events
#define CTS         0x100   // CTS is activated (set low)
#define NCTS        0x104   // CTS is deactivated (set high)
#define RXDRDY      0x108   // Data received in RXD
#define TXDRDY      0x11C   // Data sent from TXD
#define ERROR       0x124   // Error detected
#define RXTO        0x144   // Receiver timeout

// Registers
#define INTEN       0x300   // Enable or disable interrupt
#define INTENSET    0x304   // Enable interrupt
#define INTENCLR    0x308   // Disable interrupt
#define ERRORSRC    0x480   // Error source
#define ENABLE      0x500   // Enable UART
#define PSELRTS     0x508   // Pin select for RTS
#define PSELTXD     0x50C   // Pin select for TXD
#define PSELCTS     0x510   // Pin select for CTS
#define PSELRXD     0x514   // Pin select for RXD
#define RXD         0x518   // RXD register
#define TXD         0x51C   // TXD register
#define BAUDRATE    0x524   // Baud rate
#define CONFIG      0x56C   // Configuration of parity and hardware flow control


// Valid baudrates
#define baud_1200    0x0004F000
#define baud_2400    0x0009D000
#define baud_4800    0x0013B000
#define baud_9600    0x00275000
#define baud_14400   0x003B0000
#define baud_19200   0x004EA000
#define baud_28800   0x0075F000
#define baud_38400   0x009D5000
#define baud_57600   0x00EBF000
#define baud_76800   0x013A9000
#define baud_115200  0x01D7E000
#define baud_230400  0x03AFB000
#define baud_250000  0x04000000
#define baud_460800  0x075F7000
#define baud_921600  0x0EBED000
#define baud_1M      0x10000000


/*
 * Macros for writing data to registers
 */

// Tasks
#define uart_start_receiver             *(volatile uint32_t*) (UART_BASE+STARTRX) = 1
#define uart_stop_receiver              *(volatile uint32_t*) (UART_BASE+STOPRX)  = 1
#define uart_start_transmitter          *(volatile uint32_t*) (UART_BASE+STARTTX) = 1
#define uart_stop_transmitter           *(volatile uint32_t*) (UART_BASE+STOPTX)  = 1
#define uart_stop_transmitter           *(volatile uint32_t*) (UART_BASE+STOPTX)  = 1


// Events
#define uart_event_CTS                  *(volatile uint32_t*) (UART_BASE+CTS)
#define uart_event_NCTS                 *(volatile uint32_t*) (UART_BASE+NCTS)
#define uart_event_RXDRDY               *(volatile uint32_t*) (UART_BASE+RXDRDY)
#define uart_event_TXDRDY               *(volatile uint32_t*) (UART_BASE+TXDRDY)
#define uart_event_ERROR                *(volatile uint32_t*) (UART_BASE+ERROR)
#define uart_event_RXTO                 *(volatile uint32_t*) (UART_BASE+RXTO)

// write a zero to clear the event
#define clear_event(event)              event = 0

// Interrupts
#define INTERRUPT_UART0                          2
#define UART_ISR_VECTOR_ADDRESS                 (16 + INTERRUPT_UART0)*4
// that's not working:
//#define set_uart_interrupt_handler(pointer)     *(volatile uint32_t*) (UART_ISR_VECTOR_ADDRESS) = (uint32_t) pointer // +1 to indicate Thumb instruction set

#define uart_interrupt_enable                   interrupt_enable(INTERRUPT_UART0)
#define uart_interrupt_disable                  interrupt_disable(INTERRUPT_UART0)

#define uart_interrupt_upon_RXDRDY_enable       *(volatile uint32_t*) (UART_BASE+INTENSET) = (1 << 2)
#define uart_interrupt_upon_RXDRDY_disable      *(volatile uint32_t*) (UART_BASE+INTENCLR) = (1 << 2)

#define uart_interrupt_upon_TXDRDY_enable       *(volatile uint32_t*) (UART_BASE+INTENSET) = (1 << 7)
#define uart_interrupt_upon_TXDRDY_disable      *(volatile uint32_t*) (UART_BASE+INTENCLR) = (1 << 7)

#define uart_interrupt_upon_ERROR_enable        *(volatile uint32_t*) (UART_BASE+INTENSET) = (1 << 9)
#define uart_interrupt_upon_ERROR_disable       *(volatile uint32_t*) (UART_BASE+INTENCLR) = (1 << 9)

#define uart_interrupt_upon_RXTO_enable         *(volatile uint32_t*) (UART_BASE+INTENSET) = (1 << 17)
#define uart_interrupt_upon_RXTO_disable        *(volatile uint32_t*) (UART_BASE+INTENCLR) = (1 << 17)

// Configuration
#define uart_disable                    *(volatile uint32_t*) (UART_BASE+ENABLE) =  (*(volatile uint32_t*) (UART_BASE+ENABLE)) & 0xFFFFFFF8
#define uart_enable                     *(volatile uint32_t*) (UART_BASE+ENABLE) = ((*(volatile uint32_t*) (UART_BASE+ENABLE)) & 0xFFFFFFF8) + 0x4

// only configure pins while UART is disabled
#define uart_select_pin_as_TXD(pin)     *(volatile uint32_t*) (UART_BASE+PSELTXD) = pin
#define uart_select_pin_as_RXD(pin)     *(volatile uint32_t*) (UART_BASE+PSELRXD) = pin
#define uart_select_pin_as_RTS(pin)     *(volatile uint32_t*) (UART_BASE+PSELRTS) = pin
#define uart_select_pin_as_CTS(pin)     *(volatile uint32_t*) (UART_BASE+PSELCTS) = pin

// signal shall not be externalized to any pin
#define UART_PIN_DISABLE                0xFFFFFFFF

#define uart_read                     ((*(volatile uint32_t*) (UART_BASE+RXD)) & 0x000000FF)
#define uart_write(b)                   *(volatile uint32_t*) (UART_BASE+TXD) = b

#define uart_set_baud(rate)             *(volatile uint32_t*) (UART_BASE+BAUDRATE) = rate

#define uart_set_parity_exclude         *(volatile uint32_t*) (UART_BASE+CONFIG) =  (*(volatile uint32_t*) (UART_BASE+CONFIG)) & 0xFFFFFFF1
#define uart_set_parity_include         *(volatile uint32_t*) (UART_BASE+CONFIG) = ((*(volatile uint32_t*) (UART_BASE+CONFIG)) & 0xFFFFFFF1) + 0xE

#define uart_flow_control_disable       *(volatile uint32_t*) (UART_BASE+CONFIG) =  (*(volatile uint32_t*) (UART_BASE+CONFIG)) & 0xFFFFFFFE
#define uart_flow_control_enable        *(volatile uint32_t*) (UART_BASE+CONFIG) = ((*(volatile uint32_t*) (UART_BASE+CONFIG)) & 0xFFFFFFFE) + 0x1


/*
 * Functions
 */
uint32_t strlen(char* s);

void    uart_init_fifo();
void    uart_send(char* buffer, uint8_t length);
void    uart_send_string(char* s);
uint8_t uart_receive(char* buffer, uint8_t max);

#endif // UART_H
