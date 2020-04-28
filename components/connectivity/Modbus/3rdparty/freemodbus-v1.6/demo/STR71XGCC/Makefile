# 
# FreeModbus STR71X GCC Makefile
#
# Copyright (C) 2006 Christian Walter <wolti@sil.at>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# $Id: Makefile,v 1.1 2006/11/02 23:14:43 wolti Exp $
#

# MCU name and submodel
MCU      = arm7tdmi
SUBMDL   = STR711
#THUMB    = -mthumb
#THUMB_IW = -mthumb-interwork

## Create ROM-Image (final)
#RUN_MODE=ROM_RUN
## Create RAM-Image (debugging)
RUN_MODE=RAM_RUN

# --------------------------------------------------------------------------- #
# TARGET    ... name of the target executable to generate
# SRC       ... C source files ( either THUMB or ARM )
# SRCARM    ... C source files which are always compiled in ARM mode
# CPPSRC    ... C++ source files ( either THUMB or ARM )
# CPPSRCARM ... C++ source files which are always compiled in ARM mode
# --------------------------------------------------------------------------- #

TARGET    = demo
INC       = -I. -Iport \
			-Ifreertos/include \
			-Ilibstr71x/include \
			-I../../modbus/include \
			-I../../modbus/rtu \
			-I../../modbus/ascii 
SRC       = demo.c \
			$(addprefix freertos/, list.c queue.c tasks.c ) \
			$(addprefix freertos/portable/MemMang/, heap_2.c ) \
			$(addprefix freertos/portable/GCC/ARM7_STR71X/, port.c ) \
			$(addprefix libstr71x/, wdg.c rccu.c eic.c tim.c uart.c gpio.c ) \
			$(addprefix ../../modbus/, mb.c ) \
			$(addprefix ../../modbus/rtu/, mbrtu.c mbcrc.c ) \
			$(addprefix ../../modbus/ascii/, mbascii.c ) \
			$(addprefix ../../modbus/functions/, mbfunccoils.c mbfuncdiag.c mbfuncholding.c mbfuncinput.c mbfuncother.c mbfuncdisc.c mbutils.c ) 
SRCARM    = $(addprefix freertos/portable/GCC/ARM7_STR71X/, portISR.c ) \
			$(addprefix port/, portevent.c portserial.c porttimer.c ) 
CPPSRC    = 
CPPSRCARM = 

AINC      = -I.
ASRC      = 
ASRCARM   = $(addprefix system/, startup.S vector.S )

# --------------------------------------------------------------------------- #
# This section includes compiler and linker settings.
# --------------------------------------------------------------------------- #
# Flags for C and C++ (arm-elf-gcc/arm-elf-g++)
CPPFLAGS  = -gdwarf-2 -O0 -fno-strict-aliasing
CPPFLAGS  += -Wall -Wcast-align -Wimplicit -Wpointer-arith -Wswitch
CPPFLAGS  += -Wredundant-decls -Wreturn-type -Wshadow -Wunused
CPPFLAGS  += -D$(RUN_MODE) -DGCC_ARM7_STR71X=1
# Flags for C
CFLAGS    = $(CPPFLAGS) $(INC)
# Flags for C++
CCXXFLAGS = $(CFLAGS)

# Assembler flags.
#ASFLAGS = --defsym $(RUN_MODE)=1 --defsym REMAP=2 --gdwarf-2 
ASFLAGS = -gdwarf-2 -D$(RUN_MODE)=1 -DREMAP=0 $(AINC)

# Linker flags.
LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--cref -lc -lgcc
ifeq ($(RUN_MODE),RAM_RUN)
LDFLAGS +=-Tsupport/ram71x.ld
else 
LDFLAGS +=-Tsupport/rom71x.ld
endif

