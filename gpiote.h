
/**
 * GPIO Tasks and Events (GPIOTE) library
 * for the Nordic Semiconductor nRF51 series
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef GPIOTE_H
#define GPIOTE_H

#define GPIOTE_BASE         0x40006000

/*
 * Tasks
 */
#define GPIOTE_TASK_OUT     ((volatile uint32_t[4]) {GPIOTE_BASE+0x000})

/*
 * Events
 */
#define GPIOTE_EVENT_IN     ((volatile uint32_t[4]) {GPIOTE_BASE+0x100})
#define GPIOTE_PORT        (*(volatile uint32_t*)   (GPIOTE_BASE+0x170))

/*
 * Registers
 */
#define GPIOTE_INTEN       (*(volatile uint32_t*)   (GPIOTE_BASE+0x300))
#define GPIOTE_INTENSET    (*(volatile uint32_t*)   (GPIOTE_BASE+0x304))
#define GPIOTE_INTENCLR    (*(volatile uint32_t*)   (GPIOTE_BASE+0x308))
#define GPIOTE_CONFIG       ((volatile uint32_t[4]) {GPIOTE_BASE+0x510})

/*
 * GPIOTE_CONFIG values
 */
#define GPIOTE_MODE_DISABLED            0
#define GPIOTE_MODE_EVENT               1
#define GPIOTE_MODE_TASK                3

#define GPIOTE_PSEL(gpio_pin)           ((gpio_pin & 0x1F) << 8)

#define GPIOTE_POLARITY_NONE            0
#define GPIOTE_POLARITY_LOW_TO_HIGH     (1 << 16)
#define GPIOTE_POLARITY_HIGH_TO_LOW     (2 << 16)
#define GPIOTE_POLARITY_TOGGLE          (3 << 16)

#define GPIOTE_OUTINT_LOW               0
#define GPIOTE_OUTINT_HIGH              (1 << 20)

#endif
