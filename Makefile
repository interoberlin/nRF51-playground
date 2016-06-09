
#
# Makefile for nRF51 projects
#
# Author: Matthias Bock <mail@matthiasbock.net>
# License: GNU GPLv3
#

 
#
# Toolchain
#
TOOLCHAIN_PATH   = /usr/bin/
TOOLCHAIN_PREFIX = arm-none-eabi
AS      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-as
CC      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc
LD      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-ld
OBJCOPY = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy
OBJDUMP = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objdump
SIZE    = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-size
GDB     = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gdb


#
# Compiler and Linker
#
CFLAGS += -std=gnu99 -Wall -Wextra -g
CFLAGS += -mcpu=cortex-m0 -mthumb -mabi=aapcs -mfloat-abi=soft
CFLAGS += -ffreestanding
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums
CFLAGS += -I arm/
CFLAGS += -I nordic/
CFLAGS += -I sdk/

// TODO: auto-detect chip revision
CHIP_REVISION = aa

LINKER_SCRIPT = linker/nrf51-blank-xx$(CHIP_REVISION).ld
LDFLAGS += -T $(LINKER_SCRIPT)
LDFLAGS += -L /usr/lib/gcc/arm-none-eabi/4.8/armv6-m/
LDFLAGS += -L /usr/lib/arm-none-eabi/newlib/armv6-m/
LDFLAGS += -static
LDFLAGS += -nostartfiles -nostdlib
LDFLAGS += --start-group
LDFLAGS += -lgcc


#
# Build targets
#

all: demo_uart.elf demo_spi.elf demo_ad53x4.elf demo_leds.elf demo_rgbstrip.elf demo_timers.elf orchid_lamp.elf demo_radio.elf

demo_uart.elf: sdk/nrf51_startup.o nordic/system_nrf51.o sdk/strings.o sdk/fifo.o sdk/uart.o sdk/delay.o demo_uart.o 
	$(LD) $(LDFLAGS) $^ -o $@

demo_spi.elf: sdk/nrf51_startup.o nordic/system_nrf51.o sdk/strings.o sdk/fifo.o sdk/delay.o demo_spi.o 
	$(LD) $(LDFLAGS) $^ -o $@

demo_ad53x4.elf: sdk/nrf51_startup.o nordic/system_nrf51.o sdk/strings.o sdk/fifo.o sdk/uart.o sdk/delay.o libad53x4/ad53x4.o libad53x4/demo_ad53x4.o
	$(LD) $(LDFLAGS) $^ -o $@	

demo_leds.elf: sdk/nrf51_startup.o nordic/system_nrf51.o sdk/delay.o demo_leds.o
	$(LD) $(LDFLAGS) $^ -o $@

demo_rgbstrip.elf: sdk/nrf51_startup.o nordic/system_nrf51.o sdk/delay.o demo_rgbstrip.o
	$(LD) $(LDFLAGS) $^ -o $@

demo_timers.elf: /usr/lib/arm-none-eabi/newlib/libc.a sdk/nrf51_startup.o nordic/system_nrf51.o sdk/timers.o demo_timers.o
	$(LD) $(LDFLAGS) $^ -o $@

orchid_lamp.elf: sdk/nrf51_startup.o nordic/system_nrf51.o sdk/delay.o orchid_lamp.o
	$(LD) $(LDFLAGS) $^ -o $@

demo_radio.elf: /usr/lib/arm-none-eabi/newlib/libc.a sdk/nrf51_startup.o nordic/system_nrf51.o sdk/strings.o sdk/fifo.o sdk/uart.o sdk/delay.o sdk/timers.o sdk/radio.o demo_radio.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c %s
	$(CC) $(CFLAGS) -c $< -o $@

%.hex: %.elf
	$(OBJCOPY) -Oihex $< $@

%.bin: %.elf
	$(OBJCOPY) -Obinary $< $@

clean:
	rm -f *.o */*.o *.out *.bin *.elf *.hex *.map


#
# Debugger
#
OPENOCD_CFG_DIR = debug/
# workaround: problems because the folder is named like the target in the subfolder's Makefile
.PHONY: debug
include debug/Makefile

