define CompLib_Map
$(eval \
    COMP_LIB_COMPONENTS += \
        $(if \
            $(filter y,$(FEATURE_$(strip $(1)))),$(strip $(2)) \
        ) \
)
endef

define CompInc_Map
$(eval \
    COMP_LIB_COMPONENTS_INCLUDES += \
        $(if \
            $(filter y,$(FEATURE_$(strip $(1)))),$(strip $(2)) \
        ) \
)
endef

# 31, red. 32, green. 33, yellow. 34, blue. 35, magenta. 36, cyan. 37, white.
define Brief_Log
	@if [ "$1" = "CC" ]; then \
	    if echo "$@"|grep -q "\.so$$"; then \
	        COLOR_MARK="\e[1;32m"; \
	    elif echo "$@"|grep -q "\.ko$$"; then \
	        COLOR_MARK="\e[1;35m"; \
	    else \
	        COLOR_MARK="\e[1;36m"; \
	    fi \
	elif [ "$1" = "AR" ]; then \
	    COLOR_MARK="\e[1;33m"; \
	elif [ "$1" = "LD" ]; then \
	    COLOR_MARK="\e[1;31m"; \
	fi; \
	echo -ne "$${COLOR_MARK}";

	@if [ "$2" = "" ]; then \
	    FIRST_DEP="$(firstword $(filter-out FORCE,$?))"; \
	    SPACE_BAR="                                   "; \
	    if [ "$${FIRST_DEP}" != "" ]; then \
	        FIRST_DEP="$$(basename $${FIRST_DEP})"; \
	    fi; \
	    printf "\r%-40s%s\n" "[$1] $$(expr substr $$(basename $@) 1 30)" "<= $${FIRST_DEP} $${SPACE_BAR}"; \
	else \
	    printf "\r%-40s%s\n" "[$1] $$(expr substr $(2) 1 30)" "<= $${FIRST_DEP} $${SPACE_BAR}"; \
	fi

	@for i in $(wordlist 2,100,$^); do \
	    printf "%-40s%s\n" "" "   $$(basename $${i})"; \
	done
	@echo -ne "\e[0m"
endef

define CC_Log
	@if [ "$2" = "" ]; then \
	    FIRST_DEP="$(firstword $(filter-out FORCE,$?))"; \
	    SPACE_BAR="                                   "; \
	    if [ "$${FIRST_DEP}" != "" ]; then \
	        FIRST_DEP="$$(basename $${FIRST_DEP})"; \
	    fi; \
	    printf "\r%-40s%s\n" "[CC] $$(expr substr $$(basename $@) 1 30)" "<= $${FIRST_DEP} $${SPACE_BAR}"; \
	else \
	    printf "\r%-40s%s\n" "[CC] $$(expr substr $(2) 1 30)" "<= $${FIRST_DEP} $${SPACE_BAR}"; \
	fi

	@for i in $(wordlist 2,100,$^); do \
	    printf "%-40s%s\n" "" "   $$(basename $${i})"; \
	done
endef

define AR_Log
	@if [ "$2" = "" ]; then \
	    FIRST_DEP="$(firstword $(filter-out FORCE,$?))"; \
	    SPACE_BAR="                                   "; \
	    if [ "$${FIRST_DEP}" != "" ]; then \
	        FIRST_DEP="$$(basename $${FIRST_DEP})"; \
	    fi; \
	    printf "\r%-40s%s\n" "[AR] $$(expr substr $$(basename $@) 1 30)" "<= $${FIRST_DEP} $${SPACE_BAR}"; \
	else \
	    printf "\r%-40s%s\n" "[AR] $$(expr substr $(2) 1 30)" "<= $${FIRST_DEP} $${SPACE_BAR}"; \
	fi

	@for i in $(wordlist 2,100,$^); do \
	    printf "%-40s%s\n" "" "   $$(basename $${i})"; \
	done
endef

define Compile_Result
    @echo ""
    @echo "========================================================================="
    @echo "o BUILD COMPLETE WITH FOLLOWING CONFIGS:"
    @echo "----"
    @( \
    $(foreach V,$(SWITCH_VARS), \
        $(if $(findstring FEATURE_,$(V)), \
            printf "%-36s : %-s\n" "    $(V)" "$($(V))"; \
        ) \
    ) )

    @echo ""
    @echo "o RELEASE PACKAGE LAYOUT:"
    @echo "----"
    @cd $(FINAL_DIR) && echo -n "    " && pwd && echo "" && \
     find . -print | awk '!/\.$$/ { \
        for (i = 1; i < NF-1; i++) { \
            printf("|   "); \
        } \
        print "+-- "$$NF}' FS='/' | sed 's!\(.*\)!    &!g'
    @echo ""

    @echo "o BINARY FOOTPRINT CONSIST:"
    @echo "----"
    @chmod a+x $(SCRIPT_DIR)/stats_static_lib.sh
    @$(SCRIPT_DIR)/stats_static_lib.sh $(FINAL_DIR)/lib/$(COMP_LIB)
    @$(SCRIPT_DIR)/stats_static_lib.sh $(FINAL_DIR)/lib/$(PLATFORM_LIB)

    @echo "========================================================================="
    @echo ""
endef


