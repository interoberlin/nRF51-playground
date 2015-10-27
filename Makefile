
#
# Makefile for nRF51 projects
#
# Author: Matthias Bock <mail@matthiasbock.net>
# License: GNU GPLv3
#
 
#
# Toolchain setup
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
# Project setup
#
OUTPUT_NAME = demo_uart
SRCS = nrf51_startup.c system_nrf51.c heap.c delay.c uart.c $(OUTPUT_NAME).c
OBJS = nrf51_startup.o system_nrf51.o heap.o delay.o uart.o $(OUTPUT_NAME).o

#
# Compiler and Linker setup
#
CFLAGS += -std=gnu99 -Wall -g -mcpu=cortex-m0 -mthumb -mabi=aapcs -mfloat-abi=soft
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums

LINKER_SCRIPT = nrf51.ld
LDFLAGS += -L /usr/lib/gcc/arm-none-eabi/4.8/armv6-m/
LDFLAGS += -L /usr/lib/arm-none-eabi/newlib/armv6-m/
LDFLAGS += -T $(LINKER_SCRIPT)

#
# Makefile build targets
#
HEX = $(OUTPUT_NAME).hex
ELF = $(OUTPUT_NAME).elf
BIN = $(OUTPUT_NAME).bin

all: $(OBJS) $(HEX)

%.o: %.c %s
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(ELF)
	$(SIZE) $(ELF)

$(HEX): $(ELF)
	$(OBJCOPY) -Oihex $(ELF) $(HEX)

$(BIN): $(ELF)
	$(OBJCOPY) -Obinary $(ELF) $(BIN)

clean:
	rm -f *.o *.out *.bin *.elf *.hex *.map main demo_leds demo_uart

#START_ADDRESS = $($(OBJDUMP) -h $(ELF) -j .text | grep .text | awk '{print $$4}')

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
