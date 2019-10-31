LIB_SDK_TARGET := libiot_sdk.a
LIB_HAL_TARGET := libiot_hal.a

###############################################################################
# Compiler

CC := gcc
AR := ar

###############################################################################
# Path definitions

TOPDIR  := .
BLDDIR  := build
SRCDIR  := eng
EXPDIR  := examples
HDRDIR  := $(shell find $(SRCDIR) -type d)

PROG_TARGET := $(shell ls $(EXPDIR) 2>/dev/null|grep example|sed 's:\.c::g;s:_:-:g')
PROG_TARGET := $(foreach V,$(PROG_TARGET),$(BLDDIR)/$(V))

###############################################################################
# Application source files

SOURCE_FILES_C := $(shell find $(SRCDIR) -name "*.c" -not -path "*wrappers*")
WRAPPER_IMPL_C := $(shell find $(SRCDIR) -name "*.c" -path "*wrappers*")

###############################################################################
# Standard application lib/header search paths

CFLAGS  := $(addprefix -I,$(HDRDIR))
LDFLAGS := -L$(BLDDIR) -liot_sdk -liot_hal -lpthread -lrt

###############################################################################
# Dependency rules

LIB_OBJS := $(SOURCE_FILES_C:.c=.o)
LIB_OBJS := $(subst $(SRCDIR),$(BLDDIR),$(LIB_OBJS))

HAL_OBJS := $(WRAPPER_IMPL_C:.c=.o)
HAL_OBJS := $(subst $(SRCDIR),$(BLDDIR),$(HAL_OBJS))

.PHONY: prepare all clean prog

all: prepare $(BLDDIR)/$(LIB_SDK_TARGET) $(BLDDIR)/$(LIB_HAL_TARGET)

prog: all $(PROG_TARGET)

prepare:
	-@mkdir -p $(BLDDIR)

$(BLDDIR)/$(LIB_SDK_TARGET): $(LIB_OBJS)
	@echo "o Archiving $@ ..."
	@$(AR) -rcs $@ $^

$(BLDDIR)/$(LIB_HAL_TARGET): $(HAL_OBJS)
	@echo "o Archiving $@ ..."
	@$(AR) -rcs $@ $^

$(PROG_TARGET): $(BLDDIR)/$(LIB_SDK_TARGET) $(BLDDIR)/$(LIB_HAL_TARGET)
	@echo "+ Linking $@ ..."
	@$(CC) -o $@ $(shell echo $@".c"|sed 's:$(BLDDIR):$(EXPDIR):g;s:-:_:g') $(CFLAGS) $(LDFLAGS)

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	@echo ": Compiling $< ..."
	@mkdir -p $(shell dirname $@)
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@rm -rf $(BLDDIR)
