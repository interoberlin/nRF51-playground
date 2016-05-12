
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
CFLAGS += -std=gnu99 -Wall -g -mcpu=cortex-m0 -mthumb -mabi=aapcs -mfloat-abi=soft
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums

// TODO: auto-detect chip revision
CHIP_REVISION = ac

LINKER_SCRIPT = linker/nrf51-blank-xx$(CHIP_REVISION).ld
LDFLAGS += -T $(LINKER_SCRIPT)
LDFLAGS += -nostartfiles -nostdlib -lgcc -static
LDFLAGS += -L /usr/lib/gcc/arm-none-eabi/4.8/armv6-m/
LDFLAGS += -L /usr/lib/arm-none-eabi/newlib/armv6-m/

#
# Build targets
#

all: demo_uart.elf demo_leds.elf demo_rgbstrip.elf orchid_lamp.elf demo_radio.elf

demo_leds.elf: nrf51_startup.o system_nrf51.o delay.o demo_leds.o
	$(LD) $(LDFLAGS) $^ -o $@

demo_rgbstrip.elf: nrf51_startup.o system_nrf51.o delay.o demo_rgbstrip.o
	$(LD) $(LDFLAGS) $^ -o $@

orchid_lamp.elf: nrf51_startup.o system_nrf51.o delay.o orchid_lamp.o
	$(LD) $(LDFLAGS) $^ -o $@

demo_uart.elf: nrf51_startup.o system_nrf51.o strings.o heap.o fifo.o uart.o delay.o demo_uart.o 
	$(LD) $(LDFLAGS) $^ -o $@

demo_radio.elf: nrf51_startup.o system_nrf51.o strings.o heap.o fifo.o uart.o delay.o radio.o demo_radio.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c %s
	$(CC) $(CFLAGS) -c $< -o $@

%.hex: %.elf
	$(OBJCOPY) -Oihex $< $@

%.bin: %.elf
	$(OBJCOPY) -Obinary $< $@

clean:
	rm -f *.o *.out *.bin *.elf *.hex *.map

#
# Debugging targets
#
include Makefile.openocd
