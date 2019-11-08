OPS_CMDLINE_V := \
    PACKAGE_DIR \
    TOP_DIR \
    STAMP_BLD_VAR \

OPS_SCRIPT := \
    $(strip $(foreach V, $(OPS_CMDLINE_V), $(V)="$($(V))")) \
    $(SHELL) \
    $(RULE_DIR)/scripts/ops_repository.sh \
    $(STAMP_BLD_VAR)

repo-list: config
	$(TOP_Q)$(OPS_SCRIPT) list

repo-update: config
	$(TOP_Q)$(OPS_SCRIPT) update
