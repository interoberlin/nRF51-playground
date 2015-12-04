
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
OPENOCD = /home/code/openocd/src/openocd
OPENOCD_CFG = openocd.cfg

#
# Compiler and Linker
#
CFLAGS  = -std=gnu99 -Wall -g -mcpu=cortex-m0 -mthumb -mabi=aapcs -mfloat-abi=soft
# keep every function in separate section. This will allow linker to dump unused functions
#CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
#CFLAGS += -fno-builtin --short-enums
#CFLAGS += -fexceptions

LINKER_SCRIPT = nrf51.ld
LDFLAGS  = -nostartfiles -nostdlib -static --gc-sections
LDFLAGS += -L /usr/lib/gcc/arm-none-eabi/4.8/armv6-m/
LDFLAGS += -L /usr/lib/arm-none-eabi/newlib/armv6-m/
LDFLAGS += --start-group -lc -lg -lm -lgcc
LDFLAGS += -T $(LINKER_SCRIPT)

#
# Build targets
#

all: demo_leds.elf demo_uart.elf demo_radio.elf

demo_leds.elf: nrf51_startup.o system_nrf51.o delay.o demo_leds.o
	$(LD) $(LDFLAGS) $^ -o $@

demo_uart.elf: nrf51_startup.o system_nrf51.o strings.o fifo.o uart.o delay.o demo_uart.o 
	$(LD) $(LDFLAGS) $^ -o $@

demo_radio.elf: nrf51_startup.o system_nrf51.o strings.o fifo.o uart.o delay.o timer.o radio.o demo_radio.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c %s
	$(CC) $(CFLAGS) -c $< -o $@

%.hex: %.elf
	$(OBJCOPY) -Oihex $< $@

%.bin: %.elf
	$(OBJCOPY) -Obinary $< $@

clean:
	rm -f *.o *.out *.bin *.elf *.hex *.map main demo_leds demo_uart demo_radio


erase:
	$(OPENOCD) -c "set WORKAREASIZE 0;" -f $(OPENOCD_CFG) -c "init; reset halt; nrf51 mass_erase; shutdown;"

flash: $(BIN)
	$(OPENOCD) -c "set WORKAREASIZE 0;" -f $(OPENOCD_CFG) -c "init; reset halt; program $(BIN) $(STARTADDRESS) verify; shutdown;"

pinreset:
	# mww: write word to memory
	# das funktioniert so nicht, falsche Adresse:
	#$(OPENOCD) -f $(OPENOCD_CFG) -c "init; reset halt; sleep 1; mww phys 0x4001e504 2; mww 0x40000544 1; reset; shutdown;"

debug:
	$(OPENOCD) -c "set WORKAREASIZE 0;" -f $(OPENOCD_CFG)
	
gdb:
	echo "target remote localhost:3333    \n\
          monitor reset halt              \n\
          file $(ELF)                     \n\
          load                            \n\
          b _start                        \n\
          monitor reset                   \n\
          continue                        \n\
          set interactive-mode on" | $(GDB)
