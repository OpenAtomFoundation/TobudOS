#
# FreeModbus MSP430 port Makefile
#
# Copyright (c) 2006 Christian Walter, Vienna 2006.
#
# $Id: Makefile,v 1.3 2007/06/12 06:42:01 wolti Exp $
#
# ---------------------------------------------------------------------------

BASE        = /cygdrive/c/mspgcc
CC	        = $(BASE)/bin/msp430-gcc
LD          = $(BASE)/bin/msp430-ld
CXX	        = $(BASE)/bin/msp430-g++
OBJCOPY	    = $(BASE)/bin/msp430-objcopy
SIZE	    = $(BASE)/bin/msp430-size
INSIGHT     = $(BASE)/bin/msp430-insight
GDBPROXY    = $(BASE)/bin/msp430-gdbproxy

MCU         = msp430x149
MB_INC      = -Iport -I../../modbus/rtu  -I../../modbus/ascii \
              -I../../modbus/include
CFLAGS	    = -MD -g -mmcu=$(MCU) -Wall -D'FSYS_2=25000000UL'
CFLAGS      += -I. -Isystem $(MB_INC)
ASFLAGS     = -MD -g -mmcu=$(MCU)
LDFLAGS     = -g -mmcu=$(MCU)
TGT         = demo

MB_SRC      = $(addprefix ../../modbus/, mb.c )
MB_RTU_SRC  = $(addprefix ../../modbus/rtu/, mbrtu.c mbcrc.c )
MB_ASCII_SRC= $(addprefix ../../modbus/ascii/, mbascii.c )
MB_FUNC_SRC = $(addprefix ../../modbus/functions/, mbfunccoils.c \
              mbfuncdiag.c mbfuncholding.c mbfuncinput.c mbfuncother.c \
              mbfuncdisc.c mbutils.c) 
PORT_SRC    = $(addprefix port/, portserial.c porttimer.c portevent.c )
CSRC        = demo.c \
              $(MB_SRC) $(MB_RTU_SRC) $(MB_ASCII_SRC) $(MB_FUNC_SRC) \
              $(PORT_SRC)
ASRC        = system/dco-gcc.S

OBJS        = $(CSRC:.c=.o) $(ASRC:.S=.o)
DEPS        = $(OBJS:.o=.d)
BIN         = $(TGT).elf

.PHONY: clean all

all: $(BIN)

flash-programm: $(TGT).elf

flash-verify:

flash-erase:

proxy:
	$(GDBPROXY) --port=2000 msp430
debug:
	$(INSIGHT) --command=msp430.gdb --se=$(TGT).elf

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

clean:
	rm -f $(DEPS)
	rm -f $(OBJS)
	rm -f $(BIN) $(TGT).map

# ---------------------------------------------------------------------------
# rules for code generation
# ---------------------------------------------------------------------------
%.o:    %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o:    %.S
	$(CC) $(ASFLAGS) -o $@ -c $<

# ---------------------------------------------------------------------------
#  # compiler generated dependencies
# ---------------------------------------------------------------------------
-include $(DEPS)

