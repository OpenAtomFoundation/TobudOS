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

# Makefile

PROJ_ROOT = ../../../..

### ===== Toolchain =====

CROSS_COMPILE ?=
CC  = ccache $(CROSS_COMPILE)gcc
CPP = ccache $(CROSS_COMPILE)g++
LD  = $(CROSS_COMPILE)gcc
AR  = $(CROSS_COMPILE)ar

### ===== Compiler Flags =====

INCLUDES = \
    -I.    \
    -I$(PROJ_ROOT)/nimble/include             \
    -I$(PROJ_ROOT)/porting/npl/linux/include  \
    -I$(PROJ_ROOT)/porting/npl/linux/src      \
    -I$(PROJ_ROOT)/porting/nimble/include     \
    $(NULL)

DEFINES =

CFLAGS =                    \
    $(INCLUDES) $(DEFINES)  \
    -g                      \
    -D_GNU_SOURCE           \
    $(NULL)

LIBS = -lrt -lpthread -lstdc++

LDFLAGS =

### ===== Sources =====

OSAL_PATH = $(PROJ_ROOT)/porting/npl/linux/src

SRCS  = $(shell find $(OSAL_PATH) -maxdepth 1 -name '*.c')
SRCS += $(shell find $(OSAL_PATH) -maxdepth 1 -name '*.cc')
SRCS += $(PROJ_ROOT)/porting/nimble/src/os_mempool.c

OBJS  = $(patsubst %.c, %.o,$(filter %.c,  $(SRCS)))
OBJS += $(patsubst %.cc,%.o,$(filter %.cc, $(SRCS)))

TEST_SRCS  = $(shell find . -maxdepth 1 -name '*.c')
TEST_SRCS += $(shell find . -maxdepth 1 -name '*.cc')

TEST_OBJS  = $(patsubst %.c, %.o,$(filter %.c,  $(SRCS)))
TEST_OBJS += $(patsubst %.cc,%.o,$(filter %.cc, $(SRCS)))

### ===== Rules =====

all: depend                   \
     test_npl_task.exe        \
     test_npl_callout.exe     \
     test_npl_eventq.exe      \
     test_npl_sem.exe         \
     $(NULL)

test_npl_task.exe: test_npl_task.o $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)

test_npl_eventq.exe: test_npl_eventq.o $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)

test_npl_callout.exe: test_npl_callout.o $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)

test_npl_sem.exe: test_npl_sem.o $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)

test: all
	./test_npl_task.exe
	./test_npl_callout.exe
	./test_npl_eventq.exe
	./test_npl_sem.exe

show_objs:
	@echo $(OBJS)

### ===== Clean =====
clean:
	@echo "Cleaning artifacts."
	rm *~ .depend $(OBJS) *.o *.exe

### ===== Dependencies =====
### Rebuild if headers change
depend: .depend

.depend: $(SRCS) $(TEST_SRCS)
	@echo "Building dependencies."
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend;

include .depend

### Generic rules based on extension
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cc
	$(CPP) -c $(CFLAGS) $< -o $@
