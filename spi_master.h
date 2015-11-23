
/**
 * Serial Peripheral Interface (SPI) Master library
 * for the Nordic Semiconductor nRF51 series
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#define INTERRUPT_SPI 3

/*
 * Base address:
 * Selects, which of the two available hardware SPI master devices to use
 */
#define SPI0     0x40003000
#define SPI1     0x40004000

/*
 * Events
 */
#define SPI_EVENT_READY(spi_device)  (*(volatile uint32_t*) (spi_device+0x108))    // TXD byte sent and RXD byte received
         
/*
 * Tasks
 */                   
#define SPI_INTEN(spi_device)       (*(volatile uint32_t*) (spi_device+0x300))    // Enable or disable interrupt
#define SPI_INTENSET(spi_device)    (*(volatile uint32_t*) (spi_device+0x304))    // Enable interrupt
#define SPI_INTENCLR(spi_device)    (*(volatile uint32_t*) (spi_device+0x308))    // Disable interrupt
#define SPI_ENABLE(spi_device)      (*(volatile uint32_t*) (spi_device+0x500))    // Enable SPI
#define SPI_PSELSCK(spi_device)     (*(volatile uint32_t*) (spi_device+0x508))    // Pin select for SCK
#define SPI_PSELMOSI(spi_device)    (*(volatile uint32_t*) (spi_device+0x50C))    // Pin select for MOSI
#define SPI_PSELMISO(spi_device)    (*(volatile uint32_t*) (spi_device+0x510))    // Pin select for MISO
#define SPI_RXD(spi_device)         (*(volatile uint32_t*) (spi_device+0x518))    // RXD register
#define SPI_TXD(spi_device)         (*(volatile uint32_t*) (spi_device+0x51C))    // TXD register
#define SPI_FREQUENCY(spi_device)   (*(volatile uint32_t*) (spi_device+0x524))    // SPI frequency
#define SPI_CONFIG(spi_device)      (*(volatile uint32_t*) (spi_device+0x554))    // Configuration register

/*
 * Applicable register values
 */
// Pin selection
#define SPI_PIN_DISABLED        0xFFFFFFFF

// Data rate
#define SPI_FREQUENCY_125K      0x02000000      // 125 kbps
#define SPI_FREQUENCY_250K      0x04000000      // 250 kbps
#define SPI_FREQUENCY_500K      0x08000000      // 500 kbps
#define SPI_FREQUENCY_1M        0x10000000      // 1 Mbps
#define SPI_FREQUENCY_2M        0x20000000      // 2 Mbps
#define SPI_FREQUENCY_4M        0x40000000      // 4 Mbps
#define SPI_FREQUENCY_8M        0x80000000      // 8 Mbps

// Configuration
#define SPI_BITORDER_MSBFIRST           0   // Most significant bit is shifted out first
#define SPI_BITORDER_LSBFIRST           1   // Least significant bit is shifted out first

#define SPI_CLOCKPHASE_LEADING          0   // Data is sampled on the leading edge of clock and shifted out on the trailing edge
#define SPI_CLOCKPHASE_TRAILING         2   // Data is sampled on the trailing edge of clock and shifted out on the leading edge

#define SPI_CLOCKPOLARITY_ACTIVEHIGH    0   // self explaining ...
#define SPI_CLOCKPOLARITY_ACTIVELOW     4

/*
 * Register macros
 */
// Enable/Disable
#define spi_enabled(spi_device)                        (SPI_ENABLE(spi_device) & 0x07)
#define spi_enable(spi_device)                          SPI_ENABLE(spi_device) |= 1      // set bit
#define spi_disable(spi_device)                         SPI_ENABLE(spi_device) &= ~0x07  // clear bit

// Interrupts
#define spi_interrupt_upon_READY(spi_device)          ((SPI_INTEN   (spi_device)) & 0x04) >> 2)
#define spi_interrupt_upon_READY_enable(spi_device)     SPI_INTENSET(spi_device) = 0x04
#define spi_interrupt_upon_READY_disable(spi_device)    SPI_INTENCLR(spi_device) = 0x04

// Pins to use: [0..31]
#define spi_pin_select(spi_device, pin_SCK, pin_MOSI, pin_MISO) \
{ \
    SPI_PSELSCK(spi_device)  = pin_SCK;     \
    SPI_PSELMOSI(spi_device) = pin_MOSI;    \
    SPI_PSELMISO(spi_device) = pin_MISO;    \
};

// Read/Write
#define spi_read(spi_device, word_pointer)              *(word_pointer) = SPI_RXD(spi_device)
#define spi_write(spi_device, word)                 SPI_TXD(spi_device) = word
