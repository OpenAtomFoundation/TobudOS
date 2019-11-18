SUB_LOG_OPTION := $(if $(Q),,| tee -a $(OUTPUT_DIR)/$${i}/$(COMPILE_LOG))
ALL_LOG_OPTION := $(if $(Q),,| tee -a $(COMPILE_LOG))

sub-mods: toolchain config
	$(Q) \
	if [ -f $(STAMP_ONE_MK) ] && [ "$(MAKECMDGOALS)" = "" ]; then \
	    CORE_NUM=$$(cat /proc/cpuinfo 2>/dev/null| grep processor | tail -1 | awk '{ print $$NF }'); \
	    JOBS_NUM=32; \
	    if [ "$${CORE_NUM}" != "" ]; then JOBS_NUM=$${CORE_NUM}; fi; \
	    if [ "$(Q)" != "@" ]; then JOBS_NUM=0; fi; \
	    rm -rf $(OUTPUT_DIR)${bindir}/* && \
	    if [ -f $(STAMP_LCOV) ] && [ "$(WITH_LCOV)" != "1" ]; then \
	        $(MAKE) --no-print-directory clean; \
	    fi && \
	    if ([ ! -f $(STAMP_LCOV) ] && [ "$(WITH_LCOV)" = "1" ]) || [ $(TOP_DIR)/make.settings -nt $(DIST_DIR) ]; then \
	        $(MAKE) --no-print-directory clean; \
	    fi && \
	    $(MAKE) --no-print-directory -j$$((JOBS_NUM + 1)) -f $(STAMP_ONE_MK) && \
	    TMPD=$$(mktemp -d) && \
	    rm -rf $(LIBOBJ_TMPDIR) $${TMPD} && \
	    cp -rf $(OUTPUT_DIR) $${TMPD} && \
	    rm -rf $${TMPD}/{usr,stamps} && \
	    mv $${TMPD} $(LIBOBJ_TMPDIR); \
	else \
	    if [ -f $(STAMP_PRJ_CFG) ]; then true; else \
	        set -o pipefail; \
	        for i in \
	            $(SUBDIRS); do \
	                if [ ! -d $${i} ]; then continue; fi; \
	                $(MAKE) --no-print-directory Q=$(Q) $${i} 2>&1 $(SUB_LOG_OPTION); \
	                RETVAL=$$?; \
	                if [ $${RETVAL} != 0 ]; then exit $${RETVAL}; fi; \
	        done 2>&1 $(ALL_LOG_OPTION); \
	    fi; \
	fi

TOP_BUILD_VARS := \
    CC LD AR \
    CFLAGS LDFLAGS \
    PACKAGE_DIR \
    IMPORT_DIR \
    EXPORT_DIR \
    TOP_DIR \
    RULE_DIR \
    CONFIG_VENDOR \
    COMP_LIB \
    COMP_LIB_COMPONENTS \
    $(CROSS_CANDIDATES) \
    $(MAKE_ENV_VARS) \
    INSTALL_DIR \
    INSTALL_LIB_DIR \
    SYSROOT_INC \
    KERNEL_DIR \
    MAKE_ENV_VARS \
    CROSS_PREFIX \
    CROSS_CANDIDATES \
    ALL_SUB_DIRS \
    WIN32_CMAKE_SKIP \
    NOEXEC_CMAKE_DIRS \

CMDLINE_VARS := \
    HD_MAKEFILE \
    MAKE_SEGMENT \
    OUTPUT_DIR \
    PACKAGE_DIR \
    STAMP_BLD_ENV \
    STAMP_UNPACK \
    TOP_DIR \
    RULE_DIR \

# When TOP_BUILD_VARS like $(CFLAGS) contains special character '$'
# simply echo its value into 'Makefile' will cause '$' lost when GNU make read in again
#
$(STAMP_BLD_ENV): $(TOP_DIR)/makefile $(shell ls $(CONFIG_TPL) 2>/dev/null) \
                  $(wildcard $(RULE_DIR)/*.mk) \
                  $(shell grep "^ *include" $(TOP_DIR)/$(TOP_MAKEFILE)|awk '{ print $$NF }'|$(SED) '/^\$$/d')
	@rm -f $@
	@$(foreach V, \
	    $(sort $(TOP_BUILD_VARS)), \
	        echo "$(V) := $(sort $($(V)))"|$(SED) 's:\$$:$$$$:g' >> $(STAMP_BLD_ENV); \
	)
	@echo "COMP_LIB_FILES := $(foreach V,$(COMP_LIB_COMPONENTS), $(LIBA_TARGET_$(V)))" >> $@

# note:
#   $(SED) -i "/CONFIG_$${i//\//\\/}.*/d" $(CONFIG_TPL);
# above
#   $(SED) -i "1iCONFIG_$${i} = y" $(CONFIG_TPL)
# was removed since modules will be skipped in some cases

