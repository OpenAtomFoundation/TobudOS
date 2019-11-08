DEFAULT_BLD             ?= $(CONFIG_DIR)/config.ubuntu.x86
POST_ENV_HOOK           := Post_Env
POST_FINAL_OUT_HOOK     := Post_Distro

ifeq (Darwin,$(shell uname))
POST_FINAL_OUT_HOOK     :=
endif

CONFIG_LIB_EXPORT       ?= static

# Default CFLAGS setting
#
CFLAGS  += -ansi
CFLAGS  += -Wdeclaration-after-statement
CFLAGS  += -DFORCE_SSL_VERIFY

# Default INCLUDE list
#
I_DIRS  := \
    include \
    include/imports \
    include/exports \
    wrappers \

CFLAGS += $(foreach D,$(I_DIRS),$(if $(shell [ -d $(TOP_DIR)/$(D) ] && echo y), -I$(D)))

# For one makefile Generation
#
EXTRA_INSTALL_HDRS  := \
    $(EXPORT_DIR)/iot_import.h \
    $(EXPORT_DIR)/iot_export.h \
    $(EXPORT_DIR)/imports \
    $(EXPORT_DIR)/exports \
