
/**
 * Pulse Width Modulation (PWM) library
 * for the Nordic Semiconductor nRF51 series
 *
 * Authors: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 *
 * Read more about the implementation approach:
 *  https://goo.gl/l8goi4
 */

#include "pwm.h"

// setup GPIO pin, PPI channels, Timer, Timer counters, Timer bitmode
void pwm_setup(
        pwm_t*   pwm,
        uint8_t  gpio_pin,
        uint8_t  gpiote_channel,
        uint8_t  ppi_channel,
        uint32_t timer,
        uint8_t  timer_counter0,
        uint8_t  timer_counter1,
        uint8_t  timer_bitmode
        )
{
    // configure GPIO tasks and events
    GPIOTE_CONFIG[0] = GPIOTE_MODE_TASK | GPIOTE_PSEL(gpio_pin) | GPIOTE_POLARITY_HighToLow;
    GPIOTE_CONFIG[1] = GPIOTE_MODE_TASK | GPIOTE_PSEL(gpio_pin) | GPIOTE_POLARITY_LowToHigh;

    // configure timer: frequency, mode, shortcuts and counter values
    TIMER0_PRESCALER = 23; // 128 Hz
    TIMER0_MODE = TIMER_MODE_COUNTUP;
    TIMER0_BITMODE = TIMER_BITMODE_8BIT;

    // trigger an event, when the counter is set to zero
    TIMER0_CC[0] = 0;
    // trigger an event, when the counter reaches 128 (50%)
    TIMER0_CC[1] = 128;

    // configure PPI connection between GPIOTE and Timer
    PPI_CHANNEL[0]->
    EVENT = &TIMER0_EVENT_COMPARE[0];
    TASK  = &GPIOTE_OUT[0];

    PPI_CHANNEL[1]->
    EVENT = &TIMER0_EVENT_COMPARE[1];
    TASK  = &GPIOTE_OUT[1];
}

void pwm_start(pwm_t* pwm)
{
    // enable the configured PPI channel
    PPI_CHENSET(pwm->ppi_channel) = 1;

    // enable Timer
    TIMER0_START = 1;
}

void pwm_write(pwm_t* pwm, uint32_t value)
{
    // adjust switch-low counter value
    TIMER0_CC[1] = value;
}

void pwm_stop(pwm_t* pwm)
{
    // disable the configured PPI channel
    PPI_CHENCLR(pwm->ppi_channel) = 1;

    // stop/shutdown Timer
    TIMER0_SHUTDOWN = 1;
}
