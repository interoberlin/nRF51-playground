
/**
 * Clock management library
 * for the Nordic Semiconductor nRF51 series
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 *
 * License: GNU GPLv3
 */

#ifndef CLOCK_H
#define CLOCK_H

#define CLOCK_BASE                   0x40000000

// Tasks
#define CLOCK_TASK_HFCLKSTART       (*(volatile uint32_t*) (CLOCK_BASE+0x000))    // Start HFCLK crystal oscillator
#define CLOCK_TASK_HFCLKSTOP        (*(volatile uint32_t*) (CLOCK_BASE+0x004))    // Stop HFCLK crystal oscillator
#define CLOCK_TASK_LFCLKSTART       (*(volatile uint32_t*) (CLOCK_BASE+0x008))    // Start LFCLK source
#define CLOCK_TASK_LFCLKSTOP        (*(volatile uint32_t*) (CLOCK_BASE+0x00C))    // Stop LFCLK source
#define CLOCK_TASK_CAL              (*(volatile uint32_t*) (CLOCK_BASE+0x010))    // Start calibration of LFCLK RC oscillator
#define CLOCK_TASK_CTSTART          (*(volatile uint32_t*) (CLOCK_BASE+0x014))    // Start calibration timer
#define CLOCK_TASK_CTSTOP           (*(volatile uint32_t*) (CLOCK_BASE+0x018))    // Stop calibration timer

// Events
#define CLOCK_EVENT_HFCLKSTARTED    (*(volatile uint32_t*) (CLOCK_BASE+0x100))    // HFCLK oscillator started
#define CLOCK_EVENT_LFCLKSTARTED    (*(volatile uint32_t*) (CLOCK_BASE+0x104))    // LFCLK started
#define CLOCK_EVENT_DONE            (*(volatile uint32_t*) (CLOCK_BASE+0x10C))    // Calibration of LFCLK RC oscillator complete event
#define CLOCK_EVENT_CTTO            (*(volatile uint32_t*) (CLOCK_BASE+0x110))    // Calibration timer timeout

// Registers
#define CLOCK_INTENSET              (*(volatile uint32_t*) (CLOCK_BASE+0x304))    // Enable interrupt
#define CLOCK_INTENCLR              (*(volatile uint32_t*) (CLOCK_BASE+0x308))    // Disable interrupt
#define CLOCK_HFCLKRUN              (*(volatile uint32_t*) (CLOCK_BASE+0x408))    // Status indicating that HFCLKSTART task has been triggered
#define CLOCK_HFCLKSTAT             (*(volatile uint32_t*) (CLOCK_BASE+0x40C))    // Which HFCLK source is running
#define CLOCK_LFCLKRUN              (*(volatile uint32_t*) (CLOCK_BASE+0x414))    // Status indicating that LFCLKSTART task has been triggered
#define CLOCK_LFCLKSTAT             (*(volatile uint32_t*) (CLOCK_BASE+0x418))    // Which LFCLK source is running
#define CLOCK_LFCLKSRCCOPY          (*(volatile uint32_t*) (CLOCK_BASE+0x41C))    // Copy of LFCLKSRC register, set when LFCLKSTART task was triggered
#define CLOCK_LFCLKSRC              (*(volatile uint32_t*) (CLOCK_BASE+0x518))    // Clock source for the LFCLK
#define CLOCK_CTIV                  (*(volatile uint32_t*) (CLOCK_BASE+0x538))    // Calibration timer interval
#define CLOCK_XTALFREQ              (*(volatile uint32_t*) (CLOCK_BASE+0x550))    // Crystal frequency

#endif
