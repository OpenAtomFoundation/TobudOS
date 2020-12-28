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

CFGFLAGS += -I$(CUR_DIR)/common/lzma/wrapper
CFGFLAGS += -I$(CUR_DIR)/common/lzma/3rdparty
CFGFLAGS += -I$(CUR_DIR)/common/flash
CFGFLAGS += -I$(CUR_DIR)/common/partition
CFGFLAGS += -I$(CUR_DIR)/common/crc
CFGFLAGS += -I$(CUR_DIR)/common/image
CFGFLAGS += -I$(CUR_DIR)/common/env
CFGFLAGS += -I$(CUR_DIR)/common/info
CFGFLAGS += -I$(CUR_DIR)/common/diff
CFGFLAGS += -I$(CUR_DIR)/recovery/include
CFGFLAGS += -I$(CUR_DIR)/download/include
CFGFLAGS += -I$(QTOP)/components/fs/kv/include
CFGFLAGS += -I$(QTOP)/net/sal_module_wrapper

include ${QTOP}/qmk/generic/Make.tpl


