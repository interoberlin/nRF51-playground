
/**
 * Radio frequency unit library
 * for the Nordic Semiconductor nRF51 series
 *
 * Authors:
 *      Paulo B. de Oliveira Filho <pauloborgesfilho@gmail.com>
 *      Claudio Takahasi <claudio.takahasi@gmail.com>
 *      João Paulo Rechi Vita <jprvita@gmail.com>
 *      Matthias Bock <mail@matthiasbock.net>
 *
 * License: GNU GPLv3
 */

#include "radio.h"

#define MAX_BUF_LEN                    RADIO_MAX_PDU
#define MAX_PAYLOAD_LENGTH            (RADIO_MAX_PDU - 2)

static uint8_t inbuf[MAX_BUF_LEN] __attribute__ ((aligned));
static uint8_t *outbuf;

static volatile uint8_t status;
static volatile uint32_t flags;

// states of the internal state machine
#define STATUS_INITIALIZED          1
#define STATUS_RX                   2
#define STATUS_TX                   4
#define STATUS_BUSY                (STATUS_RX | STATUS_TX)

static radio_receive_callback_t receive_callback;
static radio_send_callback_t send_callback;

uint8_t radio_channel_to_frequency(uint8_t channel)
{
    /*
     * nRF51 Series Reference Manual v2.1, section 16.2.19, page 91
     * Link Layer specification section 1.4.1, Core 4.1, page 2502
     *
     * The nRF51822 is configured using the frequency offset from 2400 MHz:
     * On air frequency = 2400 MHz + RADIO_FREQUENCY MHz
     */
    switch (channel)
    {
        // advertising channels(?)
        case 37:
            return 2;
        case 38:
            return 26;
        case 39:
            return 80;
        // data channels(?)
        default:
            if (channel > 39)
                return -1;
            if (channel < 11)
                return 4 + (2 * channel);
            return 6 + (2 * channel);
    }
}

/**
 * Radio interrupt handler
 *
 * Included in nrf51_startup.c
 */
void RADIO_Handler()
{
    uint8_t old_status;
    bool active;

    RADIO_EVENT_END = 0UL;

    uart_send("i",1);

    active = false;
    old_status = status;
    status = STATUS_INITIALIZED;

    if (old_status & STATUS_RX)
    {
        if (flags & RADIO_FLAGS_TX_NEXT)
        {
            flags &= ~RADIO_FLAGS_TX_NEXT;
            status |= STATUS_TX;
            active = true;
            RADIO_PACKETPTR = (uint32_t) outbuf;
            RADIO_SHORTS &= ~RADIO_SHORTCUT_DISABLED_TXEN;
        }

        if (receive_callback)
            receive_callback(inbuf, RADIO_CRCSTATUS, active);
    }
    else if (old_status & STATUS_TX)
    {
        if (flags & RADIO_FLAGS_RX_NEXT)
        {
            flags &= ~RADIO_FLAGS_RX_NEXT;
            status |= STATUS_RX;
            active = true;
            RADIO_PACKETPTR = (uint32_t) inbuf;
            RADIO_SHORTS &= ~RADIO_SHORTCUT_DISABLED_RXEN;
        }

        if (send_callback)
            send_callback(active);
    }
}

void radio_set_callbacks(radio_receive_callback_t rcb, radio_send_callback_t scb)
{
    receive_callback = rcb;
    send_callback = scb;
}

bool radio_prepare(uint8_t channel, uint32_t addr, uint32_t crcinit)
{
    if (!(status & STATUS_INITIALIZED))
    {
        uart_send_string("radio_prepare() failed: Radio not initialized\n");
        return false;
    }

    if (status & STATUS_BUSY)
    {
        uart_send_string("radio_prepare() failed: Radio is busy\n");
        return false;
    }

    uint8_t frequency = radio_channel_to_frequency(channel);

//    uart_send(".", 1);

    RADIO_DATAWHITEIV = channel & 0x3F;
    RADIO_FREQUENCY = frequency;

    // set lower 3 bytes of address as base address
    radio_set_address_base(0, (addr << 8) & 0xFFFFFF00);

    // set the highest byte of address as address prefix
    radio_set_address_prefix(0, (addr >> 24) & 0xFF);

    RADIO_CRCINIT = crcinit;

//    uart_send_string("Radio prepared.\n");

    return true;
}

