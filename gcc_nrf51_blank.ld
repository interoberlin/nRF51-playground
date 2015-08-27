/*
 * Linker script to configure memory regions.
 *
 * According to section 3.2 of the nRF51822 datasheet,
 * the QFAC variant of the chip has
 *     256kB Flash memory in 256 pages of 1024 bytes
 *  and 32kB RAM in 4 blocks of 8kB.
 */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x40000 /* 256k */
  RAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32k */
}
INCLUDE "gcc_nrf51_common.ld"
