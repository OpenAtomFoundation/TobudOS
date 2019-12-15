.PHONY: comp-lib

ifdef COMP_LIB
ifeq (dynamic,$(CONFIG_LIB_EXPORT))
define Finalize_CompLib
( \
    $(CC) -shared -Os -o $(2)/lib$(3).so $(1) $(LDFLAGS); \
)
endef
define Info_CompLib
( \
	EXIST_OBJS="$$(ls $(2) 2>/dev/null)"; \
\
    echo -ne "\033[1;32m"; \
    printf "\r%-40s%s\n" "[AR] lib$(1).so" "<=      "; \
    for i in $${EXIST_OBJS}; do \
        printf "%-40s%s\n" "" "   $${i}"|$(SED) 's,$(LIBOBJ_TMPDIR)/,,g'; \
    done; \
    echo -ne "\033[0m"; \
)
endef
else
define Finalize_CompLib
( \
	EXIST_OBJS="$$(ls $(1) 2>/dev/null)"; \
\
	if [ "$${EXIST_OBJS}" != "" ]; then \
	    $(AR) -rcs $(2)/lib$(3).a $${EXIST_OBJS}; \
	fi \
)
endef
define Info_CompLib
( \
	EXIST_OBJS="$$(ls $(2) 2>/dev/null)"; \
\
    echo -ne "\033[1;35m"; \
    printf "\r%-40s%s\n" "[AR] lib$(1).a" "<=      "; \
    for i in $${EXIST_OBJS}; do \
        printf "%-40s%s\n" "" "   $${i}"|$(SED) 's,$(LIBOBJ_TMPDIR)/,,g'; \
    done; \
    echo -ne "\033[0m"; \
)
endef
endif # dynamic
endif # COMP_LIB

comp-lib: toolchain
ifdef COMP_LIB
	$(TOP_Q)+( \
	if [ -f $(STAMP_PRJ_CFG) ]; then true; else \
	    $(call Build_CompLib,FORCE) \
	fi)
else
	$(Q)true
endif
