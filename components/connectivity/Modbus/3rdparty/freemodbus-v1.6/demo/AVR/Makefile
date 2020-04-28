###############################################################################
# Makefile for the project AVR
###############################################################################

## General Flags
PROJECT = demo
MCU = atmega168
TARGET = demo

CC=/opt/gcc-avr/bin/avr-gcc
OBJCOPY=/opt/gcc-avr/bin/avr-objcopy
AVRDUDE=/opt/gcc-avr/bin/avrdude

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON) \
		 -Iport -I. \
		 -I../../modbus/rtu -I../../modbus/ascii -I../../modbus/include 
CFLAGS += -Wall -gstabs -DF_CPU=20000000UL  -Os -Wall -Wstrict-prototypes

CFLAGS += -Wp,-M,-MP,-MT,$(*F).o,-MF,dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gstabs

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0


## Objects that must be built in order to link
#OBJECTS = excoils.o
OBJECTS = demo.o
MBPORTOBJECTS = port/portserial.o \
			port/portevent.o \
			port/porttimer.o \
			port/mbcrc.o
MBOBJECTS = ../../modbus/mb.o \
			../../modbus/rtu/mbrtu.o \
			../../modbus/ascii/mbascii.o \
			../../modbus/functions/mbfunccoils.o \
			../../modbus/functions/mbfuncdiag.o \
			../../modbus/functions/mbfuncholding.o \
			../../modbus/functions/mbfuncinput.o \
			../../modbus/functions/mbfuncother.o \
			../../modbus/functions/mbfuncdisc.o \
			../../modbus/functions/mbutils.o 


## Build
all: $(TARGET).elf $(TARGET).cof $(TARGET).hex $(TARGET).eep 

## Compile
demo.o: demo.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET).elf: $(OBJECTS) $(MBOBJECTS) $(MBPORTOBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(MBPORTOBJECTS) $(MBOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET).elf

%.hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.cof: $(TARGET).elf
	$(OBJCOPY) --debugging \
		--change-section-address .data-0x800000 \
		--change-section-address .bss-0x800000 \
		--change-section-address .noinit-0x800000 \
		--change-section-address .eeprom-0x810000 -O coff-avr $< $@

%.eep: $(TARGET).elf
	$(OBJCOPY) $(HEX_EEPROM_FLAGS) -O ihex $< $@

%.lss: $(TARGET)
	$(OBJCOPY) -h -S $< > $@

flash:
	$(AVRDUDE) -p m168 -c stk200 -U flash:w:$(TARGET).hex				

#size: ${TARGET}
#	@echo
#	@sh avr-mem.sh ${TARGET} ${MCU}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(MBOBJECTS) $(MBPORTOBJECTS) 
	-rm -rf $(TARGET).elf $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).map
	-rm -rf dep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

