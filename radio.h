
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

#ifndef RADIO_H
#define RADIO_H

// general includes
#include <stdint.h>
#include <stdbool.h>
#include "heap.h"

// nRF51-specific includes
#include "ficr.h"
#include "clock.h"
#include "uart.h"
 
/*
 * Constants
 * as specified in the nRF51 Series Reference Manual
 */

#define RADIO_BASE              0x40001000

// Tasks
#define RADIO_TASK_TXEN         (*(volatile uint32_t*) (RADIO_BASE+0x000))   // Enable radio in TX mode
#define RADIO_TASK_RXEN         (*(volatile uint32_t*) (RADIO_BASE+0x004))   // Enable radio in RX mode
#define RADIO_TASK_START        (*(volatile uint32_t*) (RADIO_BASE+0x008))   // Start radio
#define RADIO_TASK_STOP         (*(volatile uint32_t*) (RADIO_BASE+0x00C))   // Stop radio
#define RADIO_TASK_DISABLE      (*(volatile uint32_t*) (RADIO_BASE+0x010))   // Disable radio
#define RADIO_TASK_RSSISTART    (*(volatile uint32_t*) (RADIO_BASE+0x014))   // Start the RSSI and take one single sample of the receive signal strength
#define RADIO_TASK_RSSISTOP     (*(volatile uint32_t*) (RADIO_BASE+0x018))   // Stop the RSSI measurement
#define RADIO_TASK_BCSTART      (*(volatile uint32_t*) (RADIO_BASE+0x01C))   // Start the bit counter
#define RADIO_TASK_BCSTOP       (*(volatile uint32_t*) (RADIO_BASE+0x020))   // Stop the bit counter

// Events
#define RADIO_EVENT_READY       (*(volatile uint32_t*) (RADIO_BASE+0x100))   // Radio has ramped up and is ready to be started
#define RADIO_EVENT_ADDRESS     (*(volatile uint32_t*) (RADIO_BASE+0x104))   // Address sent or received
#define RADIO_EVENT_PAYLOAD     (*(volatile uint32_t*) (RADIO_BASE+0x108))   // Packet payload sent or received
#define RADIO_EVENT_END         (*(volatile uint32_t*) (RADIO_BASE+0x10C))   // Packet sent or received
#define RADIO_EVENT_DISABLED    (*(volatile uint32_t*) (RADIO_BASE+0x110))   // Radio has been disabled
#define RADIO_EVENT_DEVMATCH    (*(volatile uint32_t*) (RADIO_BASE+0x114))   // A device address match occurred on the last received packet
#define RADIO_EVENT_DEVMISS     (*(volatile uint32_t*) (RADIO_BASE+0x118))   // No device address match occurred on the last received packet
#define RADIO_EVENT_RSSIEND     (*(volatile uint32_t*) (RADIO_BASE+0x11C))   // Sampling of receive signal strength complete. A new RSSI sample is ready for readout from the RSSISAMPLE register.
#define RADIO_EVENT_BCMATCH     (*(volatile uint32_t*) (RADIO_BASE+0x128))   // Bit counter reached bit count value specified in the BCC register

