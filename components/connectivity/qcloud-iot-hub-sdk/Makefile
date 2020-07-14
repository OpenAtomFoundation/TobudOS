###################################################################
#automatic detection QTOP and LOCALDIR
CUR_DIR := $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST)))))
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


TREE_LIB_ENABLE=1
lib=
subdirs=

CFGFLAGS += -I$(CUR_DIR)/3rdparty/include 
CFGFLAGS += -I$(CUR_DIR)/3rdparty/include/exports 
CFGFLAGS += -I$(CUR_DIR)/3rdparty/platform/at_device/esp8266 
CFGFLAGS += -I$(CUR_DIR)/3rdparty/sdk_src/internal_inc
CFGFLAGS += -I$(CUR_DIR)/3rdparty/external_libs/mbedtls/include
#CFGFLAGS += -Werror
CFGFLAGS += -Wall -Wno-error=sign-compare -Wno-error=format -Os -pthread -DFORCE_SSL_VERIFY

#包含需要排除plaform/os的.c源码文件, 故自定义LSRC
LSRCS_EXCLUDE   := $(sort $(shell find  3rdparty/platform/os -name "*.c" -o -name "*.cpp" -o  -name "*.s" -o  -name "*.cc" -o  -name  "*.C"))
LSRCS_EXCLUDE   += $(sort $(shell find  3rdparty/samples -name "*.c" -o -name "*.cpp" -o  -name "*.s" -o  -name "*.cc" -o  -name  "*.C"))

#SCRIPT_DIR=$(CUR_DIR)/3rdparty/tools/build_scripts/
#include $(CUR_DIR)/3rdparty/make.settings 
#include $(CUR_DIR)/3rdparty/tools/build_scripts/parse_make_settings.mk 


include ${QTOP}/qmk/generic/Make.tpl


