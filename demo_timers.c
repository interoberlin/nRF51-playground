
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "nrf_gpio.h"
#include "timers.h"

#define PIN_LED 20

void setup_led()
{
    nrf_gpio_pin_dir_set(PIN_LED, NRF_GPIO_PIN_DIR_OUTPUT);
}

void setup_timer()
{
    timer_init(TIMER0);
    //...
}

void main()
{
    setup_led();
    setup_timer();

    while (true)
    {
        asm("wfi");
    }
}
