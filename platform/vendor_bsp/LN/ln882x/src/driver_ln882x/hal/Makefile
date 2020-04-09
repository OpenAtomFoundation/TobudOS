#
# Rules for building library
#

# ----------------------------------------------------------------------------
# common rules
# ----------------------------------------------------------------------------

include $(ART_ROOT)/make/make_rules.mk

# ----------------------------------------------------------------------------
# library and objects
# ----------------------------------------------------------------------------
LIB_NAME := libdrv_hal.a


DIRS := $(abspath $(shell find . -type d))
LIBS := $(subst $(ART_ROOT), $(BUILD_DIR_BASE), $(firstword $(DIRS)))/$(LIB_NAME)

SRCS := $(basename $(foreach dir,$(DIRS),$(wildcard $(dir)/*.[csS])))

OBJS := $(addsuffix .o,$(subst $(ART_ROOT),$(BUILD_DIR_BASE),$(SRCS)))

# library make rules
include $(LIB_MAKE_RULES)
