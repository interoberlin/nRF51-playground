
/*
 * Startup code for the nRF51
 * Required to setup the ARM Cortex M0 before it can be used
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#include <stdint.h>

/*
 * Addresses defined in the linker script
 */
extern uint32_t code_begin;
extern uint32_t code_end;

extern uint32_t data_begin;
extern uint32_t data_end;

extern uint32_t bss_begin;
extern uint32_t bss_end;

extern uint32_t stack_begin;
extern uint32_t stack_end;


/*
 * Interrupt service routines
 */
 
// invoked upon power-up; must be defined
void Reset_Handler();

// by default all interrupts are serviced by the default handler 
void Default_Handler();

#define WEAK_ALIAS(x)       __attribute__ ((weak, alias(#x)))

void NMI_Handler()          WEAK_ALIAS(Default_Handler);
void HardFault_Handler()    WEAK_ALIAS(Default_Handler);
void MemManage_Handler()    WEAK_ALIAS(Default_Handler);
void BusFault_Handler()     WEAK_ALIAS(Default_Handler);
void UsageFault_Handler()   WEAK_ALIAS(Default_Handler);
void SVC_Handler()          WEAK_ALIAS(Default_Handler);
void DebugMon_Handler()     WEAK_ALIAS(Default_Handler);
void PendSV_Handler()       WEAK_ALIAS(Default_Handler);
void SysTick_Handler()      WEAK_ALIAS(Default_Handler);

// SoC-specific ISRs
void POWER_CLOCK_Handler()  WEAK_ALIAS(Default_Handler);
void RADIO_Handler()        WEAK_ALIAS(Default_Handler);
//extern void uart_isr();
//void UART0_Handler()        WEAK_ALIAS(uart_isr);
void UART0_Handler()        WEAK_ALIAS(Default_Handler);
void SPI0_TWI0_Handler()    WEAK_ALIAS(Default_Handler);
void SPI1_TWI1_Handler()    WEAK_ALIAS(Default_Handler);
void RESERVED()             WEAK_ALIAS(Default_Handler);
void GPIOTE_Handler()       WEAK_ALIAS(Default_Handler);
void ADC_Handler()          WEAK_ALIAS(Default_Handler);
void TIMER0_Handler()       WEAK_ALIAS(Default_Handler);
void TIMER1_Handler()       WEAK_ALIAS(Default_Handler);
void TIMER2_Handler()       WEAK_ALIAS(Default_Handler);
void RTC0_Handler()         WEAK_ALIAS(Default_Handler);
void TEMP_Handler()         WEAK_ALIAS(Default_Handler);
void RNG_Handler()          WEAK_ALIAS(Default_Handler);
void ECB_Handler()          WEAK_ALIAS(Default_Handler);
void CCM_AAR_Handler()      WEAK_ALIAS(Default_Handler);
void WDT_Handler()          WEAK_ALIAS(Default_Handler);
void RTC1_Handler()         WEAK_ALIAS(Default_Handler);
void QDEC_Handler()         WEAK_ALIAS(Default_Handler);
void LPCOMP_Handler()       WEAK_ALIAS(Default_Handler);
void SWI0_Handler()         WEAK_ALIAS(Default_Handler);
void SWI1_Handler()         WEAK_ALIAS(Default_Handler);
void SWI2_Handler()         WEAK_ALIAS(Default_Handler);
void SWI3_Handler()         WEAK_ALIAS(Default_Handler);
void SWI4_Handler()         WEAK_ALIAS(Default_Handler);
void SWI5_Handler()         WEAK_ALIAS(Default_Handler);

#define RESERVED 0

/*
 * Exception vector table
 * http://community.arm.com/docs/DOC-8769
 */
void *vectors[] __attribute__ ((section(".vectors"))) =
{
    &stack_end,

    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    RESERVED,
    RESERVED,
    RESERVED,
    RESERVED,
    SVC_Handler,
    DebugMon_Handler,
    RESERVED,
    PendSV_Handler,
    SysTick_Handler,

    POWER_CLOCK_Handler,
    RADIO_Handler,
    UART0_Handler,
    SPI0_TWI0_Handler,
    SPI1_TWI1_Handler,
    RESERVED,
    GPIOTE_Handler,
    ADC_Handler,
    TIMER0_Handler,
    TIMER1_Handler,
    TIMER2_Handler,
    RTC0_Handler,
    TEMP_Handler,
    RNG_Handler,
    ECB_Handler,
    CCM_AAR_Handler,
    WDT_Handler,
    RTC1_Handler,
    QDEC_Handler,
    LPCOMP_Handler,
    SWI0_Handler,
    SWI1_Handler,
    SWI2_Handler,
    SWI3_Handler,
    SWI4_Handler,
    SWI5_Handler,
    RESERVED,
    RESERVED,
    RESERVED,
    RESERVED,
    RESERVED,
    RESERVED
};

// Reset_Handler() jumps to main() when finished
extern int main();

/*
 * This code is the first to execute
 * when the chip is powering up or reset
 */
void Reset_Handler()
{
    // copy data section from Flash to RAM
    uint8_t *src, *dst;
    src = (uint8_t*) &code_end;
    dst = (uint8_t*) &data_begin;
    while (dst < (uint8_t *) &data_end)
        *dst++ = *src++;

    // clear the bss section
    dst = (uint8_t*) &bss_begin;
    while (dst < (uint8_t*) bss_end)
        *dst++ = 0;

    // all initialization is done, jump to main function
    main();
}

/*
 * Default interrupt service routine:
 * In case of an unhandled interrupt, go to sleep.
 */
void Default_Handler()
{
    // sleep mode: wait for interrupt
    // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/BABHHGEB.html
    //asm("wfi");
    
    // read the interrupt control state register (ICSR)
    // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/Cihfaaha.html
    uint32_t ICSR = *((uint32_t volatile *)0xE000ED04);
    uint32_t VECTACTIVE  = ICSR & 0x0000003f;
    uint32_t VECTPENDING = ICSR & 0x0003f000;
}
