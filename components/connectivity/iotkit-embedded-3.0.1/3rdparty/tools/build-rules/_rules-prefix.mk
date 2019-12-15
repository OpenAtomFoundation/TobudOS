ifeq (,$(findstring llvm,$(CC)))
CFLAGS  := "$(sort $(CFLAGS) $(CONFIG_ENV_CFLAGS))"
else
CFLAGS  := "$(CFLAGS) $(CONFIG_ENV_CFLAGS)"
endif

LDFLAGS := $(sort $(LDFLAGS) $(CONFIG_ENV_LDFLAGS))

ifeq (dynamic,$(CONFIG_LIB_EXPORT))
CFLAGS  := $(filter-out --coverage,$(CFLAGS))
endif

MAKE_ENV_VARS := \
$(foreach v, \
    $(shell grep -o 'CONFIG_ENV_[_A-Z]*' $(CONFIG_TPL) 2>/dev/null), \
        $(subst CONFIG_ENV_,,$(v)) \
)

# $(eval ...) causes '$' in CFLAGS lost
MAKE_ENV_VARS := $(sort $(filter-out CFLAGS LDFLAGS,$(MAKE_ENV_VARS)))

$(foreach V, \
    $(MAKE_ENV_VARS), \
        $(eval export $(V) := $(sort $(CONFIG_ENV_$(V)))) \
)
