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


export BP=Linux_Posix

TREE_LIB_ENABLE=1
lib=
subdirs =

CFGFLAGS += -I$(CUR_DIR)/inc
CFGFLAGS += -I$(QTOP)/net/sal_module_wrapper
CFGFLAGS += -I$(QTOP)/components/connectivity/Eclipse-Paho-MQTT/wrapper/include
CFGFLAGS += -I$(QTOP)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/include

all::
	make -C ${QTOP}/arch BP=Linux_Posix
	make -C ${QTOP}/kernel
	make -C ${QTOP}/osal
	make -C ${QTOP}/net
	make -C ${QTOP}/devices
	make -C ${QTOP}/components/connectivity/Eclipse-Paho-MQTT

exec = 
LD_A_FILES += $(LIBDIR)/libarch.a
LD_A_FILES += $(LIBDIR)/libkernel.a
LD_A_FILES += $(LIBDIR)/libhello_world.a
LD_A_FILES += $(LIBDIR)/libcmsis_os.a
LD_A_FILES += $(LIBDIR)/libEclipse-Paho-MQTT.a
LD_A_FILES += $(LIBDIR)/libsal_module_wrapper.a
LDFLAGS += -lpthread

include ${QTOP}/qmk/generic/Make.exec


