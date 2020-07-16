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


TREE_LIB_ENABLE=0
lib=
subdirs=
ifneq (help,$(findstring help,$(MAKECMDGOALS)))

ifeq (, $(strip ${BP}))
$(error when compile arch/, must specify BP obviously , see `make help`)
endif

include ${QTOP}/qmk/board-pack/bp.${BP}


ifeq (,$(strip ${ARCH_LSRCS}))
$(error when compile arch/, must specify ARCH_LSRCS on your bp.${BP} , see `make help`)
endif
endif

# arch src should be specify by bp
LSRCS := $(subst $(QTOP)/arch/,, ${ARCH_LSRCS})
include ${QTOP}/qmk/generic/Make.tpl


