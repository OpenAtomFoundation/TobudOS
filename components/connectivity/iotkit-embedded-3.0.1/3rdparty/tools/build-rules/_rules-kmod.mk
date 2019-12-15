ifdef KMOD_TARGET
KMOD_NAME := $(subst .o,,$(obj-m))
KMOD_OBJS := $(foreach mod, $(KMOD_NAME), $($(mod)-objs))
KMOD_SRCS := $(subst .o,.c,$(KMOD_OBJS))
KMOD_BUILD_DIR := $(CURDIR)/build-$(shell $(SHELL_DBG) basename $(CURDIR))-kmod
KMOD_MAKEFILE := $(KMOD_BUILD_DIR)/Makefile

$(firstword $(KMOD_TARGET)): $(KMOD_SRCS)
	$(Q)rm -rf $(KMOD_BUILD_DIR) && mkdir -p $(KMOD_BUILD_DIR)
	$(Q)cp -f $(KMOD_SRCS) $(KMOD_BUILD_DIR)
	$(Q)echo "EXTRA_CFLAGS += " \
	         "-I$(CURDIR)" \
	         "$(INTERNAL_INCLUDES)" \
	         | $(SED) 's/-I/\\\n    -I/g' \
	         >> $(KMOD_MAKEFILE)
	$(Q)echo "" >> $(KMOD_MAKEFILE)
	$(Q)echo "obj-m := $(obj-m)" >> $(KMOD_MAKEFILE)
	$(Q)echo "" >> $(KMOD_MAKEFILE)
	$(Q) \
	$(foreach mod, $(KMOD_NAME), \
	    echo "$(mod)-objs := $($(mod)-objs)" \
	    | $(SED) 's/ [_a-z]*\.o/ \\\n   &/g' \
	    >> $(KMOD_MAKEFILE); \
	    echo "" >> $(KMOD_MAKEFILE); \
	)
	@$(call Brief_Log,"CC",$(KMOD_TARGET))
	$(Q) \
	LDFLAGS=""; \
	    $(MAKE) -C $(KERNEL_DIR) M=$(KMOD_BUILD_DIR) CROSS_COMPILE=$(CROSS_PREFIX) modules
	$(Q)cp -f $(KMOD_BUILD_DIR)/*.ko $(CURDIR)
	$(Q)mkdir -p $(SYSROOT_LIB)
	$(Q)install -m 0755 $(KMOD_BUILD_DIR)/*.ko $(SYSROOT_LIB)

endif   # ifdef KMOD_TARGET

