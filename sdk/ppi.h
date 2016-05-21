
/**
 * Programmable Peripheral Interconnect (PPI) library
 * for the Nordic Semiconductor nRF51 series
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef PPI_H
#define PPI_H

#include <stdint.h>

#define PPI_BASE        0x4001F000

/*
 * Tasks
 */
static typedef struct
{
    uint32_t ENABLE;
    uint32_t DISABLE;
} ppi_chg_en_dis_t;

#define PPI_TASK_CHG    ((volatile ppi_chg_en_dis_t[4]) {PPI_BASE+0x000})

/*
 * Registers
 */
// enable/disable PPI channels
#define PPI_CHEN        (*(volatile uint32_t*) (PPI_BASE+0x500))
#define PPI_CHENSET     (*(volatile uint32_t*) (PPI_BASE+0x504))
#define PPI_CHENCLR     (*(volatile uint32_t*) (PPI_BASE+0x508))

// channels: event endpoints, task endpoints
static typedef struct
{
    uint32_t EEP;
    uint32_t TEP;
} ppi_ch_t;

#define PPI_CH          ((volatile ppi_ch_t[16]) {PPI_BASE+0x510})

// channel groups
#define PPI_CHG         ((volatile uint32_t[4]) {PPI_BASE+0x800})

#endif
