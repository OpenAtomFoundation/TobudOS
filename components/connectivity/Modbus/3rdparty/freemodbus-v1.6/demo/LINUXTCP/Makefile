#
# 	    Makefile for tcp freemodbus
#
#	 	Copyright (c) 2007 Steven Guo <gotop167@163.com>
#
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# platform dependencies
# ---------------------------------------------------------------------------
ifneq ($(strip $(shell gcc -v 2>&1 |grep "cygwin")),)
	CYGWIN_BUILD = YES
endif
CC	        = gcc
CXX	        = g++
OBJCOPY	    = objcopy
INSIGHT     = /opt/insight-x86/bin/insight

# ---------------------------------------------------------------------------
# project specifics
# ---------------------------------------------------------------------------
CFLAGS	    =  -g -Wall -Iport -I../../modbus/rtu \
		-I../../modbus/ascii -I../../modbus/include -I../../modbus/tcp
LDFLAGS     =
ifeq ($(CYGWIN_BUILD),YES)
else
LDFLAGS     += -lpthread
CFLAGS      += -pthread
endif

TGT         = tcpmodbus
OTHER_CSRC  = 
OTHER_ASRC  = 
CSRC        = demo.c port/portother.c \
              port/portevent.c port/porttcp.c \
              ../../modbus/mb.c ../../modbus/tcp/mbtcp.c \
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
BIN         = $(TGT)

.PHONY: clean all

all: $(BIN)

debug:
	$(INSIGHT) --se=$(TGT)

$(BIN): $(OBJS) $(NOLINK_OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

clean:
	rm -f $(DEPS)
	rm -f $(OBJS) $(NOLINK_OBJS)
	rm -f $(BIN)

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

