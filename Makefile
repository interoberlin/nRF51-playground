
#
# Toolchain setup
#
TOOLCHAIN_PATH   = /usr/bin/
TOOLCHAIN_PREFIX = arm-none-eabi
AS      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-as
CC      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc
LD      = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc
OBJCOPY = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy
OBJDUMP = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objdump
SIZE    = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-size
GDB     = $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gdb
OPENOCD = /home/code/openocd/src/openocd

#
# Project setup
#
SRCS = system_nrf51.c nrf_delay.c main.c
OBJS = system_nrf51.o nrf_delay.o main.o
OUTPUT_NAME = main

#
# Compiler and Linker setup
#
LINKER_SCRIPT = gcc_nrf51_blank.ld
CPUFLAGS = -mcpu=cortex-m0 -mthumb -mfloat-abi=soft
CFLAGS   = -std=gnu99 -Wall
LDFLAGS  = -T $(LINKER_SCRIPT) -Wl,-Map=$(OUTPUT_NAME).map

HEX = $(OUTPUT_NAME).hex
ELF = $(OUTPUT_NAME).elf
BIN = $(OUTPUT_NAME).bin

#
# Makefile build targets
#
all: $(OBJS) $(HEX)

%.o: %.c
	$(CC) $(CPUFLAGS) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(ELF): $(OBJS)
	$(LD) $(CPUFLAGS) $(LDFLAGS) $(OBJS) -o $(ELF)
	$(SIZE) $(ELF)

$(HEX): $(ELF)
	$(OBJCOPY) -Oihex $(ELF) $(HEX)

$(BIN): $(ELF)
	$(OBJCOPY) -Obinary $(ELF) $(BIN)

erase:
	$(OPENOCD) -c "init ; reset halt ; nrf51 mass_erase ; shutdown"

flash: $(BIN)
	START_ADDRESS = $($(OBJDUMP) -h $(ELF) -j .text | grep .text | awk '{print $$4}')
	$(OPENOCD) -c "program $(BIN) $(STARTADDRESS) verify"

pinreset:
	$(OPENOCD) -c "init ; reset halt ; mww 0x4001e504 2 ; mww 0x40000544 1 ; reset ; shutdown"

debug:
	$(OPENOCD)
	echo -e -n "target remote localhost:3333    \n\
        monitor reset halt                      \n\
        file $(ELF)                             \n\
        load                                    \n\
        b main                                  \n\
        b app_error_handler                     \n\
        monitor reset                           \n\
        continue" > $(GDB)

