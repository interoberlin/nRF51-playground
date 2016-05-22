
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"
#include "timers.h"

#define PIN_LED 20
#define my_timer TIMER0

void setup_led()
{
    // configure LED pin as output
    gpio_config_output(PIN_LED);
}

void setup_timer()
{
    // reset timer
    TIMER_TASK_STOP(my_timer) = 1;
    TIMER_TASK_CLEAR(my_timer) = 1;

    // configure 16MHz crystal frequency as clock source
    CLOCK_XTALFREQ = 0xFF;

    // start high frequency clock
    // (is that necessary?)
    CLOCK_HFCLKSTAT = 1;

    // configure timer frequency
    TIMER_PRESCALER(my_timer) = 9;

    // configure timer mode
    TIMER_MODE(my_timer)     = TIMER_MODE_TIMER;
    TIMER_BITMODE(my_timer)  = TIMER_BITMODE_24BIT;

    // configure timer interrupt
    TIMER_CC(my_timer)[0] = 0;
    TIMER_INTENSET(my_timer) = TIMER_INTERRUPT_UPON_COMPARE(0);
    interrupt_enable(TIMER0_INTERRUPT);

    // configure shortcut
    TIMER_SHORTCUTS(my_timer) =
            TIMER_SHORTCUT_COMPARE_CLEAR(0)
          | TIMER_SHORTCUT_COMPARE_STOP(0);
}

void TIMER0_Handler()
{
    // XOR with the mask for the chosen LED pin to toggle corresponding bit
    GPIO_OUT = GPIO_OUT ^ (1 << PIN_LED);

    // restart timer
    TIMER_TASK_START(my_timer) = 1;
}

void main()
{
    setup_led();
    setup_timer();

    // enable timer
    TIMER_TASK_START(my_timer) = 1;

    while (true)
    {
        asm("wfi");
    }
}
