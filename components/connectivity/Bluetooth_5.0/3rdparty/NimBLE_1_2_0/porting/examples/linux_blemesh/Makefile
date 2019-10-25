#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#  *  http://www.apache.org/licenses/LICENSE-2.0
#  * Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#

# Toolchain commands
CROSS_COMPILE ?=
CC      := ccache $(CROSS_COMPLIE)gcc
CXX     := ccache $(CROSS_COMPILE)g++
LD      := $(CROSS_COMPILE)gcc
AR      := $(CROSS_COMPILE)ar
AS      := $(CROSS_COMPILE)as
NM      := $(CROSS_COMPILE)nm
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE    := $(CROSS_COMPILE)size

# Configure NimBLE variables
NIMBLE_ROOT := ../../..
NIMBLE_CFG_TINYCRYPT := 1

# Skip files that don't build for this port
NIMBLE_IGNORE := $(NIMBLE_ROOT)/porting/nimble/src/hal_timer.c \
	$(NIMBLE_ROOT)/porting/nimble/src/os_cputime.c \
	$(NIMBLE_ROOT)/porting/nimble/src/os_cputime_pwr2.c \
	$(NULL)

include $(NIMBLE_ROOT)/porting/nimble/Makefile.defs
include $(NIMBLE_ROOT)/porting/nimble/Makefile.mesh

SRC := $(NIMBLE_SRC)

# Source files for NPL OSAL
SRC += \
	$(wildcard $(NIMBLE_ROOT)/porting/npl/linux/src/*.c) \
	$(wildcard $(NIMBLE_ROOT)/porting/npl/linux/src/*.cc) \
	$(wildcard $(NIMBLE_ROOT)/nimble/transport/socket/src/*.c) \
	$(TINYCRYPT_SRC) \
	$(NULL)

# Source files for demo app
SRC += \
	./ble.c \
	./main.c \
	$(NULL)

# Add NPL and all NimBLE directories to include paths
INC = \
	./include \
	$(NIMBLE_ROOT)/porting/npl/linux/include \
	$(NIMBLE_ROOT)/porting/npl/linux/src \
	$(NIMBLE_ROOT)/nimble/transport/socket/include \
	$(NIMBLE_INCLUDE) \
	$(TINYCRYPT_INCLUDE) \
	$(NULL)

INCLUDES := $(addprefix -I, $(INC))

SRC_C  = $(filter %.c,  $(SRC))
SRC_CC = $(filter %.cc, $(SRC))

OBJ := $(SRC_C:.c=.o)
OBJ += $(SRC_CC:.cc=.o)

TINYCRYPT_OBJ := $(TINYCRYPT_SRC:.c=.o)

CFLAGS =                    \
    $(NIMBLE_CFLAGS)        \
    $(INCLUDES)             \
    -g                      \
    -D_GNU_SOURCE           \
    $(NULL)

LIBS := -lrt -lpthread -lstdc++

.PHONY: all clean
.DEFAULT: all

all: nimble-linux-blemesh

clean:
	rm $(OBJ) -f
	rm nimble-linux-blemesh -f

$(TINYCRYPT_OBJ): CFLAGS+=$(TINYCRYPT_CFLAGS)

%.o: %.c
	$(CC) -c $(INCLUDES) $(CFLAGS) -o $@ $<

%.o: %.cc
	$(CXX) -c $(INCLUDES) $(CFLAGS) -o $@ $<

nimble-linux-blemesh: $(OBJ) $(TINYCRYPT_OBJ)
	$(LD) -o $@ $^ $(LIBS)
	$(SIZE) $@
