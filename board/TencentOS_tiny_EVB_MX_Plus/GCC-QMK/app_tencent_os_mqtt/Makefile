###################################################################
#automatic detection QTOP and LOCALDIR
CUR_DIR := $(patsubst %/,%,$(dir $(realpath $(firstword $(MAKEFILE_LIST)))))
TRYQTOP := $(shell if [ -n "$$QTOP" ] ; then\
        echo $$QTOP;\
        else\
        cd $(CUR_DIR); while /usr/bin/test ! -e qmk ; do \
        dir=`cd ../;pwd`;                       \
        if [ "$$dir" = "/" ] ; then             \
           echo Cannot find QTOP in $(firstword $(MAKEFILE_LIST)) 1>&2; \
           exit 1;                              \
        fi ;                                    \
        cd $$dir;                               \
        done ;                                  \
        pwd;                                    \
        fi)
QTOP ?= $(realpath ${TRYQTOP})

ifeq ($(QTOP),)
$(error Please run this in a tree)
endif
LOCALDIR = $(patsubst %/,%,$(subst $(realpath $(QTOP))/,,$(CUR_DIR)))
export QTOP

####################################################################


export BP=TencentOS_tiny_EVB_MX_Plus

TREE_LIB_ENABLE=1
lib=
subdirs =

LSRCS += $(QTOP)/examples/tencent_os_mqtt/mqtt_example.c
LSRCS += $(wildcard *.s)

all::
	make -C ${QTOP}/arch BP=${BP}
	make -C ${QTOP}/board/TencentOS_tiny_EVB_MX_Plus/BSP BP=${BP}
	make -C ${QTOP}/components/connectivity/Eclipse-Paho-MQTT BP=${BP}
	make -C ${QTOP}/devices BP=${BP}
	make -C ${QTOP}/kernel BP=${BP}
	make -C ${QTOP}/net/at BP=${BP}
	make -C ${QTOP}/net/sal_module_wrapper BP=${BP}
	make -C ${QTOP}/net/socket_wrapper BP=${BP}
	make -C ${QTOP}/net/tencent_firmware_module_wrapper BP=${BP}
	make -C ${QTOP}/osal BP=${BP}
	make -C ${QTOP}/platform BP=${BP}
#exec = 
LD_A_FILES += $(LIBDIR)/libarch.a
LD_A_FILES += $(LIBDIR)/libkernel.a
LD_A_FILES += $(LIBDIR)/lib$(notdir $(CUR_DIR)).a
LD_A_FILES += $(LIBDIR)/libcmsis_os.a
LD_A_FILES += $(LIBDIR)/libvendor_bsp.a
LD_A_FILES += $(LIBDIR)/libhal.a
LD_A_FILES += $(LIBDIR)/libBSP.a
LD_A_FILES += $(LIBDIR)/libEclipse-Paho-MQTT.a 
LD_A_FILES += $(LIBDIR)/libesp8266_tencent_firmware.a
LD_A_FILES += $(LIBDIR)/libesp8266.a
LD_A_FILES += $(LIBDIR)/libat.a
LD_A_FILES += $(LIBDIR)/libtencent_firmware_module_wrapper.a
LD_A_FILES += $(LIBDIR)/libsal_module_wrapper.a
LD_A_FILES += $(LIBDIR)/libsocket_wrapper.a
LD_A_LISTS += -lc -lm -lnosys
LD_L_LISTS += -specs=nosys.specs -T$(CUR_DIR)/STM32L431RCTx_FLASH.ld -Wl,-Map=$(LIBDIR)/$(notdir $(CUR_DIR)).map,--cref -Wl,--gc-sections
include ${QTOP}/qmk/generic/Make.exec


