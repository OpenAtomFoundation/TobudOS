iot_sdk_objects = $(patsubst %.c,%.o, $(IOTSDK_SRC_FILES))
iot_platform_objects = $(patsubst %.c,%.o, $(IOTPLATFORM_SRC_FILES))

.PHONY: config mbedtls clean final-out samples tests

all: config mbedtls ${COMP_LIB} ${PLATFORM_LIB} final-out samples tests 
	$(call Compile_Result)

${COMP_LIB}: ${iot_sdk_objects}
	$(call Brief_Log,"AR")
	$(TOP_Q) \
	$(AR) rcs $@ $(iot_sdk_objects)
	
	$(TOP_Q) \
	rm ${iot_sdk_objects}
	
${PLATFORM_LIB}: ${iot_platform_objects}
	$(call Brief_Log,"AR")
	$(TOP_Q) \
	$(AR) rcs $@ $(iot_platform_objects)
	
	$(TOP_Q) \
	rm ${iot_platform_objects}
	
config:
	$(TOP_Q) \
	mkdir -p ${TEMP_DIR}
	
mbedtls:
ifeq (,$(filter -DAUTH_WITH_NOTLS,$(CFLAGS)))
	$(TOP_Q) \
	make -s -C $(THIRD_PARTY_PATH)/mbedtls lib -e CC=$(PLATFORM_CC) AR=$(PLATFORM_AR)
	
	$(TOP_Q) \
	cp -RP  $(THIRD_PARTY_PATH)/mbedtls/library/libmbedtls.*  \
			$(THIRD_PARTY_PATH)/mbedtls/library/libmbedx509.* \
			$(THIRD_PARTY_PATH)/mbedtls/library/libmbedcrypto.* \
			$(TEMP_DIR)
	
	$(TOP_Q) \
	cd $(TEMP_DIR) && $(AR) x libmbedtls.a \
						&& $(AR) x libmbedx509.a \
						&& $(AR) x libmbedcrypto.a
endif

${iot_sdk_objects}:%.o:%.c
	$(TOP_Q) echo '' > $(TOP_DIR)/include/config.h
	$(call Brief_Log,"CC")
	$(TOP_Q) \
	$(PLATFORM_CC) $(CFLAGS) -c $^ -o $@
	
${iot_platform_objects}:%.o:%.c
	$(call Brief_Log,"CC")
	$(TOP_Q) \
	$(PLATFORM_CC) $(CFLAGS) -c $^ -o $@
	
final-out :
	$(TOP_Q) \
	mkdir -p ${FINAL_DIR}  ${DIST_DIR}  ${FINAL_DIR}/lib \
			 ${FINAL_DIR}/include  ${FINAL_DIR}/bin
	
	$(TOP_Q) \
	mv ${COMP_LIB} ${FINAL_DIR}/lib/ && \
	mv ${PLATFORM_LIB} ${FINAL_DIR}/lib

	$(TOP_Q) \
	cp -rf $(TOP_DIR)/include $(FINAL_DIR)/

	$(TOP_Q) \
	cp -rf $(TOP_DIR)/certs $(FINAL_DIR)/bin/
	
	$(TOP_Q) \
	cp -rf $(TOP_DIR)/device_info.json $(FINAL_DIR)/bin/
	
ifeq (,$(filter -DAUTH_WITH_NOTLS,$(CFLAGS)))
	$(TOP_Q) \
	mv ${TEMP_DIR}/*.a ${FINAL_DIR}/lib/
endif
	
	$(TOP_Q) \
	rm -rf ${TEMP_DIR}

#include $(SCRIPT_DIR)/rules-tests.mk

samples:
	$(TOP_Q) \
	make -s -C $(SAMPLE_DIR)

TLSDIR = $(THIRD_PARTY_PATH)/mbedtls
clean:
	$(TOP_Q) \
	rm -rf ${TEMP_DIR}
	
	$(TOP_Q) \
	rm -rf ${DIST_DIR}
	
ifeq (,$(filter -DAUTH_WITH_NOTLS,$(CFLAGS)))
ifeq ($(TLSDIR), $(wildcard $(THIRD_PARTY_PATH)/mbedtls))
	$(TOP_Q) \
	make -s -C $(THIRD_PARTY_PATH)/mbedtls clean
endif
endif

ifeq (,$(filter -DSDKTESTS_ENABLED,$(CFLAGS)))
	$(TOP_Q) \
	rm -rf $(TEST_LIB_DIR)
endif
