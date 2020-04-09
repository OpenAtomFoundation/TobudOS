#
# Rules for building sub-modules
#

# ----------------------------------------------------------------------------
# common rules
# ----------------------------------------------------------------------------

include $(ART_ROOT)/make/make_rules.mk

# ----------------------------------------------------------------------------
# sub-modules
# ----------------------------------------------------------------------------
SUBDIRS := driver/ll driver/hal serial utils kernel console atcmd wifi

#ifeq ($(__CONFIG_OS_FREERTOS), y)
#SUBDIRS += kernel/FreeRTOS kernel/os/FreeRTOS
#endif

ifeq ($(__CONFIG_BOOTLOADER), y)

SUBDIRS += driver/component \
	pm \
	console \
	efpg \
	fs/fatfs \
	audio/pcm \
	audio/manager \
	sys \
	net/wlan \
	net/$(LWIP_DIR) \
	net/ping \
	net/HTTPClient \
	net/mbedtls-2.2.0 \
	net/nopoll \
	net/libwebsockets/lib \
	net/mqtt \
	net/shttpd-1.42 \
	net/sntp \
	net/udhcp-0.9.8 \
	net/cloud/aliyun \
	smartlink \
	cjson \
	atcmd \
	xz \
	utils

endif

# ----------------------------------------------------------------------------
# building rules
# ----------------------------------------------------------------------------
.PHONY: all install clean install_clean $(SUBDIRS) _all _install _clean _install_clean

all:
	$(Q)$(MAKE) _all TARGET=all

install:
	$(Q)$(MAKE) _install TARGET=install

clean:
	$(Q)$(MAKE) _clean TARGET=clean

install_clean:
	$(Q)$(MAKE) _install_clean TARGET=install_clean

$(SUBDIRS):
	$(Q)$(MAKE) $(S) -C $@ $(TARGET)

_all: $(SUBDIRS)

_install: $(SUBDIRS)

_clean: $(SUBDIRS)

_install_clean: $(SUBDIRS)
