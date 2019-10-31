.PHONY: modinfo

modinfo:
	@true

MODINFO_VARS := \
    EXTRA_SRCS \
    PKG_SWITCH \
    ORIGIN \
    PKG_SOURCE \
    PKG_BRANCH \
    PKG_REVISION \
    PKG_UPSTREAM \
    REF_CFLAGS \
    REF_LDFLAGS \
    LDFLAGS \
    LIBA_TARGET \
    LIB_OBJS \
    TARGET \
    LIBSO_TARGET \

ifneq (,$(CONFIG_$(MODULE_NAME)))
$(if $(filter modinfo,$(MAKECMDGOALS)), \
    $(if $(strip $(DEPENDS)), \
        $(info DEPENDS_$(MODULE_NAME) = $(strip $(DEPENDS))) \
        $(info CONFIG_$(MODULE_NAME) = $(CONFIG_$(MODULE_NAME))) \
    ) \
)

$(if $(filter modinfo,$(MAKECMDGOALS)), \
    $(foreach v, $(MODINFO_VARS), \
        $(if $(strip $($(v))), \
            $(info $(v)_$(MODULE_NAME) = $(strip $($(v)))) \
        ) \
    ) \
)

ifeq (0,$(words $(TARGET)))
else
ifeq (1,$(words $(TARGET)))

$(if $(filter modinfo,$(MAKECMDGOALS)), \
    $(info SRCS_$(TARGET) = $(if $(SRCS_$(TARGET)), \
        $(subst $(TOP_DIR)/,,$(SRCS_$(TARGET))), \
        $(subst $(TOP_DIR)/$(MODULE_NAME)/,,$(SRCS)))) \
)

else

$(if $(filter modinfo,$(MAKECMDGOALS)), \
    $(foreach v, $(TARGET), \
        $(info SRCS_$(v) = $(SRCS_$(v))) \
    ) \
)

endif
endif
endif
