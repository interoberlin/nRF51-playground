
#include "ws2811_spi.h"

uint8_t     ws2811_pin;
uint32_t    ws2811_spi_device;
fifo_t      ws2811_buffer;
bool        transmission_in_progress = false;

const uint8_t WS2811_HIGH = 0b00011100;
const uint8_t WS2811_LOW  = 0b00011000;

// prepare pin
void ws2811_init
        (
        uint8_t     pin,
        uint32_t    spi_device_to_use
        )
{
    ws2811_pin          = pin;
    ws2811_spi_device   = spi_device_to_use;

    // initialize FIFO
    fifo_init((fifo_t*) &ws2811_buffer);
    
    nrf_gpio_pin_dir_set(pin, NRF_GPIO_PIN_DIR_OUTPUT);

    /*
     * Initialize SPI device
     */

    // make sure, the selected SPI device is disabled
    spi_disable(spi_device_to_use);
    transmission_in_progress = false;

    // setup GPIO pin
    nrf_gpio_pin_dir_set(pin,  NRF_GPIO_PIN_DIR_OUTPUT);    // MOSI pin

    // select GPIO pins for SPI device
    spi_pin_select(
                    spi_device_to_use,
                    SPI_PIN_DISABLED,   // SCK
                    pin,                // MOSI
                    SPI_PIN_DISABLED    // MISO
                  );

    // configure SPI parameters
    SPI_CONFIG(spi_device_to_use)      = SPI_BITORDER_MSBFIRST
                                       | SPI_CLOCKPOLARITY_ACTIVEHIGH
                                       | SPI_CLOCKPHASE_LEADING;

    SPI_FREQUENCY(spi_device_to_use)   = SPI_FREQUENCY_4M;

    // enable interrupt, so that we know, when a transmission is complete
    spi_interrupt_upon_READY_enable(spi_device_to_use);
    interrupt_enable(spi_device_to_use);
    //NVIC_EnableIRQ(INTERRUPT_SPI);
}

/**
 * Once an SPI transmission is complete
 * this ISR will take care of shovel new bytes
 * into the sending buffer
 */
void SPI0_Handler()
{
    // SPI0 is ready?
    // TODO: shovel new bytes into SPI_TXD buffer
}

// append 24 bits of data to buffer
void ws2811_write(uint32_t color)
{
    // translate bits
    for (uint8_t i=0; i<24; i++)
    {
        uint32_t bit = (color << i) & (1 << 23);
        if (bit)
            fifo_write_five_bits(&ws2811_buffer, &WS2811_HIGH);
        else
            fifo_write_five_bits(&ws2811_buffer, &WS2811_LOW);
    }
}

// begin sending the data in the buffer
void ws2811_send()
{
    // abort, if SPI interrupt is already/still enabled
    if (transmission_in_progress)
        return;

    // write first byte from FIFO to SPI
    if (!fifo_available(ws2811_buffer))
        return;
    fifo_read(ws2811_buffer, &SPI_TXD(ws2811_spi_device));

    // write second byte from FIFO to SPI
    if (fifo_available(ws2811_buffer))
        fifo_read(ws2811_buffer, &SPI_TXD(ws2811_spi_device));

    // enable SPI transmission
    transmission_in_progress = true;
    spi_enable(ws2811_spi_device);
}
