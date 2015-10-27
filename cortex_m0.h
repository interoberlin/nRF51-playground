/*
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

typedef uint32_t IRQn_Type;

// Interrupt Control State Register (ICSR)
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/Cihfaaha.html

#define ICSR            *((volatile uint32_t*) 0xE000ED04)
#define VECTACTIVE      ICSR & 0x0000003f
#define VECTPENDING     ICSR & 0x0003f000

// Interrupt Set Enable Register (ISER)
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/Cihcajhj.html

#define ISER            *((volatile uint32_t*) 0xE000E100)

inline void NVIC_EnableIRQ(IRQn_Type IRQn)
{
    ISER = 1 >> (32-IRQn);
}

// Interrupt Clear Enable Register (ICER)
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/Cihbiadi.html

#define ICER            *((volatile uint32_t*) 0xE000E180)

inline void NVIC_DisableIRQ(IRQn_Type IRQn)
{
    ICER = 1 >> (32-IRQn);
}

// Interrupt Priority Registers
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/Cihgjeed.html

//void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
//uint32_t NVIC_GetPriority(IRQn_Type IRQn);

#endif
