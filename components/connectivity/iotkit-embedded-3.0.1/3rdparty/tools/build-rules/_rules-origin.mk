CFLAGS := $(filter-out -Werror,$(CFLAGS))
LDFLAGS :=

.PHONY: config build install post-install

ORIGIN_Q ?= @
ifeq ($(strip $(PKG_SWITCH)),y)
all :
ifeq ($(PKG_SOURCE),)
$(error PKG_SOURCE for $(PKG_NAME) cannot be found!)
endif

	$(ORIGIN_Q) \
	MESSAGE=$(strip $(if $(filter 0 1,$(MAKELEVEL)), \
	    "\r$(PKG_NAME) already pre-built at $(IMPORT_VDRDIR)/$(PREBUILT_LIBDIR)\n","")); \
	if  [ "$(LIBA_TARGET)"  != "" -a -f $(IMPORT_VDRDIR)/$(PREBUILT_LIBDIR)/$(LIBA_TARGET) ] || \
	    [ "$(LIBSO_TARGET)" != "" -a -f $(IMPORT_VDRDIR)/$(PREBUILT_LIBDIR)/$(LIBSO_TARGET) ]; then \
	    echo -ne $${MESSAGE}; \
	    cp -P -f $(IMPORT_VDRDIR)/$(PREBUILT_LIBDIR)/{$(LIBA_TARGET),$(LIBSO_TARGET)*} \
	        $(SYSROOT_LIB) 2>/dev/null; \
	    for t in $(TARGET); do \
	        cp -f $(IMPORT_VDRDIR)/$(PREBUILT_BINDIR)/$${t} $(SYSROOT_BIN); \
	    done; \
	    touch $(STAMP_SHIELD); \
	fi

	$(ORIGIN_Q) \
	MESSAGE=$(if $(filter 0 1,$(MAKELEVEL)),"\r$(PKG_NAME) already unpacked\n",""); \
	if [ -f $(STAMP_SHIELD) ]; then true; \
	elif [ -f $(STAMP_UNPACK) ]; then \
	    echo -ne $${MESSAGE}; \
	else \
	    rm -rf $(PKG_NAME)* && \
	    if [ -f $(PKG_SOURCE) ]; then \
	        tar xf $(PKG_SOURCE) -C . && \
	        for i in $(wildcard *.patch); do \
	            cd $(PKG_NAME)* && patch -d . -p 1 < ../$${i} && cd $${OLDPWD}; \
	        done \
	    fi \
	    && touch $(STAMP_UNPACK); \
	fi

	$(ORIGIN_Q) \
	MESSAGE=$(if $(filter 0 1,$(MAKELEVEL)),"\r$(PKG_NAME) already configured\n",""); \
	if [ -f $(STAMP_SHIELD) ]; then true; \
	elif [ -f $(STAMP_CONFIG) ]; then \
	    echo -ne $${MESSAGE}; \
	else \
	    if grep -q 'config *:' $(HD_MAKEFILE); then \
	        export SHELL=$(SHELL); \
	        $(MAKE) config -f $(HD_MAKEFILE); \
	    else \
	        cd $(PKG_NAME)* && ( \
	        ./configure \
	            --prefix=$(OUTPUT_DIR)/usr \
	            --host=$(HOST) \
	            --target=$(shell $(SHELL_DBG) basename $(CROSS_PREFIX) 2>/dev/null) \
	            --enable-static --enable-shared \
	        || \
	        ./configure \
	            --prefix=$(OUTPUT_DIR)/usr \
	            --host=$(HOST) \
	            --target=$(shell $(SHELL_DBG) basename $(CROSS_PREFIX) 2>/dev/null|cut -d'-' -f1) \
	            --enable-static --enable-shared \
	        ) && cd $${OLDPWD}; \
	    fi \
	    && touch $(STAMP_CONFIG); \
	fi

	$(ORIGIN_Q) \
	MESSAGE=$(if $(filter 0 1,$(MAKELEVEL)),"\r$(PKG_NAME) already built\n",""); \
	if [ -f $(STAMP_SHIELD) ]; then true; \
	elif [ -f $(STAMP_BUILD) ]; then \
	    echo -ne $${MESSAGE}; \
	else \
	    if grep -q 'build *:' $(HD_MAKEFILE); then \
	        $(MAKE) build -f $(HD_MAKEFILE); \
	    else \
	        if [ -d $(PKG_NAME)* ]; then \
	            cd $(PKG_NAME)* && $(MAKE) -j8 all && cd ..; \
	        fi \
	    fi \
	    && touch $(STAMP_BUILD); \
	fi

	$(ORIGIN_Q) \
	MESSAGE=$(if $(filter 0 1,$(MAKELEVEL)),"\r$(PKG_NAME) already installed\n",""); \
	if [ -f $(STAMP_SHIELD) ]; then true; \
	elif [ -f $(STAMP_INSTALL) ]; then \
	    echo -ne $${MESSAGE}; \
	else \
	    if grep -q 'install *:' $(HD_MAKEFILE); then \
	        $(MAKE) install -f $(HD_MAKEFILE); \
	    else \
	        if [ -d $(PKG_NAME)* ]; then \
	            cd $(PKG_NAME)* && $(MAKE) install && cd ..; \
	        fi \
	    fi \
	    && touch $(STAMP_INSTALL); \
	fi

	$(ORIGIN_Q) \
	MESSAGE=$(if $(filter 0 1,$(MAKELEVEL)),"\r$(PKG_NAME) already post-installed\n",""); \
	if [ -f $(STAMP_POSTINS) ]; then \
	    echo -ne $${MESSAGE}; \
	else \
	    if grep -q 'post-install *:' $(HD_MAKEFILE); then \
	        $(MAKE) post-install -f $(HD_MAKEFILE); \
	    fi \
	    && touch $(STAMP_POSTINS); \
	fi

clean-prepare:
	$(Q)rm -f $(STAMP_BUILD) $(STAMP_INSTALL) $(STAMP_POSTINS)
else
all:
	$(Q)true
clean:
	$(Q)true
endif   # ifeq ($(strip $(PKG_SWITCH)),y)