# --------------------------------------------------------------------------- #
# Toolchain setup
# --------------------------------------------------------------------------- #
PREFIX  = /opt/gcc-arm/bin
AS      = $(PREFIX)/arm-elf-as
CC      = $(PREFIX)/arm-elf-gcc
CPP     = $(PREFIX)/arm-elf-g++
OBJCOPY = $(PREFIX)/arm-elf-objcopy
OBJDUMP = $(PREFIX)/arm-elf-objdump
SIZE    = $(PREFIX)/arm-elf-size
NM      = $(PREFIX)/arm-elf-nm
INSIGHT = $(PREFIX)/arm-elf-insight

# --------------------------------------------------------------------------- #
# Define all output files and dependencies based upon the specified
# source files.
# --------------------------------------------------------------------------- #
COBJ        = $(SRC:.c=.o) 
AOBJ        = $(ASRC:.S=.o)
COBJARM     = $(SRCARM:.c=.o)
AOBJARM     = $(ASRCARM:.S=.o)
CPPOBJ      = $(CPPSRC:.cpp=.o) 
CPPOBJARM   = $(CPPSRCARM:.cpp=.o)

# Flags need to generate dependencies
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
ALL_CFLAGS  = -mcpu=$(MCU) $(THUMB_IW) $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mcpu=$(MCU) $(THUMB_IW) $(ASFLAGS)

# --------------------------------------------------------------------------- #
# Make targets
# --------------------------------------------------------------------------- #

all: $(TARGET).elf $(TARGET).sym $(TARGET).hex $(TARGET).bin

# Debug target which invokes the debugger
debug: $(TARGET).hex
	$(INSIGHT) --se=$(TARGET).elf --command=support/target-71.gdb

# Flasg target which downloads the code.
flash: $(TARGET).hex $(TARGET).bin

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@
	
%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.sym: %.elf
	$(NM) -n $< > $@

$(TARGET).elf:  $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM) 
	$(CC) $(THUMB) $(THUMB_IW) $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM) --output $@ $(LDFLAGS)

# Compile: create object files from C source files. ARM/Thumb
$(COBJ) : %.o : %.c
	$(CC) -c $(THUMB) $(ALL_CFLAGS) $(CONLYFLAGS) $< -o $@ 

# Compile: create object files from C source files. ARM-only
$(COBJARM) : %.o : %.c
	$(CC) -c $(ALL_CFLAGS) $(CONLYFLAGS) $< -o $@ 

# Compile: create object files from C++ source files. ARM/Thumb
$(CPPOBJ) : %.o : %.cpp
	$(CPP) -c $(THUMB) $(ALL_CFLAGS) $(CPPFLAGS) $< -o $@ 

# Compile: create object files from C++ source files. ARM-only
$(CPPOBJARM) : %.o : %.cpp
	$(CPP) -c $(ALL_CFLAGS) $(CPPFLAGS) $< -o $@ 

# Assemble: create object files from assembler source files. ARM/Thumb
$(AOBJ) : %.o : %.S
	$(CC) -c $(THUMB) $(ALL_ASFLAGS) $< -o $@
#	$(AS) $(THUMB) $(ALL_ASFLAGS) -o $@ $<

# Assemble: create object files from assembler source files. ARM-only
$(AOBJARM) : %.o : %.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@
#	$(AS) $(ALL_ASFLAGS) -o $@ $< 

# Target: clean project.
clean: 
	rm -f $(TARGET).hex $(TARGET).bin $(TARGET).obj $(TARGET).elf $(TARGET).map
	rm -f $(TARGET).obj $(TARGET).sym $(TARGET).lss $(TARGET).lst
	rm -f $(COBJ) $(CPPOBJ) $(AOBJ) $(COBJARM) $(CPPOBJARM) $(AOBJARM)
	rm -f $(SRC:.c=.d) $(SRCARM:.c=.d) $(CPPSRC:.cpp=.d) $(CPPSRCARM:.cpp=.d)
	rm -f .dep/*

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# Listing of phony targets.
.PHONY : all clean

