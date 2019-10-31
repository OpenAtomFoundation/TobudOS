include $(CURDIR)/tools/internal_make_funcs.mk

SWITCH_VARS := \
$(shell grep '''config [_A-Z]*''' \
    $$(find -L $(TOP_DIR)/tools -name 'Config.*') \
        | cut -d: -f2 \
        | grep -v menuconfig \
        | grep -v SRCPATH \
        | awk '{ print $$NF }' \
)
SWITCH_VARS := $(foreach V,$(sort $(SWITCH_VARS)),FEATURE_$(V))

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval CFLAGS += -D$(subst FEATURE_,,$(v)))) \
)
