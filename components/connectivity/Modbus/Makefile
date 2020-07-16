###################################################################
#automatic detection QTOP and LOCALDIR
CUR_DIR := $(patsubst %/,%,$(dir $(realpath $(firstword $(MAKEFILE_LIST)))))
TRYQTOP := $(shell if [ -n "$$QTOP" ] ; then\
        echo $$QTOP;\
        else\
        cd $(CUR_DIR); while /usr/bin/test ! -d qmk ; do \
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

####################################################################


TREE_LIB_ENABLE=y
lib=
subdirs=

CFGFLAGS += -std=c99
CFGFLAGS += -I$(CUR_DIR)/porting/TencentOS_Tiny
CFGFLAGS += -I$(CUR_DIR)/3rdparty/freemodbus-v1.6/modbus/include
CFGFLAGS += -I$(CUR_DIR)/3rdparty/freemodbus-v1.6/modbus/rtu
CFGFLAGS += -I$(CUR_DIR)/3rdparty/freemodbus-v1.6/modbus/ascii

LSRCS_DIRS   += 3rdparty/freemodbus-v1.6/modbus


include ${QTOP}/qmk/generic/Make.tpl


