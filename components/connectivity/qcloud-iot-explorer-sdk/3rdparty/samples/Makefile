# Basic Settings
SHELL           := /bin/bash
TOP_DIR         ?= $(CURDIR)/../
SUBDIRS         := directory-not-exist-actually

# Settings of input directory
SCRIPT_DIR      := $(TOP_DIR)/tools/build_scripts

include $(TOP_DIR)/make.settings
include $(SCRIPT_DIR)/parse_make_settings.mk

# Makefile echo
ifeq ($(DEBUG_MAKEFILE),n)
    Q               := @
    TOP_Q           := @
else
    Q               :=
    TOP_Q           :=
endif

# Settings of output directory
SAMPLE_DIR		:= $(CURDIR)
FINAL_DIR       := $(CURDIR)/../output/$(BUILD_TYPE)

IOT_LIB_DIR = $(FINAL_DIR)/lib
IOT_INC_CFLAGS = -I$(FINAL_DIR)/include -I$(FINAL_DIR)/include/exports  -I$(CURDIR)/../sdk_src/internal_inc

LDFLAGS             := -Wl,--start-group $(IOT_LIB_DIR)/libiot_sdk.a
ifeq ($(FEATURE_AUTH_WITH_NOTLS),n)
LDFLAGS             += $(IOT_LIB_DIR)/libmbedtls.a $(IOT_LIB_DIR)/libmbedx509.a $(IOT_LIB_DIR)/libmbedcrypto.a
endif
LDFLAGS             += $(IOT_LIB_DIR)/libiot_platform.a -Wl,--end-group


CFLAGS += -Wall -Wno-error=sign-compare -Wno-error=format -Os -pthread -DFORCE_SSL_VERIFY
CFLAGS += ${IOT_INC_CFLAGS}

ifeq ($(FEATURE_AUTH_MODE),CERT)
CFLAGS += -DAUTH_MODE_CERT
endif

.PHONY: ota_mqtt_sample data_template_sample

all: ota_mqtt_sample data_template_sample


ifneq (,$(filter -DOTA_COMM_ENABLED,$(CFLAGS)))
ota_mqtt_sample:
	$(TOP_Q) \
	$(PLATFORM_CC) $(CFLAGS) $(SAMPLE_DIR)/ota/$@.c $(LDFLAGS) -o $@

	$(TOP_Q) \
	mv $@ $(FINAL_DIR)/bin
endif

ifneq (,$(filter -DDATA_TEMPLATE_ENABLED,$(CFLAGS)))
data_template_sample:
	$(TOP_Q) \
	$(PLATFORM_CC) $(CFLAGS) $(SAMPLE_DIR)/data_template/$@.c $(LDFLAGS) -o $@
	
	$(TOP_Q) \
	$(PLATFORM_CC) $(CFLAGS) $(SAMPLE_DIR)/scenarized/light_$@.c $(LDFLAGS) -o light_$@
	
	$(TOP_Q) \
	mv $@ $(FINAL_DIR)/bin && \
	mv light_$@ $(FINAL_DIR)/bin 
endif

clean:
	rm -rf $(FINAL_DIR)/bin/*

