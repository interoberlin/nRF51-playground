
OPENOCD = /home/code/openocd/src/openocd

OPENOCD_CFG = $(OPENOCD_CFG_DIR)openocd-incircuit.cfg
#OPENOCD_CFG = $(OPENOCD_CFG_DIR)openocd-100ask.cfg

erase:
	$(OPENOCD) -c "set WORKAREASIZE 0;" -f $(OPENOCD_CFG) -c "init; reset halt; nrf51 mass_erase; shutdown;"

flash: $(BIN)
	$(OPENOCD) -c "set WORKAREASIZE 0;" -f $(OPENOCD_CFG) -c "init; reset halt; program $(BIN) $(STARTADDRESS) verify; shutdown;"

pinreset:
	# mww: write word to memory
	# das funktioniert so nicht, falsche Adresse:
	#$(OPENOCD) -f $(OPENOCD_CFG) -c "init; reset halt; sleep 1; mww phys 0x4001e504 2; mww 0x40000544 1; reset; shutdown;"

debug:
	$(OPENOCD) -c "set WORKAREASIZE 0;" -f $(OPENOCD_CFG) -c "init; arm semihosting enable;"
	
gdb:
	echo "target remote localhost:3333    \n\
          monitor reset halt              \n\
          file $(ELF)                     \n\
          load                            \n\
          b _start                        \n\
          monitor reset                   \n\
          continue                        \n\
          set interactive-mode on" | $(GDB)
