ifndef LIBA_TARGET
ifndef LIBSO_TARGET
LIB_SRCS :=
endif
endif

VPATH    := $(TOP_DIR)/$(MODULE_NAME)
LIB_SRCS ?= $(foreach M,*.c */*.c */*/*.c,$(wildcard $(TOP_DIR)/$(MODULE_NAME)/$(M))) $(wildcard *.c)

.PHONY : cmake

cmake:

	$(Q)$(foreach V,$(INFO_ENV_VARS),$(subst -,_,$(V))="$($(V))") \
	    $(foreach V,$(TARGET),$(subst -,_,SRCS_$(V))="$(SRCS_$(V))") \
	    bash $(if $(TOP_Q),,-x) $(RULE_DIR)/scripts/gen_sub_cmake.sh $(TOP_DIR)/${MODULE_NAME}/CMakeLists.txt

ifdef Extra_CMake_Head
	@rm -f $(OUTPUT_DIR)/$(STAMP_CMAKE)
	@$(call Extra_CMake_Head, >> $(OUTPUT_DIR)/$(STAMP_CMAKE))
	@cat $(TOP_DIR)/${MODULE_NAME}/CMakeLists.txt >> $(OUTPUT_DIR)/$(STAMP_CMAKE)
	@$(call Extra_CMake_Foot, >> $(OUTPUT_DIR)/$(STAMP_CMAKE))
	@mv $(OUTPUT_DIR)/$(STAMP_CMAKE) $(TOP_DIR)/${MODULE_NAME}/CMakeLists.txt
endif

ifdef LIB_SRCS_PATTERN
SRC_LIST := $(foreach M,$(LIB_SRCS_PATTERN),$(shell ls $(TOP_DIR)/$(MODULE_NAME)/$(M) 2>/dev/null))
LIB_SRCS := $(SRC_LIST)
endif

LIB_SRCS := $(filter-out $(foreach M,$(LIB_SRCS_EXCLUDE),$(TOP_DIR)/$(MODULE_NAME)/$(M)),$(LIB_SRCS))
LIB_OBJS := $(LIB_SRCS:.c=.o)
LIB_OBJS := $(subst $(TOP_DIR)/$(MODULE_NAME)/,,$(LIB_OBJS))

sinclude $(LIB_OBJS:.o=.d)

ifdef LIBA_TARGET
.PHONY: StaticLib_Install

ifeq (1,$(words $(LIBA_TARGET)))

$(LIBA_TARGET) :: $(LIB_OBJS)
	@$(call Brief_Log,"AR")
	$(call Inspect_Env,$(WATCHED_VARS))
	$(Q)rm -f $@
ifdef CONFIG_LIBOBJ_STRIP
	@$(call Brief_Log,"ST")
	$(TOP_Q)$(STRIP) $(STRIP_DBGOPT) $(LIB_OBJS)
endif
	$(TOP_Q) \
	if [ "$$(echo "$(LIB_OBJS)"|awk '{ print NF }')" != "0" ]; then \
	    $(AR) -rcs $@ $(LIB_OBJS); \
	fi

$(LIBA_TARGET) :: StaticLib_Install
	$(Q)mkdir -p $(LIBOBJ_TMPDIR)/$(MODULE_NAME)
ifneq (,$(strip $(LIB_OBJS)))
ifneq ($(LIBA_TARGET),$(LIBA_SKIP_COMBO))
	$(Q)cp -f $(LIB_OBJS) $(LIBOBJ_TMPDIR)/$(MODULE_NAME) 2>/dev/null || true
endif
endif
	$(Q)mkdir -p $(SYSROOT_LIB)
	$(Q)if [ -f $@ ]; then cp -f $@ $(SYSROOT_LIB); fi
	$(call Copy_Headers, $(LIB_HEADERS),$(SYSROOT_INC),$(LIB_HDRS_DIR))

else

$(foreach t,$(sort $(LIBA_TARGET)),$(t)): FORCE
	$(Q) \
	$(MAKE) LIBA_TARGET=$@ \
	    LIB_SRCS="$(LIB_SRCS_$(subst .a,,$(subst lib,,$@)))" \
	    LIB_SRCS_PATTERN="$(LIB_SRCS_PATTERN_$(subst .a,,$(subst lib,,$@)))" \


endif   # ifeq (1,$(words $(LIBA_TARGET)))

endif   # ifdef LIBA_TARGET

ifdef LIBSO_TARGET
.PHONY: DynamicLib_Install

$(LIBSO_TARGET) :: SELF_LIBNAME = $(subst lib,,$(subst .so,,$(LIBSO_TARGET)))
$(LIBSO_TARGET) :: LDFLAGS := $(filter-out -l$(SELF_LIBNAME), $(LDFLAGS))
$(LIBSO_TARGET) :: $(LIB_OBJS) $(foreach d,$(DEPENDS_$(MODULE_NAME)),$(SYSROOT_LIB)/$(LIBA_TARGET_$(d)))
	@$(call Brief_Log,"CC")
	$(call Inspect_Env,$(WATCHED_VARS))
	$(Q)$(CC) -shared -Os \
	    $(CFLAGS) \
	    $(RPATH_CFLAGS) \
	    -o $@ \
	    $(LIB_OBJS) \
	    -Wl,--start-group $(LDFLAGS) -Wl,--end-group

$(LIBSO_TARGET) :: DynamicLib_Install
	$(Q)mkdir -p $(LIBOBJ_TMPDIR)/$(shell $(SHELL_DBG) basename $(CURDIR))
	$(Q)cp -f $(LIB_OBJS) $(LIBOBJ_TMPDIR)/$(shell $(SHELL_DBG) basename $(CURDIR))
	$(Q)mkdir -p $(SYSROOT_LIB)
	$(Q)install -m 0755 $@ $(SYSROOT_LIB)
	$(call Copy_Headers, $(LIB_HEADERS),$(SYSROOT_INC),$(LIB_HDRS_DIR))

endif   # ifdef LIBSO_TARGET

