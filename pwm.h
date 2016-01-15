
/**
 * Pulse Width Modulation (PWM) library
 * for the Nordic Semiconductor nRF51 series
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 *
 * Requires:
 *      Timer library
 *      GPIO library
 *      GPIO Tasks and Events (GPIOTE) library
 *      Programmable Peripheral Interconnect (PPI) library
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#include "timer.h"
#include "gpiote.h"
#include "ppi.h"

typedef struct
{
    uint8_t  gpio_pin;
    uint8_t  gpiote_channel0, gpiote_channel1;
    uint32_t timer;
    uint8_t  timer_counter0, timer_counter1;
    uint8_t  ppi_channel0, ppi_channel1;
} pwm_t;


// setup GPIO pin, PPI channels, Timer, Timer counters, Timer bitmode
void pwm_setup(
        pwm_t*   pwm,
        uint8_t  gpio_pin,
        uint8_t  gpiote_channel0,
        uint8_t  gpiote_channel1,
        uint32_t timer,
        uint8_t  timer_counter0,
        uint8_t  timer_counter1,
        uint8_t  timer_bitmode,
        uint8_t  ppi_channel0,
        uint8_t  ppi_channel1
        );

void pwm_start(pwm_t* pwm);

void pwm_write(pwm_t* pwm, uint32_t value);

void pwm_stop(pwm_t* pwm);

#endif
