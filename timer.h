/**
 * TIMER library
 * for the Nordic Semiconductor nRF51 series
 *
 * Authors:
 *      Matthias Bock <mail@matthiasbock.net>
 *      Paulo B. de Oliveira Filho <pauloborgesfilho@gmail.com>
 *      Claudio Takahasi <claudio.takahasi@gmail.com>
 *      João Paulo Rechi Vita <jprvita@gmail.com>
 *
 * License: GNU GPLv3
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <stdint.h>
#include "cortex_m0.h"
#include "heap.h"
#include "clock.h"

// BASE
#define TIMER0 0x40008000
#define TIMER1 0x40009000
#define TIMER2 0x4000A000

// Tasks
#define TIMER_TASK_START(timer)         (*(volatile uint32_t*) (timer+0x000))  // Start Timer
#define TIMER_TASK_STOP(timer)          (*(volatile uint32_t*) (timer+0x004))  // Stop Timer
#define TIMER_TASK_COUNT(timer)         (*(volatile uint32_t*) (timer+0x008))  // Increment Timer (Counter mode only)
#define TIMER_TASK_CLEAR(timer)         (*(volatile uint32_t*) (timer+0x00C))  // Clear time
#define TIMER_TASK_SHUTDOWN(timer)      (*(volatile uint32_t*) (timer+0x010))  // Shut down timer
#define TIMER_TASK_CAPTURE(timer)      ((volatile uint32_t[4]) {timer+0x040})  // Capture Timer value to CC[0] register

// Events
#define TIMER_EVENT_COMPARE(timer)     ((volatile uint32_t[4]) {timer+0x140})  // Compare event on CC[0] match

// Registers
#define TIMER_SHORTS(timer)             (*(volatile uint32_t*) (timer+0x200))  // Shortcut register
#define TIMER_INTENSET(timer)           (*(volatile uint32_t*) (timer+0x304))  // Enable interrupt
#define TIMER_INTENCLR(timer)           (*(volatile uint32_t*) (timer+0x308))  // Disable interrupt
#define TIMER_MODE(timer)               (*(volatile uint32_t*) (timer+0x504))  // Timer mode selection
#define TIMER_BITMODE(timer)            (*(volatile uint32_t*) (timer+0x508))  // Configure the number of bits used by the TIMER
#define TIMER_PRESCALER(timer)          (*(volatile uint32_t*) (timer+0x510))  // Timer prescaler register
#define TIMER_CC(timer)                ((volatile uint32_t[4]) {timer+0x540})  // Capture/Compare register 0

// Shortcuts
#define TIMER_SHORTCUT_COMPARE_CLEAR(compare_number)                (1 << compare_number)
#define TIMER_SHORTCUT_COMPARE_STOP(compare_number)                 (1 << (compare_number+8))

// Interrupts
#define TIMER0_INTERRUPT        8
#define TIMER1_INTERRUPT        9
#define TIMER2_INTERRUPT        10

#define TIMER_INTERRUPT_UPON_COMPARE(compare_number)                (1 << (compare_number+16))
#define timer_interrupt_upon_compare_enable(timer, compare_number)  TIMER_INTENSET(timer) = TIMER_INTERRUPT_UPON_COMPARE(compare_number)
#define timer_interrupt_upon_compare_disable(timer, compare_number) TIMER_INTENCLR(timer) = TIMER_INTERRUPT_UPON_COMPARE(compare_number)

// Mode
#define TIMER_MODE_COUNTDOWN    0
#define TIMER_MODE_COUNTUP      1

// Bitmode
#define TIMER_BITMODE_16BIT     0
#define TIMER_BITMODE_8BIT      1
#define TIMER_BITMODE_24BIT     2
#define TIMER_BITMODE_32BIT     3


// blessed
#define TIMER_SINGLESHOT        0
#define TIMER_REPEATED          1

#define TIMER_MILLIS(v)         (v * 1000UL)    /* ms -> us */
#define TIMER_SECONDS(v)        (v * 1000000UL) /* s -> us */

typedef void (*timer_callback_t)();

bool     timer_init();
int8_t   timer_create(uint8_t type);
bool     timer_start(int8_t id, uint32_t us, timer_callback_t callback);
bool     timer_stop(int8_t id);
uint32_t timer_get_remaining_us(int8_t id);

#endif
