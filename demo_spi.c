
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <cortex_m0.h>
#include <gpio.h>
#include <delay.h>
#include <random.h>
#include <fifo.h>
#include <spi_master.h>

#define my_spi SPI0

#define PIN_LED 28
#define PIN_SPI_DATA  29
#define PIN_SPI_CLOCK 25

fifo_t buffer;
fifo_t* my_buffer;

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
 * Retrieves one byte from our FIFO
 * and writes it to our SPI output
 */
void write_byte_from_fifo_to_spi()
{
    char c;
    fifo_read(&buffer, &c);
    spi_write(my_spi, c);
}

void setup_spi()
{
    // make sure, SPI is disabled
    spi_disable(my_spi);

    // select pins to use
    spi_pin_select(my_spi, PIN_SPI_CLOCK, PIN_SPI_DATA, SPI_PIN_DISABLED);

    // configure transmission parameters
    SPI_CONFIG(my_spi)      = SPI_BITORDER_MSBFIRST
                            | SPI_CLOCKPOLARITY_ACTIVELOW
                            | SPI_CLOCKPHASE_LEADING;

    // configure transmission speed
    SPI_FREQUENCY(my_spi)   = SPI_FREQUENCY_250K;

    // configure interrupts
    spi_interrupt_upon_READY_enable(my_spi);
    interrupt_enable(INTERRUPT_SPI);
}

/**
 * Initiates transmission of all bytes within the specified FIFO via SPI
 */
void spi_transmit_fifo(fifo_t* buffer)
{
//    if (spi_still_transmitting_fifo(my_spi))
//        return;

    //my_buffer = &buffer;
    setup_spi();
    write_byte_from_fifo_to_spi();
    spi_enable(my_spi);
}

/**
 * Use interrupt to reload out buffer and continue transmission
 */
void SPI0_TWI0_Handler()
{
    if (SPI_EVENT_READY(my_spi))
    {
        // event must be cleared
        SPI_EVENT_READY(my_spi) = 0;

        if (fifo_is_byte_available(my_buffer))
        {
            // enqueue next byte for transmission
            write_byte_from_fifo_to_spi();
        }
        else
        {
            // transmission sequence completed
            spi_disable(my_spi);
        }
    }
}

/*
 * Arduino-style main functions
 */
void setup()
{
    //random_init();

    my_buffer = &buffer;
    fifo_init(my_buffer);

    gpio_config_output(PIN_LED);
    gpio_clear(PIN_LED);

    setup_spi();
}

void loop()
{
    gpio_set(PIN_LED);
    delay_ms(100);

    generate_random_fifo(&buffer, 3);
    spi_transmit_fifo(&buffer);

    gpio_clear(PIN_LED);
    delay_ms(1000);
}

int main()
{
    setup();
    while (true)
        loop();
    return 0;
}
