
#include <stdbool.h>
#include <stdint.h>

#include "nrf_gpio.h"
#include "delay.h"
#include "uart.h"
#include "radio.h"

// enable debug messages
#define DEBUG_VIA_UART

/*
 * RS-232 serial port (UART)
 */

#define PIN_UART_TXD 20
#define PIN_UART_RXD 18

void uart_setup()
{
    uart_disable;

    // initialize buffers
    uart_init_fifo();

    // enable UART interrupt
    uart_interrupt_upon_RXDRDY_enable;
//    uart_interrupt_upon_RXTO_enable;
//    uart_interrupt_upon_ERROR_enable;
    uart_interrupt_enable;

    // configure transmission
    nrf_gpio_pin_dir_set(PIN_UART_TXD, NRF_GPIO_PIN_DIR_OUTPUT);
    uart_select_pin_as_TXD(PIN_UART_TXD);
    uart_select_pin_as_RTS(UART_PIN_DISABLE);
    uart_select_pin_as_CTS(UART_PIN_DISABLE);
    uart_set_baud(baud_9600);
    uart_set_parity_exclude;
    uart_flow_control_enable;

    // configure reception
    uart_select_pin_as_RXD(PIN_UART_RXD);
    uart_start_receiver;

    uart_enable;
}

int main()
{
    uart_setup();
    radio_init();

    /*
     * Receiver demo:
     * Configure radio and wait for interrupts
     */
    uart_send_string("Ready to receive BLE packets:\n");
    while (1)
    {
        radio_recv(0);
        delay_ms(100);
    }

    
    /* Link Layer specification section 2.3, Core 4.1, page 2504
     * Link Layer specification section 2.3.1.3, Core 4.1, page 2507
     *
     * ADV_NONCONN_IND PDU (39 octets):
     * +--------+--------+---------+
     * | Header |  AdvA  | AdvData |
     * +--------+--------+---------+
     *  2 octets 6 octets 31 octets
     *
     * Header:  <PDU Type=ADV_NONCONN_IND, TxAddr=RANDOM, Length=22>
     * AdvA:    <FF:EE:DD:CC:BB:AA>
     * AdvData: <AD: Len=15, Type="Complete Local Name", Data="blessed device">
     */
    uint8_t adv_nonconn_ind[] = {
        0x42, 0x16,                 /* Header */
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,     /* AdvA */
        0x0F,                       /* AD Length */
        0x09,                       /* AD Type */
        /* AD Data */
        'b', 'l', 'e', 's',
        's', 'e', 'd', ' ',
        'd', 'e', 'v', 'i',
        'c', 'e'
    };

    /* Link Layer specification Section 2.1.2, Core 4.1 page 2503 */
    #define ADV_CHANNEL_AA          0x8E89BED6

    /* Link Layer specification Section 3.1.1, Core 4.1 page 2522 */
    #define ADV_CHANNEL_CRC         0x555555

    uint8_t advertising_channels[] = {37,38,39};
    uint8_t channel_index = 0;

    while (true)
    {
//        https://github.com/interoberlin/nrf51-playground/issues/8:
//        channel_index = (channel_index + 1) % 3;
        channel_index = (channel_index + 1);
        if (channel_index > 2)
            channel_index = 0;
        uint8_t channel = advertising_channels[channel_index];
        radio_prepare(channel, ADV_CHANNEL_AA, ADV_CHANNEL_CRC);
        radio_send(adv_nonconn_ind, 0);
        delay_ms(10);
    }

    return 0;
}
