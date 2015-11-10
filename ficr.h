
/**
 * Factory Information Configuration Registers (FICR) library
 * for the Nordic Semiconductor nRF51 series
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 *
 * License: GNU GPLv3
 */

#ifndef FICR_H
#define FICR_H

#define FICR_BASE               0x10000000

#define FICR_CODEPAGESIZE       (*(volatile uint32_t*) (FICR_BASE+0x10))    // Code memory page size
#define FICR_CODESIZE           (*(volatile uint32_t*) (FICR_BASE+0x14))    // Code memory size
#define FICR_CLENR0             (*(volatile uint32_t*) (FICR_BASE+0x28))    // Length of Code region 0 in bytes
#define FICR_PPFC               (*(volatile uint32_t*) (FICR_BASE+0x2C))    // Pre-programmed factory Code present
#define FICR_NUMRAMBLOCK        (*(volatile uint32_t*) (FICR_BASE+0x34))    // Number of individually controllable RAM blocks
#define FICR_SIZERAMBLOCKS      (*(volatile uint32_t*) (FICR_BASE+0x38))    // RAM block size, in bytes
#define FICR_SIZERAMBLOCK      ((volatile uint32_t[4]) {FICR_BASE+0x38})    // Size of RAM block, in bytes
#define FICR_CONFIGID           (*(volatile uint32_t*) (FICR_BASE+0x5C))    // Configuration identifier
#define FICR_DEVICEID          ((volatile uint32_t[2]) {FICR_BASE+0x60})    // Device identifier
#define FICR_ER                ((volatile uint32_t[4]) {FICR_BASE+0x80})    // Encryption Root
#define FICR_IR                ((volatile uint32_t[4]) {FICR_BASE+0x90})    // Identity Root
#define FICR_DEVICEADDRTYPE     (*(volatile uint32_t*) (FICR_BASE+0xA0))    // Device address type
#define FICR_DEVICEADDR        ((volatile uint32_t[2]) {FICR_BASE+0xA4})    // Device address
#define FICR_OVERRIDEEN         (*(volatile uint32_t*) (FICR_BASE+0xAC))    // Override enable
#define FICR_NRF_1MBIT         ((volatile uint32_t[5]) {FICR_BASE+0xB0})    // Override value for NRF_1MBIT mode
#define FICR_BLE_1MBIT         ((volatile uint32_t[5]) {FICR_BASE+0xEC})    // Override value for BLE_1MBIT mode

#endif RADIO_H