// Registers
#define RADIO_SHORTS            (*(volatile uint32_t*) (RADIO_BASE+0x200))   // Shortcut register
#define RADIO_INTENSET          (*(volatile uint32_t*) (RADIO_BASE+0x304))   // Enable interrupt
#define RADIO_INTENCLR          (*(volatile uint32_t*) (RADIO_BASE+0x308))   // Disable interrupt
#define RADIO_CRCSTATUS         (*(volatile uint32_t*) (RADIO_BASE+0x400))   // CRC status
#define RADIO_RXMATCH           (*(volatile uint32_t*) (RADIO_BASE+0x408))   // Received address
#define RADIO_RXCRC             (*(volatile uint32_t*) (RADIO_BASE+0x40C))   // CRC field of previously received packet
#define RADIO_DAI               (*(volatile uint32_t*) (RADIO_BASE+0x410))   // Device address match index
#define RADIO_PACKETPTR         (*(volatile uint32_t*) (RADIO_BASE+0x504))   // Packet pointer
#define RADIO_FREQUENCY         (*(volatile uint32_t*) (RADIO_BASE+0x508))   // Frequency
#define RADIO_TXPOWER           (*(volatile uint32_t*) (RADIO_BASE+0x50C))   // Output power
#define RADIO_MODE              (*(volatile uint32_t*) (RADIO_BASE+0x510))   // Data rate and modulation
#define RADIO_PCNF0             (*(volatile uint32_t*) (RADIO_BASE+0x514))   // Packet configuration register 0
#define RADIO_PCNF1             (*(volatile uint32_t*) (RADIO_BASE+0x518))   // Packet configuration register 1
#define RADIO_BASE0             (*(volatile uint32_t*) (RADIO_BASE+0x51C))   // Base address 0
#define RADIO_BASE1             (*(volatile uint32_t*) (RADIO_BASE+0x520))   // Base address 1
#define RADIO_PREFIX0           (*(volatile uint32_t*) (RADIO_BASE+0x524))   // Prefixes bytes for logical addresses 0-3
#define RADIO_PREFIX1           (*(volatile uint32_t*) (RADIO_BASE+0x528))   // Prefixes bytes for logical addresses 4-7
#define RADIO_TXADDRESS         (*(volatile uint32_t*) (RADIO_BASE+0x52C))   // Transmit address select
#define RADIO_RXADDRESSES       (*(volatile uint32_t*) (RADIO_BASE+0x530))   // Receive address select
#define RADIO_CRCCNF            (*(volatile uint32_t*) (RADIO_BASE+0x534))   // CRC configuration
#define RADIO_CRCPOLY           (*(volatile uint32_t*) (RADIO_BASE+0x538))   // CRC polynomial
#define RADIO_CRCINIT           (*(volatile uint32_t*) (RADIO_BASE+0x53C))   // CRC initial value
#define RADIO_TIFS              (*(volatile uint32_t*) (RADIO_BASE+0x540))   // Test features enable register
#define RADIO_TEST              (*(volatile uint32_t*) (RADIO_BASE+0x544))   // Inter Frame Spacing: Time interval in us between consecutive packets
#define RADIO_RSSISAMPLE        (*(volatile uint32_t*) (RADIO_BASE+0x548))   // RSSI sample
#define RADIO_STATE             (*(volatile uint32_t*) (RADIO_BASE+0x550))   // Current radio state
#define RADIO_DATAWHITEIV       (*(volatile uint32_t*) (RADIO_BASE+0x554))   // Data whitening initial value
#define RADIO_BCC               (*(volatile uint32_t*) (RADIO_BASE+0x560))   // Bit counter compare
/*
 * Arrays are always pointers in C, therefore no asterisk after uint32_t[x].
 * Accessing array elements by index (e.g. RADIO_DAB[5]) equals pointer dereferencing in C, therefore no asterisk before (volatile...)
 * => RADIO_DAB is a pointer to an array of 32bit unsigned integers, which has 8 elements and is located at memory address RADIO_BASE+0x600.
 */
#define RADIO_DAB                ((volatile uint32_t[8]) {RADIO_BASE+0x600})   // Device address base
#define RADIO_DAP                ((volatile uint32_t[8]) {RADIO_BASE+0x620})   // Device address prefix
#define RADIO_DACNF             (*(volatile uint32_t*)   (RADIO_BASE+0x640))   // Device address match configuration
#define RADIO_OVERRIDE           ((volatile uint32_t[5]) {RADIO_BASE+0x724})   // Trim value override
#define RADIO_POWER             (*(volatile uint32_t*)   (RADIO_BASE+0xFFC))   // Peripheral power control


/*
 * Valid register values
 */