void radio_send(uint8_t *data, uint32_t f)
{
    // set TX status flag
    status |= STATUS_TX;
    flags |= f;

    if (f & RADIO_FLAGS_RX_NEXT)
        RADIO_SHORTS |= RADIO_SHORTCUT_DISABLED_RXEN;

    // make sure, transmission is started after ramp-up is complete
    RADIO_SHORTS |= RADIO_SHORTCUT_READY_START;

    // clear all radio event flags
    RADIO_EVENT_READY = 0;
    RADIO_EVENT_ADDRESS = 0;
    RADIO_EVENT_PAYLOAD = 0;
    RADIO_EVENT_END = 0;
    RADIO_EVENT_DISABLED = 0;

    // initiate packet transmission
    uint32_t *packet = (uint32_t) data;
    uint32_t maxlen = radio_get_max_payload_length;
    RADIO_PACKETPTR = (uint32_t) data;
    RADIO_TASK_TXEN = 1;
/*
    uart_send(">", 1);

    // wait until READY flag is raised
    while (!RADIO_EVENT_READY)
        asm("nop");
    uart_send("r", 1);

    // wait until ADDRESS flag is raised
    while (!RADIO_EVENT_ADDRESS)
        asm("nop");
    uart_send("a", 1);

    uint32_t s = RADIO_STATE;

    // wait until PAYLOAD flag is raised
    while (!RADIO_EVENT_PAYLOAD)
        asm("nop");
    uart_send("p", 1);

    // wait until END flag is raised
    while (!RADIO_EVENT_END)
        asm("nop");
    uart_send("e", 1);

    // wait until DISABLED flag is raised
    while (!RADIO_EVENT_DISABLED)
        asm("nop");
    uart_send("d", 1);

    uart_send("\n", 1);
*/
    // clear TX status flag
    status &= ~STATUS_TX;
}

void radio_recv(uint32_t f)
{
    // set RX status flag
    status |= STATUS_RX;
    flags |= f;

    if (f & RADIO_FLAGS_TX_NEXT)
        RADIO_SHORTS |= RADIO_SHORTCUT_DISABLED_TXEN;

    // receive packet
    RADIO_PACKETPTR = (uint32_t) inbuf;
    RADIO_TASK_RXEN = 1;
}

void radio_stop()
{
    if (!(status & STATUS_BUSY))
        // not started
        return;

    flags = 0;

    // clear DISABLED event
    RADIO_EVENT_DISABLED = 0;

    // manually disable radio
//    RADIO_TASK_STOP = 1;
//    RADIO_TASK_DISABLE = 1;

    // automaticall disable radio as soon as current task is finished
    RADIO_SHORTS |= RADIO_SHORTCUT_END_DISABLE;

    // wait until radio is disabled
    while (!RADIO_EVENT_DISABLED)
        asm("nop");
    // clear DISABLED event
    RADIO_EVENT_DISABLED = 0;

    // clear BUSY status flags
    status &= ~STATUS_BUSY;
}

void radio_set_out_buffer(uint8_t *buf)
{
    outbuf = buf;
}

