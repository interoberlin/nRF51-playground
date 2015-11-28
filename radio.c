
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

#define RADIO_BUFFER_LENGTH            RADIO_PDU_MAX
#define MAX_PAYLOAD_LENGTH            (RADIO_PDU_MAX - 2)

static uint8_t inbuf[RADIO_BUFFER_LENGTH] __attribute__ ((aligned));
//static uint8_t *outbuf;

// internal state machine
static volatile uint8_t status = 0;
#define STATUS_INITIALIZED          1
#define STATUS_RX                   2
#define STATUS_TX                   4
#define STATUS_BUSY                (STATUS_RX | STATUS_TX)

static radio_receive_callback_t receive_callback;
static radio_send_callback_t send_callback;

uint8_t radio_channel_to_frequency(uint8_t channel)
{
    /*
     * Link Layer specification section 1.4.1, Core 4.1, page 2502
     * nRF51 Series Reference Manual v2.1, section 16.2.19, page 91
     *
     * The nRF51822 is configured using the frequency offset from 2400 MHz:
     * On air frequency = 2400 MHz + RADIO_FREQUENCY MHz
     */
    switch (channel)
    {
        // advertising channels
        case 37:
            return 2;   // 2402 MHz
        case 38:
            return 26;  // 2426 MHz
        case 39:
            return 80;  // 2480 MHz
        // data channels
        default:
            // 0-10
            if (channel < 11)
                return 4 + (channel*2);
            // 11-36
            if (channel < 37)
                return 6 + (channel*2);
    }

    // invalid channel, return first advertising channel
    return 2;
}

/**
 * For debugging purposes:
 * Print out a hex dump of the received packet via UART
 */
void print_packet(char *buffer, uint32_t length)
{
    uint32_t i;

    // temporarily disable all interrupts
    // prevents characters to be randomly injected into ongoing uart transmission
    DINT;

    // access address
    uart_send("00 00 00 00 ", 12);

    // header
    for (i=0; i<2; i++)
    {
        char s[] = "   ";
        char2hex(s, buffer++);
        uart_send(s, 3);
    }

    uart_send("| ", 2);

    // advertising address
    for (i=2; i<8; i++)
    {
        char s[] = "   ";
        char2hex(s, buffer++);
        uart_send(s, 3);
    }

    uart_send("| ", 2);

    // device address match?
    if (RADIO_EVENT_DEVMATCH)
        uart_send("MATCH ", 8);
    if (RADIO_EVENT_DEVMISS)
        uart_send("MISS ", 7);

    uart_send("| ", 2);

    // packet bytes
    for (i=8; i<length; i++)
    {
        char s[] = "   ";
        char2hex(s, buffer++);
        uart_send(s, 3);
    }

    uart_send("| ", 2);

    // CRC
    for (i=0; i<3; i++)
    {
        char s[] = "   ";
        char c = (RADIO_RXCRC >> (i*8)) & 0xFF;
        char2hex(s, &c);
        uart_send(s, 3);
    }

    uart_send("| ", 2);

    // CRC ok?
    if (RADIO_CRC_OK)
        uart_send("OK", 2);
    else
        uart_send("ERROR", 5);

    uart_send("\n", 1);

    // re-enable interrupts
    EINT;
}

/**
 * Radio interrupt handler
 *
 * Included in nrf51_startup.c
 */
void RADIO_Handler()
{
    if (RADIO_EVENT_END)
    {
        // Transmission complete
        if (status & STATUS_TX)
        {
            status &= ~STATUS_TX;
        }

        // Reception complete
        if (status & STATUS_RX)
        {
            if (RADIO_CRC_OK)
                print_packet((char*) inbuf, RADIO_BUFFER_LENGTH);
            //status &= ~STATUS_RX;
        }

        // clear
        RADIO_EVENT_END = 0;
    }

    //uart_send("i",1);
/*
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
    */
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

    /*
     * Data whitening initial value = Radio channel index
     * Polynomial (hardwired into SoC): x^7 + x^4 + 1
     *
     * Bluetooth specification 4.1,
     * Chapter 3.2, page 2523
     */
    RADIO_DATAWHITEIV = channel;

    RADIO_CRCINIT = 0x555555;

    uint8_t frequency = radio_channel_to_frequency(channel);
    RADIO_FREQUENCY = frequency;

    // set lower 3 bytes of address as base address
    radio_set_address_base(0, addr << 8);

    // set the highest byte of address as address prefix
    radio_set_address_prefix(0, addr >> 24);

    uart_send_string("Radio prepared.\n");

    return true;
}

void radio_send(uint8_t *data)
{
    status |= STATUS_TX;

    // make sure, transmission is started after ramp-up is complete
    RADIO_SHORTS = RADIO_SHORTCUT_READY_START | RADIO_SHORTCUT_END_DISABLE;

    // only invoke radio interrupt, when an END event happens
    RADIO_INTENCLR = ~0;
    RADIO_INTENSET = RADIO_INTERRUPT_END;

    // clear all radio event flags
    radio_clear_all_events;

    // initiate packet transmission
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
        asm("wfi");
    uart_send("d", 1);

    uart_send("\n", 1);
*/
}