// for RADIO_SHORTS
#define RADIO_SHORTCUT_READY_START          (1 << 0)
#define RADIO_SHORTCUT_END_DISABLE          (1 << 1)
#define RADIO_SHORTCUT_DISABLED_TXEN        (1 << 2)
#define RADIO_SHORTCUT_DISABLED_RXEN        (1 << 3)
#define RADIO_SHORTCUT_ADDRESS_RSSISTART    (1 << 4)
#define RADIO_SHORTCUT_END_START            (1 << 5)
#define RADIO_SHORTCUT_ADDRESS_BCSTART      (1 << 6)
#define RADIO_SHORTCUT_DISABLED_RSSISTOP    (1 << 8)

// for RADIO_INTENSET and RADIO_INTENCLR
#define RADIO_INTERRUPT_READY               (1 << 0)
#define RADIO_INTERRUPT_ADDRESS             (1 << 1)
#define RADIO_INTERRUPT_PAYLOAD             (1 << 2)
#define RADIO_INTERRUPT_END                 (1 << 3)
#define RADIO_INTERRUPT_DISABLED            (1 << 4)
#define RADIO_INTERRUPT_DEVMATCH            (1 << 5)
#define RADIO_INTERRUPT_DEVMISS             (1 << 6)
#define RADIO_INTERRUPT_RSSIEND             (1 << 7)
#define RADIO_INTERRUPT_BCMATCH             (1 << 10)
#define RADIO_INTERRUPTS_ALL                0x4F

// for RADIO_TXPOWER
#define RADIO_TXPOWER_POS4DBM               0x04
#define RADIO_TXPOWER_0DBM                  0x00
#define RADIO_TXPOWER_NEG4DBM               0xFC 
#define RADIO_TXPOWER_NEG8DBM               0xF8
#define RADIO_TXPOWER_NEG12DBM              0xF4
#define RADIO_TXPOWER_NEG16DBM              0xF0
#define RADIO_TXPOWER_NEG20DBM              0xEC
#define RADIO_TXPOWER_NEG30DBM              0xD8

// for RADIO_MODE
#define RADIO_MODE_NRF_1MBIT                0
#define RADIO_MODE_NRF_2MBIT                1
#define RADIO_MODE_NRF_250KBIT              2
#define RADIO_MODE_BLE_1MBIT                3

// for RADIO_TXADDRESS
#define RADIO_TXADDR0                       0
#define RADIO_TXADDR1                       1
#define RADIO_TXADDR2                       2
#define RADIO_TXADDR3                       3
#define RADIO_TXADDR4                       4
#define RADIO_TXADDR5                       5
#define RADIO_TXADDR6                       6
#define RADIO_TXADDR7                       7

// for RADIO_RXADDRESSES
#define RADIO_RXADDR0                      (1 << 0)
#define RADIO_RXADDR1                      (1 << 1)
#define RADIO_RXADDR2                      (1 << 2)
#define RADIO_RXADDR3                      (1 << 3)
#define RADIO_RXADDR4                      (1 << 4)
#define RADIO_RXADDR5                      (1 << 5)
#define RADIO_RXADDR6                      (1 << 6)
#define RADIO_RXADDR7                      (1 << 7)

// for RADIO_CRCCNF
#define RADIO_CRCCNF_LEN_DISABLED           0
#define RADIO_CRCCNF_LEN_1                  1
#define RADIO_CRCCNF_LEN_2                  2
#define RADIO_CRCCNF_LEN_3                  3
#define RADIO_CRCCNF_INCLADDR              (0 << 8)
#define RADIO_CRCCNF_SKIPADDR              (1 << 8)

// for RADIO_STATE
#define RADIO_STATE_DISABLED                0
#define RADIO_STATE_RXRU                    1
#define RADIO_STATE_RXIDLE                  2
#define RADIO_STATE_RX                      3
#define RADIO_STATE_RXDISABLE               4
#define RADIO_STATE_TXRU                    9
#define RADIO_STATE_TXIDLE                  10
#define RADIO_STATE_TX                      11
#define RADIO_STATE_TXDISABLE               12


