
/*
 * GCC linker script for the nRF51
 *
 * Author: Matthias Bock <mail@matthiasbock.net>
 * License: GNU GPLv3
 */

/* Code execution starts in the Reset_Handler() */ 
ENTRY(Reset_Handler)

/*
 * Configure memory regions.
 *
 * According to section 3.2 of the nRF51822 datasheet,
 * the QFAC variant of the chip has
 *     256kB Flash memory in 256 pages of 1024 bytes
 *  and 32kB RAM in 4 blocks of 8kB.
 */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256k
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 32k
}

_heap_size = 512;

/*
  FICR  (r)   : ORIGIN = 0x10000000
  UICR  (r)   : ORIGIN = 0x10001000
  APB   (rw)  : ORIGIN = 0x40000000
  AHB   (rw)  : ORIGIN = 0x50000000
  PPB   (rw)  : ORIGIN = 0xE0000000
*/

SECTIONS
{
    .text :
    {
        code_begin = .;
        sdk/nrf51_startup.o(.vectors)
        *(.text*)
        code_end = .;
    } > FLASH

    /*
     * Reserve RAM space for initialized variables and constants
     * https://en.wikipedia.org/wiki/Data_segment 
     */
    .data :
    {
        data_begin = .;
        *(.data*)
        data_end = .;
    } > RAM

    /*
     * "Better save space" for uninitialized variables and constants
     * https://en.wikipedia.org/wiki/.bss
     */
    .bss :
    {
        bss_begin = .;
        *(.bss*)
        bss_end = .;
    } > RAM

    .heap :
    {
        heap_begin = .;
        *(.heap*)
        . = . + _heap_size;
        heap_end = .;
    } > RAM

    stack_begin = .;
    stack_end = ORIGIN(RAM) + LENGTH(RAM);
    
    end = .;
}
