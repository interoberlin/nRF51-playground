#ifndef GPIO_H
#define GPIO_H

#define GPIO_BASE       0x50000000

#define GPIO_OUT        (*(volatile uint32_t*) (GPIO_BASE+0x504))  // Write GPIO port
#define GPIO_OUTSET     (*(volatile uint32_t*) (GPIO_BASE+0x508))  // Set individual bits in GPIO port
#define GPIO_OUTCLR     (*(volatile uint32_t*) (GPIO_BASE+0x50C))  // Clear individual bits in GPIO port
#define GPIO_IN         (*(volatile uint32_t*) (GPIO_BASE+0x510))  // Read GPIO port
#define GPIO_DIR        (*(volatile uint32_t*) (GPIO_BASE+0x514))  // Direction of GPIO pins
#define GPIO_DIRSET     (*(volatile uint32_t*) (GPIO_BASE+0x518))  // DIR set register
#define GPIO_DIRCLR     (*(volatile uint32_t*) (GPIO_BASE+0x51C))  // DIR clear register
#define GPIO_PIN_CNF  ((volatile uint32_t[32]) {GPIO_BASE+0x700})  // Configuration of GPIO pins

/*
 * Pin configuration masks
 */
// Direction
#define GPIO_INPUT      0
#define GPIO_OUTPUT     1

// Input buffer
#define GPIO_INPUTBUFFER_CONNECT        0
#define GPIO_INPUTBUFFER_DISCONNECT    (1 << 1)

// Pull on pin level
#define GPIO_NOPULL     0
#define GPIO_PULLDOWN   1
#define GPIO_PULLUP     3

// Drive
#define GPIO_DRIVE_S0S1     0
#define GPIO_DRIVE_H0S1     1
#define GPIO_DRIVE_S0H1     2
#define GPIO_DRIVE_H0H1     3
#define GPIO_DRIVE_D0S1     4
#define GPIO_DRIVE_D0H1     5

// Pin sensing mechanism
#define GPIO_SENSE_DISABLE  0
#define GPIO_SENSE_HIGH     2
#define GPIO_SENSE_LOW      3

/*
 * Macros
 */
#define gpio_config_output(PIN) GPIO_DIRSET = (1 << (PIN))

#endif