$(STAMP_BLD_VAR): $(foreach d,$(ALL_SUB_DIRS),$(d)/$(MAKE_SEGMENT)) $(STAMP_BLD_ENV) $(wildcard $(RULE_DIR)/*.mk)
	$(TOP_Q) \
( \
	if [ ! -f $(STAMP_BLD_VAR) ]; then echo ""; VERBOSE=1; fi; \
	rm -f $(STAMP_BLD_VAR); \
	for i in $(shell echo "$(ALL_SUB_DIRS)"|tr ' ' '\n'|sort -u); do \
	    if [ "$${VERBOSE}" != "" ]; then \
	        printf "CONFIGURE .............................. [%s]\n" $${i}; \
	        $(SED) -i "1iCONFIG_$${i} = y" $(CONFIG_TPL); \
	        if ! grep -q "target-$${i}:" $(STAMP_POST_RULE) 2>/dev/null; then \
	            echo "target-$${i}:; @true" >> $(STAMP_POST_RULE); \
	        fi; \
	    fi; \
	    $(foreach V, $(CMDLINE_VARS), $(V)="$($(V))") \
	        bash $(RULE_DIR)/pre-build.sh $${i} makefile-only > /dev/null; \
	    if [ -d $(OUTPUT_DIR)/$${i} ]; then \
	        $(MAKE) -s -C $(OUTPUT_DIR)/$${i} modinfo > /dev/null; \
	        if [ $$? = 0 ]; then \
	            $(MAKE) --no-print-directory -s -C $(OUTPUT_DIR)/$${i} modinfo >> $(STAMP_BLD_VAR); \
	        else \
	            echo ""; \
	            echo "ERROR detected in '$${i}/$(MAKE_SEGMENT)'..."|grep --color '.*'; \
	            echo ""; \
	            rm -f $(STAMP_BLD_VAR) $(STAMP_PRJ_CFG); \
	            exit 13; \
	        fi \
	    fi \
	done; \
	sort -o $(STAMP_BLD_VAR) $(STAMP_BLD_VAR); \
	if [ "$${VERBOSE}" != "" ]; then echo ""; fi; \
)

pre-build: MOD = $(subst target-,,$(filter-out $@,$(MAKECMDGOALS)))
pre-build: $(STAMP_BLD_ENV)
	$(TOP_Q)rm -f $(OUTPUT_DIR)/$(MOD)/$(STAMP_UNPACK)
	$(if $(filter 0,$(MAKELEVEL)),,@) \
	$(strip $(foreach V, $(CMDLINE_VARS), $(V)="$($(V))") \
	    PKG_SOURCE="$(PKG_SOURCE_$(MOD))" \
	    PKG_BRANCH="$(PKG_BRANCH_$(MOD))" \
	    PKG_REVISION="$(PKG_REVISION_$(MOD))" \
	    PKG_UPSTREAM="$(PKG_UPSTREAM_$(MOD))" \
	    PKG_SWITCH="$(PKG_SWITCH_$(MOD))" \
	) \
	$(if $(filter 0,$(MAKELEVEL)),VERBOSE_PRE_BLD=1) \
	    bash $(RULE_DIR)/pre-build.sh $(subst target-,,$(filter-out $@,$(MAKECMDGOALS)))

.PHONY: $(ALL_SUB_DIRS)

$(ALL_SUB_DIRS): ALL_LOG_OPT = $(if $(Q),,2>&1|tee -a $(OUTPUT_DIR)/$(COMPILE_LOG))
$(ALL_SUB_DIRS): SUB_LOG_OPT = $(if $(Q),,2>&1|tee -a $(OUTPUT_DIR)/$@/$(COMPILE_LOG))

$(ALL_SUB_DIRS): $(if $(filter 0,$(MAKELEVEL)),toolchain) $(STAMP_BLD_VAR)
	$(TOP_Q)rm -f $(STAMP_PRJ_CFG)
	$(TOP_Q)$(MAKE) --no-print-directory pre-build target-$@
ifeq (0,$(MAKELEVEL))
	$(Q)$(MAKE) --no-print-directory -C $(OUTPUT_DIR)/$@ clean
endif
	$(Q) \
	if [ "$$( $(call Require_Build,$@) )" = "TRUE" ]; then \
	    $(call Build_Depends,$@) && \
	    $(call Build_CompLib,$@) && \
	    $(call Update_Extra_Srcs,$(EXTRA_SRCS_$@),$@) && \
	    $(MAKE) --no-print-directory -C $(OUTPUT_DIR)/$@ all $(SUB_LOG_OPT) $(ALL_LOG_OPT) && \
	    if [ "$$(echo $(ORIGIN_$@))" != "" ]; then \
	        touch $(OUTPUT_DIR)/$@/{$(STAMP_UNPACK),$(STAMP_CONFIG),$(STAMP_BUILD),$(STAMP_INSTALL)}; \
	    fi \
	else \
	    echo -ne "\r$$(printf '%40s' '')\r"; \
	fi

	@mkdir -p $(STAMP_DIR) && touch $(STAMP_DIR)/$$(echo "$@"|$(SED) 's:/:~:g').build.done
