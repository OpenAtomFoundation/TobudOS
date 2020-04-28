###############################################################################
# Makefile for the BARE project
###############################################################################

## General Flags
PROJECT = demo
TARGET = demo

CC=gcc
OBJCOPY=/cygdrive/c/WinAVR/bin/avr-objcopy

## Options common to compile, link and assembly rules
COMMON =

## Compile options common for all C compilation units.
CFLAGS = $(COMMON) \
		 -Iport -I. \
		 -I../../modbus/rtu -I../../modbus/ascii -I../../modbus/include 
CFLAGS += -Wall -gstabs -Os -Wall -Wstrict-prototypes

CFLAGS += -Wp,-M,-MP,-MT,$(*F).o,-MF,dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gstabs

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref

## Objects that must be built in order to link
OBJECTS = demo.o
MBPORTOBJECTS = port/portserial.o \
			port/portevent.o \
			port/porttimer.o
MBOBJECTS = ../../modbus/mb.o \
			../../modbus/rtu/mbrtu.o \
			../../modbus/rtu/mbcrc.o \
			../../modbus/ascii/mbascii.o \
			../../modbus/functions/mbfunccoils.o \
			../../modbus/functions/mbfuncdiag.o \
			../../modbus/functions/mbfuncholding.o \
			../../modbus/functions/mbfuncinput.o \
			../../modbus/functions/mbfuncother.o \
			../../modbus/functions/mbfuncdisc.o \
			../../modbus/functions/mbutils.o 


## Build
all: $(TARGET).elf 

## Compile
demo.o: demo.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET).elf: $(OBJECTS) $(MBOBJECTS) $(MBPORTOBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(MBPORTOBJECTS) $(MBOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET).elf

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(MBOBJECTS) $(MBPORTOBJECTS) $(TARGET).elf $(TARGET).map

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

