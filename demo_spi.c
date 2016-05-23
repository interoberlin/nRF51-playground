
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <delay.h>
#include <random.h>
#include <fifo.h>
#include <spi_master.h>

#define my_spi SPI0
#define spi_pin ?
fifo_t buffer;

/**
 * Fills the referenced FIFO with <count> random bytes
 */
void generate_random_fifo(fifo_t* buffer, uint8_t count)
{
    char c = 0x88;
    for (uint8_t i=0; i<count; i++)
        fifo_write(buffer, &c);
}

/**
 * Initiates transmission of all bytes within the specified FIFO via SPI
 */
void spi_transmit_fifo(fifo_t* buffer)
{
    if (spi_still_transmitting_fifo(my_spi))
        return;

    enable_interrupt(SPI_INTERRUPT);
    spi_interrupt_upon_READY_enable(my_spi);
    spi_enable(my_spi);
}

/**
 * Use interrupt to reload out buffer and continue transmission
 */
void SPI0_TWI0_Handler()
{
    // event must be cleared
    SPI_EVENT_READY(my_spi) = 0;

    if (fifo_available(buffer))
    {
        // enqueue next byte for transmission
        char c;
        fifo_read(&buffer, &c);
        spi_write(my_spi, &c);
    }
    else
    {
        // transmission sequence completed
        spi_disable(my_spi);
    }
}

/*
 * Arduino-style main functions
 */
void setup()
{
    random_init();

    // setup SPI
    // pins, frequency, etc.
    spi_pin_select(my_spi, SPI_PIN_DISABLED, spi_pin, SPI_PIN_DISABLED);
}

void loop()
{
    generate_random_fifo(&buffer, 20);
    spi_transmit_fifo(&buffer);
    delay(1000);
}

void main()
{
    setup();
    while (true)
        loop();
}
