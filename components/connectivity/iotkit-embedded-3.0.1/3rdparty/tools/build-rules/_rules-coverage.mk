.PHONY: coverage lcov test

ifneq (,$(COVERAGE_LIST))
COVERAGE_PROGS  := \(
COVERAGE_PROGS  += $(COVERAGE_LIST)
COVERAGE_PROGS  += \)
COVERAGE_CMD    := $(RULE_DIR)/scripts/exe_coverage_progs.sh
endif

ifeq (,$(COVERAGE_CMD))
coverage lcov test:
	@echo "COVERAGE_CMD not defined, skip"
else
coverage lcov test:
#
#	SKIP --coverage existence in $(CFLAGS) checking for now
#
	$(Q)$(MAKE) --no-print-directory WITH_LCOV=1

	$(Q)OUTPUT_DIR=$(OUTPUT_DIR) bash <($(SED) '2iPROGS=$(COVERAGE_PROGS)' $(COVERAGE_CMD)) || true

	$(Q)CFLAGS=$(CFLAGS) \
	    $(foreach V,$(INFO_ENV_VARS),$(V)="$($(V))") \
	    bash $(RULE_DIR)/scripts/gen_lcov_report.sh

endif
