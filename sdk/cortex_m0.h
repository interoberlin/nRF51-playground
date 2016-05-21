
/**
 * Cortex M0 Registers and Low-level Routines
 *
 * Author:  Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

#ifndef CORTEX_M0_H
#define CORTEX_M0_H

/*
 * Nested Vector Interrupt Controller (NVIC)
 */

// Interrupt Control State Register (ICSR)
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/Cihfaaha.html

#define ICSR                        *((volatile uint32_t*) 0xE000ED04)
#define VECTACTIVE                   (ICSR & 0x0000003f)
#define VECTPENDING                 ((ICSR & 0x0003f000) >> 12)

// the first 17 vectors are: 1x stack pointer + 16x exception vectors (counting starts with zero)
#define INTACTIVE                   ((VECTACTIVE  > 16) ? (VECTACTIVE  - 16) : 0)
#define INTPENDING                  ((VECTPENDING > 16) ? (VECTPENDING - 16) : 0)

// Interrupt Set Enable Register (ISER)
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/Cihcajhj.html

#define ISER                        *((volatile uint32_t*) 0xE000E100)

#define interrupt_enable(IRQn)      ISER = (1 << IRQn)

// Interrupt Clear Enable Register (ICER)
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/Cihbiadi.html

#define ICER                        *((volatile uint32_t*) 0xE000E180)

#define interrupt_disable(IRQn)     ICER = (1 << IRQn)

// Interrupt Priority Registers
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/Cihgjeed.html

//void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
//uint32_t NVIC_GetPriority(IRQn_Type IRQn);

// globally disable interrupts
#define DINT        asm("cpsid i")

// re-enable interrupts
#define EINT        asm("cpsie i")

#endif
