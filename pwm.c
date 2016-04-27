
/**
 * Pulse Width Modulation (PWM) library
 * for the Nordic Semiconductor nRF51 series
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
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
        uint8_t  gpiote_channel0,
        uint8_t  gpiote_channel1,
        uint32_t timer,
        uint8_t  timer_counter0,
        uint8_t  timer_counter1,
        uint8_t  timer_bitmode,
        uint8_t  ppi_channel0,
        uint8_t  ppi_channel1
        )
{
    // fill PWM struct with arguments
    pwm->gpio_pin           = gpio_pin;
    pwm->gpiote_channel0    = gpiote_channel0;
    pwm->gpiote_channel1    = gpiote_channel1;
    pwm->timer              = timer;
    pwm->timer_counter0     = timer_counter0;
    pwm->timer_counter1     = timer_counter1;
    pwm->ppi_channel0       = ppi_channel0;
    pwm->ppi_channel1       = ppi_channel1;

    // configure GPIO tasks and events
    GPIOTE_CONFIG[pwm->gpiote_channel0] = GPIOTE_MODE_TASK | GPIOTE_PSEL(pwm->gpio_pin) | GPIOTE_POLARITY_HIGH_TO_LOW;
    GPIOTE_CONFIG[pwm->gpiote_channel1] = GPIOTE_MODE_TASK | GPIOTE_PSEL(pwm->gpio_pin) | GPIOTE_POLARITY_LOW_TO_HIGH;

    // configure timer: frequency, mode, shortcuts and counter values
    TIMER_PRESCALER(pwm->timer)  = 23; // 128 Hz
    TIMER_MODE(pwm->timer)       = TIMER_MODE_COUNTUP;
    TIMER_BITMODE(pwm->timer)    = TIMER_BITMODE_8BIT;

    // trigger an event, when the counter is set to zero
    TIMER_CC(pwm->timer)[pwm->timer_counter0] = 0;
    // trigger an event, when the counter reaches 128 (50%)
    TIMER_CC(pwm->timer)[pwm->timer_counter1] = 128;

    // configure PPI connection between GPIOTE and Timer:

    // connect "Timer counter reaches zero"-event to "Switch GPIO pin to HIGH"-task
    PPI_CH[pwm->ppi_channel0]->EEP = &TIMER_EVENT_COMPARE(pwm->timer)[pwm->timer_counter0];
    PPI_CH[pwm->ppi_channel0]->TEP = &GPIOTE_TASK_OUT[pwm->gpiote_channel0];

    // connect "Timer counter reaches counter1 value"-event to "Switch GPIO pin to LOW"-task
    PPI_CH[pwm->ppi_channel1]->EEP = &TIMER_EVENT_COMPARE(pwm->timer)[pwm->timer_counter1];
    PPI_CH[pwm->ppi_channel1]->TEP = &GPIOTE_TASK_OUT[pwm->gpiote_channel1];
}

void pwm_start(pwm_t* pwm)
{
    // enable the configured PPI channels
    PPI_CHENSET(pwm->ppi_channel0) = 1;
    PPI_CHENSET(pwm->ppi_channel1) = 1;

    // enable Timer
    TIMER_START(pwm->timer) = 1;
}

void pwm_write(pwm_t* pwm, uint32_t value)
{
    // adjust switch-low counter value
    TIMER_CC(pwm->timer)[pwm->timer_counter1] = value;
}

void pwm_stop(pwm_t* pwm)
{
    // disable the configured PPI channels
    PPI_CHENCLR(pwm->ppi_channel0) = 1;
    PPI_CHENCLR(pwm->ppi_channel1) = 1;

    // stop/shutdown Timer
    TIMER_SHUTDOWN(pwm->timer) = 1;
}
