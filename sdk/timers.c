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
 *
 * This library only uses TIMER0.
 * (There are two more available on the nRF51822.)
 * Each counter has four counter registers (TIMER_CC[0-3]),
 * being counted up or down simultaneously.
 * Here we count down (see function timer_init)
 * with a frequency of 1 MHz.
 */

#include "timers.h"

// The high frequency clock runs at 16 MHz.
#define HFCLK               16000000UL

// timer clock = 16 MHz / (2^TIMER_PRESCALER)
// set timer clock to 1 MHz:
#define TIMER_PRESCALE      4

/*
 * The implementation of repeated timers inserts a constant drift in every
 * repetition because of the interruption context switch until initialize
 * TIMER0_IRQHandler(). The define below calculates the number of ticks until
 * the exception handler is executed.
 */
#define DRIFT_FIX           (1UL << (5 - TIMER_PRESCALE))

#define ROUNDED_DIV(A, B)   (((A) + ((B) / 2)) / (B))
#define POW2(e)             ROUNDED_DIV(2 << e, 2)
#define BIT(n)              (1 << n)

#define COUNTERS_PER_TIMER  4

typedef struct
{
    uint32_t            ticks;
    timer_callback_t    callback;
    uint8_t             enabled:1;
    uint8_t             active:1;
    uint8_t             type:1;
} cc_t;

static cc_t timer_cc[COUNTERS_PER_TIMER];
static uint8_t ccs_active = 0;

static __inline uint32_t us2ticks(uint64_t us)
{
    return ROUNDED_DIV(us * HFCLK, TIMER_SECONDS(1) * POW2(TIMER_PRESCALE));
}

static __inline uint32_t ticks2us(uint64_t ticks)
{
    return ROUNDED_DIV(ticks * TIMER_SECONDS(1) * POW2(TIMER_PRESCALE), HFCLK);
}

static __inline uint32_t get_curr_ticks()
{
    // save current value of CC[3]
    uint32_t cc3 = TIMER_CC(TIMER0)[3];

    // capture current tick count to CC[3]
    TIMER_TASK_CAPTURE(TIMER0)[3] = 1;

    // read tick count from CC[3]
    uint32_t ticks = TIMER_CC(TIMER0)[3];

    // write previous value back to CC[3]
    TIMER_CC(TIMER0)[3] = cc3;

    return ticks;
}

static __inline void set_timer(uint8_t id, uint32_t ticks)
{
    // program timeout
    TIMER_CC(TIMER0)[id] = ticks;

    // enable this compare number's interrupt
    timer_interrupt_upon_compare_enable(TIMER0, id);
}

//void TIMER0_Handler()
void stub()
{
    uint32_t curr = get_curr_ticks();
    uint8_t id_mask = 0;
    uint8_t id;

    for (id = 0; id < COUNTERS_PER_TIMER; id++)
    {
        if (TIMER_EVENT_COMPARE(TIMER0)[id])
        {
            // clear event
            TIMER_EVENT_COMPARE(TIMER0)[id] = 0UL;

            if (timer_cc[id].active)
                id_mask |= BIT(id);
        }
    }

    for (id = 0; id < COUNTERS_PER_TIMER; id++)
    {
        if (id_mask & BIT(id))
        {
            if (timer_cc[id].type == TIMER_REPEATED)
            {
                set_timer(id, curr + timer_cc[id].ticks - DRIFT_FIX);
            }
            else if (timer_cc[id].type == TIMER_SINGLESHOT)
            {
                timer_cc[id].active = 0;
                ccs_active--;

                // not counter is running any more?
                if (ccs_active == 0)
                {
                    //  disable timer
                    TIMER_TASK_STOP(TIMER0)  = 1;
                    TIMER_TASK_CLEAR(TIMER0) = 1;
                }
            }

            timer_cc[id].callback();
        }
    }
}

/**
 * Initialize and configure TIMER0
 */
bool timer_init()
{
    // configure 16MHz crystal frequency
    CLOCK_XTALFREQ = 0xFF;

    // according to the Reference Manual the RADIO requires the crystal as clock source
    CLOCK_HFCLKSTAT = 1;

    // start high frequency clock
    if (!CLOCK_EVENT_HFCLKSTARTED)
    {
        CLOCK_TASK_HFCLKSTART = 1;
        while (!CLOCK_EVENT_HFCLKSTARTED)
            asm("nop");
    }

    TIMER_MODE(TIMER0)      = TIMER_MODE_TIMER;
    TIMER_BITMODE(TIMER0)   = TIMER_BITMODE_24BIT;
    TIMER_PRESCALER(TIMER0) = TIMER_PRESCALE;

    TIMER_INTENCLR(TIMER0)  = ~0;
    interrupt_enable(TIMER0_INTERRUPT);

    // initialize counters
    memset(timer_cc, 0, sizeof(timer_cc));

    return 0;
}

/**
 * Create a new timer_cc in TIMER0
 *
 * @Returns
 *      the number of the timer_cc, if successfull
 *      -1, if no slot available
 *      -2, if the specified tiemr type was invalid
 */
int8_t timer_create(uint8_t type)
{
    int8_t id;

    if (type != TIMER_SINGLESHOT && type != TIMER_REPEATED)
        return -2;

    for (id = 0; id < COUNTERS_PER_TIMER; id++) {
        if (!timer_cc[id].enabled)
            goto create;
    }

    return -1;

create:
    timer_cc[id].enabled = 1;
    timer_cc[id].active = 0;
    timer_cc[id].type = type;

    return id;
}

bool timer_start(int8_t id, uint32_t us, timer_callback_t callback)
{
    uint32_t curr = get_curr_ticks();
    uint32_t ticks;

    if (id < 0)
        return false;

    if (!timer_cc[id].enabled)
        return false;

    if (timer_cc[id].active)
        return false;

    ticks = us2ticks(us);

    if (ticks >= 0xFFFFFF)
        return false;

    set_timer(id, curr + ticks);

    timer_cc[id].active = 1;
    timer_cc[id].ticks = ticks;
    timer_cc[id].callback = callback;

    if (ccs_active == 0) {
        TIMER_TASK_START(TIMER0) = 1;
    }

    ccs_active++;

    return true;
}

bool timer_stop(int8_t id)
{
    if (id < 0 || id >= COUNTERS_PER_TIMER)
        return false;

    if (!timer_cc[id].active)
        return false;

    // disable this compare number's interrupt
    timer_interrupt_upon_compare_disable(TIMER0, id);

    timer_cc[id].active = 0;
    ccs_active--;

    if (ccs_active == 0)
    {
        TIMER_TASK_STOP(TIMER0)  = 1;
        TIMER_TASK_CLEAR(TIMER0) = 1;
    }

    return 0;
}

uint32_t timer_get_remaining_us(int8_t id)
{
    uint32_t ticks = 0;
    uint32_t curr = get_curr_ticks();

    if (!timer_cc[id].active)
        return 0;

    if (TIMER_CC(TIMER0)[id] > curr)
        ticks = TIMER_CC(TIMER0)[id] - curr;
    else
        ticks = (0xFFFFFF - curr) + TIMER_CC(TIMER0)[id];

    return ticks2us(ticks);
}