void radio_init(void)
{
    // wait for high frequency clock to get started
    if (!CLOCK_EVENT_HFCLKSTARTED)
    {
        uart_send_string("High frequency clock has not been started. Starting...\n");
        CLOCK_TASK_HFCLKSTART = 1;
        while (!CLOCK_EVENT_HFCLKSTARTED)
            asm("nop");
    }
    uart_send_string("High frequency clock started.\n");

    /*
     * nRF51 Series Reference Manual v2.1, section 6.1.1, page 18
     * PCN-083 rev.1.1
     *
     * Factory Information Configuration Registers (FICR)
     * Apply obligatory factory settings: 
     * Fine tune BLE deviation parameters.
     */
    if (FICR_OVERRIDE_ENABLED_BLE_1MBIT)
    {
        uart_send_string("Factory overrides detected.\n");
        RADIO_OVERRIDE[0] = FICR_BLE_1MBIT[0];
        RADIO_OVERRIDE[1] = FICR_BLE_1MBIT[1];
        RADIO_OVERRIDE[2] = FICR_BLE_1MBIT[2];
        RADIO_OVERRIDE[3] = FICR_BLE_1MBIT[3];
        RADIO_OVERRIDE[4] = FICR_BLE_1MBIT[4] | 0x80000000;
        uart_send_string("Factory overrides applied.\n");
    }

    RADIO_MODE = RADIO_MODE_BLE_1MBIT;
    uart_send_string("Configuring radio for 1 MBit Bluetooth Low Energy...\n");

    /*
     * Link Layer specification section 4.1, Core 4.1, page 2524
     * nRF51 Series Reference Manual v2.1, section 16.2.7, page 92
     *
     * Set the inter frame space (T_IFS) to 150 us.
     */
    RADIO_TIFS = 150;

    /*
     *  nRF51 Series Reference Manual v2.1, section 16.2.9, page 88
     *
     * Enable data whitening, set the maximum payload length and set the
     * access address size (3 + 1 octets).
     */
    radio_data_whitening_enable;
    radio_set_max_payload_length(MAX_PAYLOAD_LENGTH);
    radio_set_access_address_size(3);

    /*
     * nRF51 Series Reference Manual v2.1, section 16.1.4, page 74
     * nRF51 Series Reference Manual v2.1, section 16.2.14-15, pages 89-90
     *
     * Preset the address to use when receive and transmit packets (logical
     * address 0, which is assembled by base address BASE0 and prefix byte
     * PREFIX0.AP0.
     */
    RADIO_RXADDRESSES = RADIO_RXADDR1;
    RADIO_TXADDRESS   = RADIO_TXADDR0;

    /*
     * nRF51 Series Reference Manual v2.1, section 16.1.7, page 76
     * nRF51 Series Reference Manual v2.1, sections 16.1.16-17, page 90
     *
     * Configure the CRC length (3 octets), polynominal and set it to
     * ignore the access address when calculate the CRC.
     */
    RADIO_CRCCNF  = RADIO_CRCCNF_LEN_3 | RADIO_CRCCNF_SKIPADDR; 
    RADIO_CRCPOLY = 0x100065B;

    /*
     * nRF51 Series Reference Manual v2.1, section 16.1.2, page 74
     * nRF51 Series Reference Manual v2.1, sections 16.1.8, page 87
     * Link Layer specification section 2.3, Core 4.1, page 2504
     * Link Layer specification section 2.4, Core 4.1, page 2511
     *
     * Configure the header size. The nRF51822 has 3 fields before the
     * payload field: S0, LENGTH and S1. These fields can be used to store
     * the PDU header.
     */
    radio_set_length_lf(8);
    radio_set_length_s0(1);
    radio_set_length_s1(0);

    /*
     * nRF51 Series Reference Manual v2.1, section 16.1.8, page 76
     * nRF51 Series Reference Manual v2.1, section 16.1.10-11, pages 78-80
     * nRF51 Series Reference Manual v2.1, section 16.2.1, page 85
     *
     * When tx/rx ramp-up (TXRU/RXRU) is complete and the READY event occurs,
     * immediately proceed with START.
     *
     * When TX or RX are completed and the END event occurs,
     * immediately proceed with DISABLE.
     */
    RADIO_SHORTS = RADIO_SHORTCUT_READY_START | RADIO_SHORTCUT_END_DISABLE;

    // Disable all radio interrupts
    RADIO_INTENCLR = ~0;
    // Trigger radio interrupt when an END event happens
    //RADIO_INTENSET = RADIO_INTERRUPT_END;

    radio_set_callbacks(NULL, NULL);
    RADIO_TXPOWER = RADIO_TXPOWER_0DBM;

    RADIO_PACKETPTR = (uint32_t) inbuf;
    memset(inbuf, 0, sizeof(inbuf));

    status = STATUS_INITIALIZED;

    uart_send_string("Radio initialized.\n");
}
