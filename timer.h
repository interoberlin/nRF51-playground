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

#define TIMER0 0x40008000
#define TIMER1 0x40009000
#define TIMER2 0x4000A000

/*
 * ...
 */

#define TIMER_SINGLESHOT        0
#define TIMER_REPEATED          1

#define TIMER_MILLIS(v)         (v * 1000UL)    /* ms -> us */
#define TIMER_SECONDS(v)        (v * 1000000UL) /* s -> us */

typedef void (*timer_cb_t)();

int16_t timer_init();
int16_t timer_create(uint8_t type);
int16_t timer_start(int16_t id, uint32_t us, timer_cb_t cb);
int16_t timer_stop(int16_t id);
uint32_t timer_get_remaining_us(int16_t id);

#endif
