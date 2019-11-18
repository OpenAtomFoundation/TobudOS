include  $(RULE_DIR)/settings.mk
sinclude $(CONFIG_TPL)

export PATH := $(PATH):$(TOOLCHAIN_DLDIR)/main/bin
unexport VERBOSE
unexport DEBUG

TOPDIR_NAME     := $(shell $(SHELL_DBG) basename $(TOP_DIR))
LIBOBJ_TMPDIR   := $(OUTPUT_DIR)/lib$(TOPDIR_NAME).objs

SYSROOT_BIN     := $(OUTPUT_DIR)${bindir}
SYSROOT_INC     := $(OUTPUT_DIR)${incdir}
SYSROOT_LIB     := $(OUTPUT_DIR)${libdir}
CONFIG_VENDOR   := $(shell grep -m 1 "VENDOR *:" $(CONFIG_TPL) 2>/dev/null|awk '{ print $$NF }')
IMPORT_VDRDIR   := $(IMPORT_DIR)/$(CONFIG_VENDOR)

HOST_ARCH_BITS  := $(shell [ "$$(uname -m)" = "x86_64" ] && echo -n "64" || echo -n "32")
PREBUILT_LIBDIR := $(if $(strip $(filter 64,$(HOST_ARCH_BITS)) $(CROSS_PREFIX)),libs,32bit-libs)
PREBUILT_BINDIR := $(if $(strip $(filter 64,$(HOST_ARCH_BITS)) $(CROSS_PREFIX)),bin,32bit-bin)

CONFIG_VSP_WEBSITE  ?= http://gitlab.alibaba-inc.com/yusheng.yx/alink-vendor-support-package

ifeq ($(shell uname), Linux)
RPATH_CFLAGS    += -Wl,-rpath='$$''ORIGIN/../lib'
endif

# CFLAGS contains '$' causes ./configure failure in ORIGIN packages
#
# Instead of:
#
# ifeq ($(shell uname), Linux)
# CFLAGS += -Wl,-rpath='$$''ORIGIN/../lib'
# CFLAGS := $(sort $(CFLAGS))
# endif
#
# Write @ package's makefile:
#
# ./configure \
#     CFLAGS='$(CFLAGS) -Wl,-rpath='\''$$$$'\''ORIGIN/../lib' \
#     --prefix=...
#
# To pass in RPATH related link flags