void radio_start_receiver(uint32_t f)
{
    // set RX status flag
    status |= STATUS_RX;

    // clear all radio event flags
    radio_clear_all_events;

    // reception starts, as soon as receiver is READY
    // receiver shutdown, as soon as reception ENDs
    RADIO_SHORTS = RADIO_SHORTCUT_READY_START
                 | RADIO_SHORTCUT_END_START;

    // only invoke radio interrupt, when reception is complete
    RADIO_INTENCLR = ~0;
    RADIO_INTENSET = RADIO_INTERRUPT_END;

    // fill buffer with zeroes
    memset(inbuf, 0, sizeof(inbuf));

    // receive
    RADIO_PACKETPTR = (uint32_t) inbuf;
    RADIO_TASK_RXEN = 1;

/*
    uart_send("<", 1);

    // wait until DISABLED flag is raised
    while (!RADIO_EVENT_DISABLED)
        asm("nop");
    uart_send("d", 1);

    uart_send("\n", 1);
*/
}

void radio_stop()
{
    radio_clear_all_events;

    // automatically disable radio as soon as current task is finished
    RADIO_SHORTS |= RADIO_SHORTCUT_END_DISABLE;

    // wait until radio is disabled
    while (!RADIO_EVENT_DISABLED)
        asm("nop");

    // clear DISABLED event
    RADIO_EVENT_DISABLED = 0;

    // clear STATUS_RX and STATUS_TX flags
    status &= ~STATUS_BUSY;
}

void radio_init(void)
{
    // wait for high frequency clock to get started
    if (!CLOCK_EVENT_HFCLKSTARTED)
    {
        uart_send_string("Starting high frequency clock... ");
        CLOCK_TASK_HFCLKSTART = 1;
        while (!CLOCK_EVENT_HFCLKSTARTED)
            asm("nop");
    }
    uart_send_string("[started]\n");

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

    uart_send_string("Configuring radio for 1 MBit Bluetooth Low Energy...\n");
    RADIO_MODE = RADIO_MODE_BLE_1MBIT;

    /*
     * Link Layer specification section 4.1, Core 4.1, page 2524
     * nRF51 Series Reference Manual v2.1, section 16.2.7, page 92
     *
     * Set the inter frame space to 150 us
     */
    RADIO_TIFS = 150;

    /*
     *  nRF51 Series Reference Manual v2.1, section 16.2.9, page 88
     *
     * Enable data whitening,
     * set air endianness to little endian i.e. LSB first,
     * set the maximum payload length
     * and set access address size to 4 (3 bytes base + 1 byte prefix)
     */
    RADIO_PCNF1 = RADIO_WHITENING_ENABLE
                | RADIO_LSB_FIRST
                | RADIO_MAX_PAYLOAD_LENGTH(MAX_PAYLOAD_LENGTH)
                | RADIO_ACCESS_ADDRESS_SIZE(4);

    /*
     * nRF51 Series Reference Manual v2.1, section 16.1.4, page 74
     * nRF51 Series Reference Manual v2.1, section 16.2.14-15, pages 89-90
     *
     * Preset the address to use when receive and transmit packets (logical
     * address 0, which is assembled by base address BASE0 and prefix byte
     * PREFIX0.AP0.
     *
     * Set RXADDRESSES to zero for promiscious mode.
     */
    RADIO_RXADDRESSES = RADIO_RXADDR0;
    RADIO_TXADDRESS   = RADIO_TXADDR0;

    /*
     * nRF51 Series Reference Manual v2.1, section 16.1.7, page 76
     * nRF51 Series Reference Manual v2.1, sections 16.1.16-17, page 90
     *
     * Configure the CRC length (3 octets), polynominal and set it to
     * ignore the access address when calculate the CRC.
     */
    RADIO_CRCCNF = RADIO_CRCCNF_LEN_3
                 | RADIO_CRCCNF_SKIPADDR;

    /*
     * CRC polynomial:
     * Bluetooth specification 4.1
     * Chapter 3: Bit stream processing, page 2522
     *
     * x^24 + x^10 + x^9 + x^6 + x^4 + x^3 + x + 1
     *   (1)000 00000 00000 11001 01101 1
     * = (1)0000 0000 0000 0110 0101 1011 =
     * = 100065B
     */
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
    RADIO_PCNF0 = RADIO_LENGTH_LF(8)
                | RADIO_LENGTH_S0(1)
                | RADIO_LENGTH_S1(0);

    // Clear all shortcuts
    RADIO_SHORTS = 0;

    // Disable all radio interrupts
    RADIO_INTENCLR = ~0;

    // Important, otherwise the above RADIO_ISR is never invoked
    radio_interrupt_enable;

    RADIO_TXPOWER = RADIO_TXPOWER_0DBM;

    // to avoid faulty behaviour due to invalid pointer
    RADIO_PACKETPTR = (uint32_t) inbuf;

    status |= STATUS_INITIALIZED;

    uart_send_string("Radio initialized.\n");
}
