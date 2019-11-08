
define Post_Env
( \
    $(foreach V,$(SWITCH_VARS), \
        $(if $(findstring FEATURE_,$(V)), \
            printf "%-40s : %-s\n" "+ $(V)" "$(if $($(V)),$($(V)),n)"; \
        ) \
    ) \
    echo ""; \
)
endef

