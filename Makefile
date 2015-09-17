
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

#
# Project setup
#
SRCS = gcc_startup_nrf51.s system_nrf51.c nrf_delay.c main.c
OBJS = gcc_startup_nrf51.o system_nrf51.o nrf_delay.o main.o
OUTPUT_NAME = main

#
# Compiler and Linker setup
#
LINKER_SCRIPT = gcc_nrf51_blank.ld
CFLAGS   += -std=gnu99 -Wall -mcpu=cortex-m0 -mthumb -mfloat-abi=soft
LDFLAGS  += -L /usr/lib/gcc/arm-none-eabi/4.8/armv6-m/ -L /usr/lib/arm-none-eabi/newlib/armv6-m/ -T $(LINKER_SCRIPT) -Map $(OUTPUT_NAME).map

HEX = $(OUTPUT_NAME).hex
ELF = $(OUTPUT_NAME).elf
BIN = $(OUTPUT_NAME).bin

#
# Makefile build targets
#
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

erase:
	$(OPENOCD) -c "init ; reset halt ; nrf51 mass_erase ; shutdown"

START_ADDRESS = $($(OBJDUMP) -h $(ELF) -j .text | grep .text | awk '{print $$4}')

flash: $(BIN)
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

clean:
	rm -f main *.o *.out *.bin *.elf *.hex *.map
