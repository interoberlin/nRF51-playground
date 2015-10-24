
#include "delay.h" 

void delay_us(uint32_t us)
{
    // CPU operating at 16 MHz
	asm volatile (
		"start: nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"sub %[cycles], #1\n\t"
		"bne start\n\t"
		: /* empty */ : [cycles] "l" (us) 
	);
}
