# Basic Settings
#
SHELL           := /bin/bash
TOP_DIR         ?= $(CURDIR)

ifeq (Darwin,$(strip $(shell uname)))
SED             := gsed
STRIP_DBGOPT    :=
else
SED             := sed
STRIP_DBGOPT    := --strip-dwo
endif

# Settings of input directory and file
#
RULE_DIR        ?= $(TOP_DIR)/build-rules
CONFIG_DIR      ?= $(TOP_DIR)/configs
PACKAGE_DIR     ?= $(TOP_DIR)/packages
IMPORT_DIR      ?= $(TOP_DIR)/import
MAKE_SEGMENT    ?= iot.mk
DOXYGEN_DIR     ?= $(TOP_DIR)

ifeq ($(shell [ ! -d $(CONFIG_DIR) ] && echo y),y)
DEFAULT_BLD     := $(RULE_DIR)/misc/config.generic.default
endif

# Settings of project information
PRJ_NAME        ?= LITE-build.prj
PRJ_VERSION     ?= LITE-build.ver

# Settings of output directory
#
CONFIG_TPL      := $(TOP_DIR)/.config
OUTPUT_DIR      ?= $(TOP_DIR)/.O
SHADOW_DIR      ?= workspace
DIST_DIR        ?= $(TOP_DIR)/output
LCOV_DIR        ?= Coverage
FINAL_DIR       ?= $(DIST_DIR)/release

# Setting of directory for flat modules
#
bindir          = /usr/bin
libdir          = /usr/lib
incdir          = /usr/include

# Setting of build process control
#
COMPILE_LOG     := compile.log
WARNING_LOG     := warnings.log
HD_MAKEFILE     := makefile
TOP_MAKEFILE    := makefile

STAMP_PRJ_CFG   := $(OUTPUT_DIR)/.just.configured
STAMP_BLD_ENV   := $(OUTPUT_DIR)/.sub-build.env
STAMP_BLD_VAR   := $(OUTPUT_DIR)/.sub-build.vars
STAMP_LCOV      := $(OUTPUT_DIR)/.coverage.done
STAMP_UNPACK    := .unpack.done
STAMP_CONFIG    := .config.done
STAMP_CMAKE     := .cmake_section
STAMP_BUILD     := .build.done
STAMP_INSTALL   := .install.done
STAMP_POSTINS   := .post.install.done
STAMP_SHIELD    := .shield.done
STAMP_POST_RULE := $(OUTPUT_DIR)/.post-rules.mk
STAMP_DIR       := $(OUTPUT_DIR)/stamps
STAMP_ONE_MK	:= $(OUTPUT_DIR)/.one_makefile

ifdef DEBUG
AUTO_HEADER_Q   :=
ORIGIN_Q        :=
SHELL_DBG       := set -x;
Q               :=
TOP_Q           :=
else
Q               := @
TOP_Q           := @
endif

ifdef DEBUG_SHELL
SHELL_DBG       := set -x;
endif

# Setting of downloading toolchains
TOOLCHAIN_DLDIR := $(OUTPUT_DIR)/compiler

# Setting of directory for origin modules
#
export INSTALL_DIR     = $(OUTPUT_DIR)/usr
export INSTALL_BIN_DIR = $(INSTALL_DIR)/bin
export INSTALL_LIB_DIR = $(INSTALL_DIR)/lib

# Setting of cmake auto-generation
#
CMAKE_EXPORT_LIBS   := \
    src/ref-impl/hal \
    src/ref-impl/tls \

include  $(RULE_DIR)/funcs.mk