COMP_LIB_NAME   := $(subst lib,,$(subst .so,,$(subst .a,,$(COMP_LIB))))
COMP_LIB_OBJS    = $(foreach d,$(COMP_LIB_COMPONENTS),$(LIBOBJ_TMPDIR)/$(d)/*.o)
RECURSIVE_MAKE  := $(MAKE) $(if $(TOP_Q),-s) -C $(TOP_DIR) -f $(TOP_MAKEFILE)
ALL_SUB_DIRS    := $(shell find -L $(TOP_DIR) \
                                ! -path "$(OUTPUT_DIR)/*" \
                                ! -path "$(TOP_DIR)/$(SHADOW_DIR)/*" \
                                  -name "$(MAKE_SEGMENT)" 2>/dev/null \
                           | $(SED) 's,$(TOP_DIR)[/]*,,;s,[/]*$(MAKE_SEGMENT),,')

SHOW_ENV_VARS   := \
    MODULE_NAME SUBDIRS PKG_NAME PKG_RPATH PKG_SOURCE PKG_SWITCH_V PKG_SWITCH \
    HOST_ARCH_BITS PREBUILT_LIBDIR RPATH_CFLAGS EXTRA_INCLUDE_DIRS \
    CROSS_PREFIX DEPENDS CFLAGS CCLD LDFLAGS \
    CC LD AR STRIP OBJCOPY COMP_LIB ALL_SUB_DIRS LIBOBJ_TMPDIR \
    MAKE_ENV_VARS DEFAULT_BLD EXTRA_CFLAGS \
    LIBA_TARGET LIBSO_TARGET TARGET KMOD_TARGET \
    SRCS OBJS LIB_SRCS LIB_OBJS LIB_HDRS_DIR LIB_HEADERS EXTRA_SRCS \
    $(foreach M,$(LIBA_TARGET),LIB_SRCS_$(subst .a,,$(subst lib,,$(M)))) \
    INTERNAL_INCLUDES TOP_DIR PRJ_NAME PRJ_VERSION COMP_LIB_COMPONENTS \
    IMPORT_DIR IMPORT_VDRDIR CONFIG_DIR PACKAGE_DIR EXTERNAL_INCLUDES \
    CONFIG_LIB_EXPORT OBJCOPY_FLAGS CONFIG_VENDOR COVERAGE_PROGS COVERAGE_CMD \

INFO_ENV_VARS   := $(SHOW_ENV_VARS) \
    ALL_SUB_DIRS SED RULE_DIR OUTPUT_DIR MAKE_SEGMENT COMP_LIB_NAME LIBOBJ_TMPDIR \
    COMP_LIB_FILES STAMP_BLD_ENV STAMP_BLD_VAR EXTRA_INSTALL_HDRS FINAL_DIR DIST_DIR \
    WIN32_CMAKE_SKIP EXTRA_INCLUDE_DIRS NOEXEC_CMAKE_DIRS COMP_LIB \
    WITH_LCOV LCOV_DIR UTEST_PROG COVERAGE_CMD STAMP_LCOV CMAKE_EXPORT_LIBS \

INFO_ENV_VARS   := $(filter-out CFLAGS,$(INFO_ENV_VARS))

ifndef CONFIG_LIB_EXPORT
ifeq (y,$(strip $(CONFIG_EMB_GATEWAY_SDK)))
CONFIG_LIB_EXPORT := dynamic
else
ifeq (y,$(strip $(CONFIG_EMB_BASIC_SDK)))
CONFIG_LIB_EXPORT := static
else
ifeq (y,$(strip $(CONFIG_EMB_ROUTER_SDK)))
CONFIG_LIB_EXPORT := dynamic
endif # ROUTER
endif # BASIC
endif # GATEWAY
else
ifneq (static,$(CONFIG_LIB_EXPORT))
ifneq (dynamic,$(CONFIG_LIB_EXPORT))
$(error CONFIG_LIB_EXPORT($(CONFIG_LIB_EXPORT)) is neither 'static' or 'dynamic')
endif # dynamic
endif # static
endif # CONFIG_LIB_EXPORT

ifneq (,$(shell ls $(STAMP_BLD_VAR) 2>/dev/null))
ifeq (,$(filter reconfig distclean,$(MAKECMDGOALS)))
ifeq (0,$(strip $(shell $(SED) '/[-_/a-zA-Z0-9]* = *..*/d' $(STAMP_BLD_VAR)|wc -l)))
include $(STAMP_BLD_VAR)
endif
endif
endif

.PHONY: all clean FORCE

ifdef SUBDIRS
include $(RULE_DIR)/_rules-top.mk
include $(RULE_DIR)/_rules-prefix.mk
include $(RULE_DIR)/_rules-repo.mk

CROSS_CANDIDATES := CC CXX AR LD STRIP OBJCOPY
export CC       := $(strip $(if $(OVERRIDE_CC),     $(OVERRIDE_CC),     $(CROSS_PREFIX)gcc))
export CXX      := $(strip $(if $(OVERRIDE_CXX),    $(OVERRIDE_CXX),    $(CROSS_PREFIX)g++))
export AR       := $(strip $(if $(OVERRIDE_AR),     $(OVERRIDE_AR),     $(CROSS_PREFIX)ar))
export LD       := $(strip $(if $(OVERRIDE_LD),     $(OVERRIDE_LD),     $(CROSS_PREFIX)ld))
export OBJCOPY  := $(strip $(if $(OVERRIDE_OBJCOPY),$(OVERRIDE_OBJCOPY),$(CROSS_PREFIX)objcopy))

ifneq (,$(OVERRIDE_STRIP))
export STRIP    := $(OVERRIDE_STRIP)
else
ifneq (,$(CROSS_PREFIX))
export STRIP    := $(CROSS_PREFIX)strip
else
export STRIP    := true
endif
endif

ifeq (gcc,$(strip $(CC)))
export STRIP    := strip
endif

ifneq (y,$(shell which $(CC) > /dev/null 2>&1 && echo 'y'))
LOCAL_TCDIR     := $(TOOLCHAIN_DLDIR)/$(shell $(call Relative_TcPath,$(CC)))
export CC       := $(LOCAL_TCDIR)/$(CC)
export AR       := $(LOCAL_TCDIR)/$(AR)
export LD       := $(LOCAL_TCDIR)/$(LD)
export OBJCOPY  := $(LOCAL_TCDIR)/$(OBJCOPY)
export STRIP    := $(LOCAL_TCDIR)/$(STRIP)
endif

ifneq (,$(filter -m32,$(strip $(CFLAGS))))
PREBUILT_LIBDIR := 32bit-libs
PREBUILT_BINDIR := 32bit-bin
endif

include $(RULE_DIR)/_rules-dist.mk
include $(RULE_DIR)/_rules-complib.mk
include $(RULE_DIR)/_rules-submods.mk

env:
	$(TOP_Q)echo ""
	$(TOP_Q)printf -- "-----------------------------------------------------------------\n"
	$(TOP_Q)$(foreach var,$(SHOW_ENV_VARS),$(call Dump_Var,$(var)))
	$(TOP_Q)printf -- "-----------------------------------------------------------------\n"
	$(TOP_Q)echo ""
	$(TOP_Q)$(call $(POST_ENV_HOOK))

else    # ifdef SUBDIRS

PKG_RPATH   := $(shell echo $(CURDIR)|$(SED) 's,$(OUTPUT_DIR)/*,,g')
PKG_NAME    ?= $(shell basename $(CURDIR))
PKG_SOURCE  ?= $(shell [ -d $(PACKAGE_DIR) ] && find $(PACKAGE_DIR) -name "$(PKG_NAME)*" | head -1)

DEPENDS     += $(DEPENDS_$(MODULE_NAME))
DEPENDS     := $(sort $(strip $(DEPENDS)))

ifdef CONFIG_SWITCH
PKG_SWITCH_V = $(CONFIG_SWITCH)
else
PKG_SWITCH_V := CONFIG_$(PKG_RPATH)
endif
PKG_SWITCH = $($(PKG_SWITCH_V))

ifdef ORIGIN

include $(RULE_DIR)/_rules-origin.mk

else    # ifdef ORIGIN

ifneq (,$(filter -m32,$(strip $(CFLAGS))))
PREBUILT_LIBDIR := 32bit-libs
PREBUILT_BINDIR := 32bit-bin
endif

include $(RULE_DIR)/_rules-flat.mk

endif   # ifdef ORIGIN

include $(RULE_DIR)/_rules-modinfo.mk

endif   # ifdef SUBDIRS

sinclude $(STAMP_POST_RULE)

ifdef UTEST_PROG
COVERAGE_LIST += \"./$(strip $(UTEST_PROG) --list)\"
COVERAGE_LIST += \"./$(strip $(UTEST_PROG) --verbose=4)\"
include $(RULE_DIR)/_rules-coverage.mk
endif
