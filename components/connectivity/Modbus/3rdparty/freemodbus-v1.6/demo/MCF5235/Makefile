#
# FreeModbus MCF5235 port- Makefile
#
# Copyright (c) 2006 Christian Walter, Vienna 2006.
#
# $Id: Makefile,v 1.2 2006/08/30 23:18:06 wolti Exp $
#
# ---------------------------------------------------------------------------

BASE        = /opt/gcc-m68k/bin
CC	        = $(BASE)/m68k-elf-gcc
CXX	        = $(BASE)/m68k-elf-g++
OBJCOPY	    = $(BASE)/m68k-elf-objcopy
INSIGHT     = $(BASE)/m68k-bdm-elf-insight

CFLAGS	    = -MD -gdwarf-2 -g3 -m528x -Wall \
			  -Imcf523x -Iport -I../../modbus/rtu \
			  -I../../modbus/ascii -I../../modbus/include \
			  -D'__IPSBAR=((vuint8 *) 0x40000000)' \
			  -D'FCPU=25000000UL'
ASFLAGS     = -MD -gdwarf-2 -g3 -m528x -Wa,--register-prefix-optional
LDSCRIPT    = m5235-ram.ld
LDFLAGS	    = -nostartfiles -m528x -Wl,--script=$(LDSCRIPT)

TGT         = demo
OTHER_CSRC  = 
OTHER_ASRC  = vector.S crt0.S
CSRC        = demo.c init.c port/portserial.c port/portother.c \
              port/portevent.c port/porttimer.c \
              ../../modbus/mb.c \
              ../../modbus/rtu/mbrtu.c ../../modbus/rtu/mbcrc.c \
			  ../../modbus/ascii/mbascii.c \
              ../../modbus/functions/mbfunccoils.c \
              ../../modbus/functions/mbfuncdiag.c \
              ../../modbus/functions/mbfuncholding.c \
              ../../modbus/functions/mbfuncinput.c \
              ../../modbus/functions/mbfuncother.c \
              ../../modbus/functions/mbfuncdisc.c \
              ../../modbus/functions/mbutils.c 
ASRC        = 
OBJS        = $(CSRC:.c=.o) $(ASRC:.S=.o)
NOLINK_OBJS = $(OTHER_CSRC:.c=.o) $(OTHER_ASRC:.S=.o)
DEPS        = $(OBJS:.o=.d) $(NOLINK_OBJS:.o=.d)
BIN         = $(TGT).elf

.PHONY: clean all

all: $(BIN)

debug:
	$(INSIGHT) --command=support/m5235.gdb --se=$(TGT).elf

$(BIN): $(OBJS) $(NOLINK_OBJS)
	$(CC) $(LDFLAGS) -Wl,-Map=$(TGT).map $(OBJS) $(LDLIBS) -o $@

clean:
	rm -f $(DEPS)
	rm -f $(OBJS) $(NOLINK_OBJS)
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
-include $(LWOS_DEPS) $(PORT_DEPS) $(APPL_DEPS)