/*
 * Register macros
 */
// Interrupt-related
#define RADIO_INTERRUPT                     1
#define radio_interrupt_enable              interrupt_enable(RADIO_INTERRUPT)
#define radio_interrupt_disable             interrupt_disable(RADIO_INTERRUPT)

// Event-related
#define radio_clear_all_events  \
{                               \
    RADIO_EVENT_READY = 0;      \
    RADIO_EVENT_ADDRESS = 0;    \
    RADIO_EVENT_PAYLOAD = 0;    \
    RADIO_EVENT_END = 0;        \
    RADIO_EVENT_DISABLED = 0;   \
}

// CRC-related
#define radio_crcstatus_ok                 (RADIO_CRCSTATUS == 1)

// for RADIO_PCNF0
#define radio_set_length_lf(num_bits)       RADIO_PCNF0 = (RADIO_PCNF0 & 0xFFFFFFF0) |  (num_bits  & 0x0F)
#define radio_set_length_s0(num_bytes)      RADIO_PCNF0 = (RADIO_PCNF0 & 0xFFFFFEFF) | ((num_bytes & 0x01) <<  8)
#define radio_set_length_s1(num_bits)       RADIO_PCNF0 = (RADIO_PCNF0 & 0xFFF0FFFF) | ((num_bits  & 0x0F) << 16)

// for RADIO_PCNF1
#define radio_data_whitening_enable         RADIO_PCNF1 |=  (1 << 25) // set bit
#define radio_data_whitening_disable        RADIO_PCNF1 &= ~(1 << 25) // clear bit 
#define radio_get_max_payload_length       (RADIO_PCNF1 & 0x000000FF)
#define radio_set_max_payload_length(len)   RADIO_PCNF1  = (RADIO_PCNF1 & 0xFFFFFF00) | (len & 0xFF)
#define radio_set_access_address_size(size) RADIO_PCNF1  = (RADIO_PCNF1 & 0xFFF8FFFF) | ((size & 0x07) << 16)

// for RADIO_BASEx
#define radio_set_address_base(n, val)      if (n == 0) \
                                                RADIO_BASE0 = val; \
                                            else if (n == 1) \
                                                RADIO_BASE1 = val;

/*
 * 8bit address prefix (AP)
 *  AP0-4: read from RADIO_PREFIX0
 *  AP5-7: read from RADIO_PREFIX1
 */
#define radio_get_address_prefix(n)         (n < 4 ? \
                                                 RADIO_PREFIX0 & (0xFF << (n*8))) \
                                            :    RADIO_PREFIX1 & (0xFF << ((n%4)*8)))
#define radio_set_address_prefix(n, val)    if (n < 4) \
                                                 RADIO_PREFIX0 = (val & 0xFF) << (n*8); \
                                            else RADIO_PREFIX1 = (val & 0xFF) << ((n%4)*8);

/*
 * Link Layer
 * Bluetooth Core 4.1 specification, section 2.1, page 2503
 */
#define RADIO_PDU_MAX            39
#define RADIO_PDU_MIN            2

#define RADIO_FLAGS_RX_NEXT      1
#define RADIO_FLAGS_TX_NEXT      2


/*
 * The active parameter informs if the radio is currently active (e.g. because
 * of a TX/RX_NEXT flag). So, if the callback implementation wants to operate
 * the radio, it will need to first stop the radio.
 */
typedef void (*radio_receive_callback_t) (const uint8_t *pdu, bool crc, bool active);
typedef void (*radio_send_callback_t) (bool active);

void radio_init();
void radio_set_callbacks(radio_receive_callback_t recv_callback, radio_send_callback_t send_callback);
bool radio_prepare(uint8_t ch, uint32_t addr, uint32_t crcinit);
void radio_recv(uint32_t flags);
void radio_send(uint8_t *data, uint32_t flags);
void radio_stop();
void radio_set_out_buffer(uint8_t *buf);

#endif
